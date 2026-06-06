# Firefly IMU Carrier — Workflow de diseño (Flux.ai ⇄ stack KiCad)

> Guía operativa para llevar el **carrier del Firefly Blue Ghost IMU** de la idea a
> los archivos de fabricación, comparando **Flux.ai (Copilot)** contra el
> **stack KiCad** (`kicad-pcb-stack`). Complementa `firefly-imu-carrier-design-brief.md`
> (ese es el *qué* se diseña; esto es el *cómo* se trabaja).
>
> Pensado para alguien nuevo en diseño de PCB. Léelo una vez de corrido; luego
> úsalo como referencia por fase.

---

## 0. La idea en 30 segundos

- Vas a hacer **el mismo carrier en dos herramientas a la vez** para comparar:
  - **Flux.ai** — EDA en la nube con copiloto AI; rápido para explorar.
  - **stack KiCad** — nuestras 6 fases con validación rigurosa y salida a fábrica.
- **KiCad es tu "fuente de verdad"** (lo dice tu brief), y archivas en **Forgejo**.
- **KiCad es el editor; Claude es el asistente.** Claude edita los archivos por
  debajo y tú abres KiCad para *ver* y aprobar. Nadie ordena ni sube nada solo:
  ese botón siempre lo aprietas tú.

---

## 1. El mapa: las 6 fases aplicadas a TU carrier

| Fase | Qué significa para este carrier | Comando / acción | Tú haces |
|---|---|---|---|
| **0. Init** | Preparar el proyecto KiCad en `~/repos/firefly-imu-node` | `/pcb-init` (o ya tienes el repo, ver §4) | confirmas constraints |
| **1. Piezas** | Conseguir U2 (LIS3MDL), Q1 (DMG2305UX/AO3401), J1 (JST-PH), R/C, y la huella del módulo XIAO | `/pcb-source` | apruebas la lista |
| **2. Esquemático** | Dibujar las conexiones de tu **tabla net-by-net** (brief §5) | "construye el esquemático" | lo revisas en KiCad |
| **3. Placa** | Colocar y rutear respetando keepout de antena + aislar el magnetómetro | "haz el layout" | guías la colocación |
| **4. Revisión** | ERC + DRC de fábrica + señal/potencia + EMC (tu checklist brief §10) | `/pcb-review --full` | decides qué arreglar |
| **5. Fab** | ZIP para JLCPCB/PCBWay (2 capas, ENIG, 0.2 mm) | `/pcb-fab jlcpcb` o `pcbway` | **subes y pagas tú** |

---

## 2. Antes de cada sesión (rutina)

```bash
cd ~/repos/firefly-imu-node
claude
```

Reglas de oro que el stack hace cumplir:

1. **Cierra la GUI de KiCad antes de que Claude escriba** el esquemático/placa.
   (Dos programas tocando el mismo archivo lo corrompen. El stack lo verifica.)
2. **FreeCAD es al revés:** para la caja 3D, FreeCAD debe estar **abierto** con su
   "RPC Server" iniciado (ver §7).
3. **Nunca es "listo para fab"** hasta que pasen *todos* los chequeos y tú lo hayas
   visto en KiCad. Si no, es "prototipo" — el stack usa esas etiquetas en serio.

---

## 3. ⚠️ El tema Flux.ai ⇄ KiCad — léelo antes de invertir el mes

Tu brief dice *"Flux.ai (Copilot) → review/round-trip source of truth in KiCad"*.
Ahí hay una trampa que conviene conocer:

- **Flux exporta bien lo de fabricación:** Gerbers (RS-274X), taladros (Excellon),
  BOM, pick-and-place y netlist IPC-D-356. Eso es estándar y lo lee cualquiera.
- **Pero el "round-trip" Flux → KiCad está flojo:** pasar el *proyecto/esquemático*
  de Flux a KiCad da resultados rotos según reportes de usuarios. Al revés
  (KiCad → Flux) sí funciona. Es decir, el round-trip que tu brief asume **no es
  limpio en la dirección que necesitas.**

### Qué hacer en la práctica (dos caminos, tú comparas)

- **Camino A — KiCad como origen (recomendado para tu "source of truth"):**
  NO dependas del export de Flux para meter el diseño en KiCad. En su lugar,
  Claude **re-captura el esquemático directamente en KiCad desde tu tabla
  net-by-net** (brief §5) — que ya está escrita con el detalle exacto que el stack
  necesita. Así KiCad es de verdad la fuente de verdad, sin pasar por el puente roto.
- **Camino B — Flux como origen:** diseñas en Flux con su copiloto (pega el prompt
  de tu brief §9), y **solo validas lo que Flux exporta bien**: Gerbers contra las
  reglas de fábrica, y el BOM contra distribuidores. Los chequeos eléctricos
  fuertes (ERC, topología, señal/potencia, EMC) **no** se pueden correr sobre un
  export roto, así que ahí el stack te ayuda poco.

