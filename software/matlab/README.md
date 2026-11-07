# Software MATLAB — `MotorCC_V3.slx`

<p align="center">
  <a href="#español">🇪🇸 Español</a> &nbsp;|&nbsp;
  <a href="#english">🇬🇧 English</a>
</p>

---

## Español

Este directorio contiene el único archivo de software necesario para controlar el motor desde MATLAB/Simulink: el modelo `MotorCC_V3.slx`.

**Requisito:** MATLAB con **Instrument Control Toolbox** (ninguna otra toolbox adicional).

---

### Estructura del modelo Simulink

#### Nivel raíz — el lazo de control

El nivel raíz del modelo es el espacio de trabajo del estudiante. Viene precargado con un ejemplo de lazo abierto para verificar el funcionamiento:

```
[Pulse Generator] ──+──→ [Sum] ──→ ┌────────────────┐ ──→ [ángulo θ]  → Scope
                                     │                │
[Constant (50)]  ──-──→             │   Motor_CC     │ ──→ [velocidad ω] → Scope1
                                     └────────────────┘
```

> El estudiante sustituye o amplía este nivel raíz para implementar su propio controlador:
> añade un bloque de referencia (Step, Sine…), el bloque de control (Gain, PID Controller,
> Transfer Fcn discreta…) y la línea de realimentación desde las salidas de `Motor_CC`.

#### El bloque `Motor_CC` — interfaz hardware

`Motor_CC` es un **subsistema con máscara** que encapsula toda la comunicación con el Arduino. El estudiante solo ve sus puertos; en su interior hay tres bloques del **Instrument Control Toolbox**:

```
                        ┌─────────────────────────────────────────────────────┐
                        │                   Motor_CC (máscara)                │
                        │                                                      │
  u ∈ [-255, 255] ─────▶│ Saturation ──▶ ZOH ──▶ int16 ──▶ [Serial Send]    │
                        │                                       │ 'A'|u|'\n'  │
                        │                                       ▼ USB serie   │
                        │                                    Arduino          │
                        │                                       │ 'V'|θ|ω|pwm│'\n'
                        │                                       ▼             │
  θ [°] ◀──────────────│ [Demux] ◀── double ◀── [Serial Receive]            │
  ω [°/s] ◀────────────│                                                      │
  (PWM eco) → Display  │  [Serial Configuration] ← (inicialización)          │
                        └─────────────────────────────────────────────────────┘
```

**Los tres bloques del Instrument Control Toolbox:**

| Bloque | Función | Parámetros clave |
|--------|---------|-----------------|
| **Serial Configuration** | Configura el puerto al iniciar la simulación (prioridad −200 000, se ejecuta antes que todo) | 115 200 baudios, 8N1, little-endian, timeout 10 s |
| **Serial Send** | Envía la acción de control al Arduino cada Ts = 10 ms | Cabecera `'A'`, payload `int16` (2 bytes), terminador `'\n'` |
| **Serial Receive** | Espera y lee el mensaje del Arduino (modo bloqueante) | Cabecera `'V'`, payload `single` × 3 floats (12 bytes), terminador `'\n'`, Ts = 10 ms |

**Cadena de envío** (u → Arduino):

```
u (double) → Saturation[-255,255] → Zero-Order Hold (Ts=0.01 s)
           → Data Type Conversion (double→int16)
           → Serial Send  →  puerto USB  →  Arduino
```

**Cadena de recepción** (Arduino → MATLAB):

```
Arduino  →  puerto USB  →  Serial Receive
         → Data Type Conversion (single→double)
         → Demux (1×3)
              ├─ salida 1: Angle  — ángulo acumulado [°]  → outport
              ├─ salida 2: Speed  — velocidad [°/s]        → outport
              └─ salida 3: PWM   — PWM aplicado (eco)      → Display
```

#### La máscara — selección automática de puerto

Al hacer doble clic en el bloque `Motor_CC`, un **callback `OpenFcn`** escanea los puertos serie disponibles (`serialportlist('available')`) y rellena automáticamente el desplegable. El puerto seleccionado se propaga a los tres bloques de comunicación simultáneamente.

---

### Puertos del bloque `Motor_CC`

| Puerto | Dirección | Señal | Unidades |
|--------|-----------|-------|---------|
| Entrada 1 | IN | Señal de control `u` | adimensional ∈ [−255, 255] |
| Salida 1 | OUT | Ángulo `θ` del eje | ° (acumulado, sin límites) |
| Salida 2 | OUT | Velocidad angular `ω` | °/s |

