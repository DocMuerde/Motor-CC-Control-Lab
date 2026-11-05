# Placa de circuito impreso / PCB

## Español

La placa centraliza las conexiones entre Arduino, driver y encoder AS5047P.

### Archivos

| Archivo | Descripción |
|---|---|
| `schematic.*` | Esquemático (KiCad / PDF) |
| `pcb_layout.*` | Layout de la placa (KiCad / Gerber) |
| `gerber/` | Archivos listos para fabricación |
| `bom.csv` | Lista de materiales |

### Especificaciones

| Parámetro | Valor |
|---|---|
| Tensión de alimentación motor | 12–24 V DC |
| Corriente máxima | 1.2 A cont. / 3 A pico |
| Tensión lógica | 5 V (Arduino) |
| Capas | 2 |

---

## English

PCB centralising connections between Arduino, motor driver (TB6612FNG) and AS5047P encoder. Gerber files in `gerber/` are ready for JLCPCB, PCBWay or Eurocircuits.
