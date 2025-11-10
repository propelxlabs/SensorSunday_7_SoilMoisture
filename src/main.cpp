#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SH1106_ESP32.h"

// ------------------- OLED CONFIG -------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106_ESP32 display(21, 22);

// ------------------- SENSOR & PUMP CONFIG -------------------
#define SOIL_PIN 34         // Soil moisture analog input
#define PUMP_PIN 25         // Motor/pump output pin

#define SOIL_THRESHOLD 2000 // Dry threshold (0-4095 for ESP32 ADC)

int soilValue = 0;

void setup() {
  Serial.begin(115200);

  pinMode(SOIL_PIN, INPUT);
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW); // Pump off initially

  Wire.begin(21, 22);
  Wire.setClock(400000);
  display.begin(SH1106_SWITCHCAPVCC, SH1106_I2C_ADDRESS, true);
  display.clearDisplay();
  display.display();

  Serial.println("Soil Moisture Monitor with Pump Started");
}

void loop() {
  // --- Read soil sensor ---
  soilValue = analogRead(SOIL_PIN); // 0 (wet) to 4095 (dry)

  // --- Pump control ---
  if (soilValue > SOIL_THRESHOLD) {
    digitalWrite(PUMP_PIN, HIGH); // Soil is dry → turn on pump
  } else {
    digitalWrite(PUMP_PIN, LOW);  // Soil is wet → turn off pump
  }

  // --- OLED display ---
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5, 0);
  display.println("Soil Moisture Monitor");

  // Soil value
  display.setCursor(5, 15);
  display.print("Value: ");
  display.println(soilValue);

  // Moisture bar
  int barWidth = map(soilValue, 0, 4095, SCREEN_WIDTH-10, 0); // inverted: low = wet
  display.drawRect(5, 35, SCREEN_WIDTH-10, 15, WHITE);
  display.fillRect(5, 35, barWidth, 15, WHITE);

  // Pump status
  display.setCursor(5, 55);
  if (digitalRead(PUMP_PIN)) {
    display.println("Pump: ON");
  } else {
    display.println("Pump: OFF");
  }

  display.display();

  delay(500); // Update twice per second
}
