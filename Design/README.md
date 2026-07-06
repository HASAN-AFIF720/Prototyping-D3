# Prototyping-D3 — Mechanical Design (CAD)

This folder contains the custom mechanical parts for **Group D3's autonomous 2WD robot vehicle**, exported as **STEP** (ISO 10303) 3D models.

These parts are purpose-designed mounts rather than a generic kit chassis. The design goal is **alignment repeatability**: sensors sit exactly where the firmware expects them, and the motors are held rigid so the wheels react predictably. A stable mechanical base directly simplifies the control algorithm — the IR sensors stay at a fixed height and spacing, and the ultrasonic sensors keep a consistent forward-facing angle.

---

## Part Index

| # | File | Qty | Format | Purpose |
|---|------|-----|--------|---------|
| 1 | `GroupD3_2xIRSensor_6May2026.STEP` | 2 | STEP | Downward-facing IR line-sensor brackets (Left / Right) |
| 2 | `GroupD3_2xMotorholderPart1_6May2026.step` | 2 | STEP | DC gear-motor clamp — part 1 (Left / Right) |
| 3 | `GroupD3_2xMotorholderPart2_6May2026.step` | 2 | STEP | DC gear-motor clamp — part 2 (Left / Right) |
| 4 | `GroupD3_2xUltrasonicSensor_6May2026.STEP` | 2 | STEP | HC-SR04 ultrasonic mounts (Front-Left / Front-Right) |
| 5 | `GroupD3_8xPCBStandoff_6May2026.STEP` | 8 | STEP | Standoffs isolating and elevating the controller / driver boards |

**Total: 16 printed parts.** All exports dated 6 May 2026.

---

## Part Details

### 1 — IR Sensor Brackets (2×)
Hold the two IR line-tracking modules downward-facing at the front edge of the chassis. A fixed, equal ride-height on both sides keeps the digital line readings symmetric, which is what the steering-correction logic assumes.

### 2 & 3 — Motor Holder (2× Part 1 + 2× Part 2)
A two-piece clamp per motor that captures the DC gear motor body and bolts it rigidly to the chassis. Rigid mounting prevents the motor from shifting under load or during hard-reverse turns, protecting wheel alignment and reducing skid.

### 4 — Ultrasonic Sensor Mounts (2×)
Position the two HC-SR04 sensors at the front corners (Front-Left / Front-Right) with a consistent forward orientation, so the left/right distance comparison used for evasive turns stays meaningful.

### 5 — PCB Standoffs (8×)
Elevate and space the Arduino UNO and L298N driver above the chassis deck, keeping solder joints and pin headers clear of the surface and separating the logic board from the motor-driver board for cleaner wiring and easier fault checking.

---

## Mounts → Hardware Mapping

| Mechanical Part | Mounts | Electrical Interface |
|-----------------|--------|----------------------|
| IR Sensor Bracket | 2× IR line sensor | D12 (Left), D13 (Right) |
| Motor Holder (P1 + P2) | 2× DC gear motor | via L298N: EN D10/D11, IN D4–D7 |
| Ultrasonic Mount | 2× HC-SR04 | A2/A3 (Left), D2/D3 (Right) |
| PCB Standoff | Arduino UNO + L298N | — |

---

## How to Open / Manufacture

STEP is a neutral CAD exchange format and opens in any MCAD tool:

- **FreeCAD** (free, open source), **Autodesk Fusion 360**, **Onshape**, **SolidWorks** — for viewing, editing, or re-exporting.
- **Online:** any browser-based STEP/glTF viewer for a quick look without installing software.

To fabricate, import the STEP file, orient the part, and export a mesh (`.STL` / `.3MF`) for your slicer. STL exports also preview directly in GitHub's built-in 3D viewer if you add them to the repo.

---

## Team — Group D3

MD Afif Hasan · Rei Halilaj · Ambrose · Jubayer Ahmed

*Systems Engineering / Prototyping project — Hochschule Hamm-Lippstadt*
