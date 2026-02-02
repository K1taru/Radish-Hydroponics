# Hydroponic System Wiring Guide

Complete wiring documentation for Radish Hydroponic System v1.0 and v2.0

---

## Table of Contents
1. [Components Overview](#components-overview)
2. [Version Differences](#version-differences)
3. [Power Requirements](#power-requirements)
4. [Arduino Uno Pin Assignments](#arduino-uno-pin-assignments)
5. [Wiring Diagrams](#wiring-diagrams)
6. [Step-by-Step Wiring Instructions](#step-by-step-wiring-instructions)
7. [Relay Module Configuration](#relay-module-configuration)
8. [Sensor Connections](#sensor-connections)
9. [Testing & Verification](#testing--verification)
10. [Troubleshooting](#troubleshooting)

---

## Components Overview

### Electronics & Control
| Component | Quantity | Purpose |
|-----------|----------|---------|
| Arduino Uno R3 | 1 | Main microcontroller |
| 4-Channel Relay Module | 1 | Pump control (5V trigger, high-current switching) |
| 20x4 I2C LCD Display | 1 | Real-time monitoring (Address: 0x27) |
| 1.3" OLED SH1106 Display | 1 | Real-time monitoring (Address: 0x3C) |
| Breadboard | 1 | Prototyping connections |
| Jumper Wires (M-M, M-F) | Assorted | Connections |

### Sensors
| Component | Quantity | Type | Connection |
|-----------|----------|------|------------|
| pH Sensor Module | 1 | Analog | Pin A0 |
| TDS Sensor V1.0 | 1 | Analog | Pin A1 |
| 10K NTC Thermistor | 1 | Analog | Pin A2 |

### Actuators
| Component | Quantity (v1) | Quantity (v2) | Purpose |
|-----------|---------------|---------------|---------|
| Water Circulation Pump (15W) | 1 | 1 | Tower irrigation |
| Peristaltic Pump (Nutrient A) | 1 | 1 | Nutrient dosing |
| Peristaltic Pump (Nutrient B) | 1 | 1 | Nutrient dosing |
| Water Refill Pump | 0 | 1 | Auto water refill |

### Power Supply
| Component | Specification | Purpose |
|-----------|---------------|---------|
| 12V DC Power Supply | 2A minimum | Pumps (via relay) |
| 5V DC Power Supply | 1A minimum | Arduino + sensors |
| USB Cable or DC Barrel Jack | - | Arduino power |

### Additional Components
| Component | Purpose |
|-----------|---------|
| 10kΩ Resistor | NTC thermistor voltage divider |
| Terminal Blocks | Power distribution |
| Wire Connectors | Secure connections |
| Heat Shrink Tubing | Insulation |

---

## Version Differences

### v1.0 (3-Relay Configuration)
- **Pumps**: 3 total
  - Water circulation pump (Pin 7)
  - Nutrient A pump (Pin 8)
  - Nutrient B pump (Pin 9)
- **Relay Module**: 3 channels used
- **Features**: Basic nutrient dosing, manual water refill

### v2.0 (4-Relay Configuration)
- **Pumps**: 4 total
  - Water circulation pump (Pin 7)
  - Nutrient A pump (Pin 8)
  - Nutrient B pump (Pin 9)
  - **Water refill pump (Pin 10)** ⭐ NEW
- **Relay Module**: All 4 channels used
- **Features**: Automatic water level detection & refill via TDS sensor

**Upgrade Path**: v1.0 → v2.0 requires adding 1 pump and 1 wire connection to relay channel 4

---

## Power Requirements

### Total Power Budget

**v1.0:**
- Arduino Uno: 5V @ 50mA
- LCD Display: 5V @ 20mA
- Sensors (pH, TDS, Temp): 5V @ 15mA total
- Relay Module: 5V @ 80mA (4-channel)
- **5V Total**: ~165mA (well under 500mA USB limit)
- Water Pump: 12V @ 1.25A (15W)
- Peristaltic Pumps (2×): 12V @ 200mA each
- **12V Total**: ~1.65A peak

**v2.0:**
- Same as v1.0, plus:
- Water Refill Pump: 12V @ 300mA
- **12V Total**: ~1.95A peak

### Recommended Power Setup

**Option 1: Dual Power Supply (Recommended)**
```
5V 1A Wall Adapter → Arduino Uno (DC barrel jack or Vin pin)
                  └→ Powers sensors and LCD automatically via 5V pin

12V 2A Wall Adapter → Terminal Block → Relay COM terminals
                                    → Pump power (switched via relay)
```

**Option 2: Single 12V Supply with Regulator**
```
12V 3A Supply → Buck Converter (12V→5V) → Arduino Vin
              └→ Terminal Block → Relay COM terminals → Pumps
```

### Safety Notes
⚠️ **NEVER** power pumps directly from Arduino pins (max 40mA per pin)  
⚠️ Always use relay module for pump control  
⚠️ Ensure common ground between Arduino and 12V supply  
⚠️ Use proper gauge wire for 12V pump circuits (18-20 AWG minimum)

---

## Arduino Uno Pin Assignments

### Complete Pin Map

| Pin | Type | Function | Component | Version |
|-----|------|----------|-----------|---------|
| **A0** | Analog | pH Sensor Input | pH Sensor Signal | v1, v2 |
| **A1** | Analog | TDS Sensor Input | TDS Sensor Signal | v1, v2 |
| **A2** | Analog | Temperature Input | NTC Thermistor | v1, v2 |
| **A4** | I2C (SDA) | Display Data | LCD/OLED SDA | v1, v2 |
| **A5** | I2C (SCL) | Display Clock | LCD/OLED SCL | v1, v2 |
| **D7** | Digital Output | Water Pump Control | Relay 1 (IN1) | v1, v2 |
| **D8** | Digital Output | Nutrient A Control | Relay 2 (IN2) | v1, v2 |
| **D9** | Digital Output | Nutrient B Control | Relay 3 (IN3) | v1, v2 |
| **D10** | Digital Output | Refill Pump Control | Relay 4 (IN4) | **v2 only** |
| **5V** | Power | +5V Output | Sensors, Display, Relay VCC | v1, v2 |
| **GND** | Ground | Common Ground | All components | v1, v2 |
| **Vin** | Power Input | 7-12V DC Input | External power (optional) | v1, v2 |

### Unused Pins (Available for Expansion)
- D0, D1 (reserved for Serial - USB communication)
- D2, D3, D4, D5, D6, D11, D12, D13
- A3

---

## Wiring Diagrams

### System Architecture Overview
```
                        ┌─────────────────────┐
                        │   Arduino Uno R3    │
                        │                     │
    ┌───────────────────┤ A0  pH Sensor       │
    │   ┌───────────────┤ A1  TDS Sensor      │
    │   │   ┌───────────┤ D2  Temp Sensor     │
    │   │   │           │                     │
    │   │   │       ┌───┤ A4  LCD SDA         │
    │   │   │       │┌──┤ A5  LCD SCL         │
    │   │   │       ││  │                     │
    │   │   │       ││  │ D7  Relay IN1       │───┐
    │   │   │       ││  │ D8  Relay IN2       │───┤
    │   │   │       ││  │ D9  Relay IN3       │───┤
    │   │   │       ││  │ D10 Relay IN4 (v2)  │───┤
    │   │   │       ││  │                     │   │
    │   │   │       ││  │ 5V ─────────────────┼───┼──┬─── Sensors VCC
    │   │   │       ││  │ GND ────────────────┼───┼──┼─── Common GND
    │   │   │       ││  └─────────────────────┘   │  │
    │   │   │       ││                             │  │
    │   │   │     ┌─▼▼──────────────┐             │  │
    │   │   │     │  20x4 I2C LCD   │             │  │
    │   │   │     │  (Address 0x27) │             │  │
    │   │   │     └─────────────────┘             │  │
    │   │   │                                      │  │
    │   │ ┌─▼────────────┐  ┌───────┐            │  │
    │   │ │ DS18B20      │──┤4.7kΩ├─────────────5V │
    │   │ │ Temp Sensor  │  └───────┘                │
    │   │ └──────────────┘                           │
    │   │                                            │
    │ ┌─▼──────────┐                                 │
    │ │ TDS Sensor │                                 │
    │ │  Module    │                                 │
    │ └────────────┘                                 │
    │                                                │
  ┌─▼──────────┐                                     │
  │ pH Sensor  │                                     │
  │  Module    │                                     │
  └────────────┘                                     │
                                                     │
  ┌──────────────────────────────────────────────┐  │
  │         4-Channel Relay Module               │◄─┘
  │  ┌────┬────┬────┬────┐                       │
  │  │ R1 │ R2 │ R3 │ R4 │  VCC ─── 5V           │
  │  │IN1 │IN2 │IN3 │IN4 │  GND ─── GND          │
  │  └─┬──┴─┬──┴─┬──┴─┬──┘                       │
  │    │    │    │    │                           │
  │  ┌─▼────▼────▼────▼──┐                       │
  │  │  COM NO NC (×4)   │                       │
  └──┴──┬───┬───┬───┬────┴───────────────────────┘
       │   │   │   │
       │   │   │   └───► Water Refill Pump (v2)
       │   │   └───────► Peristaltic Pump B
       │   └───────────► Peristaltic Pump A
       └───────────────► Water Circulation Pump
```

### v1.0 Simplified Wiring
```
Arduino          Relay Module         Pumps (12V)
┌─────┐          ┌──────────┐         
│ D7  ├──────────┤IN1    R1 ├─────┬──► Water Pump
│ D8  ├──────────┤IN2    R2 ├─────┼──► Nutrient A
│ D9  ├──────────┤IN3    R3 ├─────┼──► Nutrient B
│ 5V  ├──────────┤VCC       │     │    
│ GND ├──────────┤GND       │     │    
└─────┘          └──────────┘     │
                                  │
                 12V Supply ──────┴─── 12V GND
```

### v2.0 Simplified Wiring
```
Arduino          Relay Module         Pumps (12V)
┌─────┐          ┌──────────┐         
│ D7  ├──────────┤IN1    R1 ├─────┬──► Water Pump
│ D8  ├──────────┤IN2    R2 ├─────┼──► Nutrient A
│ D9  ├──────────┤IN3    R3 ├─────┼──► Nutrient B
│ D10 ├──────────┤IN4    R4 ├─────┼──► Refill Pump ⭐
│ 5V  ├──────────┤VCC       │     │    
│ GND ├──────────┤GND       │     │    
└─────┘          └──────────┘     │
                                  │
                 12V Supply ──────┴─── 12V GND
```

---

## Step-by-Step Wiring Instructions

### Phase 1: Arduino Power & Ground Distribution

1. **Power Setup**
   - Connect 5V power source to Arduino (USB or DC barrel jack)
   - Alternatively, connect 7-12V to Vin pin
   - **Important**: Ensure stable power supply for sensors

2. **Ground Bus Setup** (Using Breadboard)
   ```
   Arduino GND ──► Breadboard GND Rail
                  │
                  ├──► pH Sensor GND
                  ├──► TDS Sensor GND
                  ├──► DS18B20 GND
                  ├──► LCD GND
                  ├──► Relay Module GND
                  └──► 12V Power Supply GND (common ground!)
   ```

3. **5V Power Bus Setup**
   ```
   Arduino 5V ──► Breadboard 5V Rail
                 │
                 ├──► pH Sensor VCC
                 ├──► TDS Sensor VCC
                 ├──► NTC Thermistor (via 10kΩ resistor)
                 ├──► Display VCC (LCD or OLED)
                 └──► Relay Module VCC
   ```

### Phase 2: I2C Displays (Both LCD AND OLED)

Both displays connect to the same I2C bus (A4/A5) but use different addresses.

**20x4 LCD with I2C backpack** (PCF8574 or similar)

| LCD Pin | Arduino Pin | Wire Color (suggested) |
|---------|-------------|------------------------|
| VCC | 5V | Red |
| GND | GND | Black |
| SDA | A4 | Blue |
| SCL | A5 | Yellow |

**Notes for LCD**:
- Default I2C address: 0x27 (may be 0x3F on some modules)
- Adjust contrast potentiometer on I2C backpack if display is blank

**1.3" OLED SH1106 Display** (128x64 pixels)

| OLED Pin | Arduino Pin | Wire Color (suggested) |
|----------|-------------|------------------------|
| VCC | 5V | Red |
| GND | GND | Black |
| SDA | A4 | Blue |
| SCL | A5 | Yellow |

**Notes for OLED**:
- Default I2C address: 0x3C (different from LCD!)
- No backlight or contrast adjustment needed

**Wiring Both Displays Together**:
```
Arduino A4 (SDA) ────┬──── LCD SDA
                     └──── OLED SDA

Arduino A5 (SCL) ────┬──── LCD SCL
                     └──── OLED SCL

Arduino 5V ──────────┬──── LCD VCC
                     └──── OLED VCC

Arduino GND ─────────┬──── LCD GND
                     └──── OLED GND
```

**Important**: Both displays share the I2C bus but have different addresses (LCD: 0x27, OLED: 0x3C), so they work simultaneously without conflict.

### Phase 3: NTC Thermistor Temperature Sensor

**Components**: 10K NTC Thermistor, 10kΩ resistor

**Voltage Divider Circuit**:
```
         10kΩ
    5V ───/\/\/\───┬─── A2 (Analog Input)
                   │
                [NTC 10K]
                   │
                  GND
```

| Connection | Arduino Pin | Wire Color |
|------------|-------------|------------|
| Resistor to 5V | 5V | Red |
| Junction point | A2 | Yellow |
| NTC to Ground | GND | Black |

**Waterproof Version Wiring** (probe style):
| Wire | Connection |
|------|------------|
| One wire | To A2 junction (with 10kΩ to 5V) |
| Other wire | GND |

**Notes**:
- 10kΩ series resistor is **required** for voltage divider
- NTC resistance decreases as temperature increases
- Calibration constants in code: B=3950, R25=10kΩ
- Accuracy: ±1°C typical (after calibration)
- Waterproof probes available for submersion

### Phase 4: pH Sensor Module

**Components**: Analog pH sensor module with BNC connector

**Connections**:
| pH Module Pin | Arduino Pin | Notes |
|---------------|-------------|-------|
| VCC (or +) | 5V | Power input |
| GND (or -) | GND | Ground |
| Signal (or OUT) | A0 | Analog output |

**Important**:
- Store pH probe in KCl storage solution when not in use
- Calibrate with pH 4.0 and 7.0 buffer solutions
- Probe has limited lifespan (6-12 months typical)
- Keep BNC connector clean and dry

### Phase 5: TDS Sensor Module

**Components**: TDS sensor V1.0 with probe

**Connections**:
| TDS Module Pin | Arduino Pin | Notes |
|----------------|-------------|-------|
| VCC (or +) | 5V | Power input |
| GND (or -) | GND | Ground |
| A (or OUT) | A1 | Analog output |

**Probe Placement**:
- Submerge probe at least 1-2 cm below surface
- Ensure probe contacts are wet
- Clean probe regularly with distilled water
- In v2.0: Position at minimum desired water level for auto-refill trigger

**Notes**:
- Temperature compensation is performed in software
- Reading of 0-100 ppm indicates probe not submerged (v2.0 refill trigger)
- Normal range: 600-900 ppm with nutrients

### Phase 6: 4-Channel Relay Module

**Components**: 5V relay module with optocoupler isolation

**Control Signal Connections**:
| Relay Input | Arduino Pin | Function | Version |
|-------------|-------------|----------|---------|
| IN1 | D7 | Water Pump | v1, v2 |
| IN2 | D8 | Nutrient A Pump | v1, v2 |
| IN3 | D9 | Nutrient B Pump | v1, v2 |
| IN4 | D10 | Refill Pump | v2 only |
| VCC | 5V | Module power | v1, v2 |
| GND | GND | Module ground | v1, v2 |

**Relay Module Configuration**:
- **Jumper Setting**: Ensure VCC and JD-VCC jumper is installed (uses Arduino 5V)
- **Trigger Type**: Active LOW (relay ON when Arduino pin is HIGH)

**High-Current Switching** (Pump Side):

Each relay channel has 3 terminals:
- **COM** (Common): Connect to 12V+ from power supply
- **NO** (Normally Open): Connect to pump positive wire
- **NC** (Normally Closed): Leave disconnected

**Wiring Pattern** (Per Channel):
```
12V Supply (+) ──► COM
                   │
                   NO ──► Pump (+)
                   │
                   NC (unused)

Pump (-) ──────────────► 12V Supply (-)
```

**Complete Relay Wiring**:
```
Channel 1 (IN1/D7):
  COM ← 12V+
  NO → Water Pump (+)
  
Channel 2 (IN2/D8):
  COM ← 12V+
  NO → Nutrient A Pump (+)
  
Channel 3 (IN3/D9):
  COM ← 12V+
  NO → Nutrient B Pump (+)
  
Channel 4 (IN4/D10) [v2 only]:
  COM ← 12V+
  NO → Refill Pump (+)

All Pump (-) wires → 12V Supply (-)
```

**Safety Notes**:
- ⚠️ Relay module switches HIGH voltage/current - handle carefully
- COM terminals share 12V+ (can daisy-chain if terminals support it)
- Use proper terminal blocks for secure connections
- Check relay ratings (typically 10A @ 250VAC, 10A @ 30VDC)

### Phase 7: Pump Connections

**Water Circulation Pump** (15W, 12V):
```
(+) Wire → Relay 1 NO terminal
(-) Wire → 12V Supply Ground
```

**Peristaltic Pump A** (Nutrient A):
```
(+) Wire → Relay 2 NO terminal
(-) Wire → 12V Supply Ground
```

**Peristaltic Pump B** (Nutrient B):
```
(+) Wire → Relay 3 NO terminal
(-) Wire → 12V Supply Ground
```

**Water Refill Pump** (v2 only):
```
(+) Wire → Relay 4 NO terminal
(-) Wire → 12V Supply Ground
```

**Pump Tubing Setup**:
- Water Pump: Intake from reservoir → Outlet to tower top
- Nutrient A: Intake from bottle A → Outlet to reservoir
- Nutrient B: Intake from bottle B → Outlet to reservoir
- Refill Pump: Intake from water source → Outlet to reservoir

### Phase 8: Final Connections & Cable Management

1. **Double-Check All Connections**:
   - [ ] All GND connections share common ground
   - [ ] No loose wires or exposed connections
   - [ ] Relay COM terminals connected to 12V+
   - [ ] All pump (-) wires connected to 12V GND
   - [ ] Sensor VCC pins connected to 5V
   - [ ] I2C connections (SDA to A4, SCL to A5)

2. **Cable Management**:
   - Bundle signal wires separately from power wires
   - Use zip ties or cable clips
   - Label wires with masking tape
   - Keep water sensors away from electronics

3. **Waterproofing**:
   - Place Arduino and relay module in waterproof enclosure
   - Use cable glands for wires entering enclosure
   - Mount pumps above water level (or use check valves)
   - Seal sensor connections with heat shrink tubing

---

## Relay Module Configuration

### Understanding Relay Operation

**Active LOW Logic** (Standard Configuration):
- Arduino pin LOW (0V) → Relay OFF → Pump OFF
- Arduino pin HIGH (5V) → Relay ON → Pump ON

**LED Indicators**:
- Power LED: Module is powered
- Channel LED: Lights when relay is activated

### Jumper Settings

**JD-VCC and VCC Jumper**:
- **Jumper Installed**: Relay coil powered from Arduino 5V (simpler, for low-power relays)
- **Jumper Removed**: Relay coil powered from separate supply (better isolation, use for high-power loads)

**Recommended Setting**: Jumper INSTALLED for this project (total relay coil draw ~80mA)

### Relay Contact Ratings

Typical specifications:
- **10A @ 250VAC** (resistive load)
- **10A @ 30VDC** (resistive load)
- **6A @ 250VAC** (inductive load - motors/pumps)

**Our Application**:
- Water pump: 15W @ 12V = 1.25A ✅
- Peristaltic pumps: ~200-300mA each ✅
- Well within safe operating limits

---

## Sensor Connections

### pH Sensor Calibration Setup

**Required Materials**:
- pH 4.0 buffer solution
- pH 7.0 buffer solution
- Distilled water
- Beakers or cups

**Calibration Process**:
1. Rinse probe with distilled water
2. Place in pH 7.0 buffer
3. Record voltage output from Serial Monitor
4. Place in pH 4.0 buffer
5. Record voltage output
6. Calculate pH_OFFSET in code:
   ```
   Expected pH at 7.0: Voltage should be ~2.5V
   If voltage is different, adjust PH_OFFSET
   ```

### TDS Sensor Calibration Setup

**Required Materials**:
- TDS calibration solution (1413 µS/cm or 707 ppm)
- Thermometer
- Beaker

**Calibration Process**:
1. Prepare calibration solution at 25°C
2. Submerge TDS probe completely
3. Record reading from Serial Monitor
4. Calculate TDS_K value:
   ```
   TDS_K = actual_ppm / measured_ppm
   Example: 707 / 650 = 1.088
   ```
5. Update `#define TDS_K` in code

### DS18B20 Temperature Sensor

**No Calibration Needed**:
- Factory calibrated to ±0.5°C
- Digital output (no drift)
- Can verify with thermometer if accuracy critical

**Troubleshooting**:
- Reading -127°C: Sensor not detected
  - Check pull-up resistor (4.7kΩ between Data and VCC)
  - Verify wiring connections
  - Test with Dallas Temperature test sketch

### NTC Thermistor Temperature Sensor (Analog)

**Calibration Parameters** (in code):
```cpp
#define THERMISTOR_NOMINAL    10000   // Resistance at 25°C
#define TEMPERATURE_NOMINAL   25      // Reference temperature
#define B_COEFFICIENT         3950    // Beta coefficient
#define SERIES_RESISTOR       10000   // Voltage divider resistor
```

**Calibration Process**:
1. Place thermistor in ice water (0°C) - record reading
2. Place thermistor in room temperature water - record with thermometer
3. Place thermistor in warm water (~40°C) - record with thermometer
4. If readings are consistently off, adjust B_COEFFICIENT:
   - Higher B = steeper curve (larger changes per °C)
   - Lower B = flatter curve (smaller changes per °C)

**Troubleshooting**:
- Reading very high (>100°C): Check NTC is connected (not open circuit)
- Reading very low (<-40°C): Check for short circuit across NTC
- Inaccurate readings: Verify B_COEFFICIENT matches your thermistor datasheet
- Unstable readings: Add 100nF capacitor between A2 and GND

---

## Testing & Verification

### Pre-Power Checks

**Visual Inspection**:
- [ ] No short circuits (adjacent pins not touching)
- [ ] Polarity correct on all components
- [ ] Relay module COM terminals connected to 12V+
- [ ] All pumps have (-) wire to common ground
- [ ] Sensors properly connected to correct pins

**Continuity Testing** (Power OFF):
- [ ] GND continuity: Arduino GND to all component GND pins
- [ ] No continuity between VCC and GND (check for shorts)
- [ ] Relay coil pins isolated from contact pins

### Initial Power-Up (No Pumps)

**Step 1: Arduino Only**
1. Disconnect all pumps from relay module
2. Connect USB to Arduino
3. Verify power LED illuminates
4. Check LCD backlight turns on
5. LCD should display "Hydroponic v2.0 Initializing..."

**Step 2: Sensor Check**
1. Open Serial Monitor (9600 baud)
2. Verify sensor readings appear every 10 seconds:
   ```
   === Status ===
   pH: X.XX
   TDS: XXX ppm
   Temp: XX.X C
   Pump: OFF
   ```
3. Touch pH/TDS probes - values should change
4. Warm DS18B20 with fingers - temperature should rise

**Step 3: I2C LCD Check**
1. LCD should show real-time sensor values
2. If blank: Adjust contrast potentiometer on I2C backpack
3. If garbled: Check I2C address (try 0x3F if 0x27 doesn't work)

### Relay Testing (Controlled)

**Safety**: Pumps still disconnected from relay NO terminals

**Step 4: Relay Click Test**
1. Manually trigger relays with test code:
   ```cpp
   digitalWrite(7, HIGH);  // Should hear relay 1 click
   delay(1000);
   digitalWrite(7, LOW);
   ```
2. Verify relay LED indicators turn on/off
3. Test all relay channels (7, 8, 9, 10)

**Step 5: Relay Contact Check** (Multimeter)
1. Set multimeter to continuity mode
2. Relay OFF: COM to NO should be OPEN (no continuity)
3. Relay ON: COM to NO should be CLOSED (continuity)
4. Test all 4 channels

### Full System Test (With Pumps)

**Step 6: Individual Pump Tests**
1. Power OFF entire system
2. Connect ONE pump to relay channel 1
3. Connect 12V supply to relay COM terminals
4. Connect pump (-) to 12V GND
5. Power ON system
6. Manually activate pump via code/Serial command
7. Verify pump runs smoothly
8. Repeat for all pump channels

**Step 7: Automatic Operation Test**
1. Place sensors in reservoir with nutrient solution
2. Upload full program code
3. System should enter normal operation:
   - Water pump cycles 5 min ON / 15 min OFF
   - TDS checks every 60 seconds
   - LCD updates every 2 seconds

**Step 8: Nutrient Dosing Test** (Optional)
1. Lower TDS reading artificially (dilute solution)
2. When TDS drops below 650 ppm:
   - System should dose Nutrient A (5 sec)
   - Mix for 3 minutes
   - Dose Nutrient B (5 sec)
   - Mix for 5 minutes
3. Verify status updates on LCD

**Step 9: Water Refill Test** (v2 only)
1. Lower water level below TDS probe
2. TDS should read ~0-100 ppm
3. Refill pump should activate for 30 seconds
4. LCD should show "Refilling" status
5. System resumes normal operation after refill

### Verification Checklist

**Electrical**:
- [ ] All LEDs illuminated (power indicators)
- [ ] No burning smell or hot components
- [ ] Relays click audibly when activated
- [ ] LCD displays clear text

**Sensors**:
- [ ] pH readings in expected range (4-10)
- [ ] TDS readings respond to dilution/concentration
- [ ] Temperature readings match ambient (~±2°C)

**Pumps**:
- [ ] Water pump circulates solution properly
- [ ] Peristaltic pumps dose correct direction
- [ ] No leaks in tubing connections
- [ ] Pump activations logged in Serial Monitor

**Software**:
- [ ] v1.0 code displays "v1.0" on LCD (if using v1 code)
- [ ] v2.0 code displays "v2.0" on LCD (if using v2 code)
- [ ] Status updates match actual system state
- [ ] Timing cycles execute properly

---

## Troubleshooting

### LCD Display Issues

**Problem**: LCD backlight on, but no text
- **Solution**: Adjust contrast potentiometer on I2C backpack (tiny blue/white screw)

**Problem**: LCD shows random characters
- **Solution 1**: Check I2C wiring (SDA to A4, SCL to A5)
- **Solution 2**: Try alternate I2C address (0x3F instead of 0x27)
- **Solution 3**: Upload I2C scanner sketch to find actual address

**Problem**: LCD completely dark (no backlight)
- **Solution**: Check VCC and GND connections, verify 5V power supply

### Sensor Issues

**Problem**: pH reads 7.00 constantly
- **Solution**: Check analog connection to A0, verify sensor power

**Problem**: TDS reads 0 constantly
- **Solution**: Check probe is submerged, verify connection to A1

**Problem**: Temperature reads -127°C
- **Solution**: 
  - Check DS18B20 wiring (pin 2 connection)
  - Verify 4.7kΩ pull-up resistor between Data and VCC
  - Test sensor with separate sketch

**Problem**: All sensor readings frozen
- **Solution**: Check common ground connection, verify 5V power supply stability

### Relay/Pump Issues

**Problem**: Relay won't click
- **Solution**: 
  - Check signal wire to Arduino digital pin
  - Verify relay VCC and GND connections
  - Test with manual digitalWrite() command

**Problem**: Relay clicks but pump doesn't run
- **Solution**:
  - Check 12V supply to relay COM terminals
  - Verify pump wiring to relay NO terminal
  - Test pump directly with 12V supply (bypass relay)
  - Check pump (-) wire to 12V GND

**Problem**: Pump runs but won't stop
- **Solution**:
  - Code issue: Check digitalWrite(pin, LOW) commands
  - Hardware issue: Relay stuck closed (replace relay module)

**Problem**: Relay triggers but LCD shows opposite state
- **Solution**: Code logic reversed - check pumpOn variable updates

### Power Issues

**Problem**: Arduino resets when pump activates
- **Solution**:
  - Voltage sag from insufficient power supply
  - Use separate 12V supply for pumps
  - Add decoupling capacitors (100µF) near Arduino VCC/GND

**Problem**: System works on USB, fails on external power
- **Solution**:
  - Check voltage at Vin (should be 7-12V)
  - Verify polarity of external supply
  - Test with USB to isolate power issue

### v2.0 Specific Issues

**Problem**: Water refill pump won't trigger
- **Solution**:
  - Check TDS reading (should be ≤100 ppm when water low)
  - Verify pin 10 connection to relay IN4
  - Position TDS probe at correct minimum water level
  - Test refill pump manually with digitalWrite(10, HIGH)

**Problem**: Refill pump runs constantly
- **Solution**:
  - TDS calibration issue (reads too low even when water adequate)
  - Check LOW_WATER_TDS_THRESHOLD value (should be 100)
  - Verify TDS sensor probe is clean and properly submerged

---

## Safety Warnings

### Electrical Safety

⚠️ **HIGH VOLTAGE WARNING**:
- If using relay module with AC pumps, exercise extreme caution
- This guide assumes DC pumps (12V) - much safer for beginners
- Never work on AC circuits while powered

⚠️ **Water and Electricity**:
- Keep all electronics in waterproof enclosures
- Use cable glands for wire entry points
- Mount control box above water level
- Use GFCI/RCD protection if using AC power

⚠️ **Short Circuit Prevention**:
- Double-check all connections before powering on
- Use proper wire gauge (18-22 AWG for signal, 18 AWG minimum for pumps)
- Insulate all exposed connections with heat shrink tubing
- Never bridge VCC to GND

### Chemical Safety

⚠️ **pH Solutions**:
- pH adjustment solutions are strong acids/bases
- Wear gloves when handling
- Store in labeled containers
- Keep away from children and pets

⚠️ **Nutrient Solutions**:
- Follow manufacturer's dilution instructions
- Some nutrients may stain or irritate skin
- Store in cool, dry place away from sunlight

### Operational Safety

⚠️ **Pump Operation**:
- Never run water pump dry (causes overheating)
- Check tubing for kinks or blockages
- Use check valves to prevent backflow
- Monitor reservoir water level regularly

⚠️ **Long-Term Operation**:
- Inspect wiring monthly for corrosion
- Clean sensors every 2 weeks
- Check relay contacts for arcing/pitting
- Replace pH probe every 6-12 months

---

## Wiring Best Practices

### Wire Color Code (Suggested)

**Power**:
- Red: +5V or +12V
- Black: GND

**Signals**:
- Yellow: Analog signals (pH, TDS)
- Blue: Digital data (temperature, I2C SDA)
- Green: Digital control (relay triggers)
- White: I2C SCL

**Pump Control**:
- Orange: Water pump (D7)
- Purple: Nutrient A (D8)
- Gray: Nutrient B (D9)
- Brown: Refill pump (D10)

### Wire Management

**Bundling**:
- Group sensor wires together
- Keep pump power wires separate from signal wires
- Use spiral wrap or cable sleeves
- Label all wire bundles

**Strain Relief**:
- Use cable ties at regular intervals
- Avoid sharp bends (minimum 1 inch radius)
- Secure wires near connection points
- Use cable glands at enclosure entry

**Future Expansion**:
- Leave extra wire length at connections (3-6 inches)
- Use modular connectors (JST, Dupont) for easy disconnection
- Document non-standard wire colors

---

## Upgrade Path: v1.0 → v2.0

### Required Additions

**Hardware**:
1. Water refill pump (12V DC)
2. One additional jumper wire (Arduino D10 to Relay IN4)
3. Tubing for refill pump (intake to water source, outlet to reservoir)

**Software**:
1. Upload Radish_Hydroponic_v2.ino

**Wiring Changes**:
1. Connect Arduino pin D10 to relay module IN4
2. Connect refill pump (+) to relay channel 4 NO terminal
3. Connect refill pump (-) to 12V GND (common with other pumps)
4. Route refill pump tubing:
   - Intake: Clean water source (barrel, tap, etc.)
   - Outlet: Reservoir (below max water level)

**Configuration**:
1. Position TDS probe at desired minimum water level
2. Adjust LOW_WATER_TDS_THRESHOLD if needed (default 100 ppm)
3. Adjust WATER_REFILL_DURATION if needed (default 30 seconds)

**Testing**:
1. Lower water level below TDS probe
2. Verify TDS reads ≤100 ppm
3. Confirm refill pump activates for 30 seconds
4. Check LCD displays "Refilling" status
5. Ensure system resumes normal operation after refill

---

## Wiring Checklist

### Complete Installation Checklist

**Sensors** (All versions):
- [ ] pH sensor: VCC→5V, GND→GND, Signal→A0
- [ ] TDS sensor: VCC→5V, GND→GND, Signal→A1
- [ ] DS18B20: VCC→5V, GND→GND, Data→D2, 4.7kΩ pull-up installed
- [ ] LCD: VCC→5V, GND→GND, SDA→A4, SCL→A5

**Relay Module** (All versions):
- [ ] VCC→5V, GND→GND
- [ ] IN1→D7 (water pump)
- [ ] IN2→D8 (nutrient A)
- [ ] IN3→D9 (nutrient B)
- [ ] IN4→D10 (refill pump - v2 only)

**Pump Power** (All versions):
- [ ] 12V+ connected to all relay COM terminals
- [ ] Water pump (+)→Relay 1 NO, (-)→12V GND
- [ ] Nutrient A pump (+)→Relay 2 NO, (-)→12V GND
- [ ] Nutrient B pump (+)→Relay 3 NO, (-)→12V GND
- [ ] Refill pump (+)→Relay 4 NO, (-)→12V GND (v2 only)

**Safety** (All versions):
- [ ] Common ground between Arduino and 12V supply
- [ ] No exposed wire connections
- [ ] Waterproof enclosure for electronics
- [ ] Cable management and strain relief
- [ ] All connections labeled
- [ ] Fire extinguisher nearby (when testing high-power loads)

---

## Appendix: Component Specifications

### Arduino Uno R3
- **Microcontroller**: ATmega328P
- **Operating Voltage**: 5V
- **Input Voltage**: 7-12V (recommended), 6-20V (limits)
- **Digital I/O Pins**: 14 (of which 6 provide PWM)
- **Analog Input Pins**: 6
- **DC Current per I/O Pin**: 40 mA maximum
- **Flash Memory**: 32 KB (0.5 KB used by bootloader)

### 4-Channel Relay Module
- **Operating Voltage**: 5V DC
- **Trigger Current**: 15-20 mA per channel
- **Contact Rating**: 10A @ 250VAC / 30VDC
- **Contact Configuration**: SPDT (COM, NO, NC)
- **Isolation**: Optocoupler (galvanic isolation)
- **Dimensions**: ~75mm × 55mm × 19mm

### 20x4 I2C LCD
- **Display**: 20 characters × 4 lines
- **Backlight**: Blue or Green LED
- **Controller**: HD44780 + PCF8574 I2C backpack
- **I2C Address**: 0x27 or 0x3F (depends on backpack)
- **Operating Voltage**: 5V
- **Operating Current**: 20 mA (display) + 80 mA (backlight)

### DS18B20 Temperature Sensor
- **Type**: Digital 1-Wire
- **Temperature Range**: -55°C to +125°C
- **Accuracy**: ±0.5°C (-10°C to +85°C)
- **Resolution**: 9 to 12-bit (configurable)
- **Operating Voltage**: 3.0V to 5.5V
- **Pull-up Resistor**: 4.7kΩ required on data line

### pH Sensor Module
- **Type**: Analog BNC electrode
- **Measurement Range**: 0-14 pH
- **Accuracy**: ±0.1 pH (after calibration)
- **Response Time**: <1 minute
- **Operating Temperature**: 0-60°C
- **Probe Lifespan**: 6-12 months (typical)
- **Output**: 0-5V analog (2.5V at pH 7.0)

### TDS Sensor V1.0
- **Type**: Analog conductivity probe
- **Measurement Range**: 0-1000 ppm (can read higher)
- **Accuracy**: ±10% (typical)
- **Probe Material**: Copper alloy
- **Operating Voltage**: 3.3V or 5V
- **Output**: Analog voltage proportional to TDS
- **Temperature Compensation**: Software-based

---

**Document Version**: 2.0  
**Last Updated**: January 17, 2026  
**Compatible with**: Radish_Hydroponic_v1.ino, Radish_Hydroponic_v2.ino  
**Author**: K1taru

---

## Quick Reference Card

### Emergency Shutdown
1. Disconnect 12V power supply (stops all pumps)
2. Disconnect Arduino USB/power (stops control system)
3. Turn off relay module if accessible

### Common Issues & Quick Fixes
| Issue | Quick Check | Fix |
|-------|-------------|-----|
| LCD blank | Contrast | Adjust potentiometer |
| Temp -127°C | Pull-up resistor | Add 4.7kΩ resistor |
| Pump won't stop | Code/relay | Check digitalWrite LOW |
| Arduino resets | Power supply | Use separate 12V supply |
| Refill constant (v2) | TDS reading | Recalibrate sensor |

### Support Resources
- Arduino Forums: https://forum.arduino.cc/
- OneWire Library: https://github.com/PaulStoffregen/OneWire
- DallasTemperature: https://github.com/milesburton/Arduino-Temperature-Control-Library
- LiquidCrystal_I2C: https://github.com/johnrickman/LiquidCrystal_I2C

---

**END OF WIRING GUIDE**