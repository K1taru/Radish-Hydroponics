#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD Display Configuration
#define LCD_ADDRESS 0x27    // Common addresses: 0x27 or 0x3F
#define LCD_COLS 20
#define LCD_ROWS 4
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

const int adcPin = A0;

void setup() {
  Serial.begin(9600);
  
  // Initialize LCD display
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2, 1);
  lcd.print(F("pH Sensor Test"));
  lcd.setCursor(3, 2);
  lcd.print(F("Initializing..."));
  delay(2000);
  lcd.clear();
  
  Serial.println(F("=== pH Sensor Calibration ==="));
  Serial.println(F("ADC | Voltage | pH (est)"));
  Serial.println(F("----+----------+---------"));
}

void loop() {
  int adcValue = analogRead(adcPin);
  float phVoltage = (float)adcValue * 5.0 / 1024.0;
  
  // Calculate pH estimate (2.5V = pH 7, slope ~0.18V per pH unit)
  float phEstimate = 7.0 - ((phVoltage - 2.5) / 0.18);
  
  // Serial output
  Serial.print("ADC: "); 
  Serial.print(adcValue);
  Serial.print(" | Voltage: "); 
  Serial.print(phVoltage, 3);
  Serial.print("V | pH: ");
  Serial.println(phEstimate, 2);
  
  // LCD output
  // Line 0: ADC value
  lcd.setCursor(0, 0);
  lcd.print(F("                    "));  // Clear line
  lcd.setCursor(0, 0);
  lcd.print(F("ADC: "));
  lcd.print(adcValue);
  
  // Line 1: Voltage
  lcd.setCursor(0, 1);
  lcd.print(F("                    "));  // Clear line
  lcd.setCursor(0, 1);
  lcd.print(F("Voltage: "));
  lcd.print(phVoltage, 3);
  lcd.print(F("V"));
  
  // Line 2: pH estimate
  lcd.setCursor(0, 2);
  lcd.print(F("                    "));  // Clear line
  lcd.setCursor(0, 2);
  lcd.print(F("pH (est): "));
  lcd.print(phEstimate, 2);
  
  // Line 3: Calibration guidance (target: 2.50V for pH 7.0 buffer)
  lcd.setCursor(0, 3);
  lcd.print(F("                    "));  // Clear line
  lcd.setCursor(0, 3);
  if (phVoltage < 0.2 || phVoltage > 4.8) {
    lcd.print(F("SENSOR ERROR!"));
  } else if (phVoltage >= 2.48 && phVoltage <= 2.52) {
    lcd.print(F("SUCCESSFUL!"));
  } else if (phVoltage < 2.48) {
    lcd.print(F("Adjust RV1 UP"));
  } else {
    lcd.print(F("Adjust RV1 DOWN"));
  }
  
  delay(1000);
}