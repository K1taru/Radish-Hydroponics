
| Materials                              | Quantity |
|----------------------------------------|----------|
| pH Test Strips (used manually for test)| 100 pcs  |
| Nutrient Solution (RAMGO HydroGro)     | 1 pc     |
| Net Pots                               | 12 pcs   |
| Water Pump with Water Inlet (15W)      | 1 pc     |
| Water hose                             | 1 pc     |
| PVC Pipe                               | 3 pcs    |
| Plastic container                      | 1 pc     |
| Radish Seeds                           | 2 pcs    |
| Plastic Bucket                         | 1 pc     |
| Electrical Box                         | 1 pc     |
| Arduino Uno                            | 1 pc     |
| Loam soil                              | 1 pc     |
| Relay Module                           | 1 pc     |
| pH sensor                              | 1 pc     |
| TDS Sensor Board                       | 1 pc     |
| Temperature Sensor                     | 1 pc     |
| ESP32 Board                            | 1 pc     |
| LED Display                            | 1 pc     |
| LCD Display 4 lines                    | 1 pc     |
| Peristaltic Pump                       | 2 pcs    |
| Power Adapter                          | 1 pc     |
| Breadboard                             | 1 pc     |
| Jumper Wires                           | pcs      |
| Silicone tube                          | 1 pc     |
| Wooden box                             | 1 pc     |

## Tech & Arduino-Related Materials

| Materials            | Quantity |
|----------------------|----------|
| Arduino Uno          | 1 pc     |
| ESP32 Board          | 1 pc     |
| Relay Module         | 1 pc     |
| pH sensor            | 1 pc     |
| TDS Sensor Board     | 1 pc     |
| Temperature Sensor   | 1 pc     |
| LED Display          | 1 pc     |
| LCD Display 4 lines  | 1 pc     |
| Peristaltic Pump     | 2 pcs    |
| Power Adapter        | 1 pc     |
| Breadboard           | 1 pc     |
| Jumper Wires         | pcs      |

---

## System Mechanism & How It Works

### Overview

This is an **automated tower hydroponic system** designed specifically for growing radishes. The system uses an Arduino Uno R3 microcontroller to monitor and control critical environmental parameters (pH, TDS/nutrient concentration, and temperature) while automating the irrigation cycle. The tower configuration allows water to be pumped upward through a hose and drip down through multiple growing levels, providing efficient space utilization and consistent nutrient delivery to plant roots.

### Physical Setup

1. **Tower Structure**: PVC pipes arranged vertically with net pots inserted at intervals where radish plants grow
2. **Reservoir**: Plastic container/bucket at the base holding the nutrient solution
3. **Water Circulation**: 15W water pump draws solution from reservoir, pushes it up through hose to top of tower
4. **Gravity Return**: Solution drips/flows down through the tower, bathing plant roots, then returns to reservoir
5. **Dosing System**: Two peristaltic pumps for nutrient bottles (A & B) and optional pumps for pH adjustment solutions

### System Architecture

```
┌─────────────────────────────────────────────────┐
│          ARDUINO UNO R3 (Brain)                 │
│  - Reads sensors (pH, TDS, Temperature)         │
│  - Controls all pumps via relay module          │
│  - Displays data on 20x4 LCD                    │
│  - Logs to Serial Monitor for debugging         │
└─────────────────────────────────────────────────┘
         │                           │
    ┌────▼────┐                 ┌───▼────┐
    │ SENSORS │                 │ RELAYS │
    │         │                 │        │
    │ pH      │                 │ Pumps: │
    │ TDS     │                 │ - Water│
    │ Temp    │                 │ - Nut A│
    └─────────┘                 │ - Nut B│
                                │ - pH ↑ │
                                │ - pH ↓ │
                                └────────┘
```

---

## Detailed Algorithm & Control Logic

### 1. System Initialization (setup() function)

**Purpose**: Prepare all hardware and establish baseline conditions

**Process**:
1. **Serial Communication**: Opens Serial port at 9600 baud for debugging
2. **LCD Initialization**: 
   - Initializes I2C communication with 20x4 LCD
   - Turns on backlight
   - Displays "Hydroponic System Initializing..." message
