# Driver de motor — SparkFun TB6612FNG (ROB-14451)

## Español

Módulo breakout del driver de potencia para motores DC de doble canal fabricado por SparkFun Electronics.  
Basado en el chip **Toshiba TB6612FNG**.

### Archivos

| Archivo | Descripción |
|---------|-------------|
| `TB6612FNG-Datasheet-Toshiba.pdf` | Datasheet del chip TB6612FNG (Toshiba) |
| `SparkFun_Motor_Driver-TB6612FNG_v11c.pdf` | Esquemático de la placa SparkFun (PDF) |
| `SparkFun_Motor_Driver-TB6612FNG_v11c/` | Archivos de diseño Eagle (.brd + .sch) |

### Especificaciones del módulo SparkFun ROB-14451

| Parámetro | Valor |
|-----------|-------|
| Tensión de alimentación del motor (VM) | 2,5 – 13,5 V |
| Corriente continua por canal | **1,2 A** |
| Corriente pico por canal | **3,2 A** (hasta 2 s) |
| Tensión lógica (VCC) | 2,7 – 5,5 V |
| Frecuencia PWM máxima | 100 kHz |
| Canales de motor | 2 (en el equipo se usa solo el canal A) |
| Dimensiones | 20,3 × 15,2 mm |

### Conexión con Arduino UNO

| Pin SparkFun | Pin Arduino | Función |
|--------------|-------------|---------|
| PWMA | D9 (Timer1) | Control de velocidad canal A — PWM 31,25 kHz |
| AIN1 | D7 | Dirección canal A (bit 1) |
| AIN2 | D8 | Dirección canal A (bit 2) |
| STBY | D6 | Standby (HIGH = activo) |
| VM | 12–24 V | Alimentación del motor |
| VCC | 5 V | Alimentación lógica |
| GND | GND | Referencia |
| AO1, AO2 | Motor+ / Motor− | Salida al motor |

> El pin **STBY** debe estar en HIGH para que el driver esté activo. Si se deja flotante o en LOW el motor no responde.

### Referencias

- 📄 Página SparkFun: https://www.sparkfun.com/products/14451
- 📄 Repositorio GitHub (Eagle + Gerbers): https://github.com/sparkfun/SparkFun_Motor_Driver_TB6612FNG

---

## English

SparkFun breakout board for the Toshiba **TB6612FNG** dual-channel DC motor driver.  
Continuous current 1.2 A per channel, 3.2 A peak. PWM frequency up to 100 kHz (firmware uses 31.25 kHz via Timer1 prescaler=1 to eliminate audible noise).
