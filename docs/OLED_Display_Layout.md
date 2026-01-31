# OLED Display Layout Documentation

## Hardware Specifications

| Property          | Value                          |
|-------------------|--------------------------------|
| Display Type      | SH1106 OLED                    |
| Size              | 1.3" (128x64 pixels)           |
| Library           | Adafruit_SH110X.h              |
| Class             | Adafruit_SH1106G               |
| I2C Address       | 0x3C                           |
| Color             | Monochrome (Blue/White)        |
| Text Size         | 1 (6x8 pixels per character)   |
| Max Characters    | 21 chars per line              |
| Max Lines         | 5 lines (with 13px spacing)    |

---

## Display Coordinate System

```
(0,0) ──────────────────────────────► X (127)
  │
  │   128 pixels wide
  │   ◄─────────────────────────────►
  │  ┌─────────────────────────────────┐
  │  │                                 │
  │  │         OLED SCREEN             │  64
  │  │         128 x 64                │  pixels
  │  │                                 │  tall
  │  │                                 │
  │  └─────────────────────────────────┘
  ▼
  Y (63)
```

---

## Startup Sequence Displays

### Screen 1: Initialization (2 seconds)

```
┌─────────────────────────────────┐
│                                 │
│      Hydroponic v3.0            │  Y=20
│                                 │
│      Initializing...            │  Y=35
│                                 │
│                                 │
└─────────────────────────────────┘
```

**When shown:** Immediately after power-on during sensor initialization

---

### Screen 2: System Ready (1.5 seconds)

```
┌─────────────────────────────────┐
│                                 │
│                                 │
│         System Ready!           │  Y=28 (centered)
│                                 │
│                                 │
│                                 │
└─────────────────────────────────┘
```

**When shown:** After successful initialization, before entering main loop

---

## Main Operating Display

### Layout Structure

```
┌─────────────────────────────────┐
│ pH: X.XX [STATUS]               │  Line 0 (Y=0)
│ TDS: XXXX ppm [STATUS]          │  Line 1 (Y=13)
│ Temp: XX.XC [STATUS]            │  Line 2 (Y=26)
│ Pump: ON/OFF                    │  Line 3 (Y=39)
│ Status: XXXXXXXXXXX             │  Line 4 (Y=52)
└─────────────────────────────────┘
```

### Line-by-Line Breakdown

| Line | Y Position | Content Format                  | Max Width |
|------|------------|--------------------------------|-----------|
| 0    | Y=0        | `pH: X.XX [STATUS]`            | 21 chars  |
| 1    | Y=13       | `TDS: XXXX ppm [STATUS]`       | 21 chars  |
| 2    | Y=26       | `Temp: XX.XC [STATUS]`         | 21 chars  |
| 3    | Y=39       | `Pump: ON` or `Pump: OFF`      | 10 chars  |
| 4    | Y=52       | `Status: XXXXXXXXXXX`          | 19 chars  |

---

## Status Messages by Scenario

### Line 0: pH Status Messages

| Condition           | Display Example          | Meaning                        |
|---------------------|--------------------------|--------------------------------|
| pH < 5.8 (PH_MIN)   | `pH: 5.42 LOW! ADD UP`   | Too acidic, add pH UP solution |
| pH > 6.5 (PH_MAX)   | `pH: 6.82 HIGH! ADD DN`  | Too alkaline, add pH DOWN      |
| 5.8 ≤ pH ≤ 6.5      | `pH: 6.12 OK`            | Optimal range for radish       |

### Line 1: TDS Status Messages

| Condition            | Display Example          | Meaning                     |
|----------------------|--------------------------|-----------------------------|
| TDS < 600 (TDS_MIN)  | `TDS: 450 ppm LOW`       | Nutrients too low           |
| TDS > 900 (TDS_MAX)  | `TDS: 1050 ppm HIGH`     | Nutrients too high          |
| 600 ≤ TDS ≤ 900      | `TDS: 745 ppm OK`        | Optimal range               |

### Line 2: Temperature Status Messages

| Condition               | Display Example        | Meaning                  |
|-------------------------|------------------------|--------------------------|
| Temp < 18°C (TEMP_MIN)  | `Temp: 15.5C COLD`     | Too cold for growth      |
| Temp > 26°C (TEMP_WARN) | `Temp: 28.2C HOT!`     | Dangerously hot          |
| 24°C < Temp ≤ 26°C      | `Temp: 25.1C WARM`     | Slightly warm            |
| 18°C ≤ Temp ≤ 24°C      | `Temp: 22.3C OK`       | Optimal range            |

