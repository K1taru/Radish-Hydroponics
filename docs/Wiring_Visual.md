# Hydroponic System - Visual Wiring Diagram

## ⚠️ SAFETY WARNING - AC 220V HIGH VOLTAGE ⚠️

```
╔══════════════════════════════════════════════════════════════════════════════╗
║    DANGER: This system uses AC 220V for the water pump!                      ║
║                                                                              ║
║  • ALWAYS disconnect from outlet before working on wiring                    ║
║  • Use proper insulation and electrical tape on all AC connections           ║
║  • Keep AC wiring away from water and moisture                               ║
║  • Consider using a GFCI/RCD outlet for safety                               ║
║  • If unsure, consult a licensed electrician                                 ║
╚══════════════════════════════════════════════════════════════════════════════╝
```

---

## Your Actual Power Configuration

| Relay | Pump | Power Source | Voltage |
|-------|------|--------------|---------|  
| R1 (D7) | Water Pump (15W) | AC Wall Outlet | **220V AC** ⚡ |
| R2 (D8) | Nutrient A (Peristaltic) | 12V DC Adapter | 12V DC |
| R3 (D9) | Nutrient B (Peristaltic) | 12V DC Adapter | 12V DC |
| R4 (D10)| Refill (Peristaltic)     | 12V DC Adapter | 12V DC |

---

## Complete System Overview (v2.0)

```
                                                        ┌─────────────────────────┐
        ┌──────────────┐                                │    12V DC ADAPTER       │
        │  AC 220V     │                                │    ┌───────────────┐    │
        │  OUTLET      │                                │    │   ████████    │    │
        │  (Wall)      │                                │    │   12V  1A     │    │
        │  ┌───────┐   │                                │    │   DC OUTPUT   │    │
        │  │ L │ N │   │                                │    └───────┬───────┘    │
        │  │ ● │ ● │   │                                │            │            │
        │  └─┬───┬─┘   │                                │     ┌──────┴──────┐     │
        └────┼───┼─────┘                                │     │ (+)     (-) │     │
             │   │                                      │     │ RED   BLACK │     │
        LIVE │   │ NEUTRAL                              │     └──┬───────┬──┘     │
             │   │                                      └────────┼───────┼────────┘
             │   │                                               │       └────┐
             │   ▼                                               │            ▼
             │ (from pump)                                       │          12V(-) (from all pump)
             ▼                                                   └─────────────────────┐
    ┌──────────────────────────────────────────────────────────────────────────────┐   |
    │                             4-CHANNEL RELAY MODULE                           │   |
    │                                                                              │   |
    │  ┌──────────────┐  ┌────────────┐  ┌────────────┐  ┌────────────┐            │   |
    │  │ RELAY 1      │  │  RELAY 2   │  │  RELAY 3   │  │  RELAY 4   │            │   |
    │  │ AC 220V      │  │  DC 12V    │  │  DC 12V    │  │  DC 12V    │            │   |
    │  │              │  │            │  │            │  │            │            │   |
    │  │ NC COM  NO   │  │NC COM  NO  │  │NC COM  NO  │  │NC COM  NO  │            │   |
    │  │  X  │   │    │  │ X  │   │   │  │ X  ║   │   │  │ X  ║   │   │            │   |
    │  └─────┼───┼────┘  └────┼───┼───┘  └────║───┼───┘  └────║───┼───┘            │   |
    │        │   │            │   ▼           ║   ▼           ║   ▼           ┌────┤   |
    │   AC   │   │            │  NO2          ║  NO3          ║  NO4     12V(-) ───┤   |
    │  LIVE ─┘   │            └───────────────┴───────────────┴───┐      to all    │   |
    │  goes      ▼                    │                           │      pump (-)  │   |
    │  here     NO1              U-CONNECTION                     │           └────┤   |
    │        (to pump)      12V(+)──►COM2◄═══►COM3◄═══►COM4       │                │   |
    │                                   (jumper)  (jumper)        │                │   |
    │                                                             └────────────────┼───┘ 12V(+)
    │                                                                              │
    │                                                                              │
    │  ┌─────────────────────────────────────────────────────────────────────────┐ │
    │  │   IN1   IN2   IN3   IN4   VCC   GND                                     │ │
    │  │    │     │     │     │     │     │                                      │ │
    │  └────┼─────┼─────┼─────┼─────┼─────┼──────────────────────────────────────┘ │
    │       │     │     │     │     │     │                                        │
    └───────┼─────┼─────┼─────┼─────┼─────┼────────────────────────────────────────┘
            │     │     │     │     │     │
            │     │     │     │     │     │            TO ARDUINO:
            │     │     │     │     │     └──────────► GND
            │     │     │     │     └────────────────► 5V
            │     │     │     └──────────────────────► D10
            │     │     └────────────────────────────► D9
            │     └──────────────────────────────────► D8
            └────────────────────────────────────────► D7


    ═══════════════════════════════════════════════════════════════════════════════
                                 PUMP CONNECTIONS
    ═══════════════════════════════════════════════════════════════════════════════

         From Relay NO1                  From Relay NO2, NO3, NO4
              │                                   │     │     │
              │                                   │     │     │
              ▼                                   ▼     ▼     ▼
    ┌─────────────────┐            ┌────────────────────────────────────────────┐
    │   WATER PUMP    │            │            3x PERISTALTIC PUMPS            │
    │    AC 220V      │            │                                            │
    │     (15W)       │            │  ┌──────────┐  ┌──────────┐  ┌──────────┐  │
    │                 │            │  │NUTRIENT A│  │NUTRIENT B│  │  REFILL  │  │
    │   ┌─────────┐   │            │  │  PUMP    │  │  PUMP    │  │  PUMP    │  │
    │   │         │   │            │  │  12V DC  │  │  12V DC  │  │  12V DC  │  │
    │   │   MOT   │   │            │  │          │  │          │  │          │  │
    │   │         │   │            │  │  (+)(-)  │  │  (+)(-)  │  │  (+)(-)  │  │
    │   └─────────┘   │            │  │   │  │   │  │   │  │   │  │   │  │   │  │
    │                 │            │  └───┼──┼───┘  └───┼──┼───┘  └───┼──┼───┘  │
    │    ●       ●    │            │      │  │          │  │          │  │      │
    │    │       │    │            │ from │  │     from │  │     from │  │      │
    │  LIVE    NEUT   │            │ NO2 ─┘  │     NO3 ─┘  │     NO4 ─┘  │      │
    │    │       │    │            │         │             │             │      │
    │    │       │    │            │         └─────────────┴─────────────┘      │
    └────┼───────┼────┘            │                       │                    │
         │       │                 │          All pump (-) to 12V GND           │
    from │       │                 └───────────────────────┼────────────────────┘
    NO1 ─┘       │                                         │
                 │                                         │
    AC NEUTRAL ──┘                                         └──────► To 12V Adapter (-)
    (direct from outlet, uncut)
```

