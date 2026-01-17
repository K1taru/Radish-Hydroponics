# Radish Hydroponic System v2.0 - Documentation

## Overview
Automated tower hydroponic system for radish cultivation with pH/TDS/temperature monitoring, sequential nutrient dosing to prevent chemical precipitation, and automatic water level detection with refill capability.

---

## Hardware Requirements

### Microcontroller
- **Arduino Uno R3**

### Sensors
- **pH Sensor** (Analog) → Pin A0
- **TDS Sensor V1.0** (Analog) → Pin A1
- **DS18B20 Temperature Sensor** (1-Wire Digital) → Pin 2

### Actuators
- **Water Circulation Pump** (15W) → Pin 7
- **Peristaltic Pump A** (Nutrient Solution A) → Pin 8
- **Peristaltic Pump B** (Nutrient Solution B) → Pin 9
- **Water Refill Pump** → Pin 10

### Display
- **20x4 I2C LCD** (Address: 0x27)

### Libraries Required
```cpp
Wire.h                  // I2C communication
LiquidCrystal_I2C.h    // LCD control
OneWire.h              // 1-Wire protocol
DallasTemperature.h    // DS18B20 interface
```

---

## System Parameters

### pH Range (Radish Optimal)
| Parameter | Value | Action |
|-----------|-------|--------|
| Minimum | 5.8 | Display "LOW! ADD UP" |
| Maximum | 6.5 | Display "HIGH!ADD DWN" |
| Target | 6.0-6.3 | Display "OK" |

**Note:** pH adjustment is manual only. No automated dosing.

### TDS Range (Nutrient Concentration)
| Parameter | Value | Action |
|-----------|-------|--------|
| Minimum | 600 ppm | Start dosing sequence |
| Maximum | 900 ppm | Display "HIGH" warning |
| Tolerance | 50 ppm | Hysteresis band |
| Trigger | 650 ppm | TDS_MIN + TDS_TOLERANCE |

### Temperature Range
| Parameter | Value | Status |
|-----------|-------|--------|
| Minimum | 18.0°C | COLD |
| Optimal | 18-24°C | OK |
| Warning | 24-26°C | WARM |
| Critical | >26°C | HOT! |

### Low Water Detection (v2.0)
| Parameter | Value | Description |
|-----------|-------|-------------|
| Threshold | 100 ppm | TDS ≤ 100 indicates sensor not submerged |
| Refill Duration | 30 seconds | Water pump run time per refill cycle |
| Detection Method | TDS sensor | Low TDS reading = low water level |

**Note:** When water level drops below TDS sensor probe, readings fall to 0-100 ppm (vs normal 600-900 ppm), triggering auto-refill.

---

## Timing Configuration

### Pump Cycles
| Operation | Duration | Description |
|-----------|----------|-------------|
| Pump ON | 5 minutes | Water circulation cycle |
| Pump OFF | 15 minutes | Rest period |
| Duty Cycle | 25% | 5 min per 20 min period |

### Nutrient Dosing
| Operation | Duration | Description |
|-----------|----------|-------------|
| Dose Time | 5 seconds | Peristaltic pump run time |
| Mix A Time | 3 minutes | Dilute Solution A |
| Mix B Time | 5 minutes | Final mixing |
| Wait Period | 3 minutes | Sensor stabilization |

### Water Refill (v2.0)
| Operation | Duration | Description |
|-----------|----------|-------------|
| Refill Time | 30 seconds | Water refill pump run time |
| Detection Check | Continuous | Monitors TDS on every sensor read |

### Update Intervals
| Task | Interval | Purpose |
|------|----------|---------|
| Sensor Reading | 5 seconds | pH, TDS, temp |
| LCD Update | 2 seconds | Display refresh |
| Nutrient Check | 60 seconds | TDS evaluation |
| Serial Output | 10 seconds | Debug logging |

---

## Water Level Detection & Auto-Refill (v2.0)

