# Component Selections — firefly-imu-carrier

> **STATUS: PRELIMINARY DRAFT.** Generated without the JLC MCP (this session ran in the
> repo root, not in `firefly-imu-carrier/`). The IC/FET **LCSC #** are web-verified;
> **stock, price, and the passive/connector LCSC # are still to be confirmed**.
> Run `/eda:source` from a session started inside `firefly-imu-carrier/` to: (a) confirm
> live stock/price, (b) `library_fetch` symbols/footprints into `lib/`, (c) validate
> pin/pad counts. See brief §3 and `design-constraints.json`.

---

## U1 — MCU/module: Seeed XIAO nRF52840 Sense Plus

- **Type:** module (NOT sourced via JLC PCBA — hand/reflow placed)
- **Seeed SKU:** 102010694
- **Footprint:** project-local `Seeed_XIAO:XIAO-nRF52840-Plus-SMD` (already in `lib/`)
- **Symbol:** `Seeed_XIAO:XIAO-nRF52840_Plus_SMD`
- **Notes:** onboard LSM6DS3TR-C @0x6A, BQ25101, USB-C, antenna. Power pad = `VBAT` (pad 32).

## U2 — Magnetometer: ST LIS3MDL  ✓ LCSC verified

- **LCSC:** **C478483** (LIS3MDLTR) · **MPN:** LIS3MDLTR · **Mfr:** STMicroelectronics
- **Package:** LGA-12 (2×2×1 mm)
- **Function:** 3-axis magnetometer to complete 9-DOF
- **Config:** I2C, CS→VDD_IO, SA1→GND ⇒ address **0x1C**
- **Datasheet:** https://www.st.com/resource/en/datasheet/lis3mdl.pdf → save to `datasheets/C478483_LIS3MDLTR.pdf`
- **Design:** 100 nF on VDD and VDD_IO (<2 mm); DRDY/INT→D2.

## Q1 — Reverse-polarity P-FET  ✓ LCSC verified

Two valid options (logic-level, 1S LiPo):

| Option | LCSC | MPN | Vds | Vgs(th) | Package | JLC |
|---|---|---|---|---|---|---|
| **A (preferred)** | **C144153** | DMG2305UX-13 | -20 V | ~-0.45…-1.1 V | SOT-23 | Extended |
| B (alt, cheaper/basic) | **C15127** | AO3401A | -30 V | ≤-2.5 V typ. | SOT-23 | **Basic** |

- Current is trivial (~10–15 mA load + ≤100 mA charge) → choose for low threshold, not Id.
- AO3401A (C15127) is **JLC Basic** (no setup fee) and very available → good cost option.
- DMG2305UX has a lower threshold (better enhancement at 3.0–3.3 V).
- Connection (brief §6): **S→VBAT(module)**, **D→J1(+)**, **G→GND**, R1 100k gate-source.
- AO3401A datasheet: https://datasheet.lcsc.com/lcsc/1810171817_Alpha-&-Omega-Semicon-AO3401A_C15127.pdf

## J1 — Battery connector: JST-PH 2.0 mm, 2-pin  ⚠ confirm LCSC

- **Function:** swappable LiPo input. **Silkscreen +/- polarity.**
- **LCSC:** to confirm (clones exist; prefer a generic in-stock JLC part). Search "PH 2.0 2P".
- **Notes:** THT gives more mechanical robustness to swapping; SMD if a low profile is preferred.

## Passives  ⚠ LCSC candidates (confirm stock/basic)

| Ref | Value | Package | LCSC candidate | Function |
|---|---|---|---|---|
| R1 | 100 kΩ 1% | 0402 | C25741 | Q1 gate-source (default-off) |
| R2,R3 | 4.7 kΩ 1% | 0402 | C25900 | I2C pull-ups → 3V3 |
| C1,C2 | 100 nF X7R 50V | 0402 | C1525 | LIS3MDL VDD/VDD_IO decoupling |
| C3 | 10 µF X5R 25V | 0805 | C15850 | Battery input bulk |

> All candidates for JLC Basic; **confirm with `/eda:source`** (passive C# vary by
> generic/series). Tolerances/voltages are minimum targets.

---

## Next step

1. Session in `firefly-imu-carrier/` → `/eda:source` to confirm stock/price and
   `library_fetch` U2, Q1, J1, passives into `lib/` (project-local).
2. Download datasheets to `datasheets/`.
3. Then `/eda:schematic` (draws from the net-by-net table in brief §5).