**PUMP CONNECTIONS SUMMARY:**
```
    RELAY 1 (AC 220V):               RELAYS 2,3,4 (DC 12V):
    ─────────────────                ──────────────────────
    
    From Outlet LIVE ─► COM1         12V(+) ─► COM2═COM3═COM4 (U-connection)
    From NO1 ─► Water Pump           NO2 ─► Nut A Pump (+)
    Outlet NEUTRAL ─► Pump           NO3 ─► Nut B Pump (+)
    (direct, uncut)                  NO4 ─► Refill Pump (+)
                                     
                                     12V(-) spliced to all 3 pump (-)
```

---

## Arduino UNO R3 Pinout Diagram

```
                                                                       ▼     ▼     ▼     ▼    ▼   ▼
                                                                       │     │     │     │    │   │
                                                                       │     │     │     │    │   │
  ┌────────────────────────────────────────────────────────────────────┼─────┼─────┼─────┼────┼───┼────┐
  │                                                                    │     │     │     │    │   │    │
  │    ╔═══════════════════════════════════════════════════════════════╪═════╪═════╪═════╪════╪═══╪═╗  │
  │    ║                     ARDUINO UNO R3                            │     │     │     │    │   │ ║  │
  │    ║  ┌──────────────────────────────────────────────────────────┐ │     │     │     │    │   │ ║  │
  │    ║  │                    DIGITAL PINS                          │ │     │     │     │    │   │ ║  │
  │    ║  │  D0  D1  D2  D3  D4  D5  D6  D7  D8  D9  D10 D11 D12 D13 │ │     │     │     │    │   │ ║  │
  │    ║  │  ○   ○   ○   ○   ○   ○   ○   ●   ●   ●   ●   ○   ○   ○   │ │     │     │     │    │   │ ║  │
  │    ║  └─────────────────────────────┼───┼───┼───┼────────────────┘ │     │     │     │    │   │ ║  │
  │    ║                                │   │   │   │                  │     │     │     │    │   │ ║  │
  │    ║                                │   │   │   └──────────────────┼─────┼─────┼─────┘    │   │ ║  │
  │    ║                                │   │   └──────────────────────┼─────┼─────┘          │   │ ║  │
  │    ║                                │   └──────────────────────────┼─────┘                ▼   ▼ ║  │
  │    ║                                └──────────────────────────────┘                     5V  GND║  │
  │    ║                                                                                 (POWER PIN)║  │
  │    ║  ┌─────────────────────────────────────────────────────────────────────────────────────────╫─┐│
  │    ║  │                                POWER PINS                                               ║ ││
  │    ║  │  RESET   3.3V   5V   GND   GND   Vin                                                    ║ ││
  │    ║  │    ○      ○     ●     ●     ○     ○                                                     ║ ││
  │    ║  └─────────────────────────────────────────────────────────────────────────────────────────╫─┘│
  │    ║                                                                                            ║  │
  │    ║                                                                                            ║  │
  │    ║  ┌─────────────────────────────────────────────────────────────────────────────────────────╫─┐│
  │    ║  │                                 ANALOG PINS                                             ║ ││
  │    ║  │   A0   A1   A2   A3   A4   A5                                                           ║ ││
  │    ║  │    ●    ●    ●    ○    ●    ●                                                           ║ ││
  │    ║  └────┼────┼────┼────────┼────┼────────────────────────────────────────────────────────────╫─┘│
  │    ║       │    │    │        │    │                                                            ║  │
  │    ╚═══════╪════╪════╪════════╪════╪════════════════════════════════════════════════════════════╝  │
  │            │    │    │        │    │                                                               │
  └────────────┼────┼────┼────────┼────┼───────────────────────────────────────────────────────────────┘
               │    │    │        │    │
               │    │    │        │    │
               │    │    │        │    │
               ▼    │    │        ▼    ▼
    ┌──────────┐    │    │   ┌─────────────────────┐
    │pH SENSOR │    │    │   │  I2C DISPLAY        │
    │  MODULE  │    │    │   │  (LCD or OLED)      │
    ├──────────┤    │    │   ├─────────────────────┤
    │ VCC → 5V │    │    │   │ VCC → 5V            │
    │ GND → GND│    │    │   │ GND → GND           │
    │ OUT → A0 │    │    │   │ SDA → A4            │
    └──────────┘    │    │   │ SCL → A5            │
                    │    │   └─────────────────────┘
                    ▼    │
           ┌──────────┐  │   ┌─────────────────────────┐
           │TDS SENSOR│  │   │   NTC THERMISTOR        │
           │  MODULE  │  │   │   (10K, Waterproof)     │
           ├──────────┤  │   ├─────────────────────────┤
           │ VCC → 5V │  │   │                         │
           │ GND → GND│  │   │    5V ───[10kΩ]───┬── A2│
           │ OUT → A1 │  │   │                   │     │
           └──────────┘  │   │                 [NTC]   │
                         │   │                   │     │
                         └───┼───────────────────┤     │
                             │                  GND    │
                             └─────────────────────────┘
```

---

## Simplified Connection Table

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        ARDUINO UNO PIN CONNECTIONS                          │
├─────────────┬───────────────────────┬───────────────────────────────────────┤
│  ARDUINO    │      COMPONENT        │              WIRE COLOR               │
│    PIN      │                       │            (Suggested)                │
├─────────────┼───────────────────────┼───────────────────────────────────────┤
│     A0      │  pH Sensor Signal     │  🟢 Green                             │
│     A1      │  TDS Sensor Signal    │  🟡 Yellow                            │
│     A2      │  NTC Thermistor       │  🟡 Yellow                            │
│     A4      │  Display SDA (I2C)    │  🔵 Blue                              │
│     A5      │  Display SCL (I2C)    │  🟣 Purple                            │
│     D7      │  Relay IN1 (Water)    │  🟠 Orange                            │
│     D8      │  Relay IN2 (Nut A)    │  🟠 Orange                            │
│     D9      │  Relay IN3 (Nut B)    │  🟠 Orange                            │
│     D10     │  Relay IN4 (Refill)   │  🟠 Orange                            │
│     5V      │  All VCC connections  │  🔴 Red                               │
│             │  (Sensors,Display,Rly)│                                       │
│     GND     │  All GND connections  │  ⚫ Black                             │
│             │  (Sensors,Display,Rly)│                                       │
└─────────────┴───────────────────────┴───────────────────────────────────────┘
```

**Both Displays Used Simultaneously (I2C Bus Sharing):**
- **20x4 I2C LCD** (Address: 0x27) - Large text display
- **1.3" OLED SH1106** (Address: 0x3C) - High contrast display
- Both connect to same A4 (SDA) and A5 (SCL) pins

---

## 🔌 RELAY MODULE CONNECTION EXPLAINED

The 4-Channel Relay Module has **TWO SEPARATE CIRCUITS**:

### ⚡ Circuit 1: CONTROL SIDE (Arduino 5V Logic)
```
ARDUINO                  RELAY MODULE
                        ┌──────────────────┐
  5V   ───────────────► │ VCC (powers coil)│
  GND  ───────────────► │ GND (ground)     │
  D7   ───────────────► │ IN1 (signal)     │
  D8   ───────────────► │ IN2 (signal)     │
  D9   ───────────────► │ IN3 (signal)     │
  D10  ───────────────► │ IN4 (signal)     │
                        └──────────────────┘
