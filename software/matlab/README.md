# Software MATLAB — `MotorCC_V3.slx`

<p align="center">
  <a href="#español">🇪🇸 Español</a> &nbsp;|&nbsp;
  <a href="#english">🇬🇧 English</a>
</p>

---

## Español

Este directorio contiene el único archivo de software necesario para controlar el motor desde
MATLAB/Simulink: el modelo **`MotorCC_V3.slx`**.

**Requisito único:** MATLAB con **Instrument Control Toolbox**.
No se necesitan otras toolboxes (ni Control System, ni Simulink Real-Time, ni similares).

---

### Estructura del modelo

#### Nivel raíz

<!-- IMAGEN: captura del nivel raíz del modelo (Pulse Generator + Constant + Sum + bloque Motor_CC + los dos Scopes) -->

El nivel raíz es el **espacio de trabajo del estudiante**. Viene precargado con un ejemplo de
lazo abierto para verificar el funcionamiento antes de añadir ningún controlador:

| Bloque | Función en el ejemplo |
|--------|-----------------------|
| **Pulse Generator** | Genera una onda cuadrada de amplitud 50 y periodo 10 s (50 % de ciclo de trabajo) como señal de referencia |
| **Constant** | Valor de offset (50); junto con el Sum forma una referencia bipolar |
| **Sum** (`\|+-`) | Resta el offset a la referencia → señal de control de prueba |
| **Motor_CC** | Bloque con máscara que encapsula toda la comunicación con el Arduino (ver más abajo) |
| **Scope / Scope1** | Muestran en tiempo real el ángulo (°) y la velocidad (°/s) del eje |

> **Para cerrar el lazo:** el estudiante añade su bloque controlador (Gain, PID Controller,
> Transfer Fcn discreta…) entre la señal de referencia y la entrada de `Motor_CC`, y conecta
> la salida de ángulo o velocidad de vuelta al controlador como realimentación.

---

#### El bloque `Motor_CC` — vista exterior

<!-- IMAGEN: captura del bloque Motor_CC en el nivel raíz con el diálogo de máscara abierto, mostrando el desplegable de selección de puerto -->

`Motor_CC` es un **subsistema con máscara** que presenta al exterior una interfaz de un solo
parámetro — el puerto serie del Arduino — y dos salidas: ángulo y velocidad.

Al hacer **doble clic** sobre el bloque se abre el diálogo de máscara con un **desplegable de
puertos disponibles** que se rellena automáticamente en ese momento (ver [Selección automática
de puerto](#selección-automática-de-puerto-el-openFcn)).

| Puerto | Dirección | Señal | Unidades |
|--------|-----------|-------|----------|
| Entrada | IN | Señal de control `u` | adimensional ∈ [−255, 255] |
| Salida 1 | OUT | Ángulo `θ` del eje | ° (acumulado, sin límites) |
| Salida 2 | OUT | Velocidad angular `ω` | °/s |

> **Ángulo acumulado:** el AS5047P entrega ángulo en [0°, 360°) por vuelta. El firmware
> Arduino cuenta las vueltas y acumula, de modo que `θ` crece (o decrece) indefinidamente —
> igual que un encoder incremental — sin saltos ni reinicios.

---

#### Interior del bloque `Motor_CC`

<!-- IMAGEN: captura del interior del subsistema Motor_CC (doble clic → "Look Under Mask") con los tres bloques del Instrument Control Toolbox visibles: Serial Configuration, Serial Send, Serial Receive -->

Dentro del subsistema hay **seis elementos**:

```
Entrada u
    │
    ▼
┌─────────────┐    ┌──────────────────┐
│ Saturation  │───▶│ Zero-Order Hold  │   Ts = 10 ms
│ [−255, 255] │    │   (Ts = 0.01 s)  │
└─────────────┘    └────────┬─────────┘
                            │
                   ┌────────▼─────────┐
                   │ Data Type Conv.  │   double → int16
                   └────────┬─────────┘
                            │
                   ┌────────▼─────────┐    'A' | int16(u) | '\n'
                   │  Serial Send     │ ──────────────────────────▶ Arduino (USB)
                   └──────────────────┘
                                                                         │
                                              'V' | float×3 | '\n'      │
                   ┌──────────────────┐ ◀──────────────────────────────-┘
                   │  Serial Receive  │
                   └────────┬─────────┘
                            │
                   ┌────────▼─────────┐
                   │ Data Type Conv.  │   single → double
                   └────────┬─────────┘
                            │
                   ┌────────▼─────────┐
                   │  Demux (1×3)     │
                   └──┬───┬───┬───────┘
                      │   │   │
                   Angle Speed Display
                   (°)  (°/s) (PWM eco)

┌────────────────────────────────────────┐
│  Serial Configuration (inicialización) │  prioridad −200 000 → se ejecuta antes
└────────────────────────────────────────┘  que cualquier otro bloque
```

---

### Parámetros de los bloques del Instrument Control Toolbox

A continuación se detallan todos los parámetros que hay que configurar al crear o modificar
cada bloque. Estos valores son los que están guardados en `MotorCC_V3.slx`.

#### Serial Configuration

<!-- IMAGEN: captura del diálogo de configuración del bloque Serial Configuration -->

| Parámetro | Valor | Descripción |
|-----------|-------|-------------|
| **Port** | *(seleccionado por la máscara)* | Puerto serie del Arduino; se propaga automáticamente desde el desplegable del bloque `Motor_CC` |
| **Baud rate** | `115200` | Velocidad de comunicación; debe coincidir con el `Serial.begin(115200)` del firmware Arduino |
| **Data bits** | `8` | 8 bits por carácter (estándar) |
| **Parity** | `none` | Sin bit de paridad |
| **Stop bits** | `1` | Un bit de parada |
| **Byte order** | `little-endian` | El Arduino (arquitectura AVR/ARM) trabaja en little-endian; los `float` y `int16` se envían con el byte menos significativo primero |
| **Flow control** | `none` | Sin control de flujo hardware |
| **Timeout** | `10` s | Tiempo máximo de espera antes de error |
| **Priority** | `−200 000` | Valor negativo muy alto: garantiza que este bloque se inicialice antes que `Serial Send` y `Serial Receive`, que lo necesitan ya configurado |

#### Serial Send

<!-- IMAGEN: captura del diálogo del bloque Serial Send -->

| Parámetro | Valor | Descripción |
|-----------|-------|-------------|
| **Port** | *(mismo que Serial Configuration)* | Se propaga desde la máscara |
| **Header** | `'A'` | Byte de cabecera; el firmware Arduino espera el carácter `'A'` para saber que llega un dato de control |
| **Terminator** | `LF ('\n')` | Byte de fin de mensaje; el firmware usa `readStringUntil('\n')` para detectar el final |
| **Enable blocking mode** | `on` | La ejecución de Simulink espera a que el byte se haya enviado antes de continuar — evita desbordamiento del buffer serie |
| **Priority** | `1` | Se ejecuta después de `Serial Configuration` (que tiene prioridad −200 000) |

**Cadena de datos enviada al Arduino:**

```
'A'  (1 byte, cabecera)
 u   (2 bytes, int16, little-endian)  ← señal de control ∈ [−255, 255]
'\n' (1 byte, terminador)
─────────────────────────────────────
Total: 4 bytes por ciclo (100 Hz → 400 bytes/s, muy por debajo de la capacidad del puerto)
```

#### Serial Receive

<!-- IMAGEN: captura del diálogo del bloque Serial Receive -->

| Parámetro | Valor | Descripción |
|-----------|-------|-------------|
| **Port** | *(mismo que Serial Configuration)* | Se propaga desde la máscara |
| **Data type** | `single` | El Arduino envía los floats como `float` IEEE-754 de 32 bits; en MATLAB eso es `single` |
| **Header** | `'V'` | Byte de cabecera esperado; el firmware envía `'V'` antes de cada paquete de datos |
| **Data size** | `[1 3]` | Vector fila de 3 elementos: `[ángulo, velocidad, PWM_aplicado]` |
| **Terminator** | `LF ('\n')` | Fin de mensaje; el receptor busca `'\n'` para saber que el paquete está completo |
| **Input format** | `Row major` | Los 3 floats se reciben como una fila, en el orden en que el Arduino los escribe |
| **Enable blocking mode** | `on` | Simulink espera hasta recibir el paquete completo antes de avanzar al siguiente paso — mantiene el sincronismo a 100 Hz |
| **Action when data unavailable** | `Output last received value` | Si un ciclo no llega a tiempo, se repite el último valor válido en lugar de emitir un error |
| **Sample time** | `0.01` s | Ts = 10 ms → 100 Hz, igual que el Timer2 del Arduino |
| **Priority** | `1` | Mismo nivel que Serial Send |

**Paquete recibido desde el Arduino:**

```
'V'       (1 byte, cabecera)
ángulo    (4 bytes, float32 IEEE-754, little-endian)   [°]  acumulado
velocidad (4 bytes, float32 IEEE-754, little-endian)   [°/s]
PWM       (4 bytes, float32 IEEE-754, little-endian)   valor aplicado (eco)
'\n'      (1 byte, terminador)
──────────────────────────────────────────────────────────────────────
Total: 14 bytes por ciclo (100 Hz → 1 400 bytes/s)
```

---

### Selección automática de puerto — el `OpenFcn`

Este es el código más delicado del modelo. Cuando se hace doble clic en el bloque `Motor_CC`,
**antes de abrir el diálogo de máscara**, Simulink ejecuta el siguiente callback:

```matlab
% Escanear puertos serie disponibles al abrir el diálogo
try
    availablePorts = cellstr(serialportlist('available'));
catch
    serialInfo = instrhwinfo('serial');
    availablePorts = serialInfo.AvailableSerialPorts;
end
if isempty(availablePorts)
    availablePorts = {'No Ports Available'};
end
maskObj = Simulink.Mask.get(gcb);
portParam = maskObj.getParameter('PortName');
portParam.TypeOptions = availablePorts;
currentPort = get_param(gcb, 'PortName');
if ~ismember(currentPort, availablePorts)
    set_param(gcb, 'PortName', availablePorts{1});
end

open_system(gcb, 'mask');
```

**Explicación línea a línea:**

| Bloque de código | Por qué está ahí |
|-----------------|-----------------|
| `try … serialportlist('available') … catch … instrhwinfo` | `serialportlist` es la API moderna (R2020b en adelante). En versiones anteriores no existe y lanza un error; el `catch` cae a la API antigua `instrhwinfo` para garantizar compatibilidad hacia atrás |
| `if isempty(availablePorts)` | Si no hay ningún Arduino conectado, el desplegable no puede quedar vacío (Simulink lo rechaza); se rellena con el texto `'No Ports Available'` como aviso |
| `maskObj.getParameter('PortName').TypeOptions = availablePorts` | Actualiza **en tiempo real** las opciones del desplegable con los puertos detectados en ese momento — así el usuario siempre ve la lista actual, no la que había cuando se guardó el modelo |
| `if ~ismember(currentPort, availablePorts)` | Si el puerto guardado en el modelo ya no existe (porque se desenchufó el Arduino o cambió el nombre del puerto), selecciona automáticamente el primero disponible en lugar de dejar una opción inválida |
| `open_system(gcb, 'mask')` | Abre el diálogo de máscara propiamente dicho; esta línea es obligatoria porque el `OpenFcn` sustituye al comportamiento por defecto — sin ella el doble clic no abriría nada |

#### Por qué el puerto no se propaga automáticamente

Este fue el punto más costoso de resolver. Los bloques del Instrument Control Toolbox
(`Serial Configuration`, `Serial Send`, `Serial Receive`) guardan el puerto en **tres nombres
de parámetro distintos** según la versión de MATLAB (`PortSel`, `Port` y `ComPort`), y
**ninguno de ellos acepta directamente una variable del workspace de la máscara** — a
diferencia de bloques nativos de Simulink que sí resuelven variables de máscara en sus campos.

La solución es que el propio `OpenFcn` hace el `set_param` sobre el parámetro de máscara
(`PortName`), y cuando el usuario pulsa OK en el diálogo, Simulink propaga ese valor a los
bloques interiores a través de la configuración de la máscara. Para que esta propagación sea
fiable en todas las versiones fue necesario nombrar el parámetro de máscara exactamente igual
que el campo que los bloques ICT esperan, y validar el comportamiento en cada versión de
MATLAB disponible.

---

### Cómo usar el modelo

1. Conectar el Arduino por USB.
2. Abrir `MotorCC_V3.slx` en MATLAB/Simulink.
3. **Seleccionar el puerto**: hacer doble clic en el bloque `Motor_CC` → el desplegable se
   rellena automáticamente con los puertos disponibles → seleccionar el que corresponde al
   Arduino → OK.
4. Pulsar **Run** (Ctrl+T o botón ▶). La simulación corre en tiempo real a 100 Hz.
5. Observar ángulo y velocidad en los Scopes durante la ejecución.
6. **Parar siempre con el motor en reposo** (señal de control `u = 0`) antes de pulsar Stop.
   Si se para con el motor en movimiento, el driver mantiene el último PWM y el motor sigue
   girando sin control.

---

## English

This directory contains the single software file needed to control the motor from
MATLAB/Simulink: **`MotorCC_V3.slx`**.

**Single requirement:** MATLAB with the **Instrument Control Toolbox**.
No other toolboxes needed (no Control System, Simulink Real-Time, or similar).

---

### Model structure

#### Root level

<!-- IMAGE: screenshot of the model root level (Pulse Generator + Constant + Sum + Motor_CC block + two Scopes) -->

The root level is the **student's workspace**. It ships with an open-loop test example to
verify communication before adding any controller:

| Block | Role in the example |
|-------|---------------------|
| **Pulse Generator** | Square wave, amplitude 50, period 10 s, 50 % duty cycle — test reference signal |
| **Constant** | Offset value (50); forms a bipolar reference together with Sum |
| **Sum** (`\|+-`) | Subtracts the offset from the reference → test control signal |
| **Motor_CC** | Masked subsystem encapsulating all Arduino communication (see below) |
| **Scope / Scope1** | Real-time display of shaft angle (°) and velocity (°/s) |

> **To close the loop:** the student adds their controller block (Gain, PID Controller,
> discrete Transfer Fcn…) between the reference signal and `Motor_CC`'s input, and connects
> the angle or speed output back to the controller as feedback.

---

#### The `Motor_CC` block — external view

<!-- IMAGE: screenshot of the Motor_CC block at root level with the mask dialog open, showing the port selection dropdown -->

`Motor_CC` is a **masked subsystem** that presents a single-parameter interface to the
outside — the Arduino serial port — and two outputs: angle and velocity.

**Double-clicking** the block opens the mask dialog with an **auto-populated port dropdown**
(see [Automatic port selection — the OpenFcn](#automatic-port-selection--the-openfcn)).

| Port | Direction | Signal | Units |
|------|-----------|--------|-------|
| Input | IN | Control signal `u` | dimensionless ∈ [−255, 255] |
| Output 1 | OUT | Shaft angle `θ` | ° (accumulated, unbounded) |
| Output 2 | OUT | Angular velocity `ω` | °/s |

> **Accumulated angle:** the AS5047P delivers angle in [0°, 360°) per revolution. The Arduino
> firmware counts full revolutions and accumulates, so `θ` grows (or decreases) indefinitely —
> exactly like an incremental encoder — with no jumps or resets.

---

#### Inside the `Motor_CC` block

<!-- IMAGE: screenshot of the Motor_CC subsystem interior (double-click → "Look Under Mask") showing the three Instrument Control Toolbox blocks: Serial Configuration, Serial Send, Serial Receive -->

Inside the subsystem there are **six elements**:

**Send path** — u → Arduino:
```
u → Saturation[−255,255] → Zero-Order Hold (Ts=0.01 s) → int16 conversion
  → Serial Send → 'A' | int16(u) | '\n' → Arduino over USB
```

**Receive path** — Arduino → MATLAB:
```
Arduino → 'V' | float×3 | '\n' → Serial Receive
        → single→double conversion → Demux(1×3)
             ├─ Output 1: Angle  [°]
             ├─ Output 2: Speed  [°/s]
             └─ Display:  PWM echo
```

**Initialization:**
```
Serial Configuration — priority −200,000 → runs before any other block
```

---

### Instrument Control Toolbox block parameters

#### Serial Configuration

<!-- IMAGE: screenshot of the Serial Configuration block dialog -->

| Parameter | Value | Notes |
|-----------|-------|-------|
| **Port** | *(set by mask)* | Arduino serial port; propagated automatically from the `Motor_CC` dropdown |
| **Baud rate** | `115200` | Must match `Serial.begin(115200)` in the Arduino firmware |
| **Data bits** | `8` | Standard 8-bit framing |
| **Parity** | `none` | No parity bit |
| **Stop bits** | `1` | One stop bit |
| **Byte order** | `little-endian` | AVR/ARM is little-endian; floats and int16 are sent LSB first |
| **Flow control** | `none` | No hardware flow control |
| **Timeout** | `10` s | Maximum wait before error |
| **Priority** | `−200,000` | Very high negative priority ensures this block initialises before Serial Send and Serial Receive, which depend on it |

#### Serial Send

<!-- IMAGE: screenshot of the Serial Send block dialog -->

| Parameter | Value | Notes |
|-----------|-------|-------|
| **Port** | *(same as Serial Configuration)* | Propagated from mask |
| **Header** | `'A'` | Arduino firmware waits for `'A'` as the control-data start marker |
| **Terminator** | `LF ('\n')` | Firmware uses `readStringUntil('\n')` to detect end of message |
| **Enable blocking mode** | `on` | Simulink waits for the byte to be transmitted before proceeding — prevents serial buffer overflow |

**Packet sent to Arduino — 4 bytes per cycle at 100 Hz:**
```
'A'   1 byte  — header
 u    2 bytes — int16, little-endian, ∈ [−255, 255]
'\n'  1 byte  — terminator
```

#### Serial Receive

<!-- IMAGE: screenshot of the Serial Receive block dialog -->

| Parameter | Value | Notes |
|-----------|-------|-------|
| **Port** | *(same as Serial Configuration)* | Propagated from mask |
| **Data type** | `single` | Arduino sends IEEE-754 32-bit floats; MATLAB `single` matches exactly |
| **Header** | `'V'` | Firmware sends `'V'` before each data packet |
| **Data size** | `[1 3]` | Row vector of 3 elements: [angle, speed, PWM_echo] |
| **Terminator** | `LF ('\n')` | Marks end of packet |
| **Input format** | `Row major` | 3 floats received in the order the Arduino writes them |
| **Enable blocking mode** | `on` | Simulink waits for the full packet before advancing — maintains 100 Hz synchronisation |
| **Action when data unavailable** | `Output last received value` | If a cycle arrives late, the last valid value is repeated instead of raising an error |
| **Sample time** | `0.01` s | Ts = 10 ms = 100 Hz, matches the Arduino Timer2 interrupt |

**Packet received from Arduino — 14 bytes per cycle at 100 Hz:**
```
'V'        1 byte  — header
angle      4 bytes — float32, little-endian  [°]
speed      4 bytes — float32, little-endian  [°/s]
PWM_echo   4 bytes — float32, little-endian  (applied PWM, echoed back)
'\n'       1 byte  — terminator
```

---

### Automatic port selection — the `OpenFcn`

When the user double-clicks `Motor_CC`, **before the mask dialog opens**, Simulink runs this
callback:

```matlab
% Scan available serial ports when the dialog opens
try
    availablePorts = cellstr(serialportlist('available'));
catch
    serialInfo = instrhwinfo('serial');
    availablePorts = serialInfo.AvailableSerialPorts;
end
if isempty(availablePorts)
    availablePorts = {'No Ports Available'};
end
maskObj = Simulink.Mask.get(gcb);
portParam = maskObj.getParameter('PortName');
portParam.TypeOptions = availablePorts;
currentPort = get_param(gcb, 'PortName');
if ~ismember(currentPort, availablePorts)
    set_param(gcb, 'PortName', availablePorts{1});
end

open_system(gcb, 'mask');
```

**Line-by-line explanation:**

| Code | Why it is there |
|------|----------------|
| `try … serialportlist … catch … instrhwinfo` | `serialportlist` is the modern API (R2020b+). Older MATLAB versions do not have it and throw an error; the `catch` falls back to the legacy `instrhwinfo` API for backwards compatibility |
| `if isempty(availablePorts)` | If no Arduino is connected, the dropdown cannot be left empty (Simulink rejects it); a placeholder string is inserted as a warning |
| `portParam.TypeOptions = availablePorts` | Updates the dropdown options **live** with the ports currently connected — the user always sees the real list, not whatever was saved with the model |
| `if ~ismember(currentPort, availablePorts)` | If the saved port no longer exists (Arduino unplugged or OS renamed it), the first available port is selected automatically instead of leaving an invalid option |
| `open_system(gcb, 'mask')` | Opens the mask dialog itself — this line is **mandatory** because the `OpenFcn` replaces the default double-click behaviour; without it nothing would open |

#### Why the port cannot propagate automatically

This was the trickiest part of building the model. The Instrument Control Toolbox blocks
(`Serial Configuration`, `Serial Send`, `Serial Receive`) store the port name under **three
different parameter names** depending on the MATLAB version (`PortSel`, `Port`, `ComPort`),
and **none of them resolves mask workspace variables** the way native Simulink blocks do.

The solution: the `OpenFcn` updates the mask's `PortName` parameter, and when the user clicks
OK, Simulink propagates that value to the interior blocks through the mask configuration. Making
this propagation reliable across MATLAB versions required naming the mask parameter exactly as
the ICT blocks expect, and validating the behaviour on each available version.
