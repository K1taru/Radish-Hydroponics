// Automated Tower Hydroponic System for Radish - v5.0
// Supports: OLED SH1106 AND 20x4 I2C LCD (Both displays simultaneously)
// DS18B20 Digital Temperature Sensor on Analog Pin A2

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C
Adafruit_SH1106G oled = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// LCD Display Configuration
#define LCD_ADDRESS 0x27    // Common addresses: 0x27 or 0x3F
#define LCD_COLS 20
#define LCD_ROWS 4
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

// Pin Assignments
#define PH_SENSOR_PIN     A0
#define TDS_SENSOR_PIN    A1
#define TEMP_SENSOR_PIN   A2    // DS18B20 data pin (A2 used as digital)
#define WATER_PUMP_PIN    7
#define PUMP_NUTRIENT_A   8
#define PUMP_NUTRIENT_B   9
#define WATER_REFILL_PUMP 10

// DS18B20 Temperature Sensor Setup
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature tempSensors(&oneWire);

// pH Thresholds (Radish: 5.8-6.5)
#define PH_MIN            5.8
#define PH_MAX            6.5

// TDS Thresholds (Radish: 600-900 ppm)
#define TDS_MIN           600
#define TDS_MAX           900
#define TDS_TOLERANCE     50

// Low Water Detection (via TDS sensor)
#define LOW_WATER_TDS_THRESHOLD  100   // TDS <= 100 ppm indicates low water level
#define WATER_REFILL_DURATION    30000UL  // 30 seconds refill pump run time

// Temperature Thresholds (18-24°C optimal)
#define TEMP_MIN          18.0
#define TEMP_MAX          24.0
#define TEMP_WARNING      26.0

// Timing (ms)
#define PUMP_ON_TIME      300000UL  // 5 min
#define PUMP_OFF_TIME     900000UL  // 15 min
#define DOSE_TIME         5000UL    // 5 sec
#define MIX_WAIT          180000UL  // 3 min
#define MIX_A_TIME        180000UL  // 3 min
#define MIX_B_TIME        300000UL  // 5 min
#define SENSOR_INTERVAL   5000UL
#define LCD_INTERVAL      2000UL
#define CHECK_INTERVAL    60000UL

// Calibration
#define PH_OFFSET         0.0
#define TDS_K             1.0
#define VREF              5.0

// Sensor values
float pH = 7.0, tds = 0.0, temp = 20.0;

// Pump state
bool pumpOn = false;
unsigned long pumpOnTime = 0, pumpOffTime = 0;

// Timing
unsigned long lastRead = 0, lastLCD = 0, lastCheck = 0, lastDose = 0;

// Mixing state machine
enum MixState { MIX_NONE, DOSE_A, MIX_A, DOSE_B, MIX_B };
MixState mixState = MIX_NONE;
unsigned long mixStart = 0;
bool mixOverride = false;

// Status (max 11 chars for LCD line 4)
char status[12] = "Running";

// Water refill state
bool refillActive = false;
unsigned long refillStartTime = 0;

void setup() {
  Serial.begin(9600);
  
  // Initialize OLED display
  if(!oled.begin(OLED_ADDRESS, true)) {
    Serial.println(F("SH1106 OLED failed"));
  } else {
    oled.clearDisplay();
    oled.setContrast(255);
    oled.setTextSize(1);
    oled.setTextColor(SH110X_WHITE);
    oled.setCursor(20, 20);
    oled.println(F("Hydroponic v5.0"));
    oled.setCursor(20, 35);
    oled.println(F("Initializing..."));
    oled.display();
  }
  
  // Initialize LCD display
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2, 1);
  lcd.print(F("Hydroponic v5.0"));
  lcd.setCursor(3, 2);
  lcd.print(F("Initializing..."));
  
  // Initialize DS18B20 temperature sensor
  pinMode(TEMP_SENSOR_PIN, INPUT_PULLUP);  // Enable internal pull-up (no external resistor)
  tempSensors.begin();
  tempSensors.setResolution(12);  // 12-bit resolution (0.0625°C)
  
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(PUMP_NUTRIENT_A, OUTPUT);
  pinMode(PUMP_NUTRIENT_B, OUTPUT);
  pinMode(WATER_REFILL_PUMP, OUTPUT);
  
  digitalWrite(WATER_PUMP_PIN, LOW);
  digitalWrite(PUMP_NUTRIENT_A, LOW);
  digitalWrite(PUMP_NUTRIENT_B, LOW);
  digitalWrite(WATER_REFILL_PUMP, LOW);
  
  delay(2000);
  readSensors();
  
  // Show ready message on OLED
  oled.clearDisplay();
  oled.setTextColor(SH110X_WHITE);
  oled.setCursor(25, 28);
  oled.println(F("System Ready!"));
  oled.display();
  
  // Show ready message on LCD
  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print(F("System Ready!"));
  
  delay(1500);
  
  Serial.println(F("System ready"));
}

