# firefly-imu-carrier - EDA Project

## Project Overview

AI-assisted EDA project using @claude-eda toolkit.

## Project Structure

- `firefly-imu-carrier.kicad_pro`: KiCad project file (in root)
- `firefly-imu-carrier.kicad_sch`: Schematic
- `firefly-imu-carrier.kicad_pcb`: PCB layout
- `docs/`: Design documentation
- `datasheets/`: Component datasheets
- `production/`: Manufacturing outputs

## Component Libraries

**Este proyecto usa librerías PROJECT-LOCAL versionadas en git** (decisión por el
flujo de dos máquinas: MacBook aarch64 + desktop x86_64 — las libs viajan con el repo).

- `lib/footprints/Seeed_XIAO.pretty/` — footprint del XIAO nRF52840 Plus-SMD (U1)
- `lib/symbols/Seeed_Studio_XIAO_Series.kicad_sym` — símbolo (usar `XIAO-nRF52840_Plus_SMD`)
- `fp-lib-table` / `sym-lib-table` — enganchan lo anterior vía `${KIPRJMOD}`
- `lib/README.md` — mapeo de pines y nota crítica del pad BAT (VBAT pad 32 = potencia)

KiCad es 10.0.3 (no 9.0). Al sourcing de piezas genéricas (R/C/conector) el kicad-mcp
puede traerlas; los símbolos/footprints que se reusen deben quedar **dentro de `lib/`**
para que viajen en git, no en la librería global `~/Documents/KiCad/...`.

## EDA Workflow

Use these commands for the EDA workflow:

1. `/eda-new` - Define project requirements
2. `/eda-source [role]` - Source components
3. `/eda-schematic [sheet]` - Create schematic
4. `/eda-layout [phase]` - Layout PCB
5. `/eda-check [scope]` - Validate design
6. `/eda-export [format]` - Export manufacturing files

## IMPORTANT

- Always run `/eda-check full` before `/eda-export`
- Check stock levels before finalizing component selection
