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

<p align="center">🇬🇧 &nbsp;&nbsp; <strong>E N G L I S H</strong> &nbsp;&nbsp; 🇬🇧</p>

---

## English

SparkFun breakout board for the **Toshiba TB6612FNG** dual-channel DC motor driver.

### Files

| File | Description |
|------|-------------|
| `TB6612FNG-Datasheet-Toshiba.pdf` | TB6612FNG chip datasheet (Toshiba) |
| `SparkFun_Motor_Driver-TB6612FNG_v11c.pdf` | SparkFun board schematic (PDF) |
| `SparkFun_Motor_Driver-TB6612FNG_v11c/` | Eagle design files (.brd + .sch) |

### SparkFun ROB-14451 specifications

| Parameter | Value |
|-----------|-------|
| Motor supply voltage (VM) | 2.5 – 13.5 V |
| Continuous current per channel | **1.2 A** |
| Peak current per channel | **3.2 A** (up to 2 s) |
| Logic supply voltage (VCC) | 2.7 – 5.5 V |
| Maximum PWM frequency | 100 kHz |
| Motor channels | 2 (only channel A used in the rig) |
| Dimensions | 20.3 × 15.2 mm |

### Arduino UNO connections

| SparkFun pin | Arduino pin | Function |
|--------------|-------------|---------|
| PWMA | D9 (Timer1) | Channel A speed control — PWM 31.25 kHz |
| AIN1 | D7 | Channel A direction (bit 1) |
| AIN2 | D8 | Channel A direction (bit 2) |
| STBY | D6 | Standby (HIGH = active) |
| VM | 12–24 V | Motor power supply |
| VCC | 5 V | Logic power supply |
| GND | GND | Ground reference |
| AO1, AO2 | Motor+ / Motor− | Motor output |

> The **STBY** pin must be held HIGH for the driver to be active. If left floating or pulled LOW, the motor will not respond.

The firmware sets Timer1's prescaler to 1, raising the PWM frequency on pins 9 and 10 to **31 250 Hz**, well above the audible range, to eliminate motor coil whine.

### References

- 📄 SparkFun product page: https://www.sparkfun.com/products/14451
- 📄 GitHub repository (Eagle + Gerbers): https://github.com/sparkfun/SparkFun_Motor_Driver_TB6612FNG
