# MATLAB / Simulink — MotorCC V3

[Español](#español) · [English](#english)

## Español

Este directorio contiene los archivos de software necesarios para controlar el motor desde
MATLAB/Simulink. El modelo **`MotorCC_V3`** está disponible en ocho versiones exportadas,
para las versiones indicadas a continuación. También se conservan archivos V2; esta guía describe **V3**.

Para el uso habitual, consulta la [guía rápida](../../docs/manual/manual_usuario_isa.pdf). Esta página documenta el interior del modelo y cómo reproducir su máscara.

**Requisitos:** MATLAB con Simulink y **Instrument Control Toolbox como única toolbox adicional**.
No se necesitan otras toolboxes (ni Control System, ni Simulink Real-Time, ni similares).

### Versiones disponibles

Selecciona el archivo exportado para tu versión de MATLAB:

| Archivo | Versión de destino |
|---|---|
| [`MotorCC_V3_R2025b.slx`](MotorCC_V3_R2025b.slx) | R2025b |
| [`MotorCC_V3_R2024b.slx`](MotorCC_V3_R2024b.slx) | R2024b |
| [`MotorCC_V3_R2024a.slx`](MotorCC_V3_R2024a.slx) | R2024a |
| [`MotorCC_V3_R2023b.slx`](MotorCC_V3_R2023b.slx) | R2023b |
| [`MotorCC_V3_R2023a.slx`](MotorCC_V3_R2023a.slx) | R2023a |
| [`MotorCC_V3_R2022b.slx`](MotorCC_V3_R2022b.slx) | R2022b |
| [`MotorCC_V3_R2021b.slx`](MotorCC_V3_R2021b.slx) | R2021b |
| [`MotorCC_V3_R2020b.slx`](MotorCC_V3_R2020b.slx) | R2020b |

Las exportaciones están incluidas en el repositorio. Su presencia no equivale a una comprobación con el equipo físico en cada versión.

> **Archivo de trabajo:** `MotorCC_V3_R2025b.slx` es el modelo fuente. Cualquier modificación
> debe hacerse sobre él y luego propagarse a las versiones anteriores con `exportar_versiones.m`
> (ver [Exportar a otras versiones](#exportar-a-otras-versiones)).

---

### Estructura del modelo

#### Nivel raíz

![Vista del nivel raíz — Pulse Generator, Constant, Sum, bloque Motor_CC y los dos Scopes](../../docs/images/matlab/01_root_level.png)

El nivel raíz es el **espacio de trabajo del estudiante**. Viene precargado con un ejemplo de
lazo abierto para verificar el funcionamiento antes de añadir ningún controlador:

| Bloque | Función en el ejemplo |
|--------|-----------------------|
| **Pulse Generator** | Genera pulsos entre 0 y 50, de periodo 10 s y ciclo de trabajo del 50 % |
| **Constant** | Offset de 50, que se resta a la salida del generador |
| **Sum** (`\|+-`) | Resta 50 a los pulsos → señal de prueba entre −50 y 0 |
| **Motor_CC** | Bloque con máscara que encapsula toda la comunicación con el Arduino (ver más abajo) |
| **Scope / Scope1** | Muestran en tiempo real el ángulo (°) y la velocidad (°/s) del eje |

> **Para cerrar el lazo:** el estudiante añade su bloque controlador (Gain, PID Controller,
> Transfer Fcn discreta…) entre la señal de referencia y la entrada de `Motor_CC`, y conecta
> la salida de ángulo o velocidad de vuelta al controlador como realimentación.

---

#### El bloque `Motor_CC` — vista exterior

![Bloque Motor_CC en el nivel raíz con el diálogo de máscara abierto, mostrando el desplegable de selección de puerto](../../docs/images/matlab/02_motor_cc_mask_dialog.png)

`Motor_CC` es un **subsistema con máscara** que presenta al exterior una interfaz de un solo
parámetro — el puerto serie del Arduino — y dos salidas: ángulo y velocidad.

Al hacer **doble clic** sobre el bloque se abre el diálogo de máscara con un **desplegable de
puertos disponibles** que se rellena automáticamente en ese momento (ver
[Cómo recrear la máscara actual](#cómo-recrear-la-máscara-actual)).

| Puerto | Dirección | Señal | Unidades |
|--------|-----------|-------|----------|
| Entrada | IN | Señal de control `u` | adimensional ∈ [−255, 255] |
| Salida 1 | OUT | Ángulo `θ` del eje | ° (acumulado durante la ejecución del firmware) |
| Salida 2 | OUT | Velocidad angular `ω` | °/s |

> **Ángulo acumulado:** el firmware corrige los saltos de vuelta del encoder y acumula la posición. No guarda el número de vueltas al reiniciar Arduino. La velocidad se calcula por diferencia de posición cada 10 ms; las salidas enviadas no son las estimaciones del observador.

---

#### Interior del bloque `Motor_CC`

![Interior del subsistema Motor_CC — clic derecho → "Mask → Look Under Mask" — con los tres bloques del Instrument Control Toolbox: Serial Configuration, Serial Send, Serial Receive](../../docs/images/matlab/03_motor_cc_interior.png)

Para ver el interior, usa **clic derecho → Mask → Look Under Mask**. El recorrido de envío es entrada → saturación [−255, 255] → retención de orden cero (`0.01` s) → conversión a `int16` → Serial Send.

Serial Receive entrega tres `single`, que se convierten a `double` y se separan con Demux: ángulo y velocidad salen del subsistema; el eco del PWM se muestra en un Display interior. Serial Configuration configura el enlace. El diagrama resume este flujo:

![Diagrama del interior del bloque Motor_CC — flujo de señales y comunicación serie](../../docs/images/matlab/diagram_motor_cc_es.png)

---

### Parámetros de los bloques del Instrument Control Toolbox

A continuación se detallan todos los parámetros que hay que configurar al crear o modificar
cada bloque. Estos valores se han contrastado con `MotorCC_V3_R2025b.slx`. Las capturas son orientativas; para reproducir el modelo, usa los valores y el código de este README.

#### Serial Configuration

![Diálogo de configuración del bloque Serial Configuration](../../docs/images/matlab/04_serial_configuration.png)

| Parámetro | Valor | Descripción |
|-----------|-------|-------------|
| **Port** | *(seleccionado por la máscara)* | Puerto serie del Arduino; se propaga automáticamente desde el desplegable del bloque `Motor_CC` |
| **Baud rate** | `115200` | Velocidad de comunicación; debe coincidir con el `Serial.begin(115200)` del firmware Arduino |
| **Data bits** | `8` | 8 bits por carácter (estándar) |
| **Parity** | `none` | Sin bit de paridad |
| **Stop bits** | `1` | Un bit de parada |
| **Byte order** | `little-endian` | El Arduino UNO utilizado trabaja en little-endian; los `float` y `int16` se envían con el byte menos significativo primero |
| **Flow control** | `none` | Sin control de flujo hardware |
| **Timeout** | `10` s | Tiempo máximo de espera antes de error |
| **Priority** | `−200 000` | Valor negativo muy alto: garantiza que este bloque se inicialice antes que `Serial Send` y `Serial Receive`, que lo necesitan ya configurado |

#### Serial Send

![Diálogo del bloque Serial Send](../../docs/images/matlab/05_serial_send.png)

| Parámetro | Valor | Descripción |
|-----------|-------|-------------|
| **Port** | *(mismo que Serial Configuration)* | Se propaga desde la máscara |
| **Header** | `'A'` | Byte de cabecera; el firmware Arduino espera el carácter `'A'` para saber que llega un dato de control |
| **Terminator** | `LF ('\n')` | Byte de fin de mensaje; `recepcion()` lee los bytes mediante `Serial.read()` y reconoce `'\n'` |
| **Enable blocking mode** | `on` | Envío configurado en modo bloqueante |
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

![Diálogo del bloque Serial Receive](../../docs/images/matlab/06_serial_receive.png)

| Parámetro | Valor | Descripción |
|-----------|-------|-------------|
| **Port** | *(mismo que Serial Configuration)* | Se propaga desde la máscara |
| **Data type** | `single` | El Arduino envía los floats como `float` IEEE-754 de 32 bits; en MATLAB eso es `single` |
| **Header** | `'V'` | Byte de cabecera esperado; el firmware envía `'V'` antes de cada paquete de datos |
| **Data size** | `[1 3]` | Vector fila de 3 elementos: `[ángulo, velocidad, PWM_aplicado]` |
| **Terminator** | `LF ('\n')` | Fin de mensaje; el receptor busca `'\n'` para saber que el paquete está completo |
| **Input format** | `Row major` | Los 3 floats se reciben como una fila, en el orden en que el Arduino los escribe |
| **Enable blocking mode** | `on` | Recepción configurada en modo bloqueante, con periodo de muestreo de 0,01 s |
| **Action when data unavailable** | `Output last received value` | Acción guardada para datos no disponibles; el modo bloqueante y el timeout también condicionan la espera |
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

### Cómo usar el modelo

1. Conectar el Arduino por USB.
2. Abrir el archivo `MotorCC_V3_Rxxxx.slx` correspondiente a tu versión de MATLAB.
3. **Seleccionar el puerto**: hacer doble clic en el bloque `Motor_CC` → el desplegable se
   rellena automáticamente con los puertos disponibles → seleccionar el que corresponde al
   Arduino → OK.
4. Pulsar **Run** (Ctrl+T o botón ▶). El muestreo está configurado a 0,01 s; la comunicación depende de la ejecución en el PC y no constituye una garantía de tiempo real estricto.
5. Observar ángulo y velocidad en los Scopes durante la ejecución.
6. **Parar la simulación**: pulsa **Stop** y espera a que el motor se detenga. Los modelos V3 guardados no contienen un callback `StopFcn`. El firmware comienza a reducir el PWM tras más de **1 s** sin recibir órdenes, en pasos de **2 unidades cada 10 ms** hasta cero (hasta unos 1,28 s adicionales desde |PWM| = 255).

El modelo V3 suministrado aplica una señal de prueba en **lazo abierto**; no tiene un regulador realimentado conectado. Arduino envía posición, velocidad y el valor de PWM, y Simulink muestra las dos primeras señales en los Scopes.

---

### Exportar a otras versiones

El archivo de trabajo es `MotorCC_V3_R2025b.slx`. Guarda los cambios antes de ejecutar este script desde MATLAB R2025b:

```matlab
cd('<ruta>/Motor-CC-Control-Lab/software/matlab')
run('exportar_versiones.m')
```

El script carga el modelo fuente y utiliza `save_system` con `ExportToVersion` y el destino `<release>_SLX`. Sobrescribe las siete exportaciones V3 de su lista y comunica los errores de exportación. No es necesario ejecutarlo para usar los modelos ya exportados. La configuración actual de parada utiliza el watchdog del firmware; no hay un `StopFcn` guardado en los modelos V3.

### Cómo recrear la máscara actual

Los siguientes tres fragmentos se han extraído **literalmente del modelo guardado**, incluidos sus comentarios en inglés. Son iguales en las ocho exportaciones V3. Las capturas anteriores pueden mostrar código antiguo: no deben sustituir a estos fragmentos.

#### 1. Crear el parámetro del desplegable

Sobre el subsistema `Motor_CC`, abre **Mask → Create Mask** si aún no tiene máscara, o **Mask → Edit Mask** para modificarla. En **Parameters & Dialog**, añade un **Popup** y configura:

| Propiedad | Valor |
|---|---|
| Name | `PortName` |
| Prompt | `Arduino Port:` |
| Type | `popup` |
| Evaluate | `off` |
| Tunable | `off` |
| Type options iniciales | `Select a serial port` |
| Value inicial | `Select a serial port` |

El modelo puede guardar un puerto concreto de la última sesión. Para una máscara nueva, el texto inicial anterior obliga al usuario a elegirlo. No se cambia el puerto durante la simulación.

#### 2. Inicialización de la máscara: propagar el puerto

En el **Mask Editor → Code → Initialization**, introduce:

```matlab
% Pass the chosen port to all three Instrument Control Toolbox blocks.
block = gcb;
selected = get_param(block, 'PortName');
if ~ismember(selected, {'Select a serial port', 'No serial ports available'})
    set_param([block '/Serial Configuration'], 'Port', selected);
    set_param([block '/Serial Send'], 'Port', selected);
    set_param([block '/Serial Receive'], 'Port', selected);
end
```

Este código copia el texto de `PortName` al parámetro `Port` de los tres bloques serie. Los nombres `Serial Configuration`, `Serial Send` y `Serial Receive` deben coincidir con los bloques interiores. `PortName` es el nombre elegido por este código, no una variable que los bloques serie detecten por sí solos.

#### 3. Callback de bloque `OpenFcn`: actualizar la lista y abrir la máscara

En **Block Properties → Callbacks → OpenFcn** del subsistema, introduce:

```matlab
% Refresh the list before showing the mask.
block = gcb;
if strcmp(get_param(bdroot(block), 'SimulationStatus'), 'stopped')
    ports = serialportlist('available');
    if isempty(ports)
        options = {'No serial ports available'};
    else
        options = [{'Select a serial port'}, cellstr(ports(:).')];
    end
    mask = Simulink.Mask.get(block);
    port = mask.getParameter('PortName');
    selected = get_param(block, 'PortName');
    port.TypeOptions = options;
    if ~ismember(selected, options)
        set_param(block, 'PortName', options{1});
    end
end
open_system(block, 'mask');
```

Con la simulación detenida, consulta `serialportlist('available')` y actualiza las opciones. Conserva el puerto guardado si aún está disponible; si no, deja el aviso `Select a serial port`. Si no hay puertos disponibles, muestra `No serial ports available`. No identifica automáticamente cuál pertenece a Arduino ni selecciona el primer puerto real. La última línea abre la máscara.

#### 4. Callback de bloque `InitFcn`: impedir el arranque sin selección

En **Block Properties → Callbacks → InitFcn**, introduce:

```matlab
if ismember(get_param(gcb, 'PortName'), {'Select a serial port', 'No serial ports available'})
    error('MotorCC:NoSerialPort', 'Connect the Arduino and select its serial port in Motor_CC before running.');
end
```

Este callback rechaza los dos textos de aviso antes de ejecutar el modelo. No comprueba la identidad del dispositivo ni garantiza que el puerto siga disponible después de seleccionarlo.

#### 5. Guardar y comprobar

Guarda la máscara y el modelo. Con la simulación detenida, conecta Arduino, abre la máscara, selecciona su puerto y pulsa **OK**. La inicialización propaga el puerto a los tres bloques interiores. Ejecuta la prueba y comprueba las medidas. Si modificas el archivo fuente R2025b, vuelve a exportar las versiones anteriores.

La revisión de este README contrasta los valores y callbacks almacenados en los `.slx`; la prueba de comunicación con el equipo y con cada versión de MATLAB debe realizarse en el laboratorio.

---

## English

This directory contains the software files needed to control the motor from
MATLAB/Simulink. The **`MotorCC_V3`** model is available in eight exported versions,
for the releases listed below. V2 files are also retained; this guide describes **V3**.

For everyday use, see the [quick start guide](../../docs/manual/user_manual_isa.pdf). This page documents the model internals and how to reproduce its mask.

**Requirements:** MATLAB with Simulink and **Instrument Control Toolbox as the only additional toolbox**.
No other toolboxes needed (no Control System, Simulink Real-Time, or similar).

### Available versions

Select the file exported for your MATLAB release:

| File | Target release |
|---|---|
| [`MotorCC_V3_R2025b.slx`](MotorCC_V3_R2025b.slx) | R2025b |
| [`MotorCC_V3_R2024b.slx`](MotorCC_V3_R2024b.slx) | R2024b |
| [`MotorCC_V3_R2024a.slx`](MotorCC_V3_R2024a.slx) | R2024a |
| [`MotorCC_V3_R2023b.slx`](MotorCC_V3_R2023b.slx) | R2023b |
| [`MotorCC_V3_R2023a.slx`](MotorCC_V3_R2023a.slx) | R2023a |
| [`MotorCC_V3_R2022b.slx`](MotorCC_V3_R2022b.slx) | R2022b |
| [`MotorCC_V3_R2021b.slx`](MotorCC_V3_R2021b.slx) | R2021b |
| [`MotorCC_V3_R2020b.slx`](MotorCC_V3_R2020b.slx) | R2020b |

The exported files are included in the repository. Their presence does not imply hardware validation on every release.

> **Working file:** `MotorCC_V3_R2025b.slx` is the source model. Any changes must be made
> there first and then propagated to older versions with `exportar_versiones.m`
> (see [Exporting to other releases](#exporting-to-other-releases)).

---

### Model structure

#### Root level

![Root level view — Pulse Generator, Constant, Sum, Motor_CC block and two Scopes](../../docs/images/matlab/01_root_level.png)

The root level is the **student's workspace**. It ships with an open-loop test example to
verify communication before adding any controller:

| Block | Role in the example |
|-------|---------------------|
| **Pulse Generator** | Pulses between 0 and 50, period 10 s, 50 % duty cycle |
| **Constant** | Offset of 50, subtracted from the pulse generator output |
| **Sum** (`\|+-`) | Subtracts 50 from the pulses → test command between −50 and 0 |
| **Motor_CC** | Masked subsystem encapsulating all Arduino communication (see below) |
| **Scope / Scope1** | Real-time display of shaft angle (°) and velocity (°/s) |

> **To close the loop:** the student adds their controller block (Gain, PID Controller,
> discrete Transfer Fcn…) between the reference signal and `Motor_CC`'s input, and connects
> the angle or speed output back to the controller as feedback.

---

#### The `Motor_CC` block — external view

![Motor_CC block at root level with the mask dialog open, showing the port selection dropdown](../../docs/images/matlab/02_motor_cc_mask_dialog.png)

`Motor_CC` is a **masked subsystem** that presents a single-parameter interface to the
outside — the Arduino serial port — and two outputs: angle and velocity.

**Double-clicking** the block opens the mask dialog with an **auto-populated port dropdown**
(see [Recreating the current mask](#recreating-the-current-mask)).

| Port | Direction | Signal | Units |
|------|-----------|--------|-------|
| Input | IN | Control signal `u` | dimensionless ∈ [−255, 255] |
| Output 1 | OUT | Shaft angle `θ` | ° (accumulated during firmware execution) |
| Output 2 | OUT | Angular velocity `ω` | °/s |

> **Accumulated angle:** the firmware corrects encoder wraparound and accumulates position. The revolution count is not retained when Arduino resets. Velocity is computed from position differences every 10 ms; the transmitted outputs are not the observer estimates.

---

#### Inside the `Motor_CC` block

![Motor_CC subsystem interior — right-click → "Mask → Look Under Mask" — showing the three Instrument Control Toolbox blocks: Serial Configuration, Serial Send, Serial Receive](../../docs/images/matlab/03_motor_cc_interior.png)

To inspect the subsystem, use **right-click → Mask → Look Under Mask**. The transmit path is input → saturation [−255, 255] → zero-order hold (`0.01` s) → `int16` conversion → Serial Send.

Serial Receive outputs three `single` values, converted to `double` and split by Demux: angle and speed leave the subsystem; the PWM echo is shown on an internal Display. Serial Configuration configures the link. The diagram summarizes this flow:

![Inside the Motor_CC block — signal flow and serial communication with Arduino](../../docs/images/matlab/diagram_motor_cc_en.png)

---

### Instrument Control Toolbox block parameters

These values were checked against `MotorCC_V3_R2025b.slx`. Screenshots are illustrative; use the values and code in this README when recreating the model.

#### Serial Configuration

![Serial Configuration block dialog](../../docs/images/matlab/04_serial_configuration.png)

| Parameter | Value | Notes |
|-----------|-------|-------|
| **Port** | *(set by mask)* | Arduino serial port; propagated automatically from the `Motor_CC` dropdown |
| **Baud rate** | `115200` | Must match `Serial.begin(115200)` in the Arduino firmware |
| **Data bits** | `8` | Standard 8-bit framing |
| **Parity** | `none` | No parity bit |
| **Stop bits** | `1` | One stop bit |
| **Byte order** | `little-endian` | The Arduino UNO used here is little-endian; floats and int16 are sent LSB first |
| **Flow control** | `none` | No hardware flow control |
| **Timeout** | `10` s | Maximum wait before error |
| **Priority** | `−200,000` | Very high negative priority ensures this block initialises before Serial Send and Serial Receive, which depend on it |

#### Serial Send

![Serial Send block dialog](../../docs/images/matlab/05_serial_send.png)

| Parameter | Value | Notes |
|-----------|-------|-------|
| **Port** | *(same as Serial Configuration)* | Propagated from mask |
| **Header** | `'A'` | Arduino firmware waits for `'A'` as the control-data start marker |
| **Terminator** | `LF ('\n')` | `recepcion()` reads bytes with `Serial.read()` and recognizes `'\n'` |
| **Enable blocking mode** | `on` | Transmission is configured in blocking mode |
| **Priority** | `1` | Runs after Serial Configuration (priority −200,000) |

**Packet sent to Arduino — 4 bytes per cycle at 100 Hz:**

```
'A'   1 byte  — header
 u    2 bytes — int16, little-endian, ∈ [−255, 255]
'\n'  1 byte  — terminator
```

#### Serial Receive

![Serial Receive block dialog](../../docs/images/matlab/06_serial_receive.png)

| Parameter | Value | Notes |
|-----------|-------|-------|
| **Port** | *(same as Serial Configuration)* | Propagated from mask |
| **Data type** | `single` | Arduino sends IEEE-754 32-bit floats; MATLAB `single` matches exactly |
| **Header** | `'V'` | Firmware sends `'V'` before each data packet |
| **Data size** | `[1 3]` | Row vector of 3 elements: [angle, speed, PWM_echo] |
| **Terminator** | `LF ('\n')` | Marks end of packet |
| **Input format** | `Row major` | 3 floats received in the order the Arduino writes them |
| **Enable blocking mode** | `on` | Reception is configured in blocking mode with a 0.01 s sample time |
| **Action when data unavailable** | `Output last received value` | Saved action for unavailable data; blocking mode and timeout also affect waiting |
| **Sample time** | `0.01` s | Ts = 10 ms = 100 Hz, matches the Arduino Timer2 interrupt |
| **Priority** | `1` | Same level as Serial Send |

**Packet received from Arduino — 14 bytes per cycle at 100 Hz:**

```
'V'        1 byte  — header
angle      4 bytes — float32, little-endian  [°]
speed      4 bytes — float32, little-endian  [°/s]
PWM_echo   4 bytes — float32, little-endian  (applied PWM, echoed back)
'\n'       1 byte  — terminator
```

---

### How to use the model

1. Connect the Arduino via USB.
2. Open the `MotorCC_V3_Rxxxx.slx` file for your MATLAB release.
3. **Select the port**: double-click the `Motor_CC` block → the dropdown auto-populates with
   available ports → select the Arduino port → OK.
4. Press **Run** (Ctrl+T or the ▶ button). Sampling is configured at 0.01 s; communication depends on PC execution and does not provide a hard real-time guarantee.
5. Watch angle and velocity on the Scopes during execution.
6. **Stop the simulation**: click **Stop** and wait for the motor to stop. The saved V3 models do not contain a `StopFcn` callback. The firmware starts reducing PWM after more than **1 s** without commands, by **2 units every 10 ms** until zero (up to approximately 1.28 s more from |PWM| = 255).

The supplied V3 model applies an **open-loop** test signal; no feedback controller is connected. Arduino sends position, velocity and the PWM value, and Simulink displays the first two signals in the Scopes.

---

### Exporting to other releases

The working file is `MotorCC_V3_R2025b.slx`. Save changes before running this script from MATLAB R2025b:

```matlab
cd('<path>/Motor-CC-Control-Lab/software/matlab')
run('exportar_versiones.m')
```

The script loads the source model and calls `save_system` with `ExportToVersion` and a `<release>_SLX` target. It overwrites the seven V3 exports in its list and reports export errors. It is not needed to use the existing exports. The current stop behavior relies on the firmware watchdog; no `StopFcn` is saved in the V3 models.

### Recreating the current mask

The following three snippets were extracted **verbatim from the saved model**, including their comments. They are identical in all eight V3 exports. Older screenshots may show outdated code; use these snippets instead.

#### 1. Create the dropdown parameter

On the `Motor_CC` subsystem, open **Mask → Create Mask** if no mask exists, or **Mask → Edit Mask** to change it. In **Parameters & Dialog**, add a **Popup** with:

| Property | Value |
|---|---|
| Name | `PortName` |
| Prompt | `Arduino Port:` |
| Type | `popup` |
| Evaluate | `off` |
| Tunable | `off` |
| Initial type options | `Select a serial port` |
| Initial value | `Select a serial port` |

The model may retain a specific port from its last session. For a new mask, the placeholder above requires the user to select a port. The port is not changed during simulation.

#### 2. Mask initialization: propagate the port

In **Mask Editor → Code → Initialization**, enter:

```matlab
% Pass the chosen port to all three Instrument Control Toolbox blocks.
block = gcb;
selected = get_param(block, 'PortName');
if ~ismember(selected, {'Select a serial port', 'No serial ports available'})
    set_param([block '/Serial Configuration'], 'Port', selected);
    set_param([block '/Serial Send'], 'Port', selected);
    set_param([block '/Serial Receive'], 'Port', selected);
end
```

This copies the `PortName` text to the `Port` parameter of all three serial blocks. The names `Serial Configuration`, `Serial Send` and `Serial Receive` must match the internal blocks. `PortName` is the name used by this code, not a variable automatically detected by serial blocks.

#### 3. Block callback `OpenFcn`: refresh the list and open the mask

In the subsystem's **Block Properties → Callbacks → OpenFcn**, enter:

```matlab
% Refresh the list before showing the mask.
block = gcb;
if strcmp(get_param(bdroot(block), 'SimulationStatus'), 'stopped')
    ports = serialportlist('available');
    if isempty(ports)
        options = {'No serial ports available'};
    else
        options = [{'Select a serial port'}, cellstr(ports(:).')];
    end
    mask = Simulink.Mask.get(block);
    port = mask.getParameter('PortName');
    selected = get_param(block, 'PortName');
    port.TypeOptions = options;
    if ~ismember(selected, options)
        set_param(block, 'PortName', options{1});
    end
end
open_system(block, 'mask');
```

When the simulation is stopped, it calls `serialportlist('available')` and refreshes the options. It preserves the saved port if it remains available; otherwise it selects the `Select a serial port` placeholder. If no ports are available, it shows `No serial ports available`. It does not automatically identify Arduino or select the first real port. The final line opens the mask dialog.

#### 4. Block callback `InitFcn`: reject missing selection

In **Block Properties → Callbacks → InitFcn**, enter:

```matlab
if ismember(get_param(gcb, 'PortName'), {'Select a serial port', 'No serial ports available'})
    error('MotorCC:NoSerialPort', 'Connect the Arduino and select its serial port in Motor_CC before running.');
end
```

This callback rejects the two placeholders before model execution. It does not verify device identity or guarantee that the port remains available after selection.

#### 5. Save and check

Save the mask and model. With the simulation stopped, connect Arduino, open the mask, select its port and click **OK**. Mask initialization propagates the port to all three internal blocks. Run the test and check the measurements. If you change the R2025b source file, export the older releases again.

This README review checks values and callbacks stored in the `.slx` files; communication with the rig and operation on each MATLAB release still require laboratory testing.
