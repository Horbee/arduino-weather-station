#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
// #include <LowPower.h>
#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <avr/sleep.h>

#include "DisplayUtils.h"

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 

#define BME280_ADDRESS 0x76 

#define BTN_PIN 2
#define ESP_RX_PIN 5
#define ESP_TX_PIN 6

Adafruit_BME280 bme; 
DisplayUtils displayUtils(SCREEN_WIDTH, SCREEN_HEIGHT); 
SoftwareSerial espSerial(ESP_RX_PIN, ESP_TX_PIN); // RX, TX

const int watchdogInterval = 8; // WDT interval in seconds (max 8 seconds)
const int wakeupIntervals = (16 / watchdogInterval); // 600 seconds = 10 minutes
volatile int wakeupCounter = 0;
volatile bool isScreenOn = false, btnPressed = false, sendDataFlag = false, wasInSleepMode = false;

void setup() {
  Serial.begin(9600); // Serial monitor for debugging
  espSerial.begin(9600); // Communication with ESP8266


  unsigned status = bme.begin(BME280_ADDRESS, &Wire);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
    while (1) delay(10);
  }
  displayUtils.initDisplay();

  pinMode(BTN_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BTN_PIN), callback, CHANGE);

  // Setup the watchdog timer
  setupWatchdog();
}

void loop() {
  int btnState = digitalRead(BTN_PIN);
  if (btnState == 0) btnPressed = false;

  if (sendDataFlag) {
    sendDataFlag = false;
    String temp = String(bme.readTemperature());
    String pressure = String(bme.readPressure() / 100.0F);
    String humidity = String(bme.readHumidity());

    sendMeasurementsToEsp(temp, pressure, humidity);

    if (wasInSleepMode) {
      enterSleepMode(); // Go back to sleep if it was in sleep mode
    }
  }

  if (isScreenOn) {
    wasInSleepMode = false; // Reset the flag since Arduino is now active
    Serial.println("Screen is on...");
    
    String temp = String(bme.readTemperature());
    String pressure = String(bme.readPressure() / 100.0F);
    String humidity = String(bme.readHumidity());

    // Print values to the screen 
    printValues(temp, pressure, humidity);

    delay(1000);
  } else {
    enterSleepMode();
  }

  // Listen to the espSerial messages and print them to the console
  if (espSerial.available()) {
    String espMessage = espSerial.readStringUntil('\n');
    Serial.println("Message from ESP8266: " + espMessage);
  }

}

void sendMeasurementsToEsp(String temp, String pressure, String humidity) {
  Serial.println("Sending data...");
  String sensorData = temp + "," + humidity + "," + pressure;
  espSerial.println(sensorData);
}

void printValues(String temp, String pressure, String humidity) {
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

void setupWatchdog() {
  cli();  // Disable interrupts
  wdt_reset(); // Reset the watchdog timer

  // Set up WDT to interrupt mode every 8 seconds
  WDTCSR = (1 << WDCE) | (1 << WDE);  // Enable WDT change
  WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);  // Set WDT timeout to 8 seconds

  sei();  // Enable interrupts
}

ISR(WDT_vect) {
  // Watchdog timer interrupt service routine
  wakeupCounter++;
  if (wakeupCounter >= wakeupIntervals) {
    wakeupCounter = 0;
    sendDataFlag = true;
  }
}

void enterSleepMode() {
  wasInSleepMode = true; // Set the flag to indicate Arduino is going to sleep
  Serial.println("Going to sleep...");
  displayUtils.clearDisplay();
  displayUtils.updateDisplay();
  delay(50); // Debounce delay

  // LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode to power down
  sleep_enable();                       // Enable sleep mode
  sleep_mode();                         // Put the Arduino to sleep

  // Arduino will continue from here after waking up
  sleep_disable(); // Disable sleep mode after waking up
}