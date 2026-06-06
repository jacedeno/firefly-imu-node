# Firefly Blue Ghost IMU — Carrier Board Design Brief

**Target EDA:** Flux.ai (Copilot) → review/round-trip source of truth in KiCad
**Author:** GeekendZone (José Cedeño)
**Revision:** v0.1 — carrier for swappable-battery 9-DOF desk demo node

---

## 1. Objective

Design a compact 2-layer **carrier PCB** that mounts the **Seeed XIAO nRF52840 Sense Plus**
(SKU 102010694) as an SMD module and adds:

1. An external **ST LIS3MDL** magnetometer to complete a true **9-DOF** stack
   (onboard LSM6DS3TR-C accel+gyro + external LIS3MDL mag), eliminating the yaw drift of a 6-DOF build.
2. A **swappable / expandable LiPo input** via a JST-PH connector with **reverse-polarity protection**
   (high-side logic-level P-FET) on its own input path, *before* the module's BAT pad.

Firmware/transport are already proven (9-DOF Madgwick → quaternion → BLE GATT notify → Web Bluetooth
Three.js dashboard). The carrier is the hardware fix that removes the failure modes of the two prior
off-the-shelf builds (counterfeit MPU-9250 + WiFi brownouts on the ESP32-S3; flaky dev-board USB +
mbed/arm64 toolchain on the Nano 33 BLE).

---

## 2. Consolidated design decisions

| Decision | Choice | Rationale |
|---|---|---|
| MCU / radio | XIAO nRF52840 Sense Plus (module, SMD-mounted) | Robust BLE 5.4 stack, ultra-low power, onboard antenna + BQ25101 charger + USB-C; sidesteps RF, USB and toolchain problems. Plus variant exposes 20 GPIO + repositioned BAT pad for SMD mounting |
| 9-DOF completion | Keep onboard LSM6DS3TR-C (accel+gyro) + add LIS3MDL (mag) | ST + ST pairing, mature Adafruit libs, minimal BOM addition, no I2C address clash (0x6A vs 0x1C/0x1E) |
| Battery | LiPo 1S, start 250 mAh (expandable) | Demo use; ~25 h continuous, effectively "always ready" with sleep |
| Battery interface | JST-PH 2.0 mm connector (not soldered direct) | Swap/expand later without rework |
| Reverse-polarity protection | High-side logic-level P-FET (DMG2305UX / AO3401) on the connector input, before the module | Protects at the vulnerable swap point; FET conducts bidirectionally so charging is unaffected |
| Power management | Sleep-on-disconnect + wake-on-motion (LSM6DS3 INT) | No physical switch needed; idle drain negligible |
| Manufacturing | Prototype → PCBWay or JLCPCB | 2-layer, fab-friendly rules |

---

## 3. Bill of Materials (carrier additions)

| Ref | Part | Package | Function | Notes |
|---|---|---|---|---|
| U1 | Seeed XIAO nRF52840 Sense Plus (102010694) | Module, 21×17.5 mm, castellated + back SMD pads | nRF52840 / BLE 5.4 / onboard LSM6DS3TR-C @0x6A / PDM mic / NFC / BQ25101 / USB-C / antenna | Mount as SMD module. Onboard IMU: power-enable **P1.08**, INT **P0.11** |
| U2 | ST **LIS3MDL** | LGA-12, 2×2×1 mm | 3-axis magnetometer | I2C addr **0x1C** (SA1→GND) or 0x1E (SA1→VDD). CS→VDD_IO for I2C |
| Q1 | **DMG2305UX** (alt: AO3401) | SOT-23 | Reverse-polarity P-FET (logic-level) | S→BAT(module), D→J1(+), G→GND |
| J1 | JST-PH **2.0 mm**, 2-pin | THT or SMD | LiPo battery connector | **Silkscreen polarity clearly** |
| R1 | 100 kΩ | 0402/0603 | Q1 gate reference (gate-to-source) | Default-off if no battery |
| R2, R3 | 4.7 kΩ | 0402/0603 | I2C pull-ups (SDA, SCL) → 3V3 | Add if bus lacks them |
| C1 | 100 nF | 0402/0603 | LIS3MDL VDD decoupling | Close to U2 |
| C2 | 100 nF | 0402/0603 | LIS3MDL VDD_IO decoupling | Close to U2 |
| C3 | 4.7–10 µF | 0805 | Bulk at battery input | Near J1/Q1 |
| TP1–6 | Test points | — | 3V3, GND, SDA, SCL, BAT, MAG_INT | Bring-up/debug |
| H1, H2 | Mounting holes | M2 | Mechanical | Optional |

