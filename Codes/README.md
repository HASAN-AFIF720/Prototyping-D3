# Prototyping-D3 — Firmware (Arduino Sketches)

This folder contains the Arduino C++ firmware for **Group D3's autonomous 2WD robot vehicle**. Three sketches are kept here, tracking the project from a stable line-following baseline to the final hybrid line-following + obstacle-avoidance build.

> **For grading, open `GroupD3_AutonomousVehicle_Final.ino`** — it is the complete, submission version.

---

## Sketch Index

| File | Role | Milestone |
|------|------|-----------|
| `GroupD3_AutonomousVehicle_Final.ino` | **Final submission** — hybrid line following + ultrasonic obstacle avoidance | Milestone 2 |
| `GroupD3_LineFollower_Baseline.ino` | Stable, confirmed-working line follower (the base the final builds on) | Milestone 1 |
| `GroupD3_LineFollower_Fast_Experimental.ino` | Experimental higher-speed line-following variant (reference only, not graded) | — |

---

## `GroupD3_AutonomousVehicle_Final.ino` — Final Build

The complete vehicle firmware. It runs a *Sense → Think → Act* loop with a strict **safety-first priority**: obstacle avoidance is evaluated first every cycle and pre-empts line tracking.

```
loop():
  update ultrasonic readings
  read line sensors
  if obstacle detected  ->  avoid obstacle, then return   // safety first
  else                  ->  follow line                    // navigation
```

- **Line following:** proportional (KP) steering with signal filtering, dead-zone, and corner detection for smooth tracking and reliable cornering.
- **Obstacle avoidance:** triggers at the **15 cm** safety threshold, executes a timed evasive manoeuvre around the object, then re-acquires and rejoins the line. Includes distinct handling for a second obstacle (timed spin, spin-until-line, realign, resume).
- **Robust sensing:** ultrasonic reads use an echo timeout and a post-avoidance cooldown to reject spurious triggers.

## `GroupD3_LineFollower_Baseline.ino` — Stable Baseline

Pure IR line following with no obstacle sensing — the confirmed-working foundation (Milestone 1). Uses a dual-speed strategy (full speed on straights, reduced speed on curves) with a hard-reverse inner wheel for reliable turns, and raises the motor PWM frequency for smooth low-speed control. This is the known-good reference the final build extends.

## `GroupD3_LineFollower_Fast_Experimental.ino` — Experimental

A higher-speed line-following variant retained for reference. It is **not** the graded build and may behave less predictably at speed.

---

## Hardware & Pin Map

All sketches target an **Arduino UNO** driving an **L298N** dual H-bridge (ENA/ENB jumpers removed for PWM).

| Subsystem | Signal | Pin |
|-----------|--------|-----|
| IR line sensor — Left | digital | D12 |
| IR line sensor — Right | digital | D13 |
| Right motor | ENA (PWM) / IN1 / IN2 | D10 / D5 / D4 |
| Left motor | ENB (PWM) / IN3 / IN4 | D11 / D7 / D6 |
| Ultrasonic — Left | TRIG / ECHO | A2 / A3 |
| Ultrasonic — Right | TRIG / ECHO | D2 / D3 |

**IR polarity:** `HIGH` = line detected. **Obstacle threshold:** 15 cm.

---

## Build & Upload

1. Open the sketch in the [Arduino IDE](https://www.arduino.cc/en/software) (1.8+ or 2.x).
2. **Tools → Board → Arduino UNO**, then select the correct **Port**.
3. Power the L298N from the battery pack; do **not** back-feed motor voltage through the Arduino's USB.
4. **Upload**.

> Tip: in the Arduino IDE a `.ino` is expected to sit in a folder of the same name. If the IDE prompts to create one when opening, allow it — it does not change the code.

---

*Group D3 — Systems Engineering / Prototyping, Hochschule Hamm-Lippstadt*
