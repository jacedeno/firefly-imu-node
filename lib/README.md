# Librerías locales del proyecto (KiCad)

Footprints y símbolos del **Seeed XIAO nRF52840 Sense Plus** (U1 en el brief).
Origen: [Seeed-Studio/OPL_Kicad_Library](https://github.com/Seeed-Studio/OPL_Kicad_Library).

```
lib/
├── footprints/Seeed_XIAO.pretty/
│   ├── XIAO-nRF52840-Plus-SMD.kicad_mod   ← USAR ESTE (Plus, 29 pads SMD)
│   ├── XIAO-nRF52840-SMD.kicad_mod        (variante normal, referencia)
│   └── XIAO-nRF52840-DIP.kicad_mod        (castellado THT, referencia)
└── symbols/Seeed_Studio_XIAO_Series.kicad_sym
        símbolo a usar: "XIAO-nRF52840_Plus_SMD"
```

Las tablas `fp-lib-table` y `sym-lib-table` en la raíz del repo ya enganchan estas
librerías vía `${KIPRJMOD}`. **Crea el `.kicad_pro` en la raíz del repo** para que
las rutas resuelvan; si lo pones en una subcarpeta, ajusta las tablas.

## Mapeo de pines relevante para el brief (símbolo Plus)

| Pin (símbolo)        | Pad | Uso en el brief |
|----------------------|-----|-----------------|
| `P0.04_A4_D4_SDA`    | 5   | I2C_SDA → LIS3MDL SDA + R2 |
| `P0.05_A5_D5_SCL`    | 6   | I2C_SCL → LIS3MDL SCL + R3 |
| `P0.28_A2_D2`        | 3   | MAG_INT (DRDY/INT del LIS3MDL) |
| `3V3`                | 12  | Rail 3V3 → U2 VDD/VDD_IO, pull-ups, decoupling |
| `GND`                | 13, 27, 33 | GND (3 pads) |
| **`VBAT`**           | **32** | **Pad de batería → Q1.Source** (entrada de potencia 1S) |
| `P0.31_D16_BAT`      | 20  | Sense ADC de batería (divisor interno) → VBAT% en HUD (§11) |
| `SWDIO` / `SWDCLK`   | 24 / 25 | Exponer pads SWD si es práctico (§7.6) |
| `REST`               | 26  | Reset |

> **Ojo (resuelve §10):** hay DOS nodos "BAT":
> - **`VBAT` (pad 32)** es el **pad de potencia** de la batería — aquí va el Source del P-FET Q1.
> - **`P0.31_D16_BAT` (pad 20)** es solo el **GPIO de lectura ADC** (battery sense vía divisor),
>   para reportar % en el dashboard. NO es por donde entra la corriente de la batería.
>
> Verifica visualmente la posición física del pad 32 contra el footprint Plus antes del ruteo.