> **Conclusión honesta:** para *este* proyecto, el stack te da el máximo valor en el
> **Camino A**. Flux te da velocidad y un copiloto visual para explorar y para
> comparar. Trátalos como dos orígenes separados; no asumas que el diseño de Flux
> entra entero a KiCad para la revisión completa.

---

## 4. Camino A — diseñar en el stack KiCad (paso a paso)

### Fase 0 — Proyecto
Tu repo `~/repos/firefly-imu-node` **ya existe** y tiene arranque de KiCad
(`fp-lib-table`, `sym-lib-table`, `lib/`). Dos opciones:
- **Si quieres la estructura completa del stack:** dejas que Claude corra
  `claude-eda init` *dentro* del repo (no borra tus `.kicad_*`) y luego **ajustamos
  el `.mcp.json` a tu PC** (ver §7 — importante por el tema del Python).
- **Si prefieres mínimo:** Claude crea el `.kicad_pro/.kicad_sch/.kicad_pcb` con
  mixelpixx y reusa tus tablas de librerías.

Confirma los constraints del brief §8: **2 capas, FR-4 1.6 mm, 1 oz, 0.2 mm
trace/space, vías 0.3/0.6 mm, ENIG**, fab JLCPCB o PCBWay.

### Fase 1 — Piezas (`/pcb-source`)
Pídele a Claude sourcing de las **adiciones del carrier** (brief §3), p. ej.:

> *"Busca: LIS3MDL (LGA-12), DMG2305UX y alternativa AO3401 (SOT-23), JST-PH 2.0 mm
> 2-pin, y los pasivos 0402/0603 (R1 100 k, R2/R3 4.7 k, C1/C2 100 nF, C3 4.7–10 µF).
> Prioriza piezas básicas de LCSC con stock."*

Para el **módulo XIAO nRF52840 Sense Plus (U1)**: no es una pieza de catálogo
normal, va como **módulo SMD** con la **land pattern oficial de Seeed**
(castellated + pads traseros). Pídele a Claude que busque esa huella o la prepare;
confírmala contra la referencia de Seeed (brief §10, primer check).

Apruebas la tabla (MPN, precio, stock, datasheet) antes de seguir.

### Fase 2 — Esquemático
> *"Construye el esquemático desde la tabla net-by-net del brief (sección 5)."*

Claude coloca U1, U2, Q1, J1, R/C y conecta exactamente:
`3V3`, `GND`, `BAT`, `VBAT_CONN`, `I2C_SDA`, `I2C_SCL`, `MAG_INT`, y los amarres
`U2.CS→VDD_IO`, `U2.SA1→GND` (dirección 0x1C). Luego verifica la topología.
**Abre KiCad y revisa** que U2 cuelgue del 3V3 del XIAO (no del BAT) y que Q1 esté
como en el brief §6 (S→BAT, D→J1+, G→GND).

### Fase 3 — Placa (layout)
Tú guías la colocación con tus constraints (brief §7), díselo así:

> *"Coloca el XIAO con su borde de antena hacia el borde de la placa, sin cobre ni
> plano debajo de la antena. Pon el LIS3MDL en la esquina más lejana de la antena,
> del conector de batería y de las pistas de carga. C1/C2 a menos de 2 mm de U2.
> I2C corto y directo. Dos agujeros M2."*

El **keepout de antena** y la **aislación del magnetómetro** son lo más importante
aquí — un mag cerca de corriente DC lee mal el rumbo.

### Fase 4 — Revisión (`/pcb-review --full`)
Corre todos los analizadores y te da un reporte por severidad. Mapea tu
checklist del brief §10:
- **ERC** (mixelpixx) — nets flotantes, energía cruzada.
- **DRC de fábrica** (kicad-happy/jlcpcb o pcbway) — 0.2 mm trace, vías, ENIG.
- **Señal/potencia** (Seeed) — planos de tierra, decoupling, caminos de corriente.
- **EMC** (kicad-happy/emc) — desacoplo, ruteo, bordes.
- Cierra cada casilla de tu §10 (huella Seeed, mapeo I2C D4/D5, pad BAT, ajuste de
  carga BQ25101, umbral de Q1, circuito de aplicación del LIS3MDL).

### Fase 5 — Fab (`/pcb-fab`)
Genera el paquete (Gerbers + taladros + BOM + posiciones + DRC de fábrica) en un
ZIP. **Tú** entras al portal de JLCPCB/PCBWay, subes y pagas. El stack **no** sube
nada. Archiva el resultado en **Forgejo** como tu fuente de verdad.

---

