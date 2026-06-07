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

**This project uses PROJECT-LOCAL libraries tracked in git** (decision driven by the
two-machine flow: MacBook aarch64 + desktop x86_64 — the libs travel with the repo).

- `lib/footprints/Seeed_XIAO.pretty/` — XIAO nRF52840 Plus-SMD footprint (U1)
- `lib/symbols/Seeed_Studio_XIAO_Series.kicad_sym` — symbol (use `XIAO-nRF52840_Plus_SMD`)
- `fp-lib-table` / `sym-lib-table` — hook the above via `${KIPRJMOD}`
- `lib/README.md` — pin mapping and the critical BAT pad note (VBAT pad 32 = power)

KiCad is 10.0.3 (not 9.0). When sourcing generic parts (R/C/connector) the kicad-mcp
can fetch them; any reused symbols/footprints must live **inside `lib/`** so they
travel in git, not in the global library at `~/Documents/KiCad/...`.

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
