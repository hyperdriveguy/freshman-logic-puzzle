#include <Arduino.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define THRESHOLD 2

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// put function declarations here:


void setup() {
  // put your setup code here, to run once:
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int a0 = analogRead(A0);
  int a1 = analogRead(A1);
  int a2 = analogRead(A2);
  int a3 = analogRead(A3);
  float voltagea0 = a0 * (5.0 / 1023.0);
  float voltagea1 = a1 * (5.0 / 1023.0);
  float voltagea2 = a2 * (5.0 / 1023.0);
  float voltagea3 = a3 * (5.0 / 1023.0);

  // Print voltage (up to 2 decimal place) on each corner of the LCD
  lcd.setCursor(0, 0);
  lcd.print(voltagea0, 2);
  lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.print(voltagea1, 2);
  lcd.print(" ");
  lcd.setCursor(8, 0);
  lcd.print(voltagea2, 2);
  lcd.print(" ");
  lcd.setCursor(8, 1);
  lcd.print(voltagea3, 2);
  lcd.print(" ");

  Serial.print(voltagea0);
  Serial.print(" ");
  Serial.print(voltagea1);
  Serial.print(" ");
  Serial.print(voltagea2);
  Serial.print(" ");
  Serial.println(voltagea3);

  delay(500);
}

