#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SH1106_ESP32.h"

// ------------------- OLED CONFIG -------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106_ESP32 display(21, 22);

// ------------------- SENSOR CONFIG -------------------
#define SOIL_PIN 34  // Soil moisture analog input

// Calibration values (adjust for your sensor)
const int AIR_VALUE = 4095;   // Dry
const int WATER_VALUE = 1500; // Wet

int soilValue = 0;
int moisturePercent = 0;

// ------------------- DRAW SMILEY -------------------
void drawSmiley(String mood) {
  int x = 105;  // Smiley center X
  int y = 45;   // Smiley center Y
  int r = 15;   // Face radius

  // Face outline
  display.drawCircle(x, y, r, WHITE);
  display.drawCircle(x, y, r+1, WHITE);


  // Eyes
  display.fillCircle(x - 6, y - 5, 2, WHITE);
  display.fillCircle(x + 6, y - 5, 2, WHITE);

  // Mouth (made with lines)
  if (mood == "happy") {
    display.drawLine(x - 6, y + 5, x - 3, y + 8, WHITE);
    display.drawLine(x - 3, y + 8, x + 3, y + 8, WHITE);
    display.drawLine(x + 3, y + 8, x + 6, y + 5, WHITE);
  } 
  else if (mood == "normal") {
    display.drawLine(x - 6, y + 7, x + 6, y + 7, WHITE);
  } 
  else if (mood == "sad") {
    display.drawLine(x - 6, y + 10, x - 3, y + 7, WHITE);
    display.drawLine(x - 3, y + 7, x + 3, y + 7, WHITE);
    display.drawLine(x + 3, y + 7, x + 6, y + 10, WHITE);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(SOIL_PIN, INPUT);

  Wire.begin(21, 22);
  Wire.setClock(400000);
  display.begin(SH1106_SWITCHCAPVCC, SH1106_I2C_ADDRESS, true);
  display.clearDisplay();
  display.display();

  Serial.println("🌿 Soil Moisture Monitor with Smiley Started 🌿");
}

void loop() {
  // --- Read sensor ---
  soilValue = analogRead(SOIL_PIN);
  Serial.println(soilValue);
  moisturePercent = map(soilValue, AIR_VALUE, WATER_VALUE, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  // --- Determine mood ---
  String mood;
  if (moisturePercent < 30) {
    mood = "sad";
  } else if (moisturePercent <= 70) {
    mood = "normal";
  } else {
    mood = "happy";
  }

  // --- Serial output ---
  Serial.println("PropelX Labs");
  Serial.print("Soil Moisture: ");
  Serial.print(moisturePercent);
  Serial.println("%");

  // --- OLED Display ---
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5, 0);
  display.println("PropelX Labs");

  display.setCursor(5, 15);
  display.println("Moisture:");
  display.setCursor(5, 35);
  display.setTextSize(3);
  display.print(moisturePercent);
  display.println("%");

  // Draw smiley
  drawSmiley(mood);

  display.display();

  delay(1000);
}
