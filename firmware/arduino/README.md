# Firmware Arduino / Arduino Firmware

## Español

### Descripción

El firmware corre en un **Arduino UNO** y realiza las siguientes funciones:

1. **Lectura del encoder AS5047P** mediante SPI (14 bits → 16 384 posiciones/vuelta).
2. **Cálculo de la velocidad angular** como diferencia de posición dividida por Ts.
3. **Envío a MATLAB** de posición y velocidad a 100 Hz por puerto serie.
4. **Recepción de la acción de control** enviada por MATLAB.
5. **Generación de PWM** al driver TB6612FNG para mover el motor.
6. **Watchdog de comunicación** para reducir progresivamente el PWM hasta cero si MATLAB deja de enviar órdenes.

> **Nota de diseño:** El código incluye un **observador de Luenberger** (identificado con este motor a 12 V) que está implementado y validado, pero se deja desactivado. Si se necesita filtrar la velocidad, se realiza en MATLAB, que es más flexible y no requiere recompilar el firmware. Para activarlo, ver los comentarios en `main/main.ino`.

### Watchdog de comunicación: parada progresiva

El firmware incorpora un watchdog que supervisa la recepción de órdenes desde MATLAB. Si transcurre **más de 1 segundo sin recibir una orden**, comienza una rampa de parada: reduce la magnitud del PWM en **2 unidades cada 10 ms**, manteniendo su signo hasta llegar a cero. Así evita cortar de golpe la acción del motor.

Desde un PWM de magnitud 255, la rampa tarda aproximadamente **1,28 segundos adicionales** al segundo de espera. Es el tiempo hasta que el PWM llega a cero; el tiempo de detención mecánica depende del equipo. Si vuelven a recibirse órdenes durante la rampa o después de ella, el firmware vuelve a aplicar la orden recibida.

Este comportamiento está implementado en [`main/main.ino`](main/main.ino), mediante `lastReceivedTime`, el umbral de `1000` ms y `WATCHDOG_RAMP_STEP = 2`.

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
| Standby driver | D6 |

> La frecuencia del PWM en los pines 9 y 10 se eleva a **31 250 Hz** modificando el preescalador del Timer1, para eliminar el ruido audible en el bobinado del motor.

---

<p align="center">🇬🇧 &nbsp;&nbsp; <strong>E N G L I S H</strong> &nbsp;&nbsp; 🇬🇧</p>

---

## English

### Description

The firmware runs on an **Arduino UNO** and performs the following functions:

1. **AS5047P encoder reading** via SPI (14 bits → 16 384 positions/rev).
2. **Angular velocity computation** as the position difference divided by Ts.
3. **Sending data to MATLAB** — position and velocity at 100 Hz over serial.
4. **Receiving the control action** sent by MATLAB.
5. **PWM generation** to the TB6612FNG driver to move the motor.
6. **Communication watchdog** to progressively reduce PWM to zero if MATLAB stops sending commands.

> **Design note:** The code includes a **Luenberger observer** (identified for this motor at 12 V) that is fully implemented and validated, but intentionally left inactive. Velocity filtering is done in MATLAB, which is more flexible and does not require recompiling the firmware. To enable it, see the comments in `main/main.ino`.

### Communication watchdog: gradual stop

The firmware includes a watchdog that monitors incoming commands from MATLAB. After **more than 1 second without a command**, it starts a stopping ramp: PWM magnitude decreases by **2 units every 10 ms**, retaining its sign until it reaches zero. This avoids abruptly cutting the motor command.

Starting at a PWM magnitude of 255, the ramp takes approximately **1.28 seconds in addition to** the initial one-second wait. This is the time for PWM to reach zero; the mechanical stopping time depends on the rig. If commands resume during or after the ramp, the firmware applies the incoming command again.

This behavior is implemented in [`main/main.ino`](main/main.ino), using `lastReceivedTime`, the `1000` ms threshold and `WATCHDOG_RAMP_STEP = 2`.

### Required libraries

Install from the Arduino IDE Library Manager:

| Library | Purpose |
|---|---|
| **AS5047P** | Magnetic encoder driver (SPI) |
| **MsTimer2** | Interrupt-based timer (Ts = 10 ms) |

### Binary serial protocol (115 200 baud)

**Arduino → MATLAB** (every Ts = 10 ms, 14 bytes total):

```
'V' | float angle (4 B) | float w (4 B) | float pwm (4 B) | '\n'
```

| Field | Type | Description |
|---|---|---|
| `angle` | float | Accumulated angular position [°] (unbounded) |
| `w` | float | Angular velocity [°/s] |
| `pwm` | float | Applied control signal (echo for verification) |

**MATLAB → Arduino** (every Ts, 4 bytes total):

```
'A' | int16_t pwm (2 B) | '\n'
```

| Field | Type | Description |
|---|---|---|
| `pwm` | int16_t | Control signal ∈ [−255, 255]; sign = direction |

### Pin assignments (Arduino UNO)

| Function | Pin |
|---|---|
| Encoder CS (SPI) | **D4** |
| SCK (SPI) | D13 |
| MISO (SPI) | D12 |
| MOSI (SPI) | D11 |
| Encoder channel B | D2 |
| Encoder channel A | D3 |
| Encoder index | A0 |
| Motor PWM (EnA) | D9 |
| Direction (Ain1) | D7 |
| Direction (Ain2) | D8 |
| Driver standby | D6 |

> The PWM frequency on pins 9 and 10 is raised to **31 250 Hz** by modifying the Timer1 prescaler, to eliminate audible noise from the motor windings.