void loop() {
  unsigned long now = millis();
  
  if (now - lastRead >= SENSOR_INTERVAL) {
    readSensors();
    lastRead = now;
  }
  
  if (now - lastLCD >= LCD_INTERVAL) {
    updateDisplay();
    lastLCD = now;
  }
  
  controlPump(now);
  
  checkWaterLevel(now);
  
  if (now - lastCheck >= CHECK_INTERVAL) {
    checkNutrients(now);
    lastCheck = now;
  }
  
  printSerial();
}

float readDS18B20Temperature() {
  tempSensors.requestTemperatures();
  float t = tempSensors.getTempCByIndex(0);
  
  // Check for sensor error (DEVICE_DISCONNECTED_C = -127)
  if (t == DEVICE_DISCONNECTED_C || t < -50.0 || t > 100.0) {
    Serial.println(F("DS18B20 read error!"));
    return temp;  // Return last valid reading
  }
  return t;
}

void readSensors() {
  // pH reading
  float v = analogRead(PH_SENSOR_PIN) * (VREF / 1024.0);
  pH = constrain(7.0 - ((v - 2.5) / 0.18) + PH_OFFSET, 0.0, 14.0);
  
  // Temperature reading (DS18B20 digital sensor)
  float t = readDS18B20Temperature();
  if (t > -50.0 && t < 100.0) temp = t;
  
  // TDS reading with temp compensation
  v = analogRead(TDS_SENSOR_PIN) * (VREF / 1024.0);
  float comp = 1.0 + 0.02 * (temp - 25.0);
  tds = max(0.0, (v / comp) * 500.0 * TDS_K);
}

void controlPump(unsigned long now) {
  if (mixOverride) {
    if (!pumpOn) {
      digitalWrite(WATER_PUMP_PIN, HIGH);
      pumpOn = true;
    }
    return;
  }
  
  if (!pumpOn && (now - pumpOffTime >= PUMP_OFF_TIME)) {
    digitalWrite(WATER_PUMP_PIN, HIGH);
    pumpOn = true;
    pumpOnTime = now;
  } else if (pumpOn && (now - pumpOnTime >= PUMP_ON_TIME)) {
    digitalWrite(WATER_PUMP_PIN, LOW);
    pumpOn = false;
    pumpOffTime = now;
  }
}

void checkWaterLevel(unsigned long now) {
  // Check if refill is currently active
  if (refillActive) {
    if (now - refillStartTime >= WATER_REFILL_DURATION) {
      // Refill complete
      digitalWrite(WATER_REFILL_PUMP, LOW);
      refillActive = false;
      strcpy(status, "Running");
      Serial.println(F("Water refill complete"));
    }
    return;
  }
  
  // Detect low water level: TDS <= threshold indicates sensor not submerged
  if (tds <= LOW_WATER_TDS_THRESHOLD) {
    Serial.print(F("Low water detected! TDS: "));
    Serial.println(tds);
    strcpy(status, "Refilling");
    digitalWrite(WATER_REFILL_PUMP, HIGH);
    refillActive = true;
    refillStartTime = now;
  }
}

void checkNutrients(unsigned long now) {
  // Skip nutrient check if water refill is active
  if (refillActive) return;
  
  if (mixState != MIX_NONE) {
    handleMixing(now);
    return;
  }
  
  if (now - lastDose < MIX_WAIT) return;
  
  if (tds < (TDS_MIN + TDS_TOLERANCE)) {
    Serial.print(F("TDS low: "));
    Serial.println(tds);
    mixState = DOSE_A;
    mixOverride = true;
    mixStart = now;
  }
}

void handleMixing(unsigned long now) {
  unsigned long elapsed = now - mixStart;
  
  switch (mixState) {
    case DOSE_A:
      strcpy(status, "Dosing A");
      Serial.println(F("Dosing A"));
      digitalWrite(PUMP_NUTRIENT_A, HIGH);
      delay(DOSE_TIME);
      digitalWrite(PUMP_NUTRIENT_A, LOW);
      mixState = MIX_A;
      mixStart = now;
      break;
      
    case MIX_A:
      strcpy(status, "Mixing A");
      if (elapsed >= MIX_A_TIME) {
        mixState = DOSE_B;
        mixStart = now;
      }
      break;
      
    case DOSE_B:
      strcpy(status, "Dosing B");
      Serial.println(F("Dosing B"));
      delay(500);
      digitalWrite(PUMP_NUTRIENT_B, HIGH);
      delay(DOSE_TIME);
      digitalWrite(PUMP_NUTRIENT_B, LOW);
      mixState = MIX_B;
      mixStart = now;
      break;
      
    case MIX_B:
      strcpy(status, "Mixing B");
      if (elapsed >= MIX_B_TIME) {
        mixState = MIX_NONE;
        mixOverride = false;
        lastDose = now;
        strcpy(status, "Running");
        pumpOffTime = now;
        digitalWrite(WATER_PUMP_PIN, LOW);
        pumpOn = false;
        Serial.println(F("Dose complete"));
      }
      break;
      
    default:
      mixState = MIX_NONE;
      mixOverride = false;
      break;
  }
}