```
**YES! VCC and GND on the relay module MUST connect to Arduino!**
- VCC draws ~80mA to power all 4 relay coils
- This is the "brain" that tells each relay when to turn ON/OFF

---

### ⚡ Circuit 2: SWITCHING SIDE (Pump Power)

**RELAY 1 - AC 220V Water Pump:**
```
AC 220V OUTLET          RELAY 1              WATER PUMP (15W)
                       ┌────────────────┐
  LIVE  ──────────────►│ COM  NO  NC    │
  (Brown)              │  │   │   X     │
                       │  └───┼───┘     │───► Pump LIVE wire (cut)
                       │   (switch)     │
                       └────────────────┘
  
  NEUTRAL ─────────────────────────────────► Pump NEUTRAL wire (uncut)
  (Blue)
```

**RELAYS 2,3,4 - DC 12V Peristaltic Pumps (U-Connection):**
```
12V ADAPTER             RELAY MODULE           PUMPS
                       ┌────────────────────────────────────┐
                       │  RELAY 2    RELAY 3    RELAY 4     │
  12V(+) ─────────────►│  COM ══════ COM ══════ COM         │ (U-connection)
                       │   │         │         │            │
                       │   NO        NO        NO           │
                       │   │         │         │            │
                       └───┼─────────┼─────────┼────────────┘
                           │         │         │
                           ▼         ▼         ▼
                        Nut A(+)  Nut B(+)  Refill(+)
  
  12V(-) ──────────────────────────────────► All 3 pump (-) wires (spliced)
```
**This side is electrically isolated from Arduino** - it just switches power to pumps

---

## 📋 STEP-BY-STEP: How to Wire the Relay Module

### Step 1: Connect Control Signals (Arduino → Relay)
```
Wire from Arduino    →    Wire to Relay Module
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  5V pin             →    VCC (red wire)
  GND pin            →    GND (black wire)
  Pin D7             →    IN1 (orange wire)
  Pin D8             →    IN2 (orange wire)
  Pin D9             →    IN3 (orange wire)
  Pin D10            →    IN4 (orange wire)
```

### Step 2: Connect Relay 1 - AC 220V Water Pump

**⚠️ HIGH VOLTAGE - EXTREME CAUTION!**
```
1. UNPLUG the pump from wall outlet!
2. Cut the pump's power cord (not too close to pump or plug)
3. Strip the wires - identify LIVE (brown/black) and NEUTRAL (blue)
4. NEUTRAL (Blue): Reconnect these two ends together (splice/connector)
5. LIVE (Brown/Black): 
   - Plug side wire → Relay COM1 terminal
   - Pump side wire → Relay NO1 terminal
6. Insulate all connections with electrical tape or heat shrink
```

**Simplified View:**
```
    OUTLET                RELAY 1              WATER PUMP
    ══════                ═══════              ══════════
    
    LIVE ─────────────► COM ──┐
     (Brown)                  │ (relay switch)
                         NO ──┴─────────────► Pump Wire 1 (LIVE side)
                         
    NEUTRAL ────────────────────────────────► Pump Wire 2 (Blue - uncut)
     (Blue)
```

### Step 3: Connect Relays 2,3,4 - DC 12V Peristaltic Pumps (U-Connection)

**Cut 12V adapter output wire and use U-connection to reduce wire clutter:**
```
12V Adapter (+) ───────────────────────────┐
                                           │
                                           ▼
    ┌──────────┐      ┌──────────┐      ┌──────────┐
    │   COM2   │══════│   COM3   │══════│   COM4   │
    │          │ ~3cm │          │ ~3cm │          │
    └────┬─────┘jumper└────┬─────┘jumper└────┬─────┘
         │                 │                 │
        NO2               NO3               NO4
         │                 │                 │
         ▼                 ▼                 ▼
     Nut A (+)         Nut B (+)        Refill (+)
```

**Split the 12V Ground (Negative) Wire:**
```
    12V Adapter (-) BLACK
          │
          │ CUT ✂️
          │
    ┌─────┴─────┐
    │  SPLICE   │  (Use wire nuts, Wago connectors, or terminal block)
    └─┬───┬───┬─┘
      │   │   │
      │   │   └────────► Refill Pump (-)
      │   └────────────► Nut B Pump  (-)
      └────────────────► Nut A Pump  (-)
```

---

## 🎨 PHYSICAL WIRING DRAWINGS

### Drawing 1: Arduino to Relay Module (6 Control Wires)

```
    ┌─────────────────────────────────────┐
    │           ARDUINO UNO R3            │
    │  ┌───────────────────────────────┐  │
    │  │ ○ ○ ○ ○ ○ ○ ○ ● ● ● ● ○ ○ ○   │  │  ← Digital Pins
    │  │              D7 D8 D9 D10     │  │
    │  └──────────────┼──┼──┼──┼───────┘  │
    │                 │  │  │  │          │
    │  ┌──────────────┼──┼──┼──┼───────┐  │
    │  │   ○    ○    ●    ●    ○    ○  │  │  ← Power Pins
    │  │ RESET 3.3V  5V  GND  GND  Vin │  │
    │  └─────────────┼────┼────────────┘  │
    │                │    │               │
    └────────────────┼────┼───────────────┘
                     │    │
    WIRES:           │    │
    ══════           │    │    ┌─ 🟠 Orange (D7 → IN1)
                     │    │    │  🟠 Orange (D8 → IN2)
      🔴 Red ───────┘     │    │  🟠 Orange (D9 → IN3)
      (5V → VCC)          │    │  🟠 Orange (D10 → IN4)
                          │    │
      ⚫ Black ───────────┘    │
      (GND → GND)              │
                               │
                     ┌─────────┴─────────┐
                     │                   │
                     ▼                   ▼
    ┌─────────────────────────────────────────────────────────────────┐
    │                     4-CHANNEL RELAY MODULE                      │
    │                                                                 │
    │   [RELAY 1]      [RELAY 2]      [RELAY 3]      [RELAY 4]        │
    │      💡             💡             💡             💡  ← LEDs  │
    │                                                                 │
    │   ┌─────────────────────────────────────────────────────────┐   │
    │   │  IN1    IN2    IN3    IN4    VCC    GND                 │   │
    │   │   ●      ●      ●      ●      ●      ●                  │   │
    │   └───┼──────┼──────┼──────┼──────┼──────┼──────────────────┘   │
    │       │      │      │      │      │      │                      │
    └───────┼──────┼──────┼──────┼──────┼──────┼──────────────────────┘
            │      │      │      │      │      │
            │      │      │      │      │      │
          🟠│   🟠│   🟠│    🟠│   🔴│    ⚫│
         from   from   from   from   from   from
          D7     D8     D9    D10     5V    GND
```

---

### Drawing 2: AC 220V Water Pump with Wall Outlet (Relay 1)

```
    ⚠️ HIGH VOLTAGE - 220V AC ⚠️
    
    
    ══════════════════════════════════════════════════════════════════════
    
           WALL OUTLET                                      WATER PUMP
          ┌──────────┐                                    ┌───────────┐
          │  ┌───┐   │                                    │  ～～～   │
          │  │ L │●──┼── LIVE (Brown) ──────┐             │ ～MOTOR～ │
          │  ├───┤   │                      │             │  ～～～   │
          │  │ N │●──┼── NEUTRAL (Blue) ────┼─────┐       │     ║     │
          │  ├───┤   │                      │     │       │  ┌──╨──┐  │
          │  │ ⏚ │●  │  (Earth - optional)  │     │       │  │INLET│  │
          │  └───┘   │                      │     │       └──┴─────┴──┘
          └──────────┘                      │     │            │
                                            │     │            │ Water flow
               PUMP POWER CORD              │     │            ▼
              (cut in middle)               │     │       ───────────
                    ✂️                      │     │
                                            │     │
        Plug side ─────┬──LIVE (Brown)──────┘     │
                       │                          │
                       └──NEUTRAL (Blue)──────────┼───────────────────┐
                                                  │                   │
                                                  │                   │
        Pump side ─────┬──LIVE (Brown)────────────┼───────┐           │
                       │                          │       │           │
                       └──NEUTRAL (Blue)──────────┼───────┼───────────┘
                                                  │       │      Reconnect
                                                  │       │      NEUTRAL
                                            ┌─────┴───────┴─────┐ (splice)
                                            │   RELAY MODULE    │
                                            │     RELAY 1       │
                                            │   ┌─────────┐     │
                                            │   │ NC      │     │
                   From plug LIVE ─────────►│   │ COM ────┼─────┼─► To pump LIVE
                   (Brown wire)             │   │ NO      │     │   (Brown wire)
                                            │   └─────────┘     │
                                            │                   │
                                            └───────────────────┘
    
    
    WIRE PATH (Follow the arrows):
    ═══════════════════════════════
    
        LIVE:     Outlet → Plug → [CUT] → COM1 → NO1 → [CUT] → Pump
        NEUTRAL:  Outlet → Plug → [CUT] → Splice together → [CUT] → Pump
                                         (bypasses relay)
    
    
    ⚠️ SAFETY REMINDER:
    ┌────────────────────────────────────────────────────────────────────┐
    │  • Only LIVE wire goes through relay (gets switched ON/OFF)        │
    │  • NEUTRAL wire is spliced directly (never goes through relay)     │
    │  • Always UNPLUG from wall before touching any wires!              │
    │  • Use electrical tape or heat shrink on all exposed connections   │
    └────────────────────────────────────────────────────────────────────┘
```

---

### Drawing 3: 12V DC Adapter with 3 Peristaltic Pumps (U-Connection)

```
    ┌─────────────────────────────────────────────────────────────────────┐
    │                                                                     │
    │    12V DC ADAPTER                                                   │
    │    ══════════════                                                   │
    │                                                                     │
    │    ┌───────────────┐                                                │
    │    │   ████████    │◄── Plugs into wall outlet (AC 220V input)      │
    │    │   ████████    │                                                │
    │    │   ████████    │                                                │
    │    │   ▓▓▓▓▓▓▓▓    │                                                │
    │    │   12V  1A     │◄── Check: at least 1A output for 3 pumps       │
    │    │   DC ADAPTER  │                                                │
    │    └───────┬───────┘                                                │
    │            │                                                        │
    │            │ Output cable                                           │
    │            │                                                        │
    │     ┌──────┴──────┐                                                 │
    │     │ ✂️ CUT HERE │                                                 │
    │     └──────┬──────┘                                                 │
    │            │                                                        │
    │     ┌──────┴──────┐                                                 │
    │     │ (+)     (-) │                                                 │
    │     │ RED   BLACK │                                                 │
    │     └──┬───────┬──┘                                                 │
    │        │       │                                                    │
    └────────┼───────┼────────────────────────────────────────────────────┘
             │       │
             │       │
             ▼       ▼
    
    
    ═══════════════════════════════════════════════════════════════════════
                            WIRING TO RELAY MODULE
    ═══════════════════════════════════════════════════════════════════════
    
    
       12V (+) RED                              12V (-) BLACK
           │                                        │
           │                                        │
           │                                   ┌────┴────┐
           │                                   │ SPLICE  │ (3-way split)
           │                                   │ ┌─┬─┬─┐ │
           │                                   └─┼─┼─┼─┘
           │                                     │ │ │
           │                                     │ │ └──────────────────┐
           │                                     │ └────────────┐       │
           │                                     │              │       │
           │                                     │              │       │
           ▼                                     │              │       │
    ┌───────────────────────────────────────────┐│              │       │
    │           4-CHANNEL RELAY MODULE          ││              │       │
    │                                           ││              │       │
    │   RELAY 1    RELAY 2    RELAY 3   RELAY 4 ││              │       │
    │   (AC 220V)  (DC 12V)   (DC 12V)  (DC 12V)││              │       │
    │                                           ││              │       │
    │   ┌─────┐    ┌─────┐    ┌─────┐   ┌─────┐ ││              │       │
    │   │ NC  │    │ NC  │    │ NC  │   │ NC  │ ││              │       │
    │   │ COM │    │ COM │════│ COM │═══│ COM │ ││              │       │
    │   │ NO  │    │ NO  │    │ NO  │   │ NO  │ ││              │       │
    │   └──┬──┘    └──┬──┘    └──┬──┘   └──┬──┘ ││              │       │
    │      │   12V(+)─┴─jumper──┴─jumper──┴     ││              │       │
    │      │          ▲                         ││              │       │
    │      │          │                         ││              │       │
    └──────┼──────────┼─────────────────────────┘│              │       │
           │          │                          │              │       │
    (AC)   │          └──────────────────────────┼──────────────┼───────┼──── 12V(+) RED
           │                                     │              │       │
           │                                     │              │       │
           ▼                                     ▼              ▼       ▼
    ┌────────────┐                   ┌────────────┐  ┌────────────┐  ┌────────────┐
    │            │                   │  NUTRIENT  │  │  NUTRIENT  │  │   REFILL   │
    │   WATER    │                   │    A       │  │    B       │  │   PUMP     │
    │   PUMP     │                   │   PUMP     │  │   PUMP     │  │            │
    │  (AC 220V) │                   │ (Perist.)  │  │ (Perist.)  │  │ (Perist.)  │
    │            │                   │            │  │            │  │            │
    │    ⚡🌊   │                   │   (+)(-)   │  │   (+)(-)   │  │   (+)(-)   │
    │            │                   │    │  │    │  │    │  │    │  │    │  │    │
    └────────────┘                   └────┼──┼────┘  └────┼──┼────┘  └────┼──┼────┘
                                          │  │            │  │            │  │
                                    from  │  │       from │  │       from │  │
                                    NO2 ──┘  │      NO3 ──┘  │      NO4 ──┘  │
                                             │               │               │
                                             └───────────────┴───────────────┘
                                                             │
                                                             │
                                                     All (-) to 12V GND