### Line 3: Pump Status

| State | Display       | Meaning                              |
|-------|---------------|--------------------------------------|
| ON    | `Pump: ON`    | Water circulation pump is running    |
| OFF   | `Pump: OFF`   | Water circulation pump is idle       |

### Line 4: System Status Messages

| Status        | Display               | Scenario                              |
|---------------|-----------------------|---------------------------------------|
| Running       | `Status: Running`     | Normal operation                      |
| Dosing A      | `Status: Dosing A`    | Dispensing Nutrient A                 |
| Mixing A      | `Status: Mixing A`    | Waiting for Nutrient A to mix (3 min) |
| Dosing B      | `Status: Dosing B`    | Dispensing Nutrient B                 |
| Mixing B      | `Status: Mixing B`    | Waiting for Nutrient B to mix (5 min) |
| Refilling     | `Status: Refilling`   | Auto-refilling water (low level)      |

---

## Display Scenarios by System State

### Scenario 1: Normal Operation (All OK)

```
┌─────────────────────────────────┐
│ pH: 6.12 OK                     │
│ TDS: 745 ppm OK                 │
│ Temp: 22.3C OK                  │
│ Pump: ON                        │
│ Status: Running                 │
└─────────────────────────────────┘
```

**Trigger:** All sensor values within optimal ranges

---

### Scenario 2: Low Nutrients Detected

```
┌─────────────────────────────────┐
│ pH: 6.05 OK                     │
│ TDS: 520 ppm LOW                │
│ Temp: 21.5C OK                  │
│ Pump: ON                        │
│ Status: Dosing A                │
└─────────────────────────────────┘
```

**Trigger:** TDS < (TDS_MIN + TDS_TOLERANCE) = 650 ppm

**Sequence:**
1. `Status: Dosing A` → Pump A runs for 5 seconds
2. `Status: Mixing A` → Wait 3 minutes for mixing
3. `Status: Dosing B` → Pump B runs for 5 seconds
4. `Status: Mixing B` → Wait 5 minutes for mixing
5. `Status: Running` → Return to normal

---

### Scenario 3: Nutrient Mixing Phase A

```
┌─────────────────────────────────┐
│ pH: 6.15 OK                     │
│ TDS: 580 ppm LOW                │
│ Temp: 22.0C OK                  │
│ Pump: ON                        │
│ Status: Mixing A                │
└─────────────────────────────────┘
```

**Trigger:** After Nutrient A is dosed, water pump stays ON to mix

---

### Scenario 4: Nutrient Mixing Phase B

```
┌─────────────────────────────────┐
│ pH: 6.20 OK                     │
│ TDS: 650 ppm OK                 │
│ Temp: 22.0C OK                  │
│ Pump: ON                        │
│ Status: Mixing B                │
└─────────────────────────────────┘
```

**Trigger:** After Nutrient B is dosed, water pump stays ON to mix

---

### Scenario 5: Low Water Level (Auto-Refill)

```
┌─────────────────────────────────┐
│ pH: 5.80 OK                     │
│ TDS: 45 ppm LOW                 │
│ Temp: 23.1C OK                  │
│ Pump: OFF                       │
│ Status: Refilling               │
└─────────────────────────────────┘
```

**Trigger:** TDS ≤ 100 ppm (indicates TDS sensor not submerged)

**Duration:** 30 seconds of water refill pump operation

---

### Scenario 6: pH Too Low (Acidic)

```
┌─────────────────────────────────┐
│ pH: 5.42 LOW! ADD UP            │
│ TDS: 720 ppm OK                 │
│ Temp: 21.8C OK                  │
│ Pump: ON                        │
│ Status: Running                 │
└─────────────────────────────────┘
```

**Trigger:** pH < 5.8 (PH_MIN)

**Action Required:** Manually add pH UP solution (no auto-dosing in v3.0)

---

### Scenario 7: pH Too High (Alkaline)

```
┌─────────────────────────────────┐
│ pH: 6.95 HIGH! ADD DN           │
│ TDS: 810 ppm OK                 │
│ Temp: 22.5C OK                  │
│ Pump: ON                        │
│ Status: Running                 │
└─────────────────────────────────┘
```

**Trigger:** pH > 6.5 (PH_MAX)

**Action Required:** Manually add pH DOWN solution

---

### Scenario 8: Temperature Warning (Hot)

```
┌─────────────────────────────────┐
│ pH: 6.10 OK                     │
│ TDS: 780 ppm OK                 │
│ Temp: 28.5C HOT!                │
│ Pump: ON                        │
│ Status: Running                 │
└─────────────────────────────────┘
```