void updateDisplay() {
  updateOLED();
  updateLCD();
}

void updateOLED() {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SH110X_WHITE);
  
  // Line 0: pH + warning (Y=0)
  oled.setCursor(0, 0);
  oled.print(F("pH: "));
  oled.print(pH, 2);
  oled.print(F(" "));
  if (pH < PH_MIN)      oled.print(F("LOW! ADD UP"));
  else if (pH > PH_MAX) oled.print(F("HIGH! ADD DN"));
  else                  oled.print(F("OK"));
  
  // Line 1: TDS (Y=13)
  oled.setCursor(0, 13);
  oled.print(F("TDS: "));
  oled.print((int)tds);
  oled.print(F(" ppm "));
  if (tds < TDS_MIN)      oled.print(F("LOW"));
  else if (tds > TDS_MAX) oled.print(F("HIGH"));
  else                    oled.print(F("OK"));
  
  // Line 2: Temperature (Y=26)
  oled.setCursor(0, 26);
  oled.print(F("Temp: "));
  oled.print(temp, 1);
  oled.print(F("C "));
  if (temp < TEMP_MIN)         oled.print(F("COLD"));
  else if (temp > TEMP_WARNING) oled.print(F("HOT!"));
  else if (temp > TEMP_MAX)    oled.print(F("WARM"));
  else                         oled.print(F("OK"));
  
  // Line 3: Pump status (Y=39)
  oled.setCursor(0, 39);
  oled.print(F("Pump: "));
  oled.print(pumpOn ? F("ON") : F("OFF"));
  
  // Line 4: System status (Y=52)
  oled.setCursor(0, 52);
  oled.print(F("Status: "));
  oled.print(status);
  
  oled.display();
}

void updateLCD() {
  // Line 0: pH + warning
  lcd.setCursor(0, 0);
  lcd.print(F("pH:"));
  lcd.print(pH, 2);
  lcd.print(F(" "));
  if (pH < PH_MIN)      lcd.print(F("LOW! ADD UP "));
  else if (pH > PH_MAX) lcd.print(F("HIGH!ADD DWN"));
  else                  lcd.print(F("OK          "));
  
  // Line 1: TDS
  lcd.setCursor(0, 1);
  lcd.print(F("TDS:"));
  // Pad TDS value for consistent display
  if (tds < 10)        lcd.print(F("   "));
  else if (tds < 100)  lcd.print(F("  "));
  else if (tds < 1000) lcd.print(F(" "));
  lcd.print((int)tds);
  lcd.print(F("ppm "));
  if (tds < TDS_MIN)      lcd.print(F("LOW "));
  else if (tds > TDS_MAX) lcd.print(F("HIGH"));
  else                    lcd.print(F("OK  "));
  
  // Line 2: Temperature
  lcd.setCursor(0, 2);
  lcd.print(F("Temp:"));
  if (temp < 10.0) lcd.print(F(" "));
  lcd.print(temp, 1);
  lcd.print(F("C "));
  if (temp < TEMP_MIN)         lcd.print(F("COLD    "));
  else if (temp > TEMP_WARNING) lcd.print(F("HOT!    "));
  else if (temp > TEMP_MAX)    lcd.print(F("WARM    "));
  else                         lcd.print(F("OK      "));
  
  // Line 3: Pump status + System status
  lcd.setCursor(0, 3);
  lcd.print(F("Pump:"));
  lcd.print(pumpOn ? F("ON ") : F("OFF"));
  lcd.print(F(" "));
  lcd.print(status);
  // Pad remaining characters
  for (int i = strlen(status); i < 8; i++) lcd.print(F(" "));
}

void printSerial() {
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint < 10000) return;
  lastPrint = millis();
  
  Serial.println(F("=== Status ==="));
  Serial.print(F("pH:")); Serial.print(pH, 2);
  if (pH < PH_MIN) Serial.println(F(" LOW-AddUP"));
  else if (pH > PH_MAX) Serial.println(F(" HIGH-AddDOWN"));
  else Serial.println(F(" OK"));
  
  Serial.print(F("TDS:")); Serial.print((int)tds); Serial.println(F("ppm"));
  Serial.print(F("Temp:")); Serial.print(temp, 1); Serial.println(F("C"));
  Serial.print(F("Pump:")); Serial.println(pumpOn ? F("ON") : F("OFF"));
  Serial.println(F("=============="));
}
