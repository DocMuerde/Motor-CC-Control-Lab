# Firmware Arduino / Arduino Firmware

## Español

### Descripción

El firmware corre en un **Arduino UNO** y realiza las siguientes funciones:

1. **Lectura del encoder AS5047P** mediante SPI (14 bits → 16 384 posiciones/vuelta).
2. **Cálculo de la velocidad angular** como diferencia de posición dividida por Ts.
3. **Envío a MATLAB** de posición y velocidad a 100 Hz por puerto serie.
4. **Recepción de la acción de control** enviada por MATLAB.
5. **Generación de PWM** al driver L298N para mover el motor.

> **Nota de diseño:** El código incluye un **observador de Luenberger** (identificado con este motor a 12 V) que está implementado y validado, pero se deja desactivado. Si se necesita filtrar la velocidad, se realiza en MATLAB, que es más flexible y no requiere recompilar el firmware. Para activarlo, ver los comentarios en `main/main.ino`.

### Librerías necesarias

Instalar desde el Gestor de Librerías del Arduino IDE:

| Librería | Función |
|---|---|
| **AS5047P** | Driver del encoder magnético (SPI) |
| **MsTimer2** | Temporizador por interrupción (Ts = 10 ms) |

### Protocolo de comunicación serie (binario, 115 200 baud)

**Arduino → MATLAB** (cada Ts = 10 ms, 14 bytes en total):

```
'V' | float angle (4 B) | float w (4 B) | float pwm (4 B) | '\n'
```

| Campo | Tipo | Descripción |
|---|---|---|
| `angle` | float | Posición angular acumulada [°] (sin límite de rango) |
| `w` | float | Velocidad angular [°/s] |
| `pwm` | float | Señal de control aplicada (eco para verificación) |

**MATLAB → Arduino** (cada Ts, 4 bytes en total):

```
'A' | int16_t pwm (2 B) | '\n'
```

| Campo | Tipo | Descripción |
|---|---|---|
| `pwm` | int16_t | Señal de control ∈ [−255, 255]; signo = dirección |

### Pines (Arduino UNO)

| Función | Pin |
|---|---|
| CS encoder (SPI) | **D4** |
| SCK (SPI) | D13 |
| MISO (SPI) | D12 |
| MOSI (SPI) | D11 |
| Canal B encoder | D2 |
| Canal A encoder | D3 |
| Índice encoder | A0 |
| PWM motor (EnA) | D9 |
| Dirección (Ain1) | D7 |
| Dirección (Ain2) | D8 |

> La frecuencia del PWM en los pines 9 y 10 se eleva a **31 250 Hz** modificando el preescalador del Timer1, para eliminar el ruido audible en el bobinado del motor.

---

## English

### Description

The firmware runs on **Arduino UNO** and handles: AS5047P encoder reading (SPI at 400 kHz), angular velocity computation, binary serial communication with MATLAB at 100 Hz, and PWM generation for the L298N motor driver.

A **Luenberger observer** is implemented and validated in the code but intentionally left inactive — velocity filtering is done in MATLAB for flexibility. To enable it, see comments in `main/main.ino`.

### Required Libraries

Install from Arduino IDE Library Manager: **AS5047P** and **MsTimer2**.

### Binary Serial Protocol (115 200 baud)

**Arduino → MATLAB** (14 bytes): `'V'` + 3×float (angle, ω, pwm) + `'\n'`  
**MATLAB → Arduino** (4 bytes): `'A'` + int16_t (pwm ∈ [−255, 255]) + `'\n'`
