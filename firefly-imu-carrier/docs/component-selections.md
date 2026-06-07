# Component Selections — firefly-imu-carrier

> **ESTADO: BORRADOR PRELIMINAR.** Generado sin el JLC MCP (esta sesión corre en la
> raíz del repo, no en `firefly-imu-carrier/`). Los **LCSC #** de los ICs/FET están
> verificados por web; **stock, precio y los LCSC de pasivos/conector están por
> confirmar**. Corre `/eda-source` desde una sesión iniciada en `firefly-imu-carrier/`
> para: (a) confirmar stock/precio en vivo, (b) `library_fetch` de símbolos/footprints
> a `lib/`, (c) validar pin/pad. Ver brief §3 y `design-constraints.json`.

---

## U1 — MCU/módulo: Seeed XIAO nRF52840 Sense Plus

- **Tipo:** módulo (NO se sourcing por JLC PCBA — se coloca a mano / reflow)
- **SKU Seeed:** 102010694
- **Footprint:** project-local `Seeed_XIAO:XIAO-nRF52840-Plus-SMD` (ya en `lib/`)
- **Símbolo:** `Seeed_XIAO:XIAO-nRF52840_Plus_SMD`
- **Notas:** onboard LSM6DS3TR-C @0x6A, BQ25101, USB-C, antena. Pad de potencia = `VBAT` (pad 32).

## U2 — Magnetómetro: ST LIS3MDL  ✓ LCSC verificado

- **LCSC:** **C478483** (LIS3MDLTR) · **MPN:** LIS3MDLTR · **Mfr:** STMicroelectronics
- **Package:** LGA-12 (2×2×1 mm)
- **Función:** magnetómetro 3 ejes para completar 9-DOF
- **Config:** I2C, CS→VDD_IO, SA1→GND ⇒ dirección **0x1C**
- **Datasheet:** https://www.st.com/resource/en/datasheet/lis3mdl.pdf → guardar en `datasheets/C478483_LIS3MDLTR.pdf`
- **Diseño:** 100 nF en VDD y VDD_IO (<2 mm); DRDY/INT→D2.

## Q1 — P-FET protección polaridad inversa  ✓ LCSC verificado

Dos opciones válidas (logic-level, 1S LiPo):

| Opción | LCSC | MPN | Vds | Vgs(th) | Package | JLC |
|---|---|---|---|---|---|---|
| **A (preferida)** | **C144153** | DMG2305UX-13 | -20 V | ~-0.45…-1.1 V | SOT-23 | Extended |
| B (alt, más barato/básico) | **C15127** | AO3401A | -30 V | ≤-2.5 V típ. | SOT-23 | **Basic** |

- Corriente trivial (~10–15 mA load + ≤100 mA carga) → elegir por umbral bajo, no por Id.
- AO3401A (C15127) es **JLC Basic** (sin fee de setup) y archi-disponible → buena opción de costo.
- DMG2305UX tiene umbral más bajo (mejor enhancement a 3.0–3.3 V).
- Conexión (brief §6): **S→VBAT(módulo)**, **D→J1(+)**, **G→GND**, R1 100k gate-source.
- Datasheet AO3401A: https://datasheet.lcsc.com/lcsc/1810171817_Alpha-&-Omega-Semicon-AO3401A_C15127.pdf

## J1 — Conector batería: JST-PH 2.0 mm, 2-pin  ⚠ confirmar LCSC

- **Función:** entrada LiPo swappable. **Silkscreen polaridad +/-.**
- **LCSC:** por confirmar (hay clones; preferir genérico en stock JLC). Buscar "PH 2.0 2P".
- **Notas:** THT da más robustez mecánica al swap; SMD si se prefiere perfil bajo.

## Pasivos  ⚠ LCSC candidatos (confirmar stock/básico)

| Ref | Valor | Package | LCSC candidato | Función |
|---|---|---|---|---|
| R1 | 100 kΩ 1% | 0402 | C25741 | Gate-source de Q1 (default-off) |
| R2,R3 | 4.7 kΩ 1% | 0402 | C25900 | Pull-ups I2C → 3V3 |
| C1,C2 | 100 nF X7R 50V | 0402 | C1525 | Decoupling LIS3MDL VDD/VDD_IO |
| C3 | 10 µF X5R 25V | 0805 | C15850 | Bulk en entrada de batería |

> Todos candidatos a JLC Basic; **confirmar con `/eda-source`** (los C# de pasivos
> cambian según genérico/serie). Tolerancias/tensiones son objetivos mínimos.

---

## Próximo paso

1. Sesión en `firefly-imu-carrier/` → `/eda-source` para confirmar stock/precio y
   `library_fetch` de U2, Q1, J1, pasivos hacia `lib/` (project-local).
2. Descargar datasheets a `datasheets/`.
3. Luego `/eda-schematic` (dibuja desde la tabla net-by-net del brief §5).