**Trigger:** Temperature > 26°C (TEMP_WARNING)

**Action Required:** Move system to cooler location or add shade

---

### Scenario 9: Multiple Warnings

```
┌─────────────────────────────────┐
│ pH: 5.55 LOW! ADD UP            │
│ TDS: 1120 ppm HIGH              │
│ Temp: 16.2C COLD                │
│ Pump: OFF                       │
│ Status: Running                 │
└─────────────────────────────────┘
```

**Trigger:** Multiple parameters out of range simultaneously

**Priority Actions:**
1. Address temperature first (affects plant health most)
2. Dilute nutrients if TDS too high
3. Adjust pH last (nutrients affect pH)

---

## Algorithm Flow Diagram

```
                    ┌─────────────┐
                    │   STARTUP   │
                    └──────┬──────┘
                           │
                           ▼
        ┌─────────────────────────────────────┐
        │  Display: "Hydroponic v3.0"         │
        │           "Initializing..."         │
        │  Duration: 2 seconds                │
        └──────────────────┬──────────────────┘
                           │
                           ▼
        ┌─────────────────────────────────────┐
        │  Display: "System Ready!"           │
        │  Duration: 1.5 seconds              │
        └──────────────────┬──────────────────┘
                           │
                           ▼
              ┌────────────────────────┐
              │     MAIN LOOP          │◄────────────────┐
              └───────────┬────────────┘                 │
                          │                              │
          ┌───────────────┼───────────────┐              │
          ▼               ▼               ▼              │
    ┌──────────┐   ┌──────────┐   ┌──────────┐          │
    │  Read    │   │  Update  │   │  Check   │          │
    │ Sensors  │   │ Display  │   │ Pump     │          │
    │ (5 sec)  │   │ (2 sec)  │   │ Timing   │          │
    └────┬─────┘   └────┬─────┘   └────┬─────┘          │
         │              │              │                 │
         └──────────────┴──────────────┘                 │
                        │                                │
                        ▼                                │
              ┌─────────────────┐                        │
              │ Check Water     │                        │
              │ Level (TDS≤100?)│                        │
              └────────┬────────┘                        │
                       │                                 │
           ┌───────────┴───────────┐                     │
           │ YES                   │ NO                  │
           ▼                       ▼                     │
    ┌──────────────┐      ┌──────────────┐              │
    │ Status:      │      │ Check        │              │
    │ "Refilling"  │      │ Nutrients    │              │
    │ Run 30 sec   │      │ (60 sec)     │              │
    └──────┬───────┘      └──────┬───────┘              │
           │                     │                       │
           │              ┌──────┴──────┐               │
           │              │ TDS < 650?  │               │
           │              └──────┬──────┘               │
           │           YES │     │ NO                   │
           │               ▼     │                      │
           │    ┌─────────────┐  │                      │
           │    │ DOSE A→MIX A│  │                      │
           │    │ DOSE B→MIX B│  │                      │
           │    └──────┬──────┘  │                      │
           │           │         │                      │
           └───────────┴─────────┴──────────────────────┘
```

---

## Display Update Timing

| Event                | Interval       | Description                     |
|----------------------|----------------|---------------------------------|
| Sensor Reading       | 5,000 ms       | pH, TDS, Temperature updated    |
| Display Refresh      | 2,000 ms       | OLED screen redrawn             |
| Nutrient Check       | 60,000 ms      | Evaluate if dosing needed       |
| Serial Debug Print   | 10,000 ms      | Print status to Serial Monitor  |

---

## Character Limits Reference

```
Font Size 1: 6 pixels wide × 8 pixels tall
Screen: 128 × 64 pixels

Maximum characters per line: 128 ÷ 6 = 21 characters
Maximum lines (8px height): 64 ÷ 8 = 8 lines
Recommended spacing: 13px per line (5 lines visible)

Line positions used:
  Line 0: Y = 0   (pixels 0-7)
  Line 1: Y = 13  (pixels 13-20)
  Line 2: Y = 26  (pixels 26-33)
  Line 3: Y = 39  (pixels 39-46)
  Line 4: Y = 52  (pixels 52-59)
```

---

## Wiring Reference

| OLED Pin | Arduino Pin | Description        |
|----------|-------------|--------------------|
| VCC      | 5V          | Power supply       |
| GND      | GND         | Ground             |
| SCL      | A5          | I2C Clock          |
| SDA      | A4          | I2C Data           |

---
