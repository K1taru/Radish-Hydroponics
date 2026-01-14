/*
 * Automated Tower Hydroponic System for Radish
 * with pH, TDS, and Temperature monitoring
 * 
 * VERSION: 4.0
 * 
 * Hardware: Arduino Uno R3, pH sensor, TDS sensor, DS18B20 temp sensor
 * Actuators: Water pump (15W), 2x Peristaltic pumps for nutrients, 
 *            2x pH adjustment pumps, Water refill pump
 * Display: 20x4 I2C LCD
 * 
 * ==================== VERSION 4.0 CHANGES ====================
 * 1. ENHANCED pH REGULATION SYSTEM:
 *    - Fully implemented pH UP and pH DOWN pump control
 *    - Configurable pH adjustment parameters in global config section
 *    - Safety limits to prevent pH overcorrection
 *    - Maximum corrections per cycle to avoid oscillation
 * 
 * 2. CONFIGURABLE pH PARAMETERS:
 *    - PH_CORRECTION_MAX_PER_CYCLE: Maximum adjustments per hour
 *    - PH_CORRECTION_MIN_INTERVAL: Minimum time between adjustments
 *    - PH_ADJUST_PUMP_TIME: Duration of each pH dose
 *    - PH_CRITICAL_LOW/HIGH: Emergency thresholds
 * 
 * 3. RECOMMENDED pH SOLUTIONS (Safe for Hydroponics):
 *    
 *    pH DOWN (Decrease pH / Increase Acidity):
 *    - Phosphoric Acid (H3PO4) - 85% food grade [RECOMMENDED]
 *      * Dosage: 1-2 ml per 10L to lower pH by ~0.5 units
 *      * Benefits: Also provides phosphorus (P) nutrient
 *      * Safe, commonly used in commercial hydroponics
 *    - Citric Acid (C6H8O7) - Food grade powder [ORGANIC ALTERNATIVE]
 *      * Dosage: 0.5-1 gram per 10L to lower pH by ~0.5 units
 *      * Benefits: Natural, biodegradable, safe for organic systems
 *      * Caution: Less stable than phosphoric acid
 *    - Nitric Acid (HNO3) - Diluted 38% [ALTERNATIVE]
 *      * Dosage: 0.5-1 ml per 10L to lower pH by ~0.5 units
 *      * Benefits: Provides nitrogen (N) nutrient
 *      * Caution: Handle with care, corrosive
 *    
 *    pH UP (Increase pH / Decrease Acidity):
 *    - Potassium Hydroxide (KOH) - Food grade [RECOMMENDED]
 *      * Dosage: 1-2 ml per 10L to raise pH by ~0.5 units
 *      * Benefits: Provides potassium (K) nutrient
 *      * Safe, stable, commonly used in hydroponics
 *    - Potassium Carbonate (K2CO3) - Food grade [ALTERNATIVE]
 *      * Dosage: 1-2 grams per 10L to raise pH by ~0.5 units
 *      * Benefits: Less caustic than KOH, provides K
 *      * Good for systems with high CO2
 *    - Potassium Silicate (K2SiO3) - Liquid [DUAL BENEFIT]
 *      * Dosage: 2-3 ml per 10L to raise pH by ~0.5 units
 *      * Benefits: Provides K + Silica (strengthens plants)
 *      * Excellent for disease resistance
 * 
 *    SAFETY NOTES:
 *    - Always dilute concentrates before adding to reservoir
 *    - Add small amounts, wait 15-30 minutes, then re-measure
 *    - Never mix pH UP and pH DOWN concentrates directly
 *    - Store in clearly labeled, chemical-resistant containers
 *    - Use gloves and eye protection when handling
 * 
 * 4. pH ADJUSTMENT ALGORITHM:
 *    - Monitors pH continuously every 5 seconds
 *    - Adjusts only when outside target range for stability
 *    - Enforces minimum 3-minute wait between adjustments
 *    - Limits maximum corrections to prevent oscillation
 *    - Logs all pH adjustments for tracking
 * 
 * 5. SAFETY FEATURES:
 *    - Critical pH thresholds trigger warnings
 *    - Maximum corrections per cycle prevents over-dosing
 *    - Hysteresis band prevents pump hunting
 *    - Independent timing for pH vs nutrient adjustments
 * 
 * RATIONALE:
 * Proper pH control is critical for nutrient availability in hydroponics.
 * Most nutrients are available at pH 5.8-6.5. Outside this range, plants
 * cannot absorb nutrients even if they're present. Automated pH regulation
 * maintains optimal conditions 24/7 without manual intervention.
 * 
 * ==================== VERSION 3.0 CHANGES ====================
 * (Previous version - Automatic water refill system)
 * - See Version 3.0 for water level monitoring and refill details
 * 
 * ==================== VERSION 2.0 CHANGES ====================
 * (Previous version - Sequential nutrient dosing with mixing cycles)
 * - See Version 2.0 for A/B nutrient mixing algorithm details
 * ============================================================
 */

