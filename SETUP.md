# Setup & environment — firefly-imu-node

Onboarding guide for working on this repo from **either machine**.
Complements `firefly-imu-carrier-design-brief.md` (the *what*) and `WORKFLOW.md` (the *how* of PCB design).

## Machines

| Machine | Arch | OS | Role |
|---|---|---|---|
| **MacBook Air M1** | aarch64 | Fedora 44 (Asahi) | Primary/mobile: KiCad design, firmware editing, git |
| **Desktop PC** | x86_64 | Fedora 44 | At home: compile firmware, flash, test real hardware |

**Why the split:** the nRF52 toolchain has friction on ARM (the mbed/GCC arm64 failure from brief §11).
Rule: **design/edit on either; compile/flash/test on the x86_64 desktop.**
The canonical build is **GitHub Actions (x86_64 runner)** → independent of where you edit.

---

## What lives in git vs what is per-machine

**In git (travels between machines with `git pull`):**
- Firmware (`firmware/`), CI (`.github/`)
- KiCad project and **project-local libraries** (`firefly-imu-carrier/lib/`, `fp-lib-table`, `sym-lib-table`)
- EDA commands/skills/agents (`firefly-imu-carrier/.claude/`)
- Docs (brief, WORKFLOW, this SETUP, `docs/`)

**Per-machine (NOT in git — install on each machine):**
- The `~/.claude-eda` stack (kicad-mcp, schematic MCP venv)
- PlatformIO toolchains (`~/.platformio`)
- `dialout` group membership (serial flashing)

---

## Per-machine setup (run once on each machine)

### 1. Firmware (both machines; flashing only on the desktop)
```bash
pip install --upgrade platformio        # if missing
cd firmware && pio run                    # build (first run downloads the toolchain)
# Desktop x86_64 only: pio run -t upload    (board connected over USB-C)
```
- Be in the `dialout` group: `sudo usermod -aG dialout $USER` (re-login).
- Board id: `xiaoblesense_adafruit` via the **maxgerhardt fork** (the official platform lacks the XIAO). Already set in `firmware/platformio.ini`.

### 2. EDA / KiCad stack (both machines)
```bash
claude-eda doctor                        # diagnostics
claude-eda kicad-mcp --install           # PCB MCP (if missing)
claude-eda kicad-sch-mcp --install       # schematic MCP (if missing)
```
- KiCad 10.0.3. `pcbnew` is imported from `/usr/bin/python3.14` (not the PlatformIO python).

### 3. Web Bluetooth dashboard
- Use **Brave/Chromium** (Firefox does NOT support Web Bluetooth). In Brave: enable
  *Web Bluetooth API* in `brave://flags`.

---

## claude-eda stack gotchas (found during setup)

1. **`claude-eda update` is broken** ("Could not find templates directory"). That's why
   `init` left `.claude/commands|agents|skills` empty. They were copied in by hand (and are
   in git), so on the desktop they arrive with `git pull` — no need to re-copy.
   - Source of the templates: `~/repos/claude-eda/templates/claude/`.
2. **Command invocation:** they are `/eda:source`, `/eda:schematic`, etc. (**colon**,
   namespaced by the `commands/eda/` folder), NOT `/eda-source` even though CLAUDE.md says so.
3. **The KiCad MCPs only load from the subdir.** `jlc`, `kicad-pcb`, `kicad-sch` and
   `pcbparts` live in `firefly-imu-carrier/.mcp.json`. To use them:
   ```bash
   cd firefly-imu-carrier && claude
   ```
   From the repo root they are NOT available.
4. **`.mcp.json` uses absolute paths** to `/home/geekendzone/.claude-eda/...` — valid if the
   desktop uses the same user/home (`geekendzone`). If it differs, adjust them.

---

## Project status and next step

- ✅ Firmware scaffold + green CI (`main`)
- ✅ KiCad project init + constraints (`firefly-imu-carrier/docs/design-constraints.json`)
- ✅ Project-local libraries (XIAO Plus) + EDA commands/skills installed
- ✅ **Preliminary** sourcing (`docs/component-selections.md`, `docs/bom-draft.json`)
- ✅ Breadboard prototype BOM + pinout (`docs/breadboard-prototype.md`)
- ⏭️ **Next:** `cd firefly-imu-carrier && claude` → `/eda:source` (real fetch of stock +
  KiCad symbols/footprints into `lib/`), then `/eda:schematic` (draws from brief §5).

XIAO pin details and the critical BAT pad note: `firefly-imu-carrier/lib/README.md`.
