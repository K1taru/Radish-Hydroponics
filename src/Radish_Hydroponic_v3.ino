// Automated Tower Hydroponic System for Radish - v1.0

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>  
#include <OneWire.h>
#include <DallasTemperature.h>

// Pin Assignments
#define PH_SENSOR_PIN     A0
#define TDS_SENSOR_PIN    A1
#define TEMP_SENSOR_PIN   2
#define WATER_PUMP_PIN    7
#define PUMP_NUTRIENT_A   8
#define PUMP_NUTRIENT_B   9
#define WATER_REFILL_PUMP 10

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

// Hardware
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature tempSensor(&oneWire);

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
  if(!display.begin(OLED_ADDRESS, true)) {
    Serial.println(F("SH1106 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setContrast(255);  // Maximum brightness
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(20, 20);
  display.println(F("Hydroponic v3.0"));
  display.setCursor(20, 35);
  display.println(F("Initializing..."));
  display.display();
  
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(PUMP_NUTRIENT_A, OUTPUT);
  pinMode(PUMP_NUTRIENT_B, OUTPUT);
  pinMode(WATER_REFILL_PUMP, OUTPUT);
  
  digitalWrite(WATER_PUMP_PIN, LOW);
  digitalWrite(PUMP_NUTRIENT_A, LOW);
  digitalWrite(PUMP_NUTRIENT_B, LOW);
  digitalWrite(WATER_REFILL_PUMP, LOW);
  
  tempSensor.begin();
  delay(2000);
  readSensors();
  
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setCursor(25, 28);
  display.println(F("System Ready!"));
  display.display();
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

void readSensors() {
  // pH reading
  float v = analogRead(PH_SENSOR_PIN) * (VREF / 1024.0);
  pH = constrain(7.0 - ((v - 2.5) / 0.18) + PH_OFFSET, 0.0, 14.0);
  
  // TDS reading with temp compensation
  v = analogRead(TDS_SENSOR_PIN) * (VREF / 1024.0);
  float comp = 1.0 + 0.02 * (temp - 25.0);
  tds = max(0.0, (v / comp) * 500.0 * TDS_K);
  
  // Temperature
  tempSensor.requestTemperatures();
  float t = tempSensor.getTempCByIndex(0);
  if (t > -50.0 && t < 100.0) temp = t;
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
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  
  // Line 0: pH + warning (Y=0)
  display.setCursor(0, 0);
  display.print(F("pH: "));
  display.print(pH, 2);
  display.print(F(" "));
  if (pH < PH_MIN)      display.print(F("LOW! ADD UP"));
  else if (pH > PH_MAX) display.print(F("HIGH! ADD DN"));
  else                  display.print(F("OK"));
  
  // Line 1: TDS (Y=13)
  display.setCursor(0, 13);
  display.print(F("TDS: "));
  display.print((int)tds);
  display.print(F(" ppm "));
  if (tds < TDS_MIN)      display.print(F("LOW"));
  else if (tds > TDS_MAX) display.print(F("HIGH"));
  else                    display.print(F("OK"));
  
  // Line 2: Temperature (Y=26)
  display.setCursor(0, 26);
  display.print(F("Temp: "));
  display.print(temp, 1);
  display.print(F("C "));
  if (temp < TEMP_MIN)         display.print(F("COLD"));
  else if (temp > TEMP_WARNING) display.print(F("HOT!"));
  else if (temp > TEMP_MAX)    display.print(F("WARM"));
  else                         display.print(F("OK"));
  
  // Line 3: Pump status (Y=39)
  display.setCursor(0, 39);
  display.print(F("Pump: "));
  display.print(pumpOn ? F("ON") : F("OFF"));
  
  // Line 4: System status (Y=52)
  display.setCursor(0, 52);
  display.print(F("Status: "));
  display.print(status);
  
  display.display();
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