#include <Wire.h>               // For I2C communication
#include <LiquidCrystal_I2C.h>  // For I2C 20x4 LCD
#include <OneWire.h>            // For DS18B20 temperature sensor
#include <DallasTemperature.h>  // For DS18B20 temperature sensor


// ==================== PIN ASSIGNMENTS ====================
// Analog pins
#define PH_SENSOR_PIN       A0    // pH sensor analog input
#define TDS_SENSOR_PIN      A1    // TDS sensor analog input
#define WATER_LEVEL_PIN     A2    // Water level sensor analog input (V3.0)
#define TEMP_SENSOR_PIN     2     // DS18B20 digital pin

// Digital pins (Relays)
#define WATER_PUMP_PIN      7     // Main water circulation pump (15W)
#define PUMP_NUTRIENT_A     8     // Peristaltic pump for Nutrient A (Bottle A)
#define PUMP_NUTRIENT_B     9     // Peristaltic pump for Nutrient B (Bottle B)
#define PUMP_PH_DOWN        10    // Optional: pH down solution pump
#define PUMP_PH_UP          11    // Optional: pH up solution pump
#define PUMP_WATER_REFILL   12    // Water refill pump (V3.0)


// ==================== GLOBAL CONFIGS & THRESHOLDS ====================
// pH Thresholds (Optimal for radish: 5.8-6.5)
#define PH_TARGET         6.1    // Target pH setpoint
#define PH_MIN            5.8    // Minimum acceptable pH
#define PH_MAX            6.5    // Maximum acceptable pH
#define PH_TOLERANCE      0.2    // Hysteresis band to prevent pump oscillation

// V4.0: pH Regulation Parameters (Configurable)
#define PH_CRITICAL_LOW           5.0    // Critical low pH - emergency warning
#define PH_CRITICAL_HIGH          7.5    // Critical high pH - emergency warning
#define PH_CORRECTION_MAX_PER_CYCLE  5   // Maximum pH corrections allowed per cycle (prevents oscillation)
#define PH_CORRECTION_MIN_INTERVAL  180000  // 3 minutes (180,000 ms) - minimum time between pH adjustments
#define PH_ADJUST_SMALL_DOSE_TIME   1000    // 1 second - small dose for fine adjustment
#define PH_ADJUST_NORMAL_DOSE_TIME  2000    // 2 seconds - normal dose for moderate correction
#define PH_ADJUST_LARGE_DOSE_TIME   3000    // 3 seconds - large dose for significant correction

// TDS/PPM Thresholds (Optimal for radish: 600-900 ppm on 500 scale) (unit to be changed depending on sensor e.g. mS/cm)
#define TDS_TARGET        750    // Target TDS in ppm (mid-range for main growth)
#define TDS_MIN           600    // Minimum acceptable TDS (ppm)
#define TDS_MAX           900    // Maximum acceptable TDS (ppm)
#define TDS_TOLERANCE     50     // Hysteresis band

// Temperature Thresholds (Optimal: 18-24°C for radish root zone) (to be researched further)
#define TEMP_MIN          18.0   // Minimum acceptable temperature (°C)
#define TEMP_MAX          24.0   // Maximum acceptable temperature (°C)
#define TEMP_WARNING      26.0   // Warning threshold

// Water Level Thresholds (V3.0 - Automatic refill system)
#define WATER_LEVEL_MIN        30    // Minimum water level % - triggers refill
#define WATER_LEVEL_MAX        85    // Maximum water level % - stops refill (prevents overflow)
#define WATER_LEVEL_HYSTERESIS 5     // Hysteresis band to prevent pump oscillation
#define WATER_LEVEL_CRITICAL   15    // Critical low level - warning threshold

// Pump Durations (in milliseconds)
#define WATER_PUMP_ON_DURATION    300000   // 5 minutes (300,000 ms) - circulation cycle
#define WATER_PUMP_OFF_DURATION   900000   // 15 minutes (900,000 ms) - wait between cycles
#define NUTRIENT_PUMP_DOSE_TIME   5000     // 5 seconds per nutrient dose
#define PH_ADJUST_PUMP_TIME       2000     // 2 seconds per pH adjustment dose (legacy - use PH_ADJUST_*_DOSE_TIME instead)
#define PUMP_WAIT_AFTER_DOSE      180000   // 3 minutes (180,000 ms) - wait for mixing before re-check

// Mixing Durations (V2.0 - Sequential dosing with mixing prevention)
#define MIXING_DURATION_A         180000   // 3 minutes (180,000 ms) - mix Solution A before adding B
#define MIXING_DURATION_B         300000   // 5 minutes (300,000 ms) - mix Solution B after adding

// Water Refill Durations (V3.0)
#define WATER_REFILL_CHECK_INTERVAL  10000  // Check water level every 10 seconds
#define WATER_REFILL_MAX_DURATION    60000  // Maximum continuous refill time (60 sec safety limit)

// Sensor Calibration Constants
#define PH_OFFSET         0.0    // pH calibration offset (adjust after calibration)
#define TDS_K_VALUE       1.0    // TDS sensor K value (compensates for probe constant)
#define VREF              5.0    // Arduino reference voltage
#define ADC_RESOLUTION    1024.0 // 10-bit ADC

// Timing intervals
#define SENSOR_READ_INTERVAL      5000     // Read sensors every 5 seconds
#define LCD_UPDATE_INTERVAL       2000     // Update LCD every 2 seconds
#define CONTROL_CHECK_INTERVAL    60000    // Check and adjust pH/TDS every 60 seconds


// ==================== HARDWARE INITIALIZATION ====================
// LCD: 20x4 I2C display (address 0x27 or 0x3F - common for I2C LCDs)
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Change to 0x3F if 0x27 doesn't work

// Temperature sensor
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature tempSensors(&oneWire);


// ==================== GLOBAL VARIABLES ====================
// Sensor readings
float currentPH = 7.0;
float currentTDS = 0.0;
float currentTemp = 20.0;
float currentWaterLevel = 50.0;  // V3.0: Water level percentage (0-100%)

// Pump state tracking
unsigned long lastWaterPumpOn = 0;
unsigned long lastWaterPumpOff = 0;
bool waterPumpState = false;
bool waterRefillPumpState = false;  // V3.0: Refill pump state

// Timing variables
unsigned long lastSensorRead = 0;
unsigned long lastLCDUpdate = 0;
unsigned long lastControlCheck = 0;
unsigned long lastNutrientDose = 0;
unsigned long lastPHAdjust = 0;
unsigned long lastWaterLevelCheck = 0;      // V3.0: Water level check timing
unsigned long waterRefillStartTime = 0;     // V3.0: Track refill duration

// V4.0: pH correction tracking
int phCorrectionCount = 0;                   // Track number of pH corrections this cycle
unsigned long phCorrectionCycleStart = 0;    // Start time of current correction cycle (resets hourly)

// V2.0: Mixing state machine variables
enum MixingState {
  MIXING_NONE,      // No mixing in progress, normal operation
  DOSING_A,         // Currently dosing Solution A
  MIXING_A,         // Water pump mixing Solution A
  DOSING_B,         // Currently dosing Solution B
  MIXING_B          // Water pump mixing Solution B
};
MixingState mixingState = MIXING_NONE;
unsigned long mixingStartTime = 0;
bool mixingOverride = false;  // Flag to override normal pump schedule during mixing

// System status flags
bool systemInitialized = false;
String systemStatus = "Starting...";