---

## 4. Power tree

```
USB-C (on U1) ──► BQ25101 (on U1) ──► BAT node ──► U1 internal 3V3 LDO ──► 3V3 rail
                                          ▲                                  │
LiPo ──► J1 ──► Q1 (reverse-polarity) ────┘                                  ├─► U2 (LIS3MDL) VDD / VDD_IO
        (JST-PH)   S=BAT, D=J1+, G=GND                                       └─► R2/R3 pull-ups, C1/C2
```

- The **BAT node is bidirectional**: it both sources system power (discharge) and receives charge
  current from the onboard BQ25101 (USB plugged). Q1's enhanced channel conducts both directions, so
  protection does **not** block charging. Rds(on) drop is mΩ → no effect on charge termination or VBAT sensing.
- 3V3 for U2 comes from the **XIAO's 3V3 output pin** (do not feed U2 from BAT directly).

---

## 5. Net-by-net connections (schematic intent)

| Net | Connections |
|---|---|
| **3V3** | U1.3V3 → U2.VDD, U2.VDD_IO, R2(top), R3(top), C1, C2 |
| **GND** | U1.GND → U2.GND, Q1.Gate, R1, J1.(−), C1, C2, C3 |
| **BAT** | U1.BAT pad ↔ Q1.Source |
| **VBAT_CONN** | J1.(+) ↔ Q1.Drain ↔ C3 |
| **I2C_SDA** | U1.D4 (SDA) ↔ U2.SDA ↔ R2 |
| **I2C_SCL** | U1.D5 (SCL) ↔ U2.SCL ↔ R3 |
| **MAG_INT** (optional) | U1.D2 (free GPIO) ↔ U2.DRDY/INT |
| **U2.CS** | → VDD_IO (selects I2C mode) |
| **U2.SA1/SDO** | → GND (sets address 0x1C) |

> The onboard LSM6DS3TR-C (0x6A, P1.08 enable, P0.11 INT) is internal to U1 and is **not** routed on
> the carrier. Address 0x6A does not collide with the LIS3MDL (0x1C/0x1E) regardless of whether they
> share the user I2C bus — confirm bus mapping in `variant.h` for firmware, not for layout.

---

## 6. Reverse-polarity protection (Q1 detail)

High-side P-channel MOSFET, **logic-level** (gate is only driven by ~3.0–4.2 V from a 1S LiPo):

- **Source → module side (BAT pad)**, **Drain → connector +**, **Gate → GND**.
- Body diode points toward the load → conducts on connect, then the FET enhances (Vgs negative) to low Rds.
- Reverse polarity: Vgs ≈ 0 → FET off, body diode reverse-biased → blocks. Board protected.
- **R1 (100 kΩ)** gate-to-source for a defined default-off state.
- **No Vgs Zener needed** for 1S (Vgs ≤ 4.2 V is well inside the FET rating). Add a Zener only if going to 2S+.
- Part: **DMG2305UX** (Vgs(th) ≈ −0.45…−1.1 V, low Rds at −2.5 V) or **AO3401**. Current is trivial
  (~10–15 mA load + ≤100 mA charge), so the FET is not thermally constrained — pick for low threshold, not Id.

---

## 7. Placement & layout constraints

1. **Antenna keepout (highest priority for BLE range):** no copper / no ground pour under or adjacent to
   U1's onboard antenna edge. Let that edge overhang the board edge or honor Seeed's module keepout zone.
2. **Magnetometer isolation:** place U2 (LIS3MDL) in the "quietest" corner — as far as practical from
   (a) U1's antenna, (b) the battery + charge current path (J1, Q1, BAT/charge traces, C3),
   (c) USB-C, (d) any inductor / large current loop. The mag reads DC/AC magnetic fields, so high-current
   DC traces near it corrupt heading.
3. **Battery:** edge or under-board mount with foam tape; route its leads away from U2.
4. **Decoupling:** C1/C2 within ~2 mm of U2; pull-ups R2/R3 near the bus.
5. **I2C:** short, direct SDA/SCL; keep clear of the battery path.
6. **Access:** USB-C and the reset button must remain reachable; expose SWD pads if practical.
7. **Module footprint:** use Seeed's official land pattern (castellated edge + back SMD pads of the Plus).

---

## 8. PCB design rules & stackup (PCBWay / JLCPCB friendly)