## 5. Camino B — diseñar en Flux.ai (paso a paso)

1. En Flux, crea el proyecto y pega el **prompt listo** de tu brief §9 en el copiloto.
2. Itera visualmente (el copiloto coloca y rutea; tú corriges).
3. Exporta el paquete de fabricación (Gerbers, taladros, BOM, pick-and-place).
4. **Validación parcial con el stack:**
   - Gerbers → pásalos por las reglas de fábrica (JLCPCB/PCBWay).
   - BOM → compáralo contra distribuidores (pcbparts / @jlcpcb/mcp).
   - *(Recuerda §3: los chequeos eléctricos completos no aplican sobre el export.)*

---

## 6. Cómo comparar los dos (tu objetivo del mes)

Lleva una tablita simple y rellénala al final:

| Criterio | Flux.ai | Stack KiCad |
|---|---|---|
| Velocidad para el primer esquemático | | |
| Calidad/control del ruteo | | |
| Qué tan bien respeta keepout de antena + aislación del mag | | |
| Profundidad de validación (ERC/DRC/SI-PI/EMC) | | |
| Esfuerzo para llegar a Gerbers correctos | | |
| Costo final del lote y claridad del BOM | | |
| ¿Lo volverías a usar? | | |

Mismo brief, dos caminos → decides con datos, no por impresión.

---

## 7. Apéndice: configuración específica de TU PC

Tu máquina es **Fedora Asahi (aarch64), KiCad 10.0.3**. Hay un detalle clave:
tu `python3` por defecto es el de **PlatformIO** (sin `pcbnew`), y `pcbnew` solo se
importa desde `/usr/bin/python3.14`. Por eso el `.mcp.json` del proyecto debe
apuntar a los intérpretes correctos. Este es el bloque que debe quedar:

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

Notas:
- **mixelpixx** necesita `KICAD_PYTHON=/usr/bin/python3.14` para hallar `pcbnew`
  (si no, agarra el Python de PlatformIO y falla).
- **seeed** corre desde su venv aislado `~/.venvs/seeed/bin/python` (ve `pcbnew`,
  tiene `fastmcp` + el server). El bare `python3` daría modo básico (sin señal/pot.).
- **pcbparts** es HTTP en la nube — no se instala, no necesita API key para JLCPCB.
- **freecad** (cajas 3D, opcional): además de esto, abre FreeCAD → workbench
  **"MCP Addon"** → **"Start RPC Server"**. El addon ya está copiado en el sandbox
  de Flatpak. Sin ese paso manual, el server de FreeCAD no conecta.

Estado de la instalación del stack en esta PC: KiCad 10.0.3 ✓, uv/bun ✓,
claude-eda ✓, mixelpixx ✓, Seeed ✓, ngspice 46 ✓, FreeCAD 1.1.1 + addon ✓.

---

## 8. Glosario mínimo (PCB para nuevos)

- **Esquemático** — el "diagrama" de qué se conecta con qué (las nets).
- **Net** — un cable lógico (p. ej. `3V3`, `GND`, `I2C_SDA`).
- **Footprint / huella** — la marca física donde se suelda una pieza en la placa.
- **Layout / ruteo** — colocar las huellas y dibujar las pistas de cobre.
- **ERC** — chequeo eléctrico (pines sueltos, energía cruzada).
- **DRC** — chequeo de fabricación (ancho de pista, taladros, distancias).
- **Gerber** — los "planos" de cada capa que entiende la fábrica.
- **BOM** — lista de materiales (qué piezas y cuántas).
- **Pick-and-place / CPL / centroid** — dónde y con qué rotación va cada pieza (para
  ensamblaje automático).
- **Stackup** — cómo se apilan las capas (aquí: 2 capas, FR-4 1.6 mm, 1 oz).
- **ENIG / HASL** — acabados del cobre; ENIG es más fino y limpio (mejor para el
  LGA-12 del LIS3MDL).
- **Keepout** — zona donde *no* debe haber cobre (aquí: bajo la antena del XIAO).
- **Pour / plano** — relleno de cobre (típicamente GND).
- **Via** — agujero metalizado que conecta capas.

---

## 9. Reglas de seguridad (no negociables)

1. **GUI de KiCad cerrada** antes de que Claude escriba (FreeCAD, abierto, al revés).
2. **Nunca "fab-ready"** sin: ERC ok, DRC de fábrica ok, señal/EMC sin bloqueos, y tu
   visto bueno visual en KiCad. Si falta algo, es "prototipo".
3. **Nadie ordena ni sube** por ti. El stack genera el ZIP; tú lo subes al portal.
4. **Datasheets:** si una afirmación eléctrica no está respaldada por el datasheet
   cacheado, el stack dirá "solo consistencia", no "verificado". Créelo.
```