### Detection Principle
The system uses the TDS sensor as a dual-purpose device:
- **Normal Operation**: Reads nutrient concentration (600-900 ppm)
- **Low Water Detection**: When water level drops below sensor probe, TDS reads ≤100 ppm

### Why This Works
- TDS measures electrical conductivity of solution
- When sensor is not submerged, conductivity drops to near zero
- Threshold of 100 ppm provides reliable detection with safety margin
- Avoids need for dedicated water level sensor

### Refill State Machine
```
MONITORING (Normal)
    ↓ TDS ≤ 100 ppm
REFILLING (Pump ON 30 sec)
    ↓ Complete
MONITORING (Resume)
```

### Algorithm Flow
```cpp
if (refillActive) {
    if (elapsed >= 30 seconds) {
        Turn OFF refill pump
        refillActive = false
        status = "Running"
    }
    return  // Skip other checks
}

if (tds <= 100) {
    Turn ON refill pump
    refillActive = true
    status = "Refilling"
    Record start time
}
```

### Safety Features
- **Fixed Duration**: 30-second timeout prevents overflow
- **Blocks Nutrient Dosing**: No nutrients added during refill
- **Status Display**: LCD shows "Refilling" during operation
- **Logging**: Serial monitor records refill events

### Hardware Setup
- Refill pump draws from external clean water source (tap/barrel)
- Position TDS probe at desired minimum water level in reservoir
- Ensure probe is vertically mounted for consistent detection

---

## Sequential Dosing Algorithm

### Why Sequential?
Nutrient solutions A and B contain chemicals that precipitate when mixed directly:
- **Solution A**: Calcium compounds
- **Solution B**: Phosphate/sulfate compounds
- **Problem**: Ca + PO₄ → CaPO₄ (solid precipitate)

### Solution: Dilute-Then-Mix
```
Step 1: Dose A → Dilute with water
Step 2: Dose B → Mix everything
```

### State Machine Flow
```
MIX_NONE (Normal Operation)
    ↓ TDS < 650 ppm
DOSE_A (Pump A ON 5 sec)
    ↓ Complete
MIX_A (Water Pump 3 min)
    ↓ Complete
DOSE_B (Pump B ON 5 sec)
    ↓ Complete
MIX_B (Water Pump 5 min)
    ↓ Complete
MIX_NONE (Resume Normal)
```

### State Descriptions

#### MIX_NONE
- Normal operation
- Water pump follows 5min/15min schedule
- Check TDS every 60 seconds
- If TDS < 650 ppm → Enter DOSE_A

#### DOSE_A
- Status: "Dosing A"
- Turn ON Nutrient A pump for 5 seconds
- Transition to MIX_A immediately

#### MIX_A
- Status: "Mixing A"
- Force water pump ON (override normal schedule)
- Wait 3 minutes for dilution
- Transition to DOSE_B when complete

#### DOSE_B
- Status: "Dosing B"
- Wait 0.5 seconds (safety gap)
- Turn ON Nutrient B pump for 5 seconds
- Transition to MIX_B immediately

#### MIX_B
- Status: "Mixing B"
- Force water pump ON (override)
- Wait 5 minutes for thorough mixing
- Transition to MIX_NONE when complete
- Record dose time, turn OFF pump

---

## Sensor Algorithms

### pH Calculation
```cpp
voltage = analogRead(A0) * (5.0 / 1024.0)
pH = 7.0 - ((voltage - 2.5) / 0.18) + PH_OFFSET
pH = constrain(pH, 0.0, 14.0)
```
**Calibration:**
- 2.5V = pH 7.0 (neutral)
- Each 0.18V change = 1 pH unit
- Adjust PH_OFFSET after calibration with buffer solutions

### TDS Calculation (with Temperature Compensation)
```cpp
voltage = analogRead(A1) * (5.0 / 1024.0)
comp_coefficient = 1.0 + 0.02 * (temp - 25.0)
comp_voltage = voltage / comp_coefficient
tds = (comp_voltage * 500.0 * TDS_K)
```
**Temperature Compensation:**
- Conductivity increases ~2% per °C above 25°C
- Compensation corrects to 25°C reference

