# Firefly IMU Carrier — Design workflow (Flux.ai ⇄ KiCad stack)

> Operational guide to take the **Firefly Blue Ghost IMU carrier** from idea to
> manufacturing files, comparing **Flux.ai (Copilot)** against the
> **KiCad stack** (`kicad-pcb-stack`). Complements `firefly-imu-carrier-design-brief.md`
> (that is the *what* gets designed; this is the *how* you work).
>
> Written for someone new to PCB design. Read it through once; then use it as a
> per-phase reference.

---

## 0. The idea in 30 seconds

- You will build **the same carrier in two tools at once** to compare them:
  - **Flux.ai** — cloud EDA with an AI copilot; fast for exploring.
  - **KiCad stack** — our 6 phases with rigorous validation and fab output.
- **KiCad is your "source of truth"** (as your brief states), and you archive in **Forgejo**.
- **KiCad is the editor; Claude is the assistant.** Claude edits the files
  underneath and you open KiCad to *see* and approve. Nobody orders or uploads
  anything on its own: you always press that button.

---

## 1. The map: the 6 phases applied to YOUR carrier

| Phase | What it means for this carrier | Command / action | You do |
|---|---|---|---|
| **0. Init** | Set up the KiCad project in `~/repos/firefly-imu-node` | `/pcb-init` (or you already have the repo, see §4) | confirm constraints |
| **1. Parts** | Get U2 (LIS3MDL), Q1 (DMG2305UX/AO3401), J1 (JST-PH), R/C, and the XIAO module footprint | `/pcb-source` | approve the list |
| **2. Schematic** | Draw the connections from your **net-by-net table** (brief §5) | "build the schematic" | review it in KiCad |
| **3. Board** | Place and route honoring the antenna keepout + isolating the magnetometer | "do the layout" | guide placement |
| **4. Review** | ERC + fab DRC + signal/power + EMC (your checklist, brief §10) | `/pcb-review --full` | decide what to fix |
| **5. Fab** | ZIP for JLCPCB/PCBWay (2 layers, ENIG, 0.2 mm) | `/pcb-fab jlcpcb` or `pcbway` | **upload and pay** |

---

## 2. Before each session (routine)

```bash
cd ~/repos/firefly-imu-node
claude
```

Golden rules the stack enforces:

1. **Close the KiCad GUI before Claude writes** the schematic/board.
   (Two programs touching the same file corrupt it. The stack checks for this.)
2. **FreeCAD is the opposite:** for the 3D enclosure, FreeCAD must be **open** with
   its "RPC Server" started (see §7).
3. **It is never "fab-ready"** until *all* checks pass and you have seen it in
   KiCad. Otherwise it is a "prototype" — the stack uses those labels seriously.

---

## 3. ⚠️ The Flux.ai ⇄ KiCad issue — read before investing the month

Your brief says *"Flux.ai (Copilot) → review/round-trip source of truth in KiCad"*.
There is a trap worth knowing about:

- **Flux exports fab data well:** Gerbers (RS-274X), drills (Excellon), BOM,
  pick-and-place and IPC-D-356 netlist. That is standard and anyone can read it.
- **But the Flux → KiCad "round-trip" is weak:** moving the *project/schematic*
  from Flux to KiCad gives broken results according to user reports. The other way
  around (KiCad → Flux) does work. In other words, the round-trip your brief
  assumes **is not clean in the direction you need.**

### What to do in practice (two paths, you compare)

- **Path A — KiCad as the source (recommended for your "source of truth"):**
  Do NOT rely on the Flux export to get the design into KiCad. Instead, Claude
  **re-captures the schematic directly in KiCad from your net-by-net table**
  (brief §5) — which is already written with the exact detail the stack needs.
  That way KiCad is genuinely the source of truth, without crossing the broken bridge.
- **Path B — Flux as the source:** you design in Flux with its copilot (paste the
  prompt from brief §9), and **only validate what Flux exports well**: Gerbers
  against the fab rules, and the BOM against distributors. The strong electrical
  checks (ERC, topology, signal/power, EMC) **cannot** be run on a broken export,
  so the stack helps little there.

> **Honest conclusion:** for *this* project, the stack gives you the most value on
> **Path A**. Flux gives you speed and a visual copilot for exploring and comparing.
> Treat them as two separate sources; do not assume the Flux design enters KiCad
> whole for the full review.

---

## 4. Path A — designing in the KiCad stack (step by step)

### Phase 0 — Project
Your repo `~/repos/firefly-imu-node` **already exists** and has a KiCad bootstrap
(`fp-lib-table`, `sym-lib-table`, `lib/`). Two options:
- **If you want the full stack structure:** let Claude run `claude-eda init`
  *inside* the repo (it does not delete your `.kicad_*`) and then **adjust the
  `.mcp.json` for your machine** (see §7 — important because of the Python issue).
- **If you prefer minimal:** Claude creates the `.kicad_pro/.kicad_sch/.kicad_pcb`
  with mixelpixx and reuses your library tables.

Confirm the constraints from brief §8: **2 layers, FR-4 1.6 mm, 1 oz, 0.2 mm
trace/space, 0.3/0.6 mm vias, ENIG**, fab JLCPCB or PCBWay.

### Phase 1 — Parts (`/pcb-source`)
Ask Claude to source the **carrier additions** (brief §3), e.g.:

> *"Find: LIS3MDL (LGA-12), DMG2305UX and the AO3401 alternative (SOT-23), JST-PH
> 2.0 mm 2-pin, and the 0402/0603 passives (R1 100 k, R2/R3 4.7 k, C1/C2 100 nF,
> C3 4.7–10 µF). Prioritize LCSC basic parts in stock."*

For the **XIAO nRF52840 Sense Plus module (U1)**: it is not a normal catalog part,
it goes in as an **SMD module** with the **official Seeed land pattern** (castellated
+ back pads). Ask Claude to find or prepare that footprint; confirm it against the
Seeed reference (brief §10, first check).

You approve the table (MPN, price, stock, datasheet) before continuing.

### Phase 2 — Schematic
> *"Build the schematic from the net-by-net table in the brief (section 5)."*

Claude places U1, U2, Q1, J1, R/C and connects exactly:
`3V3`, `GND`, `BAT`, `VBAT_CONN`, `I2C_SDA`, `I2C_SCL`, `MAG_INT`, plus the ties
`U2.CS→VDD_IO`, `U2.SA1→GND` (address 0x1C). Then it verifies the topology.
**Open KiCad and check** that U2 hangs off the XIAO's 3V3 (not BAT) and that Q1 is
as in brief §6 (S→BAT, D→J1+, G→GND).

### Phase 3 — Board (layout)
You guide placement with your constraints (brief §7); tell it like this:

> *"Place the XIAO with its antenna edge toward the board edge, with no copper or
> plane under the antenna. Put the LIS3MDL in the corner farthest from the antenna,
> the battery connector and the charge traces. C1/C2 within 2 mm of U2. Short,
> direct I2C. Two M2 holes."*

The **antenna keepout** and the **magnetometer isolation** are the most important
things here — a mag near DC current reads heading wrong.

### Phase 4 — Review (`/pcb-review --full`)
Runs all analyzers and gives you a report by severity. Map it to your checklist in
brief §10:
- **ERC** (mixelpixx) — floating nets, crossed power.
- **Fab DRC** (kicad-happy/jlcpcb or pcbway) — 0.2 mm trace, vias, ENIG.
- **Signal/power** (Seeed) — ground planes, decoupling, current paths.
- **EMC** (kicad-happy/emc) — decoupling, routing, edges.
- Close every box in your §10 (Seeed footprint, I2C D4/D5 mapping, BAT pad,
  BQ25101 charge setting, Q1 threshold, LIS3MDL application circuit).

### Phase 5 — Fab (`/pcb-fab`)
Generates the package (Gerbers + drills + BOM + positions + fab DRC) as a ZIP.
**You** go to the JLCPCB/PCBWay portal, upload and pay. The stack does **not**
upload anything. Archive the result in **Forgejo** as your source of truth.

---

## 5. Path B — designing in Flux.ai (step by step)

1. In Flux, create the project and paste the **ready prompt** from brief §9 into the copilot.
2. Iterate visually (the copilot places and routes; you correct).
3. Export the fab package (Gerbers, drills, BOM, pick-and-place).
4. **Partial validation with the stack:**
   - Gerbers → run them through the fab rules (JLCPCB/PCBWay).
   - BOM → compare against distributors (pcbparts / @jlcpcb/mcp).
   - *(Remember §3: the full electrical checks do not apply to the export.)*

---

## 6. How to compare the two (your goal for the month)

Keep a simple table and fill it in at the end:

| Criterion | Flux.ai | KiCad stack |
|---|---|---|
| Speed to first schematic | | |
| Routing quality/control | | |
| How well it honors antenna keepout + mag isolation | | |
| Validation depth (ERC/DRC/SI-PI/EMC) | | |
| Effort to reach correct Gerbers | | |
| Final batch cost and BOM clarity | | |
| Would you use it again? | | |

Same brief, two paths → you decide with data, not impressions.

---

## 7. Appendix: configuration specific to YOUR machine

Your machine is **Fedora Asahi (aarch64), KiCad 10.0.3**. There is a key detail:
your default `python3` is the **PlatformIO** one (without `pcbnew`), and `pcbnew`
only imports from `/usr/bin/python3.14`. That is why the project `.mcp.json` must
point at the correct interpreters. This is the block it should end up with:

```json
{
  "mcpServers": {
    "mixelpixx-kicad": {
      "command": "node",
      "args": ["/home/geekendzone/.claude-eda/kicad-mcp/dist/index.js"],
      "env": { "KICAD_PYTHON": "/usr/bin/python3.14" }
    },
    "seeed-kicad": {
      "command": "/home/geekendzone/.venvs/seeed/bin/python",
      "args": ["-m", "kicad_mcp_server"],
      "env": {
        "KICAD_PROJECT_PATHS": "/home/geekendzone/repos/firefly-imu-node",
        "PYTHONUNBUFFERED": "1"
      }
    },
    "jlcpcb":   { "command": "npx", "args": ["-y", "@jlcpcb/mcp"] },
    "pcbparts": { "type": "http", "url": "https://pcbparts.dev/mcp" },
    "freecad":  { "command": "uvx", "args": ["freecad-mcp"] }
  }
}
```

Notes:
- **mixelpixx** needs `KICAD_PYTHON=/usr/bin/python3.14` to find `pcbnew`
  (otherwise it grabs the PlatformIO Python and fails).
- **seeed** runs from its isolated venv `~/.venvs/seeed/bin/python` (it sees
  `pcbnew`, has `fastmcp` + the server). Bare `python3` would give basic mode (no signal/power).
- **pcbparts** is HTTP in the cloud — no install, no API key for JLCPCB.
- **freecad** (3D enclosures, optional): in addition to this, open FreeCAD → workbench
  **"MCP Addon"** → **"Start RPC Server"**. The addon is already copied into the Flatpak
  sandbox. Without that manual step, the FreeCAD server does not connect.

Stack install status on this machine: KiCad 10.0.3 ✓, uv/bun ✓,
claude-eda ✓, mixelpixx ✓, Seeed ✓, ngspice 46 ✓, FreeCAD 1.1.1 + addon ✓.

---

## 8. Minimal glossary (PCB for newcomers)

- **Schematic** — the "diagram" of what connects to what (the nets).
- **Net** — a logical wire (e.g. `3V3`, `GND`, `I2C_SDA`).
- **Footprint** — the physical pattern where a part is soldered on the board.
- **Layout / routing** — placing the footprints and drawing the copper traces.
- **ERC** — electrical check (dangling pins, crossed power).
- **DRC** — fabrication check (trace width, drills, clearances).
- **Gerber** — the "drawings" of each layer that the fab understands.
- **BOM** — bill of materials (which parts and how many).
- **Pick-and-place / CPL / centroid** — where and at what rotation each part goes (for
  automated assembly).
- **Stackup** — how the layers stack (here: 2 layers, FR-4 1.6 mm, 1 oz).
- **ENIG / HASL** — copper finishes; ENIG is finer and cleaner (better for the
  LIS3MDL's LGA-12).
- **Keepout** — a zone where there must be *no* copper (here: under the XIAO antenna).
- **Pour / plane** — a copper fill (typically GND).
- **Via** — a plated hole connecting layers.

---

## 9. Safety rules (non-negotiable)

1. **KiCad GUI closed** before Claude writes (FreeCAD, open, is the opposite).
2. **Never "fab-ready"** without: ERC ok, fab DRC ok, signal/EMC with no blockers, and your
   visual sign-off in KiCad. If anything is missing, it is a "prototype".
3. **Nobody orders or uploads** for you. The stack generates the ZIP; you upload it to the portal.
4. **Datasheets:** if an electrical claim is not backed by the cached datasheet,
   the stack will say "consistency only", not "verified". Believe it.
