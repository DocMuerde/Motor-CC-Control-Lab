# Motor CC — Equipo de Prácticas de Control / DC Motor Control Lab

<p align="center">
  <img src="docs/images/equipo_render.png" alt="Render del equipo de prácticas" width="600"/>
</p>

<p align="center">
  <a href="#español">🇪🇸 Español</a> &nbsp;|&nbsp;
  <a href="#english">🇬🇧 English</a>
</p>

---

## Español

### Descripción del sistema

Este repositorio contiene todo el material necesario para montar y utilizar el **equipo de prácticas de control de un motor de corriente continua** desarrollado en el Departamento de Ingeniería de Sistemas y Automática de la Universidad de Sevilla (GEPOC).

El equipo permite implementar y validar experimentalmente distintas estrategias de control (PID, control en espacio de estados, etc.) sobre un motor CC real, cerrando el bucle desde MATLAB a través de comunicación serie con un microcontrolador Arduino.

### Componentes del sistema

| Componente | Modelo | Descripción |
|---|---|---|
| Motor | Maxon RE / EC | Motor CC con reductora planetaria |
| Driver | Toshiba TB6612FNG | Driver dual de motor, 1.2 A cont. / 3 A pico |
| Encoder | ams AS5047P | Encoder magnético 14 bits, lado de carga |
| Microcontrolador | Arduino (Uno / Mega) | Adquisición y protocolo de comunicaciones |
| Software de control | MATLAB / Simulink | Cálculo de la acción de control en PC |
| Estructura | PLA (impresión 3D) | Bancada y soportes fabricados en casa |

### Esquema del sistema

```
  PC (MATLAB)
      │  Comunicación serie (USB)
      ▼
  Arduino
  ┌─────────────────────────────────┐
  │  · Lee encoder AS5047P (SPI)    │
  │  · Envía posición a MATLAB      │
  │  · Recibe acción de control     │
  │  · Genera PWM → Driver L298N    │
  └────────────┬────────────────────┘
               │ PWM + dirección
               ▼
           Driver L298N
               │
               ▼
        Motor Maxon + reductora
               │
        Disco de inercia
               │
           AS5047P (encoder magnético)
```

### Estructura del repositorio

```
Motor-CC-Control-Lab/
├── firmware/
│   └── arduino/          # Código Arduino: protocolo serie + lectura encoder + PWM
├── software/
│   └── matlab/           # Scripts MATLAB: comunicación, control, identificación
├── hardware/
│   ├── 3d-printing/      # Archivos STL para imprimir las piezas
│   └── pcb/              # Esquemáticos y layout de la placa de circuito impreso
├── datasheets/
│   ├── motor/            # Datasheet del motor Maxon
│   ├── encoder/          # Datasheet AS5047P
│   └── driver/           # Datasheet del driver
├── docs/                 # Documentación, fotos, manual de usuario
└── labs/                 # Guiones de prácticas
    ├── lab01/
    ├── lab02/
    └── lab03/
```

### Requisitos

- **Hardware:** Ver lista de componentes en [`docs/bill_of_materials.md`](docs/bill_of_materials.md)
- **Software:** MATLAB R2020b o superior, Arduino IDE 2.x
- **Librerías Arduino:** `SPI.h` (incluida en el IDE)

### Cómo empezar

1. Clona el repositorio:
   ```bash
   git clone https://github.com/DocMuerde/Motor-CC-Control-Lab.git
   ```
2. Imprime las piezas 3D → [`hardware/3d-printing/`](hardware/3d-printing/)
3. Fabrica o monta la placa → [`hardware/pcb/`](hardware/pcb/)
4. Carga el firmware en Arduino → [`firmware/arduino/`](firmware/arduino/)
5. Ejecuta el script de MATLAB → [`software/matlab/`](software/matlab/)
6. Realiza las prácticas → [`labs/`](labs/)

### Licencia

Este proyecto se distribuye bajo licencia **Creative Commons Attribution-ShareAlike 4.0** (CC BY-SA 4.0).

### Créditos

Desarrollado en el **Grupo de Ingeniería de Sistemas y Automática (GEPOC)**  
Universidad de Sevilla — Departamento de Ingeniería de Sistemas y Automática  
Contacto: [ialvarado@us.es](mailto:ialvarado@us.es)

---

## English

### System Description

This repository contains all the material needed to assemble and use the **DC motor control lab setup** developed at the University of Seville (GEPOC).

The rig allows implementing and experimentally validating different control strategies (PID, state-space, etc.) on a real DC motor, closing the loop from MATLAB via serial communication with an Arduino microcontroller.

### System Components

| Component | Model | Description |
|---|---|---|
| Motor | Maxon RE / EC | DC motor with planetary gearbox |
| Driver | Toshiba TB6612FNG | Dual motor driver, 1.2 A cont. / 3 A peak |
| Encoder | ams AS5047P | 14-bit magnetic encoder, load side |
| Microcontroller | Arduino (Uno / Mega) | Data acquisition and comm. protocol |
| Control software | MATLAB / Simulink | Control law computation on PC |
| Structure | PLA (3D printed) | Bench and supports, in-house fabricated |

### Getting Started

1. Clone the repository:
   ```bash
   git clone https://github.com/DocMuerde/Motor-CC-Control-Lab.git
   ```
2. Print the 3D parts → [`hardware/3d-printing/`](hardware/3d-printing/)
3. Build or assemble the PCB → [`hardware/pcb/`](hardware/pcb/)
4. Upload the firmware to Arduino → [`firmware/arduino/`](firmware/arduino/)
5. Run the MATLAB script → [`software/matlab/`](software/matlab/)
6. Follow the lab sessions → [`labs/`](labs/)

### License

CC BY-SA 4.0 — See [LICENSE](LICENSE) for details.

### Credits

**Systems Engineering and Automation Group (GEPOC)**  
University of Seville — Contact: [ialvarado@us.es](mailto:ialvarado@us.es)
