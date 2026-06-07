# Project-local libraries (KiCad)

Footprints and symbols for the **Seeed XIAO nRF52840 Sense Plus** (U1 in the brief).
Source: [Seeed-Studio/OPL_Kicad_Library](https://github.com/Seeed-Studio/OPL_Kicad_Library).

```
lib/
├── footprints/Seeed_XIAO.pretty/
│   ├── XIAO-nRF52840-Plus-SMD.kicad_mod   ← USE THIS ONE (Plus, 29 SMD pads)
│   ├── XIAO-nRF52840-SMD.kicad_mod        (standard variant, reference)
│   └── XIAO-nRF52840-DIP.kicad_mod        (castellated THT, reference)
└── symbols/Seeed_Studio_XIAO_Series.kicad_sym
        symbol to use: "XIAO-nRF52840_Plus_SMD"
```

The `fp-lib-table` and `sym-lib-table` files in the project root (`firefly-imu-carrier/`)
already hook these libraries via `${KIPRJMOD}`. The `.kicad_pro` is created inside
`firefly-imu-carrier/` so the paths resolve; if you move it elsewhere, adjust the tables.

## Pin mapping relevant to the brief (Plus symbol)

| Pin (symbol)         | Pad | Use in the brief |
|----------------------|-----|------------------|
| `P0.04_A4_D4_SDA`    | 5   | I2C_SDA → LIS3MDL SDA + R2 |
| `P0.05_A5_D5_SCL`    | 6   | I2C_SCL → LIS3MDL SCL + R3 |
| `P0.28_A2_D2`        | 3   | MAG_INT (LIS3MDL DRDY/INT) |
| `3V3`                | 12  | 3V3 rail → U2 VDD/VDD_IO, pull-ups, decoupling |
| `GND`                | 13, 27, 33 | GND (3 pads) |
| **`VBAT`**           | **32** | **Battery power pad → Q1.Source** (1S power input) |
| `P0.31_D16_BAT`      | 20  | Battery ADC sense (internal divider) → VBAT% in HUD (§11) |
| `SWDIO` / `SWDCLK`   | 24 / 25 | Expose SWD pads if practical (§7.6) |
| `REST`               | 26  | Reset |

> **Heads up (resolves §10):** there are TWO "BAT" nodes:
> - **`VBAT` (pad 32)** is the battery **power pad** — the P-FET Q1 Source connects here.
> - **`P0.31_D16_BAT` (pad 20)** is only the **ADC sense GPIO** (battery sense via divider),
>   to report % on the dashboard. It is NOT where the battery current flows.
>
> Visually verify the physical position of pad 32 against the Plus footprint before routing.
