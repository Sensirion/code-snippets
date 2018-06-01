/*
  Mirror Heater

  Condensation on the mirror is avoided by attaching a foil heater
  to the mirror and turning it on if a fast increase in relative
  humidity is detected or if the relative humidity is above a given
  threshold.

  Dependencies:
    * arduino-sht library

  The circuit:
    * Heater pin transistor connected to pin 13
    * SHT3x connected to the I2C data and clock pins

  Released into the public domain.

  https://developer.sensirion.com/labs/anti-fogging-for-bathroom-mirrors/

*/

#include <Wire.h>
#include "SHTSensor.h"

// threshold to detect showering event
#define THRESHOLD_DELTA_REL_HUMIDITY 15.0f
#define THRESHOLD_REL_HUMIDITY       85.0f

// default showering duration
#define DEFAULT_SHOWERING_DURATION_MS 150000  // 2.5min

// pin with heater transistor
#define HEATER_PIN 13

SHTSensor sht(SHTSensor::SHT3X);
unsigned long initTimeMs;
float initRelativeHumidity;

void setup() {
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM);  // only supported by SHT3x

  Wire.begin();
  Serial.begin(9600);
  delay(1000);  // let serial console settle

  pinMode(HEATER_PIN, OUTPUT);

  initTimeMs = millis();
  sht.readSample();
  initRelativeHumidity = sht.getHumidity();
}

void loop() {
  sht.readSample();
  float relativeHumidity = sht.getHumidity();
  float temperature = sht.getTemperature();
  unsigned long currentTimeMs = millis();

  // print measured values
  Serial.print("RH: ");
  Serial.print(relativeHumidity, 2);
  Serial.print("T:  ");
  Serial.print(temperature, 2);
  Serial.print("  time:  ");
  Serial.print(currentTimeMs / 1000.0f, 0);

  // detect shower event
  if (currentTimeMs < initTimeMs + DEFAULT_SHOWERING_DURATION_MS ||
      relativeHumidity > THRESHOLD_REL_HUMIDITY ||
      relativeHumidity > initRelativeHumidity + THRESHOLD_DELTA_REL_HUMIDITY) {
    digitalWrite(HEATER_PIN, 1);
    Serial.print(" Heater On ");
  } else {
    digitalWrite(HEATER_PIN, 0);
    Serial.print(" Heater Off ");
  }

  Serial.print("\n");
  delay(1000);
}