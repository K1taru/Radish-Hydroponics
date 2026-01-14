/*
 * Automated Tower Hydroponic System for Radish
 * with pH, TDS, and Temperature monitoring
 * 
 * Hardware: Arduino Uno R3, pH sensor, TDS sensor, DS18B20 temp sensor
 * Actuators: Water pump (15W), 2x Peristaltic pumps for nutrients
 * Display: 20x4 I2C LCD
 */

#include <Wire.h>               // For I2C communication
#include <LiquidCrystal_I2C.h>  // For I2C 20x4 LCD
#include <OneWire.h>            // For DS18B20 temperature sensor
#include <DallasTemperature.h>  // For DS18B20 temperature sensor


// ==================== PIN ASSIGNMENTS ====================
// Analog pins
#define PH_SENSOR_PIN     A0    // pH sensor analog input
#define TDS_SENSOR_PIN    A1    // TDS sensor analog input
#define TEMP_SENSOR_PIN   2     // DS18B20 digital pin

// Digital pins (Relays)
#define WATER_PUMP_PIN    7     // Main water circulation pump (15W)
#define PUMP_NUTRIENT_A   8     // Peristaltic pump for Nutrient A (Bottle A)
#define PUMP_NUTRIENT_B   9     // Peristaltic pump for Nutrient B (Bottle B)


// ==================== GLOBAL CONFIGS & THRESHOLDS ====================
// pH Thresholds (Optimal for radish: 5.8-6.5)
#define PH_TARGET         6.1    // Target pH setpoint
#define PH_MIN            5.8    // Minimum acceptable pH
#define PH_MAX            6.5    // Maximum acceptable pH
#define PH_TOLERANCE      0.2    // Hysteresis band to prevent pump oscillation

// TDS/PPM Thresholds (Optimal for radish: 600-900 ppm on 500 scale) (unit to be changed depending on sensor e.g. mS/cm)
#define TDS_TARGET        750    // Target TDS in ppm (mid-range for main growth)
#define TDS_MIN           600    // Minimum acceptable TDS (ppm)
#define TDS_MAX           900    // Maximum acceptable TDS (ppm)
#define TDS_TOLERANCE     50     // Hysteresis band

// Temperature Thresholds (Optimal: 18-24°C for radish root zone) (to be researched further)
#define TEMP_MIN          18.0   // Minimum acceptable temperature (°C)
#define TEMP_MAX          24.0   // Maximum acceptable temperature (°C)
#define TEMP_WARNING      26.0   // Warning threshold

// Pump Durations (in milliseconds)
#define WATER_PUMP_ON_DURATION    300000   // 5 minutes (300,000 ms) - circulation cycle
#define WATER_PUMP_OFF_DURATION   900000   // 15 minutes (900,000 ms) - wait between cycles
#define NUTRIENT_PUMP_DOSE_TIME   5000     // 5 seconds per nutrient dose
#define PUMP_WAIT_AFTER_DOSE      180000   // 3 minutes (180,000 ms) - wait for mixing before re-check

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

// Pump state tracking
unsigned long lastWaterPumpOn = 0;
unsigned long lastWaterPumpOff = 0;
bool waterPumpState = false;

// Timing variables
unsigned long lastSensorRead = 0;
unsigned long lastLCDUpdate = 0;
unsigned long lastControlCheck = 0;
unsigned long lastNutrientDose = 0;

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
  
  // Ensure all pumps are OFF at startup
  digitalWrite(WATER_PUMP_PIN, LOW);
  digitalWrite(PUMP_NUTRIENT_A, LOW);
  digitalWrite(PUMP_NUTRIENT_B, LOW);
  
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

// ==================== PUMP CONTROL FUNCTIONS ====================
void controlWaterPump(unsigned long currentMillis) {
  // Tower hydroponic: pump water up, let it drip down through plants
  
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
  // Wait after recent dosing to allow mixing and sensor stabilization
  if (currentMillis - lastNutrientDose < PUMP_WAIT_AFTER_DOSE) {
    return;  // Skip adjustment, waiting for solution to mix
  }
  
  // Check TDS level and add nutrients if needed
  if (currentTDS < (TDS_MIN + TDS_TOLERANCE)) {
    // TDS too low - add nutrients A and B (equal doses)
    Serial.print(F("TDS low ("));
    Serial.print(currentTDS);
    Serial.println(F(" ppm) - Adding nutrients..."));
    
    systemStatus = "Add Nutrients";
    updateLCD();
    
    // Dose Nutrient A
    digitalWrite(PUMP_NUTRIENT_A, HIGH);
    delay(NUTRIENT_PUMP_DOSE_TIME);
    digitalWrite(PUMP_NUTRIENT_A, LOW);
    
    delay(1000);  // Small gap between A and B
    
    // Dose Nutrient B
    digitalWrite(PUMP_NUTRIENT_B, HIGH);
    delay(NUTRIENT_PUMP_DOSE_TIME);
    digitalWrite(PUMP_NUTRIENT_B, LOW);
    
    lastNutrientDose = currentMillis;
    systemStatus = "Running";
    
    Serial.println(F("Nutrient dosing complete. Waiting for mixing..."));
  }
  
  // Check pH level and provide manual adjustment instructions
  if (currentPH < (PH_MIN + PH_TOLERANCE)) {
    // pH too low - manual adjustment needed
    Serial.print(F("pH low ("));
    Serial.print(currentPH);
    Serial.println(F(") - MANUAL: Add pH UP solution"));
    systemStatus = "pH LOW - Add UP";
    
  } else if (currentPH > (PH_MAX - PH_TOLERANCE)) {
    // pH too high - manual adjustment needed
    Serial.print(F("pH high ("));
    Serial.print(currentPH);
    Serial.println(F(") - MANUAL: Add pH DOWN solution"));
    systemStatus = "pH HIGH - Add DOWN";
  }
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
  
  // Line 3: Temperature
  lcd.setCursor(0, 2);
  lcd.print("Temp:");
  lcd.print(currentTemp, 1);
  lcd.print("C ");
  if (currentTemp >= TEMP_MIN && currentTemp <= TEMP_MAX) {
    lcd.print("OK");
  } else if (currentTemp < TEMP_MIN) {
    lcd.print("COLD");
  } else if (currentTemp > TEMP_WARNING) {
    lcd.print("HOT!");
  } else {
    lcd.print("WARM");
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
    Serial.println(F(")"));
    
    Serial.print(F("TDS: "));
    Serial.print(currentTDS, 0);
    Serial.print(F(" ppm (Target: "));
    Serial.print(TDS_TARGET);
    Serial.println(F(" ppm)"));
    
    Serial.print(F("Temperature: "));
    Serial.print(currentTemp, 1);
    Serial.println(F(" °C"));
    
    Serial.print(F("Water Pump: "));
    Serial.println(waterPumpState ? F("ON") : F("OFF"));
    
    Serial.print(F("System Status: "));
    Serial.println(systemStatus);
    Serial.println(F("===================================="));
    
    lastSerialPrint = currentMillis;
  }
}