/* =============================================================================
   EQUIPO DE PRÁCTICAS — CONTROL DE MOTOR CC
   Departamento de Ingeniería de Sistemas y Automática — ISA
   Universidad de Sevilla
   Contacto: ialvarado@us.es
   
   Repositorio: https://github.com/DocMuerde/Motor-CC-Control-Lab

   DESCRIPCIÓN GENERAL
   ───────────────────
   Firmware para Arduino UNO que actúa como nodo de adquisición y actuación
   en un sistema de control de posición/velocidad de motor CC con lazo cerrado
   en MATLAB.

   El bucle de control corre íntegramente en el PC (MATLAB). Arduino se encarga
   únicamente de:
     1. Leer el ángulo del encoder magnético AS5047P (vía SPI).
     2. Calcular la velocidad angular (diferencia de posición / Ts).
     3. Enviar posición y velocidad a MATLAB por puerto serie (100 Hz).
     4. Recibir la señal de control (PWM con signo) desde MATLAB.
     5. Aplicar dicha señal al driver TB6612FNG para mover el motor.

   El código incluye además un observador de Luenberger (comentado) que puede
   activarse para estimar velocidad filtrada en lugar de la diferencia cruda.

   CONEXIONES DE HARDWARE
   ──────────────────────
   Encoder AS5047P (SPI):
     MISO  → pin 12
     MOSI  → pin 11
     CLK   → pin 13
     CS    → pin 4
     B     → pin 2   (canal B cuadratura — 4000 pulsos/vuelta)
     A     → pin 3   (canal A cuadratura)
     I     → A0      (índice — 1 pulso/vuelta)

   Driver TB6612FNG (Motor A — Timer1 → 31 250 Hz):
     EnA   → pin 9   (PWM — Enable motor A)
     Ain1  → pin 7   (dirección)
     Ain2  → pin 8   (dirección)
     EnB   → pin 10  (PWM — Enable motor B, no usado actualmente)
     Bin1  → pin 6
     Bin2  → pin 5

   PROTOCOLO DE COMUNICACIÓN SERIE (binario, 115200 baud)
   ──────────────────────────────────────────────────────
   Arduino → MATLAB (cada Ts = 10 ms):
     'V' | float angle (4 B) | float w (4 B) | float pwm (4 B) | '\n'
     Total: 14 bytes.
     - angle : posición angular acumulada [°] (sin límite de rango)
     - w     : velocidad angular [°/s]  (= Δangle / 0.01)
     - pwm   : señal de control aplicada al driver (mismo valor recibido)

   MATLAB → Arduino (cada Ts):
     'A' | int16_t pwm (2 B) | '\n'
     Total: 4 bytes.
     - pwm ∈ [−255, 255]; signo = dirección, magnitud = ciclo de trabajo.

   LIBRERÍAS NECESARIAS
   ────────────────────
   - AS5047P  : https://github.com/MarginallyClever/AS5047P
     Instalar desde Arduino IDE → Gestor de librerías → "AS5047P"
   - MsTimer2 : https://github.com/PaulStoffregen/MsTimer2
     Instalar desde Arduino IDE → Gestor de librerías → "MsTimer2"

   PARÁMETROS DEL OBSERVADOR DE LUENBERGER
   ─────────────────────────────────────────
   El observador está identificado con el motor de este equipo a 12 V:
     Ts  = 0.01 s   K ≈ 3   τ ≈ 0.07 s
   Matrices del modelo discreto (ZOH):
     A = [0.8669, 0;  0.0093, 1]
     B = [0.4140; 0.0021]
   Ganancia del observador (polo de convergencia más rápido que la planta):
     L = [0.15, 0.0093; 0, 0.8]
   NOTA: el factor ×4 en la entrada del observador compensa que el modelo
   fue identificado con un driver Wemos (PWM en [0, 1023]) y aquí se usa
   el L298N (PWM en [0, 255] → escala ×4).

   VERSIÓN
   ───────
   v1.0 — 2024  Versión inicial para identificación (observador desactivado)
   v1.1 — 2026  Watchdog de comunicación serie: si MATLAB deja de enviar durante
                1 s, el motor se para automáticamente. Se reanuda al volver la señal.
============================================================================= */

#include <AS5047P.h>   // Driver del encoder magnético AS5047P (SPI)
#include <math.h>      // Funciones matemáticas (no usadas directamente, reservado)
#include <MsTimer2.h>  // Temporizador por interrupción basado en Timer2

// ─── Modo depuración ────────────────────────────────────────────────────────
// Poner a 1 para activar mensajes de depuración por serie (reduce rendimiento)
#define debug 0

// ─── Pines ──────────────────────────────────────────────────────────────────
#define CS   4   // Chip Select del encoder AS5047P (SPI)
#define EnA  9   // Enable (PWM) del canal A del driver TB6612FNG
#define Ain1 7   // Control de dirección motor (pin 1)
#define Ain2 8   // Control de dirección motor (pin 2)
#define EnB  10  // Enable (PWM) del canal B del driver (no usado)
#define Bin1 6   // Control de dirección canal B (no usado)
#define Bin2 5   // Control de dirección canal B (no usado)

// ─── Configuración SPI ──────────────────────────────────────────────────────
// Velocidad del bus SPI para el AS5047P (máx. 10 MHz; usamos 400 kHz, conservador)
#define AS5047P_CUSTOM_SPI_BUS_SPEED 400000

// ─── Objeto encoder ─────────────────────────────────────────────────────────
AS5047P as5047p(CS, AS5047P_CUSTOM_SPI_BUS_SPEED);

// ─── Variables globales ─────────────────────────────────────────────────────

// Flag de período de muestreo: la ISR del Timer2 lo pone a true cada 10 ms
volatile bool Sample_time = false;

// Señal de control recibida de MATLAB [−255, 255]
int pwm = 0;

// Flag que indica que se ha recibido un paquete completo de MATLAB
bool data_complete = false;

// ─── Watchdog de comunicación ────────────────────────────────────────────────
// Timestamp (ms) del último paquete válido recibido de MATLAB.
// Si pasa más de 1 s sin recibir nada, se para el motor por seguridad.
unsigned long lastReceivedTime = 0;

// ─── Unión para recepción de datos (MATLAB → Arduino) ───────────────────────
// MATLAB envía la señal de control como int16_t (2 bytes, little-endian)
typedef union {
  int16_t number;    // Valor entero interpretado
  uint8_t bytes[2];  // Bytes recibidos por serie
} data_rec;
data_rec u;

// ─── Unión para envío de datos (Arduino → MATLAB) ───────────────────────────
// Arduino envía 3 floats (angle, w, pwm) = 12 bytes, precedidos de 'V' y seguidos de '\n'
typedef union {
  float   number[3];  // [0]=ángulo [°], [1]=velocidad [°/s], [2]=pwm aplicado
  uint8_t bytes[12];  // Representación en bytes para envío serie
} data_send;
data_send y;


// =============================================================================
// SETUP
// =============================================================================
void setup() {
  Serial.begin(115200);  // Comunicación serie a 115200 baud (debe coincidir con MATLAB)

  // ── Inicialización del encoder AS5047P ──────────────────────────────────
  while (!as5047p.initSPI()) {
    // Si el encoder no responde, avisar y reintentar cada 5 s
    Serial.println(F("Error: no se puede conectar con el AS5047P. Revisa el cableado SPI."));
    delay(5000);
  }

  // ── Temporizador de muestreo (Timer2 → ISR cada 10 ms = 100 Hz) ─────────
  MsTimer2::set(10, Temp2_ISR);  // Período = 10 ms
  MsTimer2::start();

  // ── Inicializar watchdog de comunicación ─────────────────────────────────
  lastReceivedTime = millis();

  // ── Frecuencia PWM del Timer1 (pines 9 y 10) → 31 250 Hz ────────────────
  // El registro TCCR1B controla el preescalador del Timer1.
  // Poniendo los 3 bits de preescalador a 001 (sin división) se obtiene:
  //   f_PWM = f_clk / (256 * prescaler) = 16e6 / (256 * 1) = 62500 Hz (Fast PWM 8-bit)
  // Con Phase Correct PWM: 16e6 / (510 * 1) ≈ 31 372 Hz
  // Se elige alta frecuencia para evitar ruido audible en el bobinado del motor.
  TCCR1B = TCCR1B & 0b11111000 | 0b001;

  // ── Pines de dirección del motor como salidas digitales ──────────────────
  pinMode(Ain1, OUTPUT);
  pinMode(Ain2, OUTPUT);
  pinMode(Bin1, OUTPUT);
  pinMode(Bin2, OUTPUT);
}