// ==================== SETUP ====================
void setup() {
  Serial.begin(9600);
  Serial.println(F("Tower Hydroponic System - Initializing..."));
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hydroponic System");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  
  // Configure pin modes
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(PUMP_NUTRIENT_A, OUTPUT);
  pinMode(PUMP_NUTRIENT_B, OUTPUT);
  pinMode(PUMP_PH_DOWN, OUTPUT);
  pinMode(PUMP_PH_UP, OUTPUT);
  pinMode(PUMP_WATER_REFILL, OUTPUT);  // V3.0: Water refill pump
  
  // Ensure all pumps are OFF at startup
  digitalWrite(WATER_PUMP_PIN, LOW);
  digitalWrite(PUMP_NUTRIENT_A, LOW);
  digitalWrite(PUMP_NUTRIENT_B, LOW);
  digitalWrite(PUMP_PH_DOWN, LOW);
  digitalWrite(PUMP_PH_UP, LOW);
  digitalWrite(PUMP_WATER_REFILL, LOW);  // V3.0: Refill pump OFF
  
  // Initialize temperature sensor
  tempSensors.begin();
  
  delay(2000);  // Let sensors stabilize
  
  // Read initial sensor values
  readSensors();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Ready!");
  delay(2000);
  
  systemInitialized = true;
  systemStatus = "Running";
  phCorrectionCycleStart = millis();  // V4.0: Initialize pH correction cycle timer
  Serial.println(F("System initialized successfully!"));
}


// ==================== MAIN LOOP ====================
void loop() {
  unsigned long currentMillis = millis();
  
  // Read sensors at regular intervals
  if (currentMillis - lastSensorRead >= SENSOR_READ_INTERVAL) {
    readSensors();
    lastSensorRead = currentMillis;
  }
  
  // Update LCD display
  if (currentMillis - lastLCDUpdate >= LCD_UPDATE_INTERVAL) {
    updateLCD();
    lastLCDUpdate = currentMillis;
  }
  
  // Control water pump (tower irrigation cycle)
  controlWaterPump(currentMillis);
  
  // V3.0: Check and maintain water level
  if (currentMillis - lastWaterLevelCheck >= WATER_REFILL_CHECK_INTERVAL) {
    controlWaterRefill(currentMillis);
    lastWaterLevelCheck = currentMillis;
  }
  
  // Check and adjust pH/TDS
  if (currentMillis - lastControlCheck >= CONTROL_CHECK_INTERVAL) {
    checkAndAdjustNutrients(currentMillis);
    lastControlCheck = currentMillis;
  }
  
  // Print to serial for debugging
  printSerialData();
}

// ==================== SENSOR READING FUNCTIONS ====================
void readSensors() {
  currentPH = readPH();
  currentTDS = readTDS();
  currentTemp = readTemperature();
  currentWaterLevel = readWaterLevel();  // V3.0: Read water level
}

float readPH() {
  int rawValue = analogRead(PH_SENSOR_PIN);
  float voltage = rawValue * (VREF / ADC_RESOLUTION);
  
  // pH calculation (typical pH sensor: pH = 7 at 2.5V, -0.18V per pH unit)
  // Adjust this formula based on your specific pH sensor calibration
  float pH = 7.0 - ((voltage - 2.5) / 0.18) + PH_OFFSET;
  
  // Clamp to realistic range
  if (pH < 0.0) pH = 0.0;
  if (pH > 14.0) pH = 14.0;
  
  return pH;
}

float readTDS() {
  int rawValue = analogRead(TDS_SENSOR_PIN);
  float voltage = rawValue * (VREF / ADC_RESOLUTION);
  
  // Temperature compensation for TDS
  float compensationCoefficient = 1.0 + 0.02 * (currentTemp - 25.0);
  float compensationVoltage = voltage / compensationCoefficient;
  
  // TDS calculation (typical: TDS = (133.42 * voltage^3 - 255.86 * voltage^2 + 857.39 * voltage) * 0.5)
  // Simplified linear approximation: TDS (ppm) ≈ voltage * 500 * K_value
  float tds = (compensationVoltage * 500.0 * TDS_K_VALUE);
  
  if (tds < 0.0) tds = 0.0;
  
  return tds;
}

float readTemperature() {
  tempSensors.requestTemperatures();
  float temp = tempSensors.getTempCByIndex(0);
  
  // Check for sensor error
  if (temp == DEVICE_DISCONNECTED_C || temp < -50.0 || temp > 100.0) {
    Serial.println(F("Warning: Temperature sensor error!"));
    return currentTemp;  // Return last valid reading
  }
  
  return temp;
}