3. **Pin Configuration**:
   - Sets all pump pins (7-11) as OUTPUT
   - Initializes all pump states to LOW (OFF) for safety
4. **Sensor Setup**:
   - Initializes DS18B20 temperature sensor on OneWire bus
5. **Stabilization Period**: 
   - 2-second delay allows sensors to stabilize
   - Takes initial sensor readings
6. **Ready State**: 
   - Displays "System Ready!" on LCD
   - Sets system status to "Running"
   - System enters main loop

**Safety Feature**: All pumps default to OFF state on power-up or reset

---

### 2. Main Control Loop (loop() function)

The main loop operates on a **time-sliced architecture** using `millis()` for non-blocking execution. This allows multiple tasks to run concurrently without using delay().

#### Loop Structure:

```
LOOP (runs continuously):
  ├─ Check if 5 seconds elapsed → Read Sensors
  ├─ Check if 2 seconds elapsed → Update LCD Display
  ├─ Check water pump timing → Control Irrigation Cycle
  ├─ Check if 60 seconds elapsed → Adjust Nutrients/pH
  └─ Check if 10 seconds elapsed → Print Serial Debug Data
```

**Key Timing Intervals** (configurable via #define constants):
- `SENSOR_READ_INTERVAL`: 5,000 ms (5 seconds)
- `LCD_UPDATE_INTERVAL`: 2,000 ms (2 seconds)  
- `CONTROL_CHECK_INTERVAL`: 60,000 ms (60 seconds)
- Serial Debug Print: 10,000 ms (10 seconds)

---

### 3. Sensor Reading Algorithm

#### 3.1 pH Sensor Reading (`readPH()`)

**Algorithm**:
1. **Analog-to-Digital Conversion**:
   ```
   rawValue = analogRead(A0)  // 10-bit ADC (0-1023)
   voltage = rawValue × (5.0V / 1024)
   ```

2. **pH Calculation** (based on typical analog pH sensor):
   ```
   pH = 7.0 - ((voltage - 2.5V) / 0.18V) + OFFSET
   ```
   - **Principle**: Most analog pH probes output 2.5V at pH 7.0 (neutral)
   - **Slope**: -0.18V per pH unit (acidic = higher voltage, basic = lower voltage)
   - **Offset**: Calibration adjustment (set to 0.0 by default, adjust after calibration)

3. **Range Clamping**: Ensures pH stays within realistic 0.0-14.0 range

**Note**: This formula must be adjusted based on your specific pH sensor's datasheet

#### 3.2 TDS Sensor Reading (`readTDS()`)

**Algorithm**:
1. **Analog-to-Digital Conversion**:
   ```
   rawValue = analogRead(A1)
   voltage = rawValue × (5.0V / 1024)
   ```

2. **Temperature Compensation**:
   ```
   compensationCoefficient = 1.0 + 0.02 × (currentTemp - 25.0°C)
   compensationVoltage = voltage / compensationCoefficient
   ```
   - **Principle**: Electrical conductivity increases ~2% per °C above 25°C
   - **Reference**: 25°C is standard calibration temperature
   - This normalizes readings to 25°C equivalent

3. **TDS Calculation**:
   ```
   TDS (ppm) = compensationVoltage × 500 × K_VALUE
   ```
   - **500**: Conversion factor for typical TDS sensors (500 scale)
   - **K_VALUE**: Probe constant (default 1.0, adjust per sensor datasheet)

**Advanced Note**: More accurate TDS sensors use cubic polynomial:
```
TDS = (133.42×V³ - 255.86×V² + 857.39×V) × 0.5
```

#### 3.3 Temperature Sensor Reading (`readTemperature()`)

**Algorithm**:
1. **Request Temperature**:
   ```
   tempSensors.requestTemperatures()
   ```
   - Triggers DS18B20 to perform conversion (takes ~750ms for 12-bit resolution)

2. **Read Value**:
   ```
   temp = tempSensors.getTempCByIndex(0)  // First sensor on bus
   ```

3. **Error Checking**:
   ```
   IF temp == DEVICE_DISCONNECTED_C OR temp < -50°C OR temp > 100°C:
       Print error warning
       Return last valid reading
   ELSE:
       Return new reading
   ```
   - **Safety**: Prevents invalid readings from disrupting system
   - **Resilience**: Uses last known good value during sensor glitches

---

### 4. Water Pump Control Algorithm (`controlWaterPump()`)

**Purpose**: Implement tower irrigation with timed ON/OFF cycles

**State Machine**:

```
┌─────────────────────────────────────────────┐
│  State: PUMP OFF                            │
│                                             │
│  Wait: WATER_PUMP_OFF_DURATION (15 min)    │
│                                             │
│  Condition: Time elapsed?                   │
│      YES → Turn pump ON, transition         │
│      NO  → Continue waiting                 │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│  State: PUMP ON                             │
│                                             │
│  Wait: WATER_PUMP_ON_DURATION (5 min)      │
│                                             │
│  Condition: Time elapsed?                   │
│      YES → Turn pump OFF, transition        │
│      NO  → Continue pumping                 │
└─────────────────────────────────────────────┘
                    ↓
              (Loop back to OFF)
```

**Detailed Logic**:
```
IF waterPumpState == OFF:
    IF (currentTime - lastWaterPumpOff) >= WATER_PUMP_OFF_DURATION:
        digitalWrite(WATER_PUMP_PIN, HIGH)  // Turn ON
        waterPumpState = TRUE
        lastWaterPumpOn = currentTime
        Log: "Water pump ON - Irrigation cycle started"
    ENDIF
ELSE (pump is ON):
    IF (currentTime - lastWaterPumpOn) >= WATER_PUMP_ON_DURATION:
        digitalWrite(WATER_PUMP_PIN, LOW)  // Turn OFF
        waterPumpState = FALSE
        lastWaterPumpOff = currentTime
        Log: "Water pump OFF - Rest period"
    ENDIF
ENDIF
```

**Rationale**:
- **5 minutes ON**: Sufficient time to pump solution to top and saturate all root zones
- **15 minutes OFF**: Allows roots to access oxygen (prevents waterlogging/root rot)
- **Cycle**: 5 min ON / 15 min OFF = 20 min total cycle = 3 cycles per hour
- **Duty Cycle**: 25% (efficient power use, proper oxygenation)

**Configurable Parameters**:
- `WATER_PUMP_ON_DURATION`: Adjust based on tower height and pump flow rate
- `WATER_PUMP_OFF_DURATION`: Adjust based on plant type and environmental conditions

---

### 5. Nutrient & pH Adjustment Algorithm (`checkAndAdjustNutrients()`)

This is the **most critical control algorithm** for maintaining optimal growing conditions.

#### Algorithm Overview:

```
FUNCTION checkAndAdjustNutrients():
    
    // SAFETY GATE: Prevent dosing during mixing period
    IF (time since last nutrient dose) < PUMP_WAIT_AFTER_DOSE:
        RETURN  // Exit, solution still mixing
    ENDIF
    
    // NUTRIENT (TDS) CONTROL
    IF currentTDS < (TDS_MIN + TDS_TOLERANCE):
        Dose Nutrient A for NUTRIENT_PUMP_DOSE_TIME
        Wait 1 second
        Dose Nutrient B for NUTRIENT_PUMP_DOSE_TIME
        Update lastNutrientDose timestamp
        Wait for mixing period
    ENDIF
    
    // pH CONTROL (only if mixing period elapsed)
    IF (time since last pH adjustment) >= PUMP_WAIT_AFTER_DOSE:
        
        IF currentPH < (PH_MIN + PH_TOLERANCE):
            Dose pH UP solution for PH_ADJUST_PUMP_TIME
            Update lastPHAdjust timestamp
            
        ELSE IF currentPH > (PH_MAX - PH_TOLERANCE):
            Dose pH DOWN solution for PH_ADJUST_PUMP_TIME
            Update lastPHAdjust timestamp
        ENDIF
        
    ENDIF
    
END FUNCTION
```

#### 5.1 TDS/Nutrient Control Logic

**Hysteresis Implementation** (prevents pump oscillation):

```
Control Band:
├─────────┼─────────┼─────────┼─────────┼─────────┤
│         │         │         │         │         │
0       TDS_MIN   TDS_MIN+  TDS_TARGET TDS_MAX   1500+
               TOL=650ppm              TOL=950ppm

Zones:
  < 650 ppm:  DOSE NUTRIENTS (too low)
  650-900:    OK ZONE (no action)
  > 950 ppm:  WARNING HIGH (manual water dilution needed)
```

**Decision Tree**:
```
IF TDS < (TDS_MIN + TDS_TOLERANCE):  // < 650 ppm
    1. Display "Add Nutrients" status
    2. Activate PUMP_NUTRIENT_A relay (HIGH)
    3. Wait NUTRIENT_PUMP_DOSE_TIME (5 seconds)
    4. Deactivate PUMP_NUTRIENT_A (LOW)
    5. Wait 1 second (gap between A and B)
    6. Activate PUMP_NUTRIENT_B relay (HIGH)
    7. Wait NUTRIENT_PUMP_DOSE_TIME (5 seconds)
    8. Deactivate PUMP_NUTRIENT_B (LOW)
    9. Record lastNutrientDose = currentTime
    10. Display "Running" status
    11. Wait PUMP_WAIT_AFTER_DOSE (3 min) before next check
ENDIF
```

**Why Equal A+B Dosing?**
- RAMGO HydroGro is a 2-part system requiring equal volumes of A and B
- Bottle A: Calcium nitrate + chelated iron (cannot be mixed concentrated with B)
- Bottle B: Phosphates + sulfates + magnesium + trace elements
- Equal dosing maintains proper NPK ratios

**Critical Timing**: 
- `PUMP_WAIT_AFTER_DOSE` (3 minutes) allows:
  1. Nutrients to disperse throughout reservoir
  2. Chemical reactions to stabilize
  3. Sensors to read accurate values

#### 5.2 pH Control Logic

**Hysteresis Implementation**:

```
Control Band:
├─────┼─────┼─────┼─────┼─────┼─────┼─────┤
│     │     │     │     │     │     │     │
5.0  PH_MIN PH_MIN+ PH_   PH_MAX- PH_MAX 7.0
    =5.8   +TOL  TARGET  -TOL  =6.5
           =6.0   =6.1   =6.3

Actions:
  < 6.0:    DOSE pH UP (too acidic)
  6.0-6.3:  OK ZONE (no action)
  > 6.3:    DOSE pH DOWN (too alkaline)
```

**Decision Tree**:
```
IF (time since last pH adjustment) >= PUMP_WAIT_AFTER_DOSE:
    
    IF pH < (PH_MIN + PH_TOLERANCE):  // < 6.0
        1. Display "Adjust pH UP" status
        2. Activate PUMP_PH_UP relay
        3. Wait PH_ADJUST_PUMP_TIME (2 seconds)
        4. Deactivate PUMP_PH_UP
        5. Record lastPHAdjust = currentTime
        6. Display "Running" status
    
    ELSE IF pH > (PH_MAX - PH_TOLERANCE):  // > 6.3
        1. Display "Adjust pH DOWN" status
        2. Activate PUMP_PH_DOWN relay
        3. Wait PH_ADJUST_PUMP_TIME (2 seconds)
        4. Deactivate PUMP_PH_DOWN
        5. Record lastPHAdjust = currentTime
        6. Display "Running" status
    ENDIF
    
ENDIF
```

**Why Separate Timing from Nutrient Dosing?**
1. **Priority**: TDS adjusted first (nutrients affect pH)
2. **Stability**: Nutrients must disperse before pH adjustment
3. **Accuracy**: Fresh nutrient addition temporarily shifts pH; let it stabilize
4. **Safety**: Prevents simultaneous dosing conflicts

**pH Adjustment Caution**:
- Small doses (2 seconds) prevent overcorrection
- pH adjusters are concentrated acids/bases (dangerous if overdosed)
- System waits 3 minutes after adjustment before re-checking
- Gradual approach preferred over rapid correction

---

### 6. LCD Display Algorithm (`updateLCD()`)

**Purpose**: Provide real-time visual feedback to operator

**Display Layout** (20x4 LCD):
```
┌────────────────────┐
│pH:6.12 OK          │  Line 1: pH value + status
│TDS:745ppm OK       │  Line 2: TDS value + status
│Temp:22.3C OK       │  Line 3: Temperature + status
│Pump:ON Running     │  Line 4: Pump state + system status
└────────────────────┘
```

**Status Determination Logic**:

For **pH**:
```
IF pH >= PH_MIN AND pH <= PH_MAX:
    Display "OK"
ELSE IF pH < PH_MIN:
    Display "LOW"
ELSE:
    Display "HIGH"
```

For **TDS**:
```
IF TDS >= TDS_MIN AND TDS <= TDS_MAX:
    Display "OK"
ELSE IF TDS < TDS_MIN:
    Display "LOW"
ELSE:
    Display "HIGH"
```

For **Temperature**:
```
IF temp >= TEMP_MIN AND temp <= TEMP_MAX:
    Display "OK"
ELSE IF temp < TEMP_MIN:
    Display "COLD"
ELSE IF temp > TEMP_WARNING:
    Display "HOT!"
ELSE:
    Display "WARM"
```

**Pump Status**: "ON" or "OFF" based on `waterPumpState` boolean

**System Status**: Dynamic string showing current activity:
- "Running" - Normal operation
- "Add Nutrients" - Dosing nutrients
- "Adjust pH UP" - Increasing pH
- "Adjust pH DOWN" - Decreasing pH
- "Starting..." - Initialization phase

---

### 7. Serial Monitor Debug Output

**Purpose**: Detailed logging for troubleshooting and data analysis

**Output Format** (every 10 seconds):
```
========== Sensor Readings ==========
pH: 6.12 (Target: 6.1)
TDS: 745 ppm (Target: 750 ppm)
Temperature: 22.3 °C
Water Pump: ON
System Status: Running
====================================
```

**Use Cases**:
- **Calibration**: Compare sensor values to known standards
- **Trending**: Monitor parameter drift over time
- **Debugging**: Identify sensor failures or control logic issues
- **Data Logging**: Can be captured to SD card or sent to cloud (future enhancement)

---

## Control System Theory

### Hysteresis Control (Bang-Bang with Deadband)

**Problem**: Simple on/off control causes oscillation
```
Without hysteresis:
TDS ↓ to 749 ppm → Dose nutrients
TDS ↑ to 751 ppm → Stop
TDS ↓ to 749 ppm → Dose again (oscillation!)
```

**Solution**: Deadband (tolerance zone)
```
With hysteresis:
TDS ↓ to 649 ppm → Dose nutrients
TDS ↑ to 751 ppm → Still OK (no action)
TDS ↑ to 900 ppm → Still OK
TDS ↓ to 649 ppm → Dose again (stable control)
```

**Benefits**:
1. **Pump Longevity**: Reduces wear from excessive switching
2. **Stability**: Allows natural fluctuations without intervention
3. **Accuracy**: Prevents hunting around setpoint
4. **Energy Efficiency**: Minimizes unnecessary pump activations

### Non-Blocking Timing Architecture

**Traditional Approach (blocking - BAD)**:
```
loop():
    readSensors()
    delay(5000)  // Freezes entire program!
```

**Our Approach (non-blocking - GOOD)**:
```
loop():
    IF (currentTime - lastSensorRead) >= 5000:
        readSensors()
        lastSensorRead = currentTime
    // Other tasks continue executing
```

**Advantages**:
1. **Concurrent Operations**: Multiple tasks run "simultaneously"
2. **Responsiveness**: System reacts immediately to conditions
3. **Flexibility**: Easy to add new timed tasks
4. **Scalability**: Can manage dozens of independent timers

---

## Fail-Safe & Safety Features

### 1. Sensor Error Handling
- **Temperature**: Returns last valid reading if sensor disconnected
- **Range Clamping**: pH clamped to 0-14, TDS to positive values
- **Serial Warnings**: Alerts operator to sensor malfunctions

### 2. Pump Safety
- **Default OFF**: All pumps initialize to OFF state on boot
- **Relay Control**: Pumps controlled via relays (electrical isolation)
- **Timed Dosing**: Fixed-duration doses prevent runaway dosing

### 3. Mixing Delays
- **3-Minute Wait**: Prevents premature re-dosing during mixing
- **Separate Timers**: Independent tracking for nutrients vs pH
- **Sequential Dosing**: A then B (prevents A+B reaction in concentrate)

### 4. Hysteresis Bands
- **Prevents Oscillation**: Deadband zones avoid rapid switching
- **Operator Configurable**: Easy threshold adjustment via #define

### 5. Status Monitoring
- **LCD Real-Time Display**: Immediate visual feedback
- **Serial Logging**: Permanent record for analysis
- **Status Strings**: Clear indication of system activity

---

## Calibration & Tuning Guide

### Sensor Calibration

**pH Sensor**:
1. Prepare pH 4.0, 7.0, and 10.0 calibration solutions
2. Test each solution, note Serial Monitor voltage output
3. Calculate slope: `(pH_high - pH_low) / (V_low - V_high)`
4. Adjust `PH_OFFSET` constant to match pH 7.0 buffer
5. Verify with all three buffers

**TDS Sensor**:
1. Prepare standard solution (e.g., 1413 μS/cm = 707 ppm on 500 scale)
2. Measure at 25.0°C
3. Compare reading to standard value
4. Adjust `TDS_K_VALUE` constant:
   ```
   K_VALUE = actual_ppm / measured_ppm
   ```
5. Verify across range (500-1000 ppm)

**Temperature Sensor**:
- DS18B20 is factory calibrated (±0.5°C accuracy)
- No adjustment typically needed
- Verify with thermometer if concerned

### System Tuning

**Dosing Duration Adjustment**:
1. Measure peristaltic pump flow rate (ml/min)
2. Calculate reservoir volume (liters)
3. Determine desired dose per cycle (ml)
4. Set duration: `time(ms) = (dose_ml / flow_rate_ml/min) × 60,000`

**Example**:
- Pump: 100 ml/min
- Desired dose: 10 ml
- Duration: (10 / 100) × 60,000 = 6,000 ms

**Timing Interval Optimization**:
- **Small reservoirs (<10L)**: Shorter check intervals (30s)
- **Large reservoirs (>30L)**: Longer intervals (120s)
- **Aggressive plants**: More frequent irrigation (3min ON / 12min OFF)
- **Root-sensitive plants**: Less frequent (5min ON / 20min OFF)

---

## System Performance Specifications

| Parameter | Specification |
|-----------|---------------|
| **Control Loop Speed** | ~1ms per iteration (non-blocking) |
| **Sensor Update Rate** | 5 seconds (configurable) |
| **LCD Refresh Rate** | 2 seconds (configurable) |
| **Control Response Time** | 60 seconds (configurable) |
| **pH Control Accuracy** | ±0.2 pH units (with hysteresis) |
| **TDS Control Accuracy** | ±50 ppm (with hysteresis) |
| **Temperature Sensing** | ±0.5°C (DS18B20 spec) |
| **Pump Switching Resolution** | 1 ms (relay response time) |
| **Maximum Dosing Frequency** | Once per 3 minutes (safety limit) |
| **Power Consumption** | ~20W total (15W pump + 5W electronics) |

---

## Future Enhancement Possibilities

1. **Data Logging**: Write sensor data to SD card for analysis
2. **WiFi/Bluetooth**: Remote monitoring via ESP32 module
3. **Adaptive Control**: Machine learning for optimal setpoints
4. **Multi-Zone**: Control multiple towers independently
5. **Flow Sensor**: Verify pump operation and detect clogs
6. **Water Level Sensor**: Auto-fill reservoir when low
7. **Light Control**: Integrate grow lights with day/night cycles
8. **CO₂ Enrichment**: Monitor and control CO₂ for faster growth
9. **Camera Integration**: Automated plant health monitoring
10. **Mobile App**: Android/iOS interface for system control

---
