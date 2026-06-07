# Setup & entorno — firefly-imu-node

Guía de onboarding para trabajar este repo desde **cualquiera de las dos máquinas**.
Complementa `firefly-imu-carrier-design-brief.md` (el *qué*) y `WORKFLOW.md` (el *cómo* de diseño PCB).

## Máquinas

| Máquina | Arch | OS | Rol |
|---|---|---|---|
| **MacBook Air M1** | aarch64 | Fedora 44 (Asahi) | Primaria/móvil: diseño KiCad, edición firmware, git |
| **Desktop PC** | x86_64 | Fedora 44 | En casa: compilar firmware, flashear, probar hardware real |

**Por qué el split:** el toolchain nRF52 en ARM tiene fricción (el fallo mbed/GCC arm64 del brief §11).
Regla: **diseño/edición en cualquiera; compilar/flashear/probar en el desktop x86_64.**
La build canónica es **GitHub Actions (runner x86_64)** → independiente de dónde edites.

---

## Qué va en git vs qué es por-máquina

**En git (viaja entre máquinas con `git pull`):**
- Firmware (`firmware/`), CI (`.github/`)
- Proyecto KiCad y **librerías project-local** (`firefly-imu-carrier/lib/`, `fp-lib-table`, `sym-lib-table`)
- Comandos/skills/agentes EDA (`firefly-imu-carrier/.claude/`)
- Docs (brief, WORKFLOW, este SETUP, `docs/`)

**Por-máquina (NO en git — instalar en cada equipo):**
- El stack `~/.claude-eda` (kicad-mcp, venv del schematic MCP)
- Toolchains de PlatformIO (`~/.platformio`)
- Pertenencia al grupo `dialout` (flasheo serial)

---

## Setup por máquina (correr una vez en cada equipo)

### 1. Firmware (ambas máquinas; flasheo solo en desktop)
```bash
pip install --upgrade platformio        # si no está
cd firmware && pio run                   # compila (1ª vez baja toolchain)
# Solo desktop x86_64: pio run -t upload   (placa conectada por USB-C)
```
- Estar en el grupo `dialout`: `sudo usermod -aG dialout $USER` (re-login).
- Board id: `xiaoblesense_adafruit` vía el **fork de maxgerhardt** (el platform oficial no trae el XIAO). Ya está en `firmware/platformio.ini`.

### 2. Stack EDA / KiCad (ambas máquinas)
```bash
claude-eda doctor                        # diagnóstico
claude-eda kicad-mcp --install           # MCP de PCB (si falta)
claude-eda kicad-sch-mcp --install       # MCP de esquemático (si falta)
```
- KiCad 10.0.3. `pcbnew` se importa desde `/usr/bin/python3.14` (no el python de PlatformIO).

### 3. Dashboard Web Bluetooth
- Usar **Brave/Chromium** (Firefox NO soporta Web Bluetooth). En Brave: habilitar
  *Web Bluetooth API* en `brave://flags`.

---

## Gotchas del stack claude-eda (descubiertos en el setup)

1. **`claude-eda update` está roto** ("Could not find templates directory"). Por eso
   `init` dejó `.claude/commands|agents|skills` vacíos. Ya se copiaron a mano al proyecto
   (y están en git), así que en el desktop llegan con `git pull` — no hay que re-copiarlos.
   - Fuente de los templates: `~/repos/claude-eda/templates/claude/`.
2. **Invocación de comandos:** son `/eda:source`, `/eda:schematic`, etc. (**dos puntos**,
   namespace por la carpeta `commands/eda/`), NO `/eda-source` aunque el CLAUDE.md lo diga.
3. **Los MCP de KiCad cargan solo desde el subdir.** `jlc`, `kicad-pcb` y `kicad-sch`
   están en `firefly-imu-carrier/.mcp.json`. Para usarlos:
   ```bash
   cd firefly-imu-carrier && claude
   ```
   Desde la raíz del repo NO están disponibles.
4. **`.mcp.json` usa rutas absolutas** a `/home/geekendzone/.claude-eda/...` — válido si
   el desktop usa el mismo usuario/home (`geekendzone`). Si difiere, ajustarlas.

---

## Estado del proyecto y siguiente paso

- ✅ Firmware scaffold + CI verde (`main`)
- ✅ Proyecto KiCad init + constraints (`firefly-imu-carrier/docs/design-constraints.json`)
- ✅ Librerías project-local (XIAO Plus) + comandos/skills EDA instalados
- ✅ Sourcing **PRELIMINAR** (`docs/component-selections.md`, `docs/bom-draft.json`)
- ⏭️ **Siguiente:** `cd firefly-imu-carrier && claude` → `/eda:source` (fetch real de stock +
  símbolos/footprints KiCad a `lib/`), luego `/eda:schematic` (dibuja desde brief §5).

Detalle de pines del XIAO y la nota crítica del pad BAT: `firefly-imu-carrier/lib/README.md`.