### Temperature Reading
```cpp
tempSensor.requestTemperatures()
temp = tempSensor.getTempCByIndex(0)
if (temp < -50.0 || temp > 100.0) {
  // Use last valid reading (sensor error)
}
```

---

## LCD Display Format

### Layout (20 characters × 4 lines)
```
Line 0: pH:X.XX [STATUS]
Line 1: TDS:XXXXppm [STATUS]
Line 2: Temp:XX.XC [STATUS]
Line 3: Pump:[ON/OFF] [Status]
```

### Examples
```
pH:6.12 OK          
TDS:750ppm OK       
Temp:22.5C OK       
Pump:ON  Running   
```

```
pH:5.65 LOW! ADD UP 
TDS:580ppm LOW      
Temp:16.3C COLD     
Pump:OFF Dosing A  
```

```
pH:6.88 HIGH!ADD DWN
TDS:950ppm HIGH     
Temp:27.1C HOT!     
Pump:ON  Mixing B  
```

### Water Refill Mode (v2.0)
```
pH:7.02 OK          
TDS:85ppm LOW       
Temp:21.5C OK       
Pump:OFF Refilling 
```

---

## Operating Modes

### Normal Operation
- Water pump cycles: 5 min ON / 15 min OFF
- Sensors read every 5 seconds
- LCD updates every 2 seconds
- TDS checked every 60 seconds
- pH monitoring with manual adjustment prompts

### Nutrient Dosing Mode
- Triggered when TDS < 650 ppm
- Water pump override (forced ON)
- Sequential A→Mix→B→Mix cycle
- Normal operation resumes after completion
- 3-minute wait before next TDS check

### Water Refill Mode (v2.0)
- Triggered when TDS ≤ 100 ppm
- Refill pump activated for 30 seconds
- All nutrient dosing blocked during refill
- Resumes monitoring after completion
- Automatic detection on every sensor read cycle

---

## Safety Features

### Sensor Error Handling
- **Temperature**: Reject readings outside -50°C to 100°C
- **pH**: Clamp to 0-14 range
- **TDS**: Reject negative values

### Timing Overflow Protection
- Uses unsigned long (millis() wraparound safe)
- Subtraction handles 50-day rollover correctly

### Sequential Dosing Safety
- A fully diluted before B is added
- 0.5-second safety gap between pumps
- Extended mixing times ensure distribution
- Prevents chemical precipitation

### Hysteresis Protection
- TDS tolerance band prevents oscillation
- Won't re-trigger dosing until well below minimum
- 3-minute stabilization after dosing

### Mixing Override
- Normal pump schedule suspended during dosing
- Ensures continuous mixing during critical period
- Automatically resumes normal operation

### Water Refill Protection (v2.0)
- Fixed 30-second duration prevents reservoir overflow
- Nutrient dosing suspended during refill
- Prevents dosing into low-water conditions
- Automatic resumption after refill complete

---

## Serial Monitor Output

### Debug Information (Every 10 seconds)
```
=== Status ===
pH:6.12 OK
TDS:750ppm
Temp:22.5C
Pump:ON
==============
```

### With pH Warnings
```
=== Status ===
pH:5.65 LOW-AddUP
TDS:580ppm
Temp:16.3C
Pump:OFF
==============
```

---

## Calibration Procedures

### pH Sensor Calibration
1. Use pH 4.0 and pH 7.0 buffer solutions
2. Measure voltage at both points
3. Calculate offset: `PH_OFFSET = measured_pH - calculated_pH`
4. Update `#define PH_OFFSET` in code

### TDS Sensor Calibration
1. Use calibration solution (e.g., 1413 µS/cm)
2. Measure TDS reading
3. Calculate K factor: `TDS_K = actual_value / measured_value`
4. Update `#define TDS_K` in code

### Temperature Sensor
- DS18B20 pre-calibrated from factory
- No adjustment typically needed
- Verify with thermometer if accuracy critical

