# Hardware mecánico / Mechanical Parts

## Español

### Piezas del equipo

El equipo combina piezas fabricadas en **PLA por impresión 3D**, piezas mecanizadas en **metal** y elementos comerciales estándar.

| Archivo STL | Pieza | Material | Fabricación |
|-------------|-------|----------|-------------|
| `Base.stl` | Base del equipo | PLA | Impresión 3D |
| `Soporte_disco.stl` | Soporte del disco de inercia | PLA | Impresión 3D |
| `Caja.stl` | Cuerpo de la caja de electrónica | PLA | Impresión 3D |
| `Tapa.stl` | Tapa de la caja de electrónica | PLA | Impresión 3D |
| `SMotor.stl` | Soporte del motor | PLA | Impresión 3D |
| `SSensor_integrado.stl` | Soporte del sensor (encoder) | PLA | Impresión 3D |
| `Disco_inercia.stl` | Disco de inercia ⚠️ | **Aluminio** | **Mecanizado CNC** (o servicio externo) |
| `Adaptador.stl` | Adaptador disco–eje motor ⚠️ | **Metal** | **Pieza comercial** (con tornillos de fijación) |

> ⚠️ **Disco de inercia y adaptador no son piezas impresas en 3D.** Los archivos STL se incluyen únicamente como referencia geométrica del modelo CAD.  
> El disco se mecanizó en CNC a partir del modelo; también puede encargarse a un servicio de mecanizado externo (ej. PCBWay, Xometry, talleres locales).  
> El adaptador es una pieza comercial estándar de acoplamiento de eje.

### Parámetros de impresión 3D (piezas PLA)

| Parámetro | Valor |
|-----------|-------|
| Material | PLA |
| Relleno (piezas estructurales) | **100%** — base, soportes |
| Relleno (carcasas) | 20–40% — caja, tapa |
| Altura de capa | 0,2 mm |
| Perímetros | 3 |
| Soporte | Solo en voladizos > 45° |
| Laminador recomendado | PrusaSlicer / Cura |

### Lista de materiales — Tornillería y elementos de unión

> 📝 **Pendiente de completar** — añadir referencia, cantidad y dónde se usa cada tornillo.

| Referencia | Descripción | Cantidad | Ubicación |
|------------|-------------|----------|-----------|
| M? × ?mm | — | — | — |

### Archivos CAD

El ensamblaje completo en formato STEP está disponible en la carpeta `step/`:

| Archivo | Descripción |
|---------|-------------|
| `Eq_practicas v83 v7.step` | Ensamblaje completo (todas las piezas) |

Compatible con: **Fusion 360**, FreeCAD, SolidWorks, CATIA, OnShape.

---

<p align="center">🇬🇧 &nbsp;&nbsp; <strong>E N G L I S H</strong> &nbsp;&nbsp; 🇬🇧</p>

---

## English

### Parts overview

The rig combines parts made from **PLA (3D-printed)**, **metal (CNC-machined)**, and standard commercial elements.

| STL file | Part | Material | Manufacturing |
|----------|------|----------|---------------|
| `Base.stl` | Rig base | PLA | 3D printed |
| `Soporte_disco.stl` | Inertia disk support | PLA | 3D printed |
| `Caja.stl` | Electronics enclosure body | PLA | 3D printed |
| `Tapa.stl` | Enclosure lid | PLA | 3D printed |
| `SMotor.stl` | Motor bracket | PLA | 3D printed |
| `SSensor_integrado.stl` | Encoder bracket | PLA | 3D printed |
| `Disco_inercia.stl` | Inertia disk ⚠️ | **Aluminium** | **CNC machined** (or outsourced) |
| `Adaptador.stl` | Disk-to-shaft adapter ⚠️ | **Metal** | **Commercial part** (with set screws) |

> ⚠️ **The inertia disk and adapter are not 3D-printed parts.** STL files are included as geometric reference only.  
> The disk was CNC-machined from the model; it can also be ordered from an external machining service (e.g. PCBWay, Xometry, local workshops).  
> The adapter is a standard commercial shaft coupling.

### 3D print settings (PLA parts)

| Parameter | Value |
|-----------|-------|
| Material | PLA |
| Infill (structural parts) | **100%** — base, brackets |
| Infill (enclosures) | 20–40% — box, lid |
| Layer height | 0.2 mm |
| Perimeters | 3 |
| Supports | Only for overhangs > 45° |
| Recommended slicer | PrusaSlicer / Cura |

### Bill of Materials — Fasteners and joining elements

> 📝 **To be completed** — add reference, quantity and location for each fastener.

| Reference | Description | Quantity | Location |
|-----------|-------------|----------|----------|
| M? × ?mm | — | — | — |

### CAD files

The complete assembly in STEP format is available in the `step/` folder:

| File | Description |
|------|-------------|
| `Eq_practicas v83 v7.step` | Complete assembly (all parts) |

Compatible with: **Fusion 360**, FreeCAD, SolidWorks, CATIA, OnShape.
