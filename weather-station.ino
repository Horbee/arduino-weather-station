#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define BME280_ADDRESS 0x76 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_BME280 bme; 

void setup() {
  Serial.begin(9600);

  unsigned status = bme.begin(BME280_ADDRESS, &Wire);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
    while (1) delay(10);
  }

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds
}

void loop() {
  printValues();
  delay(1000);
}

void printValues() {
  String temp = String(bme.readTemperature());
  String pressure = String(bme.readPressure() / 100.0F);
  String humidity = String(bme.readHumidity());

  display.clearDisplay();

  drawText("Temp.", 0, 0);
  drawText(temp + " C", 60, 0);

  drawText("Pressure", 0, 10);
  drawText(pressure + " hPa", 60, 10);

  drawText("Humidity", 0, 20);
  drawText(humidity + " %", 60, 20);

  display.display();

  Serial.println("T: " + temp + " °C, P: " + pressure + " hPa, Hum" + humidity + " %");
}

void drawText(String text, int x, int y) {
  drawText(text, x, y, 1);
}

void drawText(String text, int x, int y, int textSize) {
  display.setTextSize(textSize);             
  display.setTextColor(WHITE);        
  display.setCursor(x, y);             
  display.println(text);
}