---

## Maintenance

### Daily
- Check LCD for warnings
- Verify pump operation
- Monitor TDS trends

### Weekly
- Clean pH probe (KCl storage solution)
- Clean TDS probe (distilled water)
- Check nutrient solution levels
- Inspect pump tubing

### Monthly
- Calibrate pH sensor with buffers
- Verify TDS calibration
- Clean temperature sensor
- Check all electrical connections

---

## Troubleshooting

### TDS Always Low
- Check sensor connection (Pin A1)
- Verify pump A/B operation
- Check nutrient solution concentration
- Calibrate TDS sensor

### pH Drift
- Recalibrate pH sensor
- Replace pH probe if old (6-12 month lifespan)
- Check reference electrode
- Verify storage in KCl solution

### Pump Not Running
- Check relay connections (Pins 7, 8, 9)
- Verify power supply
- Test pump manually
- Check for air locks in tubing

### LCD Shows Garbage
- Verify I2C address (0x27 or 0x3F)
- Check SDA/SCL connections
- Test with I2C scanner sketch

### Temperature Reading -127°C
- DS18B20 not connected or damaged
- Check 1-Wire connection (Pin 2)
- Verify 4.7kΩ pull-up resistor
- Replace sensor if defective

### Water Refill Not Triggering (v2.0)
- Check TDS sensor reads ≤100 ppm when water low
- Verify refill pump connection (Pin 10)
- Ensure TDS probe positioned at desired minimum level
- Test pump manually with digitalWrite() test sketch

### Water Refill Constant Triggering
- TDS sensor reading too low (calibration issue)
- Check sensor is properly submerged during normal operation
- Verify normal TDS reads 600-900 ppm when water adequate
- Inspect for sensor damage or contamination

---

## Technical Specifications

### Memory Usage
- Program Storage: ~8KB / 32KB (25%)
- Dynamic Memory: ~1KB / 2KB (50%)
- Global Variables: Optimized with char arrays

### Power Requirements
- Arduino: 5V @ 50mA
- LCD: 5V @ 20mA
- Sensors: 5V @ 15mA total
- Pumps: 12V (separate supply recommended)

### Environmental Limits
- Operating Temp: 0-50°C
- Humidity: <80% RH (non-condensing)
- Water: Use in splash-proof enclosure

---

## Version History

### v2.0 (Current)
- **NEW:** Automatic water level detection via TDS sensor
- **NEW:** Auto-refill pump control (30-second cycles)
- **NEW:** Low water detection at TDS ≤ 100 ppm threshold
- Sequential nutrient dosing with mixing prevention
- pH monitoring only (manual adjustment)
- Temperature-compensated TDS
- 20x4 LCD optimized display
- Non-blocking operation
- Memory optimized (char arrays vs String)

### v1.0
- Initial release
- Basic pH/TDS/temperature monitoring
- Sequential nutrient dosing algorithm
- Manual water level management

---

## Future Enhancements

### Completed
- ✅ Water level detection with auto-refill (v2.0)

### Potential Additions
- pH automation with dosing pumps
- Data logging to SD card
- WiFi/Bluetooth monitoring
- Multi-zone support
- Growth stage profiles
- Flow sensor for pump verification

---

## References

### Radish Growing Parameters
- pH Range: 5.8-6.5 (optimal: 6.0-6.3)
- TDS Range: 600-900 ppm during vegetative growth
- Temperature: 18-24°C for roots
- Harvest: 25-30 days from seed

### Nutrient Solution Mixing
- Always dilute calcium sources first
- Never mix A+B concentrates directly
- Use continuous circulation during dosing
- Wait 15 minutes after dosing before testing

---

## Support & Contact

For issues or questions:
1. Check troubleshooting section
2. Verify hardware connections
3. Review serial monitor output
4. Test sensors individually

---

**Last Updated:** January 17, 2026  
**Code Version:** 2.0  
**Author:** K1taru