```



---

### Drawing 4: U-Connection Close-Up (Relay COM Terminals)

```
    U-CONNECTION DETAIL (Top View of Relay Screw Terminals)
    ═══════════════════════════════════════════════════════
    
    
                    12V Adapter (+) RED wire
                            │
                            │
                            ▼
    ┌─────────────────────────────────────────────────────────────┐
    │                                                             │
    │     RELAY 2          RELAY 3          RELAY 4               │
    │   ┌─────────┐      ┌─────────┐      ┌─────────┐             │
    │   │  ○ NC   │      │  ○ NC   │      │  ○ NC   │             │
    │   │         │      │         │      │         │             │
    │   │  ● COM ─┼──────┼─ ● COM ─┼──────┼─ ● COM  │             │
    │   │    ▲    │~3cm  │    ▲    │~3cm  │         │             │
    │   │    │    │jumper│    │    │jumper│         │             │
    │   │    │    │      │    │    │      │         │             │
    │   │  ● NO   │      │  ● NO   │      │  ● NO   │             │
    │   │    │    │      │    │    │      │    │    │             │
    │   └────┼────┘      └────┼────┘      └────┼────┘             │
    │        │                │                │                  │
    │        ▼                ▼                ▼                  │
    │     To Pump          To Pump          To Pump               │
    │     A (+)            B (+)            Refill (+)            │
    │                                                             │
    └─────────────────────────────────────────────────────────────┘
    
    
    JUMPER WIRE DETAIL:
    ════════════════════
    
         ┌──── ~3cm wire ────┐      ┌──── ~3cm wire ────┐
         │                   │      │                   │
      [COM2]══════════════[COM3]══════════════════[COM4]
         │                   ▲                         │
         │                   │                         │
         │            12V(+) enters                    │
         │            here (center)                    │
         │                                             │
       strip                                        strip
       wire                                         wire
       ends                                         ends
```

---

### ⚠️ IMPORTANT: Why Two Separate Power Supplies?

| Connection | Voltage | Purpose | Current Draw |
|------------|---------|---------|--------------|
| Arduino VCC/GND | 5V | Powers relay coils & logic | ~80-100mA |
| Relay COM terminals | 12V | Powers the pumps | ~2A peak |

**Key Point:** The Arduino's 5V output can only provide ~500mA max. Your pumps need 2A at 12V, which would instantly destroy the Arduino if connected directly. The relay acts as an **electrically isolated switch** - Arduino's 5V controls the relay coil, and the relay contacts switch the separate 12V pump power.

---

## 🔍 Inside the Relay Module (How It Works)

```
EACH RELAY CHANNEL HAS THIS STRUCTURE:

  FROM ARDUINO                    ISOLATED                  TO PUMP
  ════════════════               ════════════              ════════════
                                    
   5V ──►[LED]─┐                                         
               │                                         
   INx ───────►[Optocoupler]──►[Transistor]──►[Coil]    
               │                                │         
   GND ───────►│                                │         
               │                                │         
               └────────────────────────────────┘         
                     Control Side                         
                      (5V logic)                          
                                                          
                                    ║                     
                                    ║  Electrical        
                                    ║  Isolation         
                                    ║  (Magnetic)        
                                    ║                     
                                                          
                      Switching Side                      
                      (12V power)                         
                                    │                     
                      ┌─────────────┴─────────┐          
                      │     [Contacts]        │          
                      │                       │          
        12V+ ────────►│ COM                   │          
                      │  │                    │          
                      │  │  ┌─────────────────┼────────► Pump (+)
                      │  └──┤ NO (opens/closes)         
                      │     │                 │          
                      │     └─────────────────┼────────► (unused)
                      │       NC              │          
                      │                       │          
                      └───────────────────────┘          
                                                          
        Pump (-) ──────────────────────────────────────► 12V GND
```

**How it works:**
1. Arduino sends 5V signal to INx pin
2. Optocoupler activates (provides electrical isolation)
3. Transistor energizes the relay coil (electromagnet)
4. Magnetic field pulls the switch contact closed
5. COM connects to NO (Normally Open) terminal
6. 12V power flows: 12V+ → COM → NO → Pump (+) → Pump → 12V GND
7. When Arduino sends 0V (LOW), coil de-energizes and contact opens

---

## 📸 Real-World Relay Module Pin Layout

```
TOP VIEW OF 4-CHANNEL RELAY MODULE:
═══════════════════════════════════════════════════════════════

    ┌─────────────────────────────────────────────────────┐
    │  [RELAY 1]    [RELAY 2]    [RELAY 3]    [RELAY 4]   │
    │    LED          LED          LED          LED       │
    │     ●            ●            ●            ●        │
    │                                                     │
    │  Terminal Blocks (High Voltage Side - Pumps):       │
    │  ┌──────┐    ┌──────┐    ┌──────┐    ┌──────┐       │
    │  │ NC   │    │ NC   │    │ NC   │    │ NC   │       │
    │  │ COM  │    │ COM  │    │ COM  │    │ COM  │       │ ← 12V+ here
    │  │ NO   │    │ NO   │    │ NO   │    │ NO   │       │ ← Pumps here
    │  └──────┘    └──────┘    └──────┘    └──────┘       │
    │                                                     │
    │  Control Pins (Low Voltage Side - Arduino):         │
    │  ┌─────┬─────┬─────┬─────┬─────┬─────┐              │
    │  │ IN1 │ IN2 │ IN3 │ IN4 │ VCC │ GND │              │
    │  │  ●  │  ●  │  ●  │  ●  │  ●  │  ●  │              │
    │  └─────┴─────┴─────┴─────┴─────┴─────┘              │
    │    │     │     │     │     │     │                  │
    │    D7    D8    D9   D10   5V   GND  ← To Arduino    │
    │                                                     │
    │  [JD-VCC Jumper] ← Usually installed                │
    │                                                     │
    └─────────────────────────────────────────────────────┘
