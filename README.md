# Prototyping-D3 — Autonomous 2WD Robot Vehicle
 
![Platform](https://img.shields.io/badge/platform-Arduino%20UNO-00979D)
![Language](https://img.shields.io/badge/language-C%2B%2B-blue)
![Modeling](https://img.shields.io/badge/modeling-SysML-6f42c1)
![Status](https://img.shields.io/badge/status-Milestone%202%3A%20Obstacle%20Avoidance-orange)
 
An **untethered, two-wheel-drive robot car** that operates as a hybrid **line follower** and **obstacle avoider**. The vehicle follows a strict safety-first priority: obstacle detection (safety) always overrides line tracking (navigation).
 
![Assembled vehicle](vehicle%20assembly%20photo.png)
 
---
 
## Overview
 
Built for the Systems Engineering / Prototyping module at **Hochschule Hamm-Lippstadt**, this project pairs a formal **SysML model** with a physically assembled Arduino prototype. Every loop follows a *Sense → Think → Act* pipeline: read the ultrasonic sensors first, and only fall through to IR line tracking when the path ahead is clear.
 
**Key features**
 
- Hybrid **line following + obstacle avoidance** on a single Arduino UNO
- **Safety-first priority gate** — obstacle avoidance pre-empts navigation every cycle
- **Dual HC-SR04** front sensors for left/right clearance comparison and evasive turns
- **Dual downward IR** sensors for line tracking
- **Isolated power** — motor current separated from Arduino logic supply
- Full **SysML documentation** and **custom 3D-modeled mounts**
---
 
## Repository Structure
 
| Path | Contents |
|------|----------|
| [`Diagrams/`](Diagrams) | **Current** SysML diagram set (7 PNGs) with its own README |
| [`Design/`](Design) | Mechanical CAD parts (STEP) with its own README |
| `zz_diagrams_archive/` | Superseded diagrams — kept for history only, ignore (current set is in `Diagrams/`) |
| `stable_line_follower_final.ino` | Confirmed-working line-following firmware (Milestone 1) |
| `line_follower_fast_UNSTABLE.ino` | Experimental higher-speed variant — not yet stable |
| `CircuitDiagram.pdf` | Full wiring / circuit schematic |
| `vehicle assembly photo.png` | Photo of the assembled prototype |
 
---
 
## Hardware Architecture
 
| Component | Part | Role |
|-----------|------|------|
| Microcontroller | 1× Arduino UNO (Joy-IT), 5V logic | Central decision maker |
| Motor Driver | 1× L298N Dual H-Bridge | Drives motors; ENA/ENB jumpers removed for PWM |
| Actuators | 2× DC gear motors (rubber tread) | Left / right differential drive |
| Obstacle Sensors | 2× HC-SR04 ultrasonic | Front-Left / Front-Right distance |
| Tracking Sensors | 2× IR line sensors | Downward-facing line detection |
| Power | 12V / AA battery pack | Motor power isolated from logic power |
 
### Pin Map
 
| Subsystem | Signal | Arduino Pin |
|-----------|--------|-------------|
| IR line sensor — Left | digital | D12 |
| IR line sensor — Right | digital | D13 |
| Right motor | ENA (PWM) / IN1 / IN2 | D10 / D5 / D4 |
| Left motor | ENB (PWM) / IN3 / IN4 | D11 / D7 / D6 |
| Ultrasonic — Left | TRIG / ECHO | A2 / A3 |
| Ultrasonic — Right | TRIG / ECHO | D2 / D3 |
 
> The Left/Right IR pin assignment has differed between firmware revisions — confirm it against the sketch you flash before testing.
 
---
 
## Control Logic (Sense → Think → Act)
 
The `void loop()` executes a two-tier priority hierarchy:
 
**Priority 1 — Obstacle Avoidance (safety)**
Trigger both HC-SR04 sensors. If *either* reads an obstacle within the **15 cm** threshold, halt the motors, compare the left and right distances, and turn toward the side with greater open distance.
 
**Priority 2 — Line Tracking (navigation)**
If the path is clear, read the IR sensors and correct course — turn toward the sensor that detects the line, or drive straight when centered.
 
![Control algorithm](Diagrams/06_Activity_Diagram.png)
 
Full behavioral and architectural models are in [`Diagrams/`](Diagrams): Requirement, BDD, IBD, Use Case, Sequence, Activity, and Package diagrams.
 
---
 
## Firmware — Build & Upload
 
**Requirements:** [Arduino IDE](https://www.arduino.cc/en/software) (1.8+ or 2.x).
 
1. Open the target sketch (`stable_line_follower_final.ino` recommended).
2. Select **Tools → Board → Arduino UNO**.
3. Select the correct **Port** under **Tools → Port**.
4. Wire the hardware per the Pin Map above (or `CircuitDiagram.pdf`).
5. Power the L298N from the battery pack; **do not** back-feed motor voltage through the Arduino's USB.
6. Click **Upload**.
**Bench-test tips**
 
- Verify IR sensor polarity before driving — inverted line logic is the most common failure mode here.
- Confirm ultrasonic timing is intact (avoid Timer0 modifications, which distort `delay()`/`pulseIn()` used by HC-SR04).
- Test line following and obstacle avoidance in isolation before running the hybrid loop.
---
 
## Documentation
 
| Document | Location |
|----------|----------|
| SysML diagram set (7) | [`Diagrams/`](Diagrams) |
| Mechanical CAD (STEP) | [`Design/`](Design) |
| Circuit schematic | `CircuitDiagram.pdf` |
 
---
 
## Project Milestones
 
- **Milestone 1 — Lane Following** ✅ Vehicle follows the track via IR line detection.
- **Milestone 2 — Obstacle Avoidance** 🔄 HC-SR04 avoidance layered on the confirmed line-following base.
---
 
## Team — Group D3
 
| Member | GitHub |
|--------|--------|
| Md Afif Hasan | [@HASAN-AFIF720](https://github.com/HASAN-AFIF720) |
| Jubayer Ahmed | [@MainReturn0](https://github.com/MainReturn0) |
| Rei Halilaj | [@ReiHalilaj](https://github.com/ReiHalilaj) |
| Ambrose | — |
 
*Systems Engineering / Prototyping project — Hochschule Hamm-Lippstadt*
 
---
 
## License
 
Academic coursework, shared for educational reference. If you intend to reuse this work, please credit Group D3 and consider adding an explicit open-source license (e.g. MIT) to the repository.