// V3.0: Read water level sensor
float readWaterLevel() {
  int rawValue = analogRead(WATER_LEVEL_PIN);
  
  // Water level calculation depends on sensor type:
  // Option 1: Ultrasonic sensor (HC-SR04) - would use digital pins with pulseIn()
  // Option 2: Analog float sensor - voltage proportional to height
  // Option 3: Pressure/submersible sensor - voltage proportional to depth
  
  // For analog water level sensor (0-1023 ADC maps to 0-100% water level)
  // Adjust this calculation based on your specific sensor
  float waterLevelPercent = (rawValue / ADC_RESOLUTION) * 100.0;
  
  // Clamp to 0-100% range
  if (waterLevelPercent < 0.0) waterLevelPercent = 0.0;
  if (waterLevelPercent > 100.0) waterLevelPercent = 100.0;
  
  return waterLevelPercent;
}

// ==================== WATER REFILL CONTROL (V3.0) ====================
void controlWaterRefill(unsigned long currentMillis) {
  // Safety check: Don't refill during nutrient mixing (prevents dilution of freshly dosed nutrients)
  if (mixingState != MIXING_NONE) {
    return;  // Skip refill during nutrient mixing sequence
  }
  
  // Check for refill timeout (safety limit)
  if (waterRefillPumpState && (currentMillis - waterRefillStartTime >= WATER_REFILL_MAX_DURATION)) {
    // Pump has been running too long, stop and log warning
    digitalWrite(PUMP_WATER_REFILL, LOW);
    waterRefillPumpState = false;
    Serial.println(F("WARNING: Water refill timeout - pump stopped for safety"));
    systemStatus = "Refill Timeout!";
    return;
  }
  
  // Check water level and control refill pump
  if (!waterRefillPumpState) {
    // Pump is OFF - check if refill needed
    if (currentWaterLevel < (WATER_LEVEL_MIN + WATER_LEVEL_HYSTERESIS)) {
      // Water level too low - start refill
      digitalWrite(PUMP_WATER_REFILL, HIGH);
      waterRefillPumpState = true;
      waterRefillStartTime = currentMillis;
      Serial.print(F("Water level low ("));
      Serial.print(currentWaterLevel, 1);
      Serial.println(F("%) - Starting refill..."));
      systemStatus = "Refilling H2O";
    }
  } else {
    // Pump is ON - check if we should stop
    if (currentWaterLevel >= (WATER_LEVEL_MAX - WATER_LEVEL_HYSTERESIS)) {
      // Water level reached maximum, stop refill
      digitalWrite(PUMP_WATER_REFILL, LOW);
      waterRefillPumpState = false;
      Serial.print(F("Water refill complete. Level: "));
      Serial.print(currentWaterLevel, 1);
      Serial.println(F("%"));
      systemStatus = "Running";
    }
  }
}

// ==================== PUMP CONTROL FUNCTIONS ====================
void controlWaterPump(unsigned long currentMillis) {
  // V2.0: Check if mixing override is active (nutrient mixing in progress)
  if (mixingOverride) {
    // Keep pump ON during mixing cycles, skip normal schedule
    if (!waterPumpState) {
      digitalWrite(WATER_PUMP_PIN, HIGH);
      waterPumpState = true;
      Serial.println(F("Water pump ON - Mixing cycle override"));
    }
    return;  // Exit, mixing logic will control pump
  }
  
  // Normal tower hydroponic irrigation: pump water up, let it drip down through plants
  if (!waterPumpState) {
    // Pump is OFF - check if it's time to turn ON
    if (currentMillis - lastWaterPumpOff >= WATER_PUMP_OFF_DURATION) {
      digitalWrite(WATER_PUMP_PIN, HIGH);
      waterPumpState = true;
      lastWaterPumpOn = currentMillis;
      Serial.println(F("Water pump ON - Irrigation cycle started"));
    }
  } else {
    // Pump is ON - check if it's time to turn OFF
    if (currentMillis - lastWaterPumpOn >= WATER_PUMP_ON_DURATION) {
      digitalWrite(WATER_PUMP_PIN, LOW);
      waterPumpState = false;
      lastWaterPumpOff = currentMillis;
      Serial.println(F("Water pump OFF - Rest period"));
    }
  }
}

void checkAndAdjustNutrients(unsigned long currentMillis) {
  // V2.0: Handle mixing state machine for sequential dosing
  if (mixingState != MIXING_NONE) {
    handleMixingSequence(currentMillis);
    return;  // Skip normal checks during mixing sequence
  }
  
  // Wait after recent dosing to allow mixing and sensor stabilization
  if (currentMillis - lastNutrientDose < PUMP_WAIT_AFTER_DOSE) {
    return;  // Skip adjustment, waiting for solution to mix
  }
  
  // Check TDS level and add nutrients if needed
  if (currentTDS < (TDS_MIN + TDS_TOLERANCE)) {
    // TDS too low - start sequential dosing with mixing
    Serial.print(F("TDS low ("));
    Serial.print(currentTDS);
    Serial.println(F(" ppm) - Starting nutrient dosing sequence..."));
    
    // V2.0: Start mixing sequence with Solution A
    mixingState = DOSING_A;
    mixingOverride = true;  // Override normal pump schedule
    mixingStartTime = currentMillis;
  }
  
  // Check pH level and adjust if needed (after nutrient adjustment settles)
  // V4.0: Enhanced pH regulation with safety limits
  if (currentMillis - lastPHAdjust >= PH_CORRECTION_MIN_INTERVAL) {
    adjustPH(currentMillis);
  }
}

// V2.0: Handle sequential nutrient dosing with mixing cycles
void handleMixingSequence(unsigned long currentMillis) {
  unsigned long elapsedTime = currentMillis - mixingStartTime;
  
  switch (mixingState) {
    case DOSING_A:
      // Dose Solution A
      systemStatus = "Dosing A";
      updateLCD();
      
      Serial.println(F("Dosing Solution A..."));
      digitalWrite(PUMP_NUTRIENT_A, HIGH);
      delay(NUTRIENT_PUMP_DOSE_TIME);
      digitalWrite(PUMP_NUTRIENT_A, LOW);
      
      // Transition to mixing A
      mixingState = MIXING_A;
      mixingStartTime = currentMillis;
      Serial.println(F("Solution A dosed. Starting mixing cycle..."));
      break;
      
    case MIXING_A:
      // Mix Solution A with water pump
      systemStatus = "Mixing A";
      if (elapsedTime >= MIXING_DURATION_A) {
        // Mixing complete, move to dose B
        mixingState = DOSING_B;
        mixingStartTime = currentMillis;
        Serial.println(F("Solution A mixed. Ready for Solution B..."));
      }
      break;
      
    case DOSING_B:
      // Dose Solution B (after A is diluted)
      systemStatus = "Dosing B";
      updateLCD();
      
      Serial.println(F("Dosing Solution B..."));
      delay(1000);  // Small gap for safety
      digitalWrite(PUMP_NUTRIENT_B, HIGH);
      delay(NUTRIENT_PUMP_DOSE_TIME);
      digitalWrite(PUMP_NUTRIENT_B, LOW);
      
      // Transition to mixing B
      mixingState = MIXING_B;
      mixingStartTime = currentMillis;
      Serial.println(F("Solution B dosed. Starting final mixing cycle..."));
      break;
      
    case MIXING_B:
      // Mix Solution B with water pump (longer mixing)
      systemStatus = "Mixing B";
      if (elapsedTime >= MIXING_DURATION_B) {
        // All mixing complete, return to normal operation
        mixingState = MIXING_NONE;
        mixingOverride = false;
        lastNutrientDose = currentMillis;
        systemStatus = "Running";
        
        // Reset water pump tracking to resume normal schedule
        lastWaterPumpOff = currentMillis;
        digitalWrite(WATER_PUMP_PIN, LOW);
        waterPumpState = false;
        
        Serial.println(F("Nutrient dosing sequence complete. Returning to normal operation."));
      }
      break;
      
    default:
      // Should never reach here, reset to safe state
      mixingState = MIXING_NONE;
      mixingOverride = false;
      break;
  }
}