```

---

## Detailed Wiring Diagram - Sensors

```
                            5V POWER BUS (RED)
     ═══════════════════════════●═══════════════════════════════════════════════════════
                                │
            ┌───────────────────┼───────────────────┬───────────────────┬───────────────┐
            │                   │                   │                   │               │
            ▼                   ▼                   ▼                   ▼               ▼
      ┌─────────┐         ┌─────────┐         ┌─────────┐         ┌─────────┐    ┌─────────┐
      │ pH      │         │ TDS     │         │  NTC    │         │  LCD    │    │  OLED   │
      │ SENSOR  │         │ SENSOR  │         │THERMIST.│         │  20x4   │    │ SH1106  │
      │         │         │         │         │         │         │  I2C    │    │  I2C    │
      │ [VCC]●──┤         │ [VCC]●──┤         │  10kΩ   │         │ [VCC]●──┤    │ [VCC]●──┤
      │ [GND]●──┤         │ [GND]●──┤         │ ┌─[===]─┤         │ [GND]●──┤    │ [GND]●──┤
      │ [OUT]●──┼── A0    │ [OUT]●──┼── A1    │ │       │         │ [SDA]●──┼─┬──│ [SDA]●──┼── A4
      └─────────┘         └─────────┘         │ └───────┼── A2    │ [SCL]●──┼─┼──│ [SCL]●──┼── A5
            │                   │             │  [NTC]  │         └─────────┘ │  └─────────┘
            │                   │             │    │    │         Addr:0x27   │  Addr:0x3C
            │                   │             └────┼────┘               │     │        │
            │                   │                  │                    │     │        │
            ▼                   ▼                  ▼                    ▼     ▼        ▼
     ═══════════════════════════●═════════════════════════════════════════════════════════
                            GND BUS (BLACK)
```

**Both Displays Share I2C Bus (Different Addresses):**
- LCD uses address 0x27 (or 0x3F)
- OLED uses address 0x3C
- Both SDA lines connect together to A4
- Both SCL lines connect together to A5

**NTC Thermistor Voltage Divider:**
```
    5V ───[10kΩ Resistor]───┬─── A2 (to Arduino)
                            │
                          [NTC]
                            │
                           GND
```

---

## Detailed Wiring Diagram - Relay & Pumps (YOUR ACTUAL SETUP)

```
    AC 220V OUTLET                              12V DC ADAPTER
    ══════════════                              ══════════════
         │                                           │
    LIVE │ NEUTRAL                            (+)RED │ (-)BLACK
         │ │                                         │ │
         │ │                                    ┌────┘ └──────────────────┐
         │ │                                    │                         │
         ▼ │                                    ▼                         │
    ┌──────┼───────────────────────────────────────────────────────────┐  │
    │      │          4-CHANNEL RELAY MODULE                           │  │
    │      │                                                           │  │
    │  ┌───┼───────────┐ ┌─────────────┐ ┌─────────────┐ ┌───────────┐ │  │
    │  │ RELAY 1       │ │  RELAY 2    │ │  RELAY 3    │ │  RELAY 4  │ │  │
    │  │ ⚡AC 220V     │ │  DC 12V     │ │  DC 12V     │ │  DC 12V   │ │  │
    │  │ (Water Pump)  │ │  (Nut A)    │ │  (Nut B)    │ │  (Refill) │ │  │
    │  │               │ │             │ │             │ │           │ │  │
    │  │COM←AC LIVE    │ │ COM ◄═══════╪═╪═══► COM ◄═══╪═╪══► COM    │ │  │
    │  │NO →Pump LIVE  │ │ NO →Pump+   │ │ NO →Pump+   │ │ NO→Pump+  │ │  │
    │  │NC  (unused)   │ │ NC (unused) │ │ NC (unused) │ │NC(unused) │ │  │
    │  └───┬───────────┘ └──────┬──────┘ └──────┬──────┘ └─────┬─────┘ │  │
    │      │                    │               │              │       │  │
    │      │               U-CONNECTION: 12V(+) enters one COM,        │  │
    │      │               then jumps to adjacent COMs (~3cm wires)    │  │
    │      │                    │               │              │       │  │
    │  ┌───┴────────────────────┴───────────────┴──────────────┴────┐  │  │
    │  │   IN1   IN2   IN3   IN4   VCC   GND                        │  │  │
    │  │    │     │     │     │     │     │                         │  │  │
    │  └────┼─────┼─────┼─────┼─────┼─────┼─────────────────────────┘  │  │
    │       │     │     │     │     │     │                            │  │
    └───────┼─────┼─────┼─────┼─────┼─────┼────────────────────────────┘  │
            │     │     │     │     │     │       TO ARDUINO:             │
            │     │     │     │     │     └────► GND                      │
            │     │     │     │     └──────────► 5V                       │
            │     │     │     └────────────────► D10                      │
            │     │     └──────────────────────► D9                       │
            │     └────────────────────────────► D8                       │
            └──────────────────────────────────► D7                       │
                                                                          │
        ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐            │
        │  WATER   │  │NUTRIENT A│  │NUTRIENT B│  │  REFILL  │            │
        │  PUMP    │  │  PUMP    │  │  PUMP    │  │  PUMP    │            │
        │⚡AC 220V │  │ DC 12V   │  │ DC 12V   │  │ DC 12V   │            │
        │  (15W)   │  │(Perist.) │  │(Perist.) │  │(Perist.) │            │
        │          │  │          │  │          │  │          │            │
        │  ●   ●   │  │ (+)  (-) │  │ (+)  (-) │  │ (+)  (-) │            │
        └──┼───┼───┘  └──┬────┬──┘  └──┬────┬──┘  └──┬────┬──┘            │
           │   │         │    │        │    │        │    │               │
      From │   │    From │    │   From │    │   From │    │               │
      NO1 ─┘   │    NO2 ─┘    │   NO3 ─┘    │   NO4 ─┘    │               │
               │              │            │              │               │
    AC NEUTRAL ┘              └────────────┴──────────────┴───────────────┤
    (direct, uncut)                   All DC pump (-) to 12V GND ─────────┘