// =============================================================================
// LOOP PRINCIPAL
// =============================================================================
void loop()
{
  // ── Contador de vueltas completas ─────────────────────────────────────────
  // El AS5047P devuelve ángulo en [0°, 360°). Para obtener posición absoluta
  // acumulada necesitamos contar cuántas vueltas completas se han dado.
  static int num = 0;

  // Variables de posición y velocidad
  float angle, angle_raw, w, w_raw, aux_w = 0, aux_p = 0;

  // Valor anterior del ángulo y estado del observador (persistentes entre llamadas)
  static float wh = 0;         // Velocidad estimada por el observador [°/s]
  static float aux = 0;        // Variable auxiliar del observador
  // Posición inicial: se centra el rango en 0° restando 180° al primer valor leído
  static float ph = as5047p.readAngleDegree() - 180;
  static float angle_ant = 0;  // Ángulo raw en el instante anterior

  // ── Parámetros del observador de Luenberger (identificado con este motor) ──
  float a11 = 0.8669, a12 = 0, a21 = 0.0093, a22 = 1;    // Matriz A (modelo discreto)
  float b11 = 0.4140, b21 = 0.0021;                        // Matriz B
  float l11 = 0.15,   l12 = 0.0093, l21 = 0, l22 = 0.8;  // Ganancia L del observador

  // ── Recepción de datos de MATLAB ──────────────────────────────────────────
  // Se llama a recepcion() cada vez que hay bytes en el buffer serie.
  // La función construye el paquete byte a byte y activa data_complete al terminar.
  if (Serial.available() > 0) {
    recepcion();
  }

  // ── Bloque síncrono (ejecuta solo cuando el Timer2 dispara) ───────────────
  if (Sample_time)
  {
    // ── Watchdog de comunicación (timeout 1 s) ──────────────────────────────
    // Si MATLAB deja de enviar datos más de 1 segundo, se para el motor por
    // seguridad (p. ej. si se detiene la simulación con PWM ≠ 0).
    // En cuanto vuelvan a llegar datos, se reanuda la operación normal.
    if (millis() - lastReceivedTime > 1000UL) {
      // Timeout: forzar parada
      pwm           = 0;
      set_speed(0);
      data_complete = false;
    } else if (data_complete) {
      // Comunicación activa: aplicar la señal de control recibida
      set_speed(pwm);
      data_complete = false;
    }

    // ── Leer ángulo del encoder ──────────────────────────────────────────────
    // readAngleDegree() devuelve [0°, 360°). Se resta 180° para centrar en 0.
    angle_raw = as5047p.readAngleDegree() - 180;

    // ── Calcular velocidad angular (diferencia de posición / Ts) ─────────────
    w_raw = angle_raw - angle_ant;

    // Gestión de saltos de ±360° al cruzar 0°/360°
    if (w_raw >=  180) { w_raw -= 360; num--; }  // Vuelta hacia atrás
    if (w_raw <= -180) { w_raw += 360; num++; }  // Vuelta hacia delante

    // Velocidad en [°/s]: Δangle / Ts, con Ts = 0.01 s → factor 100
    w = 100.0 * w_raw;

    // Posición absoluta acumulada [°] (sin límite de rango)
    angle = angle_raw + 360 * num;

    // ── Observador de Luenberger ────────────────────────────────────────────────
    // El observador ESTÁ IMPLEMENTADO Y FUNCIONA, pero se deja desactivado
    // intencionalmente: si se desea filtrar la velocidad, se hace en MATLAB
    // (más flexible, sin recompilar firmware). Activarlo aquí es sencillo:
    //   1. Comentar las líneas y.number[0]=angle y y.number[1]=w (más abajo).
    //   2. Descomentar las líneas y.number[0]=ph e y.number[1]=wh.
    aux_w = wh;
    aux_p = ph;
    wh = a11*wh + a12*ph + l11*(w - wh)    + l12*(angle - ph) + b11*pwm*4;
    ph = a21*aux + a22*ph + l21*(w - aux_w) + l22*(angle - aux_p) + b21*pwm*4;
    // y.number[0] = ph;  // Posición estimada (observador)
    // y.number[1] = wh;  // Velocidad estimada (observador)

    // ── Paquete de datos a enviar a MATLAB ───────────────────────────────────
    y.number[0] = angle;  // Posición absoluta acumulada [°]
    y.number[1] = w;      // Velocidad angular [°/s]
    y.number[2] = pwm;    // Señal de control aplicada (eco para verificación)

    // ── Envío por serie: 'V' | 12 bytes | '\n' ───────────────────────────────
    Serial.write('V');
    for (int i = 0; i < 12; i++) {
      Serial.write(y.bytes[i]);
    }
    Serial.write('\n');

    // ── Actualizar estado para el siguiente período ───────────────────────────
    angle_ant  = angle_raw;
    Sample_time = false;    // Esperar al siguiente disparo del Timer2
  }
}