// V4.0: Enhanced pH adjustment with safety limits and variable dosing
void adjustPH(unsigned long currentMillis) {
  // Reset correction counter every hour (3600000 ms)
  if (currentMillis - phCorrectionCycleStart >= 3600000) {
    phCorrectionCount = 0;
    phCorrectionCycleStart = currentMillis;
    Serial.println(F("pH correction cycle reset - counter cleared"));
  }
  
  // Check if maximum corrections reached for this cycle
  if (phCorrectionCount >= PH_CORRECTION_MAX_PER_CYCLE) {
    static unsigned long lastWarning = 0;
    if (currentMillis - lastWarning >= 600000) {  // Warn every 10 minutes
      Serial.print(F("WARNING: Maximum pH corrections ("));
      Serial.print(PH_CORRECTION_MAX_PER_CYCLE);
      Serial.println(F(") reached this cycle. Manual check recommended."));
      lastWarning = currentMillis;
    }
    return;  // Skip adjustment until cycle resets
  }
  
  // Critical pH level warnings
  if (currentPH < PH_CRITICAL_LOW) {
    Serial.print(F("CRITICAL: pH dangerously low ("));
    Serial.print(currentPH, 2);
    Serial.println(F(")! Check pH UP solution and sensor calibration!"));
    systemStatus = "pH CRIT LOW!";
  } else if (currentPH > PH_CRITICAL_HIGH) {
    Serial.print(F("CRITICAL: pH dangerously high ("));
    Serial.print(currentPH, 2);
    Serial.println(F(")! Check pH DOWN solution and sensor calibration!"));
    systemStatus = "pH CRIT HIGH!";
  }
  
  // Determine pH deviation from target range
  float phDeviation = 0.0;
  bool needsAdjustment = false;
  bool adjustUp = false;
  
  if (currentPH < (PH_MIN + PH_TOLERANCE)) {
    // pH too low (acidic) - need to raise pH
    phDeviation = PH_TARGET - currentPH;
    needsAdjustment = true;
    adjustUp = true;
  } else if (currentPH > (PH_MAX - PH_TOLERANCE)) {
    // pH too high (alkaline) - need to lower pH
    phDeviation = currentPH - PH_TARGET;
    needsAdjustment = true;
    adjustUp = false;
  }
  
  if (!needsAdjustment) {
    return;  // pH is within acceptable range
  }
  
  // Select dose duration based on deviation magnitude
  int doseDuration;
  if (phDeviation >= 1.0) {
    // Large deviation (≥1.0 pH units)
    doseDuration = PH_ADJUST_LARGE_DOSE_TIME;
    Serial.print(F("Large pH deviation detected: "));
  } else if (phDeviation >= 0.5) {
    // Moderate deviation (0.5-1.0 pH units)
    doseDuration = PH_ADJUST_NORMAL_DOSE_TIME;
    Serial.print(F("Moderate pH deviation detected: "));
  } else {
    // Small deviation (<0.5 pH units)
    doseDuration = PH_ADJUST_SMALL_DOSE_TIME;
    Serial.print(F("Small pH deviation detected: "));
  }
  
  Serial.print(phDeviation, 2);
  Serial.println(F(" pH units"));
  
  // Perform pH adjustment
  if (adjustUp) {
    // Add pH UP solution (Potassium Hydroxide, Potassium Carbonate, etc.)
    Serial.print(F("pH low ("));
    Serial.print(currentPH, 2);
    Serial.print(F(") - Adding pH UP for "));
    Serial.print(doseDuration / 1000);
    Serial.println(F(" seconds..."));
    
    systemStatus = "pH UP";
    updateLCD();
    
    digitalWrite(PUMP_PH_UP, HIGH);
    delay(doseDuration);
    digitalWrite(PUMP_PH_UP, LOW);
    
  } else {
    // Add pH DOWN solution (Phosphoric Acid, Citric Acid, Nitric Acid, etc.)
    Serial.print(F("pH high ("));
    Serial.print(currentPH, 2);
    Serial.print(F(") - Adding pH DOWN for "));
    Serial.print(doseDuration / 1000);
    Serial.println(F(" seconds..."));
    
    systemStatus = "pH DOWN";
    updateLCD();
    
    digitalWrite(PUMP_PH_DOWN, HIGH);
    delay(doseDuration);
    digitalWrite(PUMP_PH_DOWN, LOW);
  }
  
  // Update timing and counter
  lastPHAdjust = currentMillis;
  phCorrectionCount++;
  systemStatus = "Running";
  
  Serial.print(F("pH adjustment complete. Correction #"));
  Serial.print(phCorrectionCount);
  Serial.print(F(" of "));
  Serial.print(PH_CORRECTION_MAX_PER_CYCLE);
  Serial.println(F(" this cycle."));
  Serial.print(F("Waiting "));
  Serial.print(PH_CORRECTION_MIN_INTERVAL / 60000);
  Serial.println(F(" minutes before next pH check..."));
}