```

---

## 📊 COMPLETE WIRING SUMMARY TABLE

| Connection | From | To | Wire Color | Notes |
|------------|------|-----|------------|-------|
| **Control Side (Arduino → Relay)** |
| Signal | Arduino D7 | Relay IN1 | Orange | Water pump control |
| Signal | Arduino D8 | Relay IN2 | Orange | Nut A control |
| Signal | Arduino D9 | Relay IN3 | Orange | Nut B control |
| Signal | Arduino D10 | Relay IN4 | Orange | Refill control |
| Power | Arduino 5V | Relay VCC | Red | Powers relay coils |
| Ground | Arduino GND | Relay GND | Black | Common ground |
| **Relay 1 - AC Water Pump (220V)** |
| Live | Outlet LIVE | Relay COM1 | Brown | **⚠️ HIGH VOLTAGE!** |
| Switched | Relay NO1 | Pump wire | Brown | Cut from LIVE |
| Neutral | Outlet | Pump wire | Blue | Direct (uncut) |
| **Relays 2,3,4 - DC Peristaltic (12V)** |
| 12V+ | Adapter (+) | Relay COM3 | Red | Center COM |
| Jumper | COM2 | COM3 | Red | ~3cm wire |
| Jumper | COM3 | COM4 | Red | ~3cm wire |
| Pump+ | Relay NO2 | Nut A (+) | Red | Switched power |
| Pump+ | Relay NO3 | Nut B (+) | Red | Switched power |
| Pump+ | Relay NO4 | Refill (+) | Red | Switched power |
| 12V- | Adapter (-) | All pump (-) | Black | Spliced 3-way |

---

## Physical Layout Suggestion

```
       ┌───────────────────────────────────────────────────────────┐
       │                    ELECTRICAL BOX                         │
       │  ┌─────────────────────────────────────────────────────┐  │
       │  │                                                     │  │
       │  │   ┌──────────┐          ┌───────────────────────┐   │  │
       │  │   │ ARDUINO  │          │    RELAY MODULE       │   │  │
       │  │   │   UNO    │◄────────►│    (4-Channel)        │   │  │
       │  │   │          │  Signal  │                       │   │  │
       │  │   └────┬─────┘  Wires   └───────────┬───────────┘   │  │
       │  │        │                            │               │  │
       │  │        │                            │ To Pumps      │  │
       │  │   ┌────┴────┐                       │               │  │
       │  │   │BREADBOARD│                      │               │  │
       │  │   │ (Power  │                       │               │  │
       │  │   │  Dist.) │                       │               │  │
       │  │   └────┬────┘                       │               │  │
       │  │        │                            │               │  │
       │  │   Sensor Wires                      │               │  │
       │  │        │                            │               │  │
       │  └────────┼────────────────────────────┼───────────────┘  │
       │           │                            │                  │
       └───────────┼────────────────────────────┼──────────────────┘
                   │                            │
     ┌─────────────┴────────────┐    ┌──────────┴──────────┐
     │       TO RESERVOIR       │    │      TO PUMPS       │
     │                          │    │                     │
     │  • pH Sensor Probe       │    │  • Water Pump       │
     │  • TDS Sensor Probe      │    │  • Nutrient A Pump  │
     │  • Temp Sensor Probe     │    │  • Nutrient B Pump  │
     │                          │    │  • Refill Pump      │
     └──────────────────────────┘    └─────────────────────┘
```

---

## Color-Coded Wire Legend

| Color | Purpose | Voltage |
|-------|---------|---------|
| 🔴 **Red** | +5V Power (Arduino/Sensors) | 5V DC |
| ⚫ **Black** | Ground (All DC) | 0V |
| 🟢 **Green** | pH Sensor Signal | 0-5V Analog |
| 🟡 **Yellow** | TDS Sensor / NTC Thermistor Signal | 0-5V Analog |
| 🔵 **Blue** | I2C SDA (Display Data) / AC Neutral | Digital / 220V |
| 🟣 **Purple** | I2C SCL (Display Clock) | Digital |
| 🟠 **Orange** | Relay Control Signals | 0-5V Digital |
| 🔴 **Red (thick)** | +12V Power (Peristaltic Pumps) | 12V DC |
| ⚫ **Black (thick)** | 12V Ground (Peristaltic Pumps) | 0V |
| 🟤 **Brown (thick)** | AC LIVE (Water Pump) | ⚠️ 220V AC |
| 🔵 **Blue (thick)** | AC NEUTRAL (Water Pump) | ⚠️ 220V AC |

---

## Quick Reference - Pin Summary

```
   ARDUINO UNO R3
   ══════════════
        
   ┌─────────────────────────────────────────┐
   │  DIGITAL SIDE           ANALOG SIDE     │
   │                                         │
   │  D7  ──── Relay IN1     A0 ──── pH      │
   │  D8  ──── Relay IN2     A1 ──── TDS     │
   │  D9  ──── Relay IN3     A2 ──── NTC Temp│
   │  D10 ──── Relay IN4     A4 ──── SDA     │
   │                         A5 ──── SCL     │
   │                                         │
   │  5V  ──── All VCC (sensors,relay,disp)  │
   │  GND ──── All Ground connections        │
   └─────────────────────────────────────────┘
```

**Both Displays on Same I2C Bus:**
- LCD Address: 0x27 (or 0x3F)
- OLED Address: 0x3C
- Both connect to A4/A5 simultaneously

---

## Safety Checklist Before Power-On

### ⚠️ AC 220V SAFETY (CRITICAL!)
- [ ] **UNPLUG AC before ANY wiring changes!**
- [ ] AC LIVE goes through relay COM1 → NO1 only
- [ ] AC NEUTRAL goes DIRECTLY to pump (never through relay)
- [ ] All AC connections properly insulated (heat shrink/electrical tape)
- [ ] No exposed copper on AC wires
- [ ] Keep AC wires away from DC wires

### Arduino & Sensors (5V DC)
- [ ] All DC GND wires connected to common ground
- [ ] **Relay VCC connected to Arduino 5V** ✅
- [ ] **Relay GND connected to Arduino GND** ✅
- [ ] 5V connections only to low-power components (sensors, display, relay logic)
- [ ] 10kΩ series resistor installed for NTC thermistor voltage divider

### Peristaltic Pumps (12V DC with U-Connection)
- [ ] 12V adapter (+) connected to center COM (e.g., COM3)
- [ ] U-jumpers connecting COM2↔COM3↔COM4 (~3cm wires)
- [ ] Relay NO2, NO3, NO4 terminals connected to pump (+) wires
- [ ] All 3 pump (-) wires spliced to 12V adapter (-)

### General Safety
- [ ] No bare wires touching each other
- [ ] Electronics box is waterproof and elevated
- [ ] Test DC circuits BEFORE connecting AC

---

## ❓ Frequently Asked Questions

### Q1: Do I connect relay VCC/GND to Arduino or 12V supply?
**A:** Connect to **Arduino 5V and GND**. The relay module's control circuit runs on 5V. The 12V supply only connects to the COM terminals on the switching side.

### Q2: What if my Arduino keeps resetting when pumps turn on?
**A:** This means your 5V power supply is insufficient. Solutions:
- Use a dedicated 5V 1A power adapter (not USB)
- Add a 100µF capacitor between Arduino 5V and GND
- Ensure relay module VCC draws power from external 5V, not USB

### Q3: Can I power everything from one 12V supply?
**A:** Yes, but you need a **buck converter** (12V → 5V regulator):
```
12V Supply ─┬─► Buck Converter (12V→5V) ─► Arduino Vin
            │
            └─► Relay COM terminals (12V) ─► Pumps