- **Stackup:** 2-layer FR-4, 1.6 mm, 1 oz copper.
- **Min trace / space:** 0.2 mm (8 mil) target; 0.15 mm available if needed.
- **Min via:** 0.3 mm drill / 0.6 mm pad.
- **Finish:** **ENIG** recommended (clean fine-pitch LGA-12 soldering); HASL acceptable for cost.
- **Silkscreen:** mark **J1 battery polarity (+ / −)** unambiguously, module orientation, U2 pin 1.
- **Assembly options:** module is hand-/reflow-placed by you; LIS3MDL + passives can go through
  JLCPCB PCBA (parts in their library) or be hand-soldered with a stencil + hot air.

---

## 9. Flux.ai Copilot prompt (ready to paste)

> Design a compact 2-layer carrier PCB for a Seeed XIAO nRF52840 Sense Plus module mounted as an SMD
> component (use its castellated + back-pad land pattern; 21×17.5 mm). Add:
>
> 1. An ST LIS3MDL magnetometer (LGA-12) on the user I2C bus: connect XIAO D4=SDA and D5=SCL to the
>    LIS3MDL SDA/SCL, with 4.7 kΩ pull-ups to 3V3. Tie LIS3MDL CS to VDD_IO (I2C mode) and SA1/SDO to GND
>    (address 0x1C). Power LIS3MDL VDD and VDD_IO from the XIAO 3V3 pin with 100 nF decoupling on each.
>    Route the LIS3MDL DRDY/INT pin to XIAO D2.
> 2. A 2-pin JST-PH 2.0 mm battery connector with reverse-polarity protection: a high-side logic-level
>    P-channel MOSFET (DMG2305UX, SOT-23) with Source to the XIAO BAT pad, Drain to the JST positive pin,
>    Gate to GND, and a 100 kΩ gate-to-source resistor. Add a 4.7–10 µF bulk cap at the battery input.
>
> Constraints: keep a copper/ground keepout under the XIAO antenna edge; place the LIS3MDL far from the
> antenna, the battery connector, and the charge/current traces; short direct I2C traces; decoupling caps
> within 2 mm of the LIS3MDL. Add test points for 3V3, GND, SDA, SCL, BAT, and MAG_INT, plus two M2
> mounting holes. Silkscreen the JST battery polarity clearly. Target PCBWay/JLCPCB rules: 0.2 mm
> trace/space, 0.3/0.6 mm vias, 1 oz copper, ENIG finish.

---

## 10. Verify before fab (review checklist)

- [ ] Import the **official Seeed XIAO nRF52840 Sense Plus** footprint/land pattern (castellated + back SMD pads); confirm antenna keepout zone from Seeed reference.
- [ ] Confirm **user I2C pin mapping** (D4=SDA / D5=SCL → nRF GPIO) and that the LIS3MDL bus is correct.
- [ ] Confirm **BAT pad location** on the Plus (repositioned vs. standard) and its tie to BQ25101.
- [ ] Confirm **BQ25101 charge-current** setting and how to change it (default ≈ 50 mA) for the chosen battery.
- [ ] Verify **Q1 logic-level threshold** and Rds at Vgs = −2.5 V at the supplier; check stock/price for DMG2305UX vs AO3401.
- [ ] LIS3MDL application circuit per ST datasheet (CS→VDD_IO, SA1 address, VDD/VDD_IO decoupling).
- [ ] DRC/ERC clean in KiCad after Flux export; archive in Forgejo as source of truth.

---

## 11. Firmware delta (carrier-specific; transport unchanged)

- **Core:** Seeed/Adafruit nRF52 Arduino core (NOT mbed) — avoids the GCC 7.2 / arm64 (Asahi) build failure entirely. Keep a GitHub Actions build as canonical regardless.
- **BLE:** Adafruit Bluefruit; **keep the existing GATT service/characteristic UUIDs and the 20-byte (10× int16) packet** so the Web Bluetooth dashboard is untouched.
- **Accel+gyro:** drive **P1.08 high** to power the onboard LSM6DS3TR-C; read at 0x6A.
- **Magnetometer:** Adafruit_LIS3MDL on the user I2C bus.
- **Fusion:** reuse the existing Madgwick/Mahony `sensor_fusion.h`; **re-run hard/soft-iron + axis-sign calibration** for the LIS3MDL (same pattern already used for the LSM9DS1).
- **Sleep:** System OFF; wake on the LSM6DS3 **wake-on-motion** interrupt (P0.11), then re-advertise/reconnect.
- **Optional:** append VBAT (read via the XIAO onboard divider) to the BLE packet to show battery % in the lander HUD.