// ==================== LCD DISPLAY FUNCTION ====================
void updateLCD() {
  lcd.clear();
  
  // Line 1: pH value
  lcd.setCursor(0, 0);
  lcd.print("pH:");
  lcd.print(currentPH, 2);
  lcd.print(" ");
  if (currentPH >= PH_MIN && currentPH <= PH_MAX) {
    lcd.print("OK");
  } else if (currentPH < PH_MIN) {
    lcd.print("LOW");
  } else {
    lcd.print("HIGH");
  }
  
  // Line 2: TDS value
  lcd.setCursor(0, 1);
  lcd.print("TDS:");
  lcd.print((int)currentTDS);
  lcd.print("ppm ");
  if (currentTDS >= TDS_MIN && currentTDS <= TDS_MAX) {
    lcd.print("OK");
  } else if (currentTDS < TDS_MIN) {
    lcd.print("LOW");
  } else {
    lcd.print("HIGH");
  }
  
  // Line 3: Water Level (V3.0 - Changed from temperature)
  lcd.setCursor(0, 2);
  lcd.print("Water:");
  lcd.print((int)currentWaterLevel);
  lcd.print("% ");
  if (currentWaterLevel >= WATER_LEVEL_MIN && currentWaterLevel <= WATER_LEVEL_MAX) {
    lcd.print("OK");
  } else if (currentWaterLevel < WATER_LEVEL_CRITICAL) {
    lcd.print("CRIT!");
  } else if (currentWaterLevel < WATER_LEVEL_MIN) {
    lcd.print("LOW");
  } else {
    lcd.print("HIGH");
  }
  
  // Line 4: Pump status
  lcd.setCursor(0, 3);
  lcd.print("Pump:");
  if (waterPumpState) {
    lcd.print("ON ");
  } else {
    lcd.print("OFF");
  }
  lcd.print(" ");
  lcd.print(systemStatus);
}

// ==================== SERIAL DEBUG OUTPUT ====================
void printSerialData() {
  static unsigned long lastSerialPrint = 0;
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastSerialPrint >= 10000) {  // Print every 10 seconds
    Serial.println(F("========== Sensor Readings =========="));
    Serial.print(F("pH: "));
    Serial.print(currentPH, 2);
    Serial.print(F(" (Target: "));
    Serial.print(PH_TARGET);
    Serial.print(F(", Range: "));
    Serial.print(PH_MIN);
    Serial.print(F("-"));
    Serial.print(PH_MAX);
    Serial.println(F(")"));
    
    // V4.0: pH correction statistics
    Serial.print(F("pH Corrections: "));
    Serial.print(phCorrectionCount);
    Serial.print(F(" / "));
    Serial.print(PH_CORRECTION_MAX_PER_CYCLE);
    Serial.println(F(" this cycle"));
    
    Serial.print(F("TDS: "));
    Serial.print(currentTDS, 0);
    Serial.print(F(" ppm (Target: "));
    Serial.print(TDS_TARGET);
    Serial.println(F(" ppm)"));
    
    Serial.print(F("Temperature: "));
    Serial.print(currentTemp, 1);
    Serial.println(F(" °C"));
    
    // V3.0: Water level information
    Serial.print(F("Water Level: "));
    Serial.print(currentWaterLevel, 1);
    Serial.print(F("% (Min: "));
    Serial.print(WATER_LEVEL_MIN);
    Serial.print(F("%, Max: "));
    Serial.print(WATER_LEVEL_MAX);
    Serial.println(F("%)"));
    
    Serial.print(F("Water Pump: "));
    Serial.println(waterPumpState ? F("ON") : F("OFF"));
    
    // V3.0: Refill pump status
    Serial.print(F("Refill Pump: "));
    Serial.println(waterRefillPumpState ? F("ON") : F("OFF"));
    
    Serial.print(F("System Status: "));
    Serial.println(systemStatus);
    Serial.println(F("===================================="));
    
    lastSerialPrint = currentMillis;
  }
}