> **Ángulo acumulado:** el AS5047P entrega ángulo en [0°, 360°) por vuelta.
> El firmware Arduino cuenta las vueltas y acumula, de modo que `θ` crece
> (o decrece) indefinidamente, como un encoder incremental.

---

### Cómo usar el modelo

1. **Conectar el Arduino** por USB y abrir `MotorCC_V3.slx` en MATLAB.
2. **Seleccionar el puerto**: doble clic en `Motor_CC` → seleccionar el puerto del Arduino en el desplegable → OK.
3. **Pulsar Run** (Ctrl+T o botón ▶). La simulación corre en tiempo real a 100 Hz.
4. **Ver las señales** en los Scopes (ángulo y velocidad) durante la ejecución.
5. **Parar** con el botón Stop — siempre con el motor **en reposo** (u = 0).

> Para construir un lazo cerrado: añadir el bloque controlador entre la señal de referencia
> y la entrada `u` de `Motor_CC`, y conectar la salida de ángulo o velocidad de vuelta al
> controlador como realimentación.

---

## English

This directory contains the single software file needed to control the motor from MATLAB/Simulink: the model `MotorCC_V3.slx`.

**Requirement:** MATLAB with **Instrument Control Toolbox** (no other toolboxes needed).

---

### Simulink Model Structure

#### Root level — the control loop

The root level is the student's workspace. It ships with an open-loop test example:

```
[Pulse Generator] ──+──→ [Sum] ──→ ┌────────────────┐ ──→ [angle θ]    → Scope
                                     │                │
[Constant (50)]  ──-──→             │   Motor_CC     │ ──→ [velocity ω] → Scope1
                                     └────────────────┘
```

> The student replaces or extends this level to implement their own controller:
> add a reference block (Step, Sine…), a control block (Gain, PID Controller,
> discrete Transfer Fcn…), and a feedback line from the `Motor_CC` outputs.

#### The `Motor_CC` block — hardware interface

`Motor_CC` is a **masked subsystem** that encapsulates all Arduino communication. Students only see its ports; inside are three **Instrument Control Toolbox** blocks:

**The three Instrument Control Toolbox blocks:**

| Block | Purpose | Key parameters |
|-------|---------|---------------|
| **Serial Configuration** | Configures the port at simulation start (priority −200 000, runs before everything else) | 115 200 baud, 8N1, little-endian, 10 s timeout |
| **Serial Send** | Sends the control action to Arduino every Ts = 10 ms | Header `'A'`, payload `int16` (2 bytes), terminator `'\n'` |
| **Serial Receive** | Waits for and reads Arduino's reply (blocking mode) | Header `'V'`, payload `single` × 3 floats (12 bytes), terminator `'\n'`, Ts = 10 ms |

**Send chain** (u → Arduino):

```
u (double) → Saturation[-255,255] → Zero-Order Hold (Ts=0.01 s)
           → Data Type Conversion (double→int16)
           → Serial Send  →  USB port  →  Arduino
```

**Receive chain** (Arduino → MATLAB):

```
Arduino  →  USB port  →  Serial Receive
         → Data Type Conversion (single→double)
         → Demux (1×3)
              ├─ output 1: Angle  — accumulated angle [°]  → outport
              ├─ output 2: Speed  — angular speed [°/s]     → outport
              └─ output 3: PWM   — applied PWM (echo)       → Display
```

#### The mask — automatic port selection

Double-clicking the `Motor_CC` block triggers an `OpenFcn` callback that scans available serial ports (`serialportlist('available')`) and populates the dropdown automatically. The selected port propagates to all three communication blocks simultaneously.

---

### `Motor_CC` block ports

| Port | Direction | Signal | Units |
|------|-----------|--------|-------|
| Input 1 | IN | Control signal `u` | dimensionless ∈ [−255, 255] |
| Output 1 | OUT | Shaft angle `θ` | ° (accumulated, unbounded) |
| Output 2 | OUT | Angular velocity `ω` | °/s |

---

### How to use the model

1. **Connect the Arduino** via USB and open `MotorCC_V3.slx` in MATLAB.
2. **Select the port**: double-click `Motor_CC` → pick the Arduino port from the dropdown → OK.
3. **Press Run** (Ctrl+T or ▶). The simulation runs in real time at 100 Hz.
4. **View signals** on the Scopes (angle and velocity) during the run.
5. **Stop** with the Stop button — always with the motor **at rest** (u = 0).

> To build a closed loop: add your controller block between the reference signal and the
> `Motor_CC` input, and connect the angle or speed output back to the controller as feedback.