```

### Q4: What's the JD-VCC jumper on the relay module?
**A:** 
- **Jumper INSTALLED** (default): VCC powers both logic and relay coils - simpler wiring
- **Jumper REMOVED**: Separate power for coils - better isolation, prevents Arduino resets

**For this project:** Keep jumper INSTALLED for simplicity.

### Q5: My relay clicks but pump doesn't run. Why?
**A:** Check these in order:

**For AC Water Pump (Relay 1):**
1. Is AC LIVE connected to relay COM1?
2. Is pump LIVE wire connected to relay NO1?
3. Is pump NEUTRAL connected directly to outlet NEUTRAL?
4. Test pump: plug directly into outlet (bypassing relay)

**For 12V Peristaltic Pumps (Relays 2-4):**
1. Is 12V+ connected via U-jumpers to all COM terminals?
2. Is pump (+) wire connected to correct relay NO terminal?
3. Is pump (-) wire spliced to 12V GND?
4. Test pump directly: connect pump (+) to 12V+ and pump (-) to 12V GND

### Q6: Can I use 3 relays for v1.0 instead of 4?
**A:** Yes! Just don't connect D10 or use relay channel 4. The code for v1.0 doesn't use pin 10.

### Q7: What if my relay module has different pin labels?
**A:** Common variations:
- `S1, S2, S3, S4` instead of `IN1-IN4` (same function)
- `+V` or `DC+` instead of `VCC` (same - connect to 5V)
- `DC-` instead of `GND` (same - connect to GND)

### Q8: How do I know if my relay is "active LOW" or "active HIGH"?
**A:** Most are **active LOW**:
- Arduino `digitalWrite(7, HIGH)` → Relay ON → LED lights up
- Arduino `digitalWrite(7, LOW)` → Relay OFF → LED goes dark

The code in this project is written for active LOW (standard).

---

## 🛠️ Quick Troubleshooting Guide

| Problem | Check This | Solution |
|---------|-----------|----------|
| **Relay doesn't click** | VCC/GND connected? | Connect relay VCC to Arduino 5V |
| | Signal wire connected? | Connect INx to correct Arduino pin |
| **Relay clicks but pump silent** | 12V power at COM? | Connect 12V+ to all COM terminals |
| | Pump wire to NO? | Move pump (+) wire to NO terminal |
| **Arduino resets randomly** | Power supply weak? | Use 5V 1A adapter, not USB |
| | Brown-out from pump surge? | Add capacitor near Arduino |
| **All relays click together** | Wiring short? | Check each INx goes to different pin |
| **Pump runs backwards** | Wrong polarity? | Swap pump (+) and (-) wires |

---

## 📊 Power Budget Summary

```
TOTAL SYSTEM POWER REQUIREMENTS:
═══════════════════════════════════════════════════════════

5V Circuit (Arduino side):
┌─────────────────────────────────────────────┐
│ Arduino Uno           50 mA                 │
│ LCD 20x4 I2C          20 mA                 │
│ pH Sensor              5 mA                 │
│ TDS Sensor             5 mA                 │
│ DS18B20 Temp           5 mA                 │
│ Relay Module (4ch)    80 mA                 │
├─────────────────────────────────────────────┤
│ TOTAL 5V:            165 mA ✅ OK           │
│ (Arduino 5V pin provides up to 500mA)       │
└─────────────────────────────────────────────┘

AC 220V Circuit (Water Pump via Relay 1):
┌─────────────────────────────────────────────┐
│ Water Pump (15W)      ~0.07 A @ 220V        │
│ ⚠️ Switched through Relay 1 COM→NO         │
│ NEUTRAL goes direct (never through relay)   │
└─────────────────────────────────────────────┘

12V DC Circuit (Peristaltic Pumps via Relays 2-4):
┌─────────────────────────────────────────────┐
│ Peristaltic A          0.2 A                │
│ Peristaltic B          0.2 A                │
│ Refill Pump            0.3 A                │
├─────────────────────────────────────────────┤
│ TOTAL 12V:           0.7 A peak             │
│ (With U-connection sharing one adapter)     │
│ Recommend: 12V 1A adapter (minimum)         │
└─────────────────────────────────────────────┘
```

---

## 🎯 TL;DR - The Critical Connections (YOUR ACTUAL SETUP)

### YES, Relay VCC and GND MUST Connect to Arduino!

```
┌─────────────────────────────────────────────────────────────────┐
│                    THE SIMPLE ANSWER:                           │
│                                                                 │
│  RELAY MODULE has 6 control pins that ALL go to ARDUINO:        │
│                                                                 │
│     Relay VCC  ──────────► Arduino 5V                           │
│     Relay GND  ──────────► Arduino GND                          │
│     Relay IN1  ──────────► Arduino D7  (AC Water Pump)          │
│     Relay IN2  ──────────► Arduino D8  (DC Nut A)               │
│     Relay IN3  ──────────► Arduino D9  (DC Nut B)               │
│     Relay IN4  ──────────► Arduino D10 (DC Refill)              │
│                                                                 │
│  That's 6 wires total from Arduino to Relay Module.             │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Visual Summary - YOUR AC + DC Setup with U-Connection:

```
        ARDUINO                RELAY MODULE                POWER & PUMPS
        ═══════                ════════════                ═════════════
        
         [5V]  ────────────►  [VCC]
         
         [GND] ────────────►  [GND]
         
         [D7]  ────────────►  [IN1]     [COM1]◄───────── AC LIVE (220V)
                                        [NO1] ────────► Water Pump (LIVE)
                                        [NC1]  unused
                                        Water Pump NEUTRAL → direct to outlet
         
         [D8]  ────────────►  [IN2]     [COM2]◄══╗
                                        [NO2] ───╫────► Nut A Pump(+)
                                        [NC2]    ║ unused
                                                 ║
         [D9]  ────────────►  [IN3]     [COM3]◄══╬═════ 12V(+) adapter
                                        [NO3] ───╫────► Nut B Pump(+)
                                        [NC3]    ║ unused
                                                 ║ U-JUMPERS
         [D10] ────────────►  [IN4]     [COM4]◄══╝ (~3cm wires)
                                        [NO4] ────────► Refill Pump(+)
                                        [NC4]  unused
         
                                        All 3 DC Pumps(-) spliced ──► 12V(-)
```

### U-Connection Explained:
```
    12V Adapter (+) ─────┐
                         ▼
    [COM2] ◄══ jumper ══[COM3]══ jumper ══► [COM4]
       │                   ▲                    │
       │          12V enters here               │
       │          (center COM)                  │
       ▼                                        ▼
    [NO2]→Pump A(+)                       [NO4]→Refill(+)
                   [NO3]→Pump B(+)
```

**Remember:** 
- **AC Circuit (Relay 1):** Switches mains voltage - LIVE through relay, NEUTRAL direct
- **DC Circuit (Relays 2-4):** U-connection shares one 12V+ wire across 3 relays
- The relay module acts like remote-controlled switches - Arduino provides the 5V control signal, relays switch high-power loads!

---

**END OF VISUAL WIRING GUIDE**

