# Encoder — ams AS5047P

## Español

Encoder magnético de posición rotativa de alta resolución utilizado en el equipo de prácticas.  
Montado en el lado de **carga** (eje de salida de la reductora + disco de inercia).

### Archivos

| Archivo | Descripción |
|---------|-------------|
| `AS5047P-Datasheet.pdf` | Datasheet completo del AS5047P (ams-OSRAM) |
| `AS5047P.png` | Fotografía del componente en el equipo |

### Parámetros clave

| Parámetro | Valor |
|-----------|-------|
| Resolución | 14 bits → **16 384 pasos/vuelta** |
| Interfaz de comunicación | SPI (modos 1 y 3), ABI, PWM, UVW |
| Tensión de alimentación | 3,3 V |
| Temperatura de operación | −40 °C a +150 °C |
| Frecuencia SPI máxima | 10 MHz |

### Conexión con Arduino UNO

| Pin AS5047P | Pin Arduino | Función |
|-------------|-------------|---------|
| CLK | D13 (SCK) | Reloj SPI |
| DO | D12 (MISO) | Datos del encoder al Arduino |
| DI | D11 (MOSI) | Datos del Arduino al encoder |
| CSn | **D4** | Chip Select (activo en bajo) |
| GND | GND | Referencia |
| VDD | 3,3 V | Alimentación |

> ⚠️ El AS5047P opera a **3,3 V**. No conectar directamente a las salidas de 5 V del Arduino sin un divisor de tensión o level-shifter en las líneas de entrada del chip.

### Librería Arduino

```
Biblioteca: "AS5047P" de Adrien Plazas  
Repositorio: https://github.com/adplaz/AS5047P
```

Instalación desde el IDE de Arduino: *Sketch → Include Library → Manage Libraries → buscar "AS5047P"*

### Referencia

📄 Página oficial del fabricante:  
https://ams-osram.com/products/sensors/position-sensors/ams-as5047p-high-resolution-rotary-position-sensor

---

## English

High-resolution magnetic rotary position encoder used in the lab rig.  
Mounted on the **load side** (gearbox output shaft + inertia disk).

### Key parameters

- Resolution: 14-bit → **16 384 steps/rev**
- Interface: SPI (modes 1 & 3), ABI, PWM, UVW
- Supply voltage: 3.3 V
- CS pin on Arduino: **D4**
