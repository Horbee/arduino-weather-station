#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LowPower.h>

#include "DisplayUtils.h"

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 

#define BME280_ADDRESS 0x76 

#define BTN_PIN 2

volatile bool isScreenOn = false, btnPressed = false;

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

  pinMode(BTN_PIN, INPUT);
  attachInterrupt(0, callback, CHANGE);
}

void loop() {
  int btnState = digitalRead(BTN_PIN);
  if (btnState == 0) btnPressed = false;

  if (isScreenOn) {
    Serial.println("Screen is on...");
    printValues();
    delay(1000);
  } else {
    Serial.println("Going to sleep...");
    displayUtils.clearDisplay();
    displayUtils.updateDisplay();
    // Debounce delay
    delay(50);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
  }

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

void callback() {
  if (btnPressed) return;

  btnPressed = true;
  Serial.println("Callback called");
  isScreenOn = !isScreenOn;
  Serial.print("isScreenOn: ");
  Serial.println(isScreenOn);
}