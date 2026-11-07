# PCB — Shield Arduino Motor CC / DC Motor Arduino Shield

<p align="center">
  <img src="../../docs/images/pcb_render.png" alt="Render 3D de la placa" width="500"/>
</p>

## Español

Shield diseñado a medida para Arduino UNO que integra en una sola placa el driver de motor y el conector del encoder, eliminando el cableado volante y haciendo el montaje completamente plug & play.

### Componentes integrados

| Ref | Componente | Valor | Función |
|---|---|---|---|
| TB66121 | **Toshiba TB6612FNG** | — | Driver DC motor — 1,2 A cont. / 3 A pico |
| C1 | Condensador film CBB | **100 nF (0,1 µF), 250 V** | Desacoplo de la alimentación lógica |
| U2 | Condensador electrolítico | **10 µF** (Rubycon 420BXC10MEFC10X20, Ø10×20 mm) | Desacoplo de la alimentación VM del motor |
| CN1 | Conector 2×8 pines, paso 2,54 mm | — | Interfaz con encoder **ams AS5047P** (CS, CLK, MOSI, MISO, 3,3V, 5V, GND, GND) |
| U4, U18 | 2× bornera de tornillo (2 pines, paso 2,54 mm) | — | Salidas al motor (AO1/AO2 y BO1/BO2) |

### Conexiones al Arduino

La placa se apila directamente sobre el Arduino UNO. Las señales de control quedan así:

| Señal | Pin Arduino | Descripción |
|---|---|---|
| PWMA | D9 | PWM del motor (Timer1, 31 250 Hz) |
| AIN1 | D7 | Dirección motor (bit 1) |
| AIN2 | D8 | Dirección motor (bit 2) |
| STBY | D6 | Standby driver (HIGH = activo) |
| CS encoder | D4 | Chip Select AS5047P (SPI) |
| SCK | D13 | Reloj SPI |
| MISO | D12 | Datos encoder → Arduino |
| MOSI | D11 | Datos Arduino → encoder |

### Alimentación

- **VCC (lógica):** 5 V desde el propio Arduino
- **VM (motor):** alimentación externa 9–12 V (bornera o pin VIN del Arduino con fuente externa)
- El condensador electrolítico en VM amortigua los transitorios de corriente del motor

### Archivos del diseño

| Archivo | Contenido |
|---|---|
| [`Schematic_Control_Motor_CC_2026-11-06.pdf`](Schematic_Control_Motor_CC_2026-11-06.pdf) | Esquemático completo |
| [`Gerber_Control_Motor_CC_PCB_Control_Motor_CC_2026-11-06.zip`](Gerber_Control_Motor_CC_PCB_Control_Motor_CC_2026-11-06.zip) | Archivos Gerber para fabricación |
| [`BOM_Control_Motor_CC_2026-11-06.csv`](BOM_Control_Motor_CC_2026-11-06.csv) | Lista de materiales (BOM) |
| [`OBJ_PCB_Control_Motor_CC_2026-11-06.zip`](OBJ_PCB_Control_Motor_CC_2026-11-06.zip) | Modelo 3D de la PCB (formato OBJ) |

> **Diseñado con:** [EasyEDA](https://easyeda.com)

### Notas de fabricación

- Placa de 2 capas, FR4
- Grosor de pista recomendado: ≥ 1 mm en las pistas de potencia (VM → motor)
- Compatible con servicios estándar de PCB (JLCPCB, PCBWay, etc.)

---

<p align="center">🇬🇧 &nbsp;&nbsp; <strong>E N G L I S H</strong> &nbsp;&nbsp; 🇬🇧</p>

---

## English

Custom Arduino UNO shield integrating the motor driver and encoder connector on a single board, eliminating loose wiring for a fully plug-and-play assembly.

### Integrated components

| Ref | Component | Value | Function |
|---|---|---|---|
| TB66121 | **Toshiba TB6612FNG** | — | DC motor driver — 1.2 A cont. / 3 A peak |
| C1 | CBB film capacitor | **100 nF (0.1 µF), 250 V** | Logic supply decoupling |
| U2 | Electrolytic capacitor | **10 µF** (Rubycon 420BXC10MEFC10X20, Ø10×20 mm) | Motor VM supply decoupling |
| CN1 | 2×8-pin connector, 2.54 mm pitch | — | Interface to **ams AS5047P** encoder (CS, CLK, MOSI, MISO, 3.3V, 5V, GND, GND) |
| U4, U18 | 2× screw terminal (2-pin, 2.54 mm pitch) | — | Motor outputs (AO1/AO2 and BO1/BO2) |

### Arduino connections

The shield stacks directly on top of the Arduino UNO. Control signals are mapped as follows:

| Signal | Arduino pin | Description |
|---|---|---|
| PWMA | D9 | Motor PWM (Timer1, 31 250 Hz) |
| AIN1 | D7 | Motor direction (bit 1) |
| AIN2 | D8 | Motor direction (bit 2) |
| STBY | D6 | Driver standby (HIGH = active) |
| Encoder CS | D4 | AS5047P Chip Select (SPI) |
| SCK | D13 | SPI clock |
| MISO | D12 | Encoder data → Arduino |
| MOSI | D11 | Arduino data → encoder |

### Power supply

- **VCC (logic):** 5 V from the Arduino itself
- **VM (motor):** external supply 9–12 V (screw terminal or Arduino VIN pin with external power supply)
- The electrolytic capacitor on VM absorbs motor current transients

### Design files

| File | Contents |
|---|---|
| [`Schematic_Control_Motor_CC_2026-11-06.pdf`](Schematic_Control_Motor_CC_2026-11-06.pdf) | Full schematic |
| [`Gerber_Control_Motor_CC_PCB_Control_Motor_CC_2026-11-06.zip`](Gerber_Control_Motor_CC_PCB_Control_Motor_CC_2026-11-06.zip) | Gerber files for manufacturing |
| [`BOM_Control_Motor_CC_2026-11-06.csv`](BOM_Control_Motor_CC_2026-11-06.csv) | Bill of materials (BOM) |
| [`OBJ_PCB_Control_Motor_CC_2026-11-06.zip`](OBJ_PCB_Control_Motor_CC_2026-11-06.zip) | PCB 3D model (OBJ format) |

> **Designed with:** [EasyEDA](https://easyeda.com)

### Fabrication notes

- 2-layer board, FR4
- Recommended trace width: ≥ 1 mm on power traces (VM → motor)
- Compatible with standard PCB services (JLCPCB, PCBWay, etc.)
