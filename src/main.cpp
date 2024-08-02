/*
Diode Logic Puzzle
Copyright (C) 2024  Collin Lambert, Carson Bush

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Consider a photodiode covered when it is below this threshold
#define THRESHOLD 1

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Logic functions
bool and_logic(float voltage_1, float voltage_2);
bool or_logic(float voltage_1, float voltage_2);
bool xor_logic(float voltage_1, float voltage_2);
bool nand_logic(float voltage_1, float voltage_2);
bool nor_logic(float voltage_1, float voltage_2);
bool xnor_logic(float voltage_1, float voltage_2);

#define NUM_LOGIC_FUNCTIONS 6

void debug_print_voltages(float voltagea0, float voltagea1, float voltagea2, float voltagea3);
void cond_inc_selected_logic(char* selected_logic, float selector_voltage, byte* selector_state);

// THESE ARE THE KEYS TO THE PUZZLE! ONE OF THESE SHOULD BE SELECTED TO BE CORRECT!
const String all_clue_results[] = {"Cougar", "Farnsworth", "ECEn", "Linux"};

bool (*logic_functions[])(float, float) = {and_logic, or_logic, xor_logic, nand_logic, nor_logic, xnor_logic};
String logic_function_names[] = {"AND", "OR", "XOR", "NAND", "NOR", "XNOR"};
char selected_logic_ab = 0;
char selected_logic_cd = 0;

// Simple State Machine
#define INIT_STATE 0
#define LATCH_TRANSITION_STATE 1

byte selector_ab_state = INIT_STATE;
byte selector_cd_state = INIT_STATE;
bool state_changed = true;

char last_result = 0;

void setup() {
  // put your setup code here, to run once:
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  // A4 and A5 are used for I2C communication
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int a0 = analogRead(A0);
  int a1 = analogRead(A1);
  int a2 = analogRead(A2);
  int a3 = analogRead(A3);
  int a6 = analogRead(A6);
  int a7 = analogRead(A7);
  float voltagea0 = a0 * (5.0 / 1023.0);
  float voltagea1 = a1 * (5.0 / 1023.0);
  float voltagea2 = a2 * (5.0 / 1023.0);
  float voltagea3 = a3 * (5.0 / 1023.0);
  float voltagea6 = a6 * (5.0 / 1023.0);
  float voltagea7 = a7 * (5.0 / 1023.0);

  // debug_print_voltages(voltagea0, voltagea1, voltagea2, voltagea3);

  // Handle incrementing logic function
  cond_inc_selected_logic(&selected_logic_ab, voltagea6, &selector_ab_state);
  cond_inc_selected_logic(&selected_logic_cd, voltagea7, &selector_cd_state);

  if (state_changed) {
    lcd.setCursor(0, 0);
    lcd.print(logic_function_names[selected_logic_ab]);
    lcd.setCursor(0, 1);
    lcd.print(logic_function_names[selected_logic_cd]);
    state_changed = false;
  }

  char output_logic_ab = logic_functions[selected_logic_ab](voltagea0, voltagea1);
  char output_logic_cd = logic_functions[selected_logic_cd](voltagea2, voltagea3);
  char full_result = (output_logic_ab << 1) | output_logic_cd;
  if (last_result != full_result) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(logic_function_names[selected_logic_ab]);
    lcd.setCursor(0, 1);
    lcd.print(logic_function_names[selected_logic_cd]);
    last_result = full_result;
    lcd.setCursor(5, 0);
    lcd.print(all_clue_results[full_result]);
  }

  delay(100);
}

void debug_print_voltages(float voltagea0, float voltagea1, float voltagea2, float voltagea3) {
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

  // Print voltage (up to 2 decimal place) on the Serial Monitor
  Serial.print("Voltages: ");
  Serial.print(voltagea0);
  Serial.print(" ");
  Serial.print(voltagea1);
  Serial.print(" ");
  Serial.print(voltagea2);
  Serial.print(" ");
  Serial.println(voltagea3);

  return;
}

bool and_logic(float voltage_1, float voltage_2) {
  return ((voltage_1 < THRESHOLD) && (voltage_2 < THRESHOLD));
}

bool or_logic(float voltage_1, float voltage_2) {
  return ((voltage_1 < THRESHOLD) || (voltage_2 < THRESHOLD));
}

bool xor_logic(float voltage_1, float voltage_2) {
  return ((voltage_1 < THRESHOLD) ^ (voltage_2 < THRESHOLD));
}

bool nand_logic(float voltage_1, float voltage_2) {
  return !((voltage_1 < THRESHOLD) && (voltage_2 < THRESHOLD));
}

bool nor_logic(float voltage_1, float voltage_2) {
  return !((voltage_1 < THRESHOLD) || (voltage_2 < THRESHOLD));
}

bool xnor_logic(float voltage_1, float voltage_2) {
  return !((voltage_1 < THRESHOLD) ^ (voltage_2 < THRESHOLD));
}

void cond_inc_selected_logic(char* selected_logic, float selector_voltage, byte* selector_state) {
  if (*selector_state == INIT_STATE && selector_voltage < THRESHOLD) {
    *selector_state = LATCH_TRANSITION_STATE;
    state_changed = true;
  }
  if (*selector_state == LATCH_TRANSITION_STATE && selector_voltage > THRESHOLD) {
    (*selected_logic) = ((*selected_logic) + 1) % NUM_LOGIC_FUNCTIONS;
    *selector_state = INIT_STATE;
    state_changed = true;
  }
  return;
}
