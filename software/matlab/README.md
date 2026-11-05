# Software MATLAB / MATLAB Software

## Español

Scripts que implementan el **lado PC** del sistema: abren el puerto serie, leen el ángulo del encoder, calculan la acción de control y la envían al Arduino en tiempo real.

### Archivos principales

| Archivo | Descripción |
|---|---|
| `main_control.m` | Lazo de control en tiempo real |
| `identificacion.m` | Experimentos de identificación del sistema |
| `plot_results.m` | Visualización de datos experimentales |
| `config.m` | Parámetros globales (puerto, Ts, ganancias) |

### Parámetros clave (`config.m`)

```matlab
PORT  = '/dev/tty.usbmodem1101'; % Mac — ajustar según equipo
BAUD  = 115200;
Ts    = 0.01;    % Periodo de muestreo [s]
Kp    = 1.0;
Ki    = 0.5;
Kd    = 0.0;
T_exp = 10;      % Duración del experimento [s]
```

### Requisitos
- MATLAB R2020b o superior
- **Instrument Control Toolbox** (única toolbox necesaria — para `serialport`)

---

## English

MATLAB scripts implementing the PC side: serial port communication, angle reading, control law computation, and data logging.
