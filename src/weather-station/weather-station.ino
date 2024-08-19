#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "DisplayUtils.h"

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 

#define BME280_ADDRESS 0x76 

Adafruit_BME280 bme; 
DisplayUtils displayUtils(SCREEN_WIDTH, SCREEN_HEIGHT); 

void setup() {
  Serial.begin(9600);

  unsigned status = bme.begin(BME280_ADDRESS, &Wire);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
    while (1) delay(10);
  }
  displayUtils.initDisplay();
}

void loop() {
  printValues();
  delay(1000);
}

void printValues() {
  String temp = String(bme.readTemperature());
  String pressure = String(bme.readPressure() / 100.0F);
  String humidity = String(bme.readHumidity());

  displayUtils.clearDisplay();

  displayUtils.drawText("Temp.", 0, 0);
  displayUtils.drawText(temp + " C", 60, 0);

  displayUtils.drawText("Pressure", 0, 10);
  displayUtils.drawText(pressure + " hPa", 60, 10);

  displayUtils.drawText("Humidity", 0, 20);
  displayUtils.drawText(humidity + " %", 60, 20);

  displayUtils.updateDisplay();

  Serial.println("T: " + temp + " °C, P: " + pressure + " hPa, Hum " + humidity + " %");
}

