# Firmware Arduino / Arduino Firmware

## Español

El firmware corre en un **Arduino Uno/Mega** y realiza tres funciones principales:

1. **Lectura del encoder AS5047P** mediante SPI (14 bits → 16384 posiciones/vuelta).
2. **Protocolo de comunicación serie** con MATLAB: envía posición angular y recibe la acción de control.
3. **Generación de PWM** hacia el driver de potencia para mover el motor.

### Protocolo de comunicación (ASCII, terminador `\n`)

| Dirección | Formato | Ejemplo |
|---|---|---|
| Arduino → MATLAB | `A:<ángulo_deg>\n` | `A:123.456\n` |
| MATLAB → Arduino | `C:<pwm_0_255>:<dir_0_1>\n` | `C:180:1\n` |

### Pines (Arduino Uno)

| Función | Pin |
|---|---|
| CS encoder (SPI) | D10 |
| SCK | D13 |
| MISO | D12 |
| MOSI | D11 |
| PWM motor | D9 |
| Dirección motor | D8 |
| Enable driver | D7 |

### Dependencias
- `SPI.h` — incluida en Arduino IDE (sin instalar nada extra)

---

## English

The firmware runs on **Arduino Uno/Mega** and handles: AS5047P encoder reading (SPI), serial comm protocol with MATLAB, and PWM generation for the motor driver.