// =============================================================================
// set_speed — Aplica una señal de control al driver TB6612FNG
// =============================================================================
// Parámetro v: PWM con signo, v ∈ [−255, 255]
//   v > 0 → motor gira en sentido directo
//   v < 0 → motor gira en sentido inverso
//   v = 0 → motor parado (Ain1 = Ain2 = LOW)
// =============================================================================
void set_speed(int v)
{
  if (v >= 0) {
    digitalWrite(Ain1, HIGH);
    digitalWrite(Ain2, LOW);
    analogWrite(EnA, v);
  } else {
    digitalWrite(Ain1, LOW);
    digitalWrite(Ain2, HIGH);
    analogWrite(EnA, -v);
  }
}


// =============================================================================
// Temp2_ISR — Rutina de servicio de interrupción del Timer2
// =============================================================================
// Se ejecuta cada 10 ms (100 Hz). Solo activa el flag Sample_time para que
// el loop principal realice las tareas de muestreo de forma síncrona.
// IMPORTANTE: mantener la ISR lo más corta posible.
// =============================================================================
void Temp2_ISR() {
  Sample_time = true;
}


// =============================================================================
// recepcion — Recibe y ensambla el paquete enviado por MATLAB
// =============================================================================
// Formato del paquete (4 bytes):
//   'A'  →  marca de inicio (resetea el índice de bytes)
//   byte0, byte1  →  int16_t con la señal de control (little-endian)
//   '\n'  →  fin de trama; activa data_complete y guarda el valor en pwm
//
// La función se llama desde el loop() y no desde la ISR para evitar problemas
// de concurrencia con el buffer serie de Arduino.
// =============================================================================
int recepcion() {
  static int i = 0;  // Índice dentro del buffer u.bytes[]
  char inChar;

  while (Serial.available() > 0) {
    inChar = Serial.read();

    if (inChar == 'A') {
      // Cabecera detectada: reiniciar índice de recepción
      i = 0;
    }
    else if (inChar == '\n') {
      // Fin de trama: el valor int16_t ya está ensamblado en u.bytes[]
      pwm              = u.number;   // Pasar el valor al controlador
      data_complete    = true;       // Señalizar al loop() que el dato es válido
      lastReceivedTime = millis();   // Actualizar timestamp del watchdog
    }
    else {
      // Bytes de datos: acumular en el buffer de la unión
      u.bytes[i] = inChar;
      i++;
    }
  }
}
