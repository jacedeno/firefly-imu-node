# Footprint fix — XIAO nRF52840 Plus (pad-number mismatch)

> **Why:** the Seeed OPL library has a footprint↔symbol pad-number mismatch. If used
> as-is, the battery power net (`VBAT`) and one `GND` do not map to any copper pad →
> the battery pad is electrically unconnected (scrap board). This is checklist item
> #1 in the brief (§10). Resolve this **before** schematic/layout/fab.
>
> Run this in a session started inside `firefly-imu-carrier/` so the KiCad MCP loads:
> `cd firefly-imu-carrier && claude`

## The mismatch

| | Pad numbers |
|---|---|
| Footprint `XIAO-nRF52840-Plus-SMD` | `1…29` |
| Symbol `XIAO-nRF52840_Plus_SMD` | `1…27, 32, 33` |

- Common pads `1…27` already match → leave them.
- Delta: footprint **#28 / #29** are the two back battery pads; the symbol calls
  them **#32 (VBAT) / #33 (GND)**. Only these two pads need renumbering.

The two pads in question (both `smd roundrect 2.5×1.1`, layer `F.Cu`):

| Current pad | Position (x, y) |
|---|---|
| `28` | `(-5.517823, -0.993988)` |
| `29` | `(-5.517823,  1.006012)` |

## Fix procedure (in KiCad, with Seeed pinout open)

1. **Get the authoritative polarity.** Open Seeed's official pinout / "Bottom Pad
   Data" (Resources section of the XIAO nRF52840 wiki). Identify which physical back
   pad is **BAT+ (VBAT)** and which is **BAT− (GND)**.
   - ⚠️ Do NOT guess. A reversed VBAT/GND assignment drives the battery backward into
     the module and destroys it. This is the whole reason for the visual check.
2. **Open the footprint** in the KiCad Footprint Editor (or via the `kicad-pcb` MCP).
3. **Renumber the two pads** so they match the symbol, by physical position:
   - the pad that is **BAT+** → rename to **`32`**
   - the pad that is **BAT−** → rename to **`33`**
4. **Validate pin/pad consistency** with the schematic MCP after assigning the
   footprint to U1, e.g. confirm the symbol pin set `{1…27, 32, 33}` now has a
   matching footprint pad for every pin (no "pin not connected to any pad" / no
   orphan pads). The `kicad-sch` MCP can report the symbol pin list to cross-check.
5. **Sanity-check a few known pins** while you're there (e.g. `3V3`=12, `GND`=13,
   `D4/SDA`=5, `D5/SCL`=6) so you trust pads `1…27` too.

## Also (cosmetic, optional)

The footprint references a wrong 3D model — a SAMD21 `.step` via `${AMZPATH}`. Replace
or remove it so the 3D view is correct. Does not affect copper/fab.

## Alternative (instead of patching)

Fetch an authoritative footprint rather than patching OPL: Seeed's official KiCad
export, or `cse_get_kicad` (SamacSys via the `pcbparts` MCP). If you go this route,
re-verify the same VBAT/GND polarity before trusting it.

## Definition of done

- Footprint pad set == symbol pad set (`{1…27, 32, 33}`), no orphan pins/pads in ERC.
- VBAT/GND polarity visually confirmed against Seeed's pinout.
- (optional) correct 3D model.

After this, the footprint risk (#1) is closed and the project can proceed to
`/eda:schematic`.
