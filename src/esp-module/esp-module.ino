#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "";
const char* password = "";
const char* host = "arduino-nextjs-test.vercel.app";
const int httpsPort = 443; // HTTPS port
const unsigned long wifiTimeout = 20000; // 20 seconds timeout

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    // Read the sensor data from the Arduino
    String sensorData = Serial.readStringUntil('\n');

    // Parse the sensor data
    float temperature = sensorData.substring(0, sensorData.indexOf(',')).toFloat();
    float humidity = sensorData.substring(sensorData.indexOf(',') + 1, sensorData.lastIndexOf(',')).toFloat();
    float pressure = sensorData.substring(sensorData.lastIndexOf(',') + 1).toFloat();

    // Submit the measurements to the server
    submitMeasurements(temperature, humidity, pressure);
  }
}

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    // Check if the timeout has been reached
    if (millis() - startTime >= wifiTimeout) {
      Serial.println("\nWi-Fi connection timed out");
      return; // Exit the function if timeout is reached
    }
  }

  Serial.println("\nConnected!");
}

void submitMeasurements(float temperature, float humidity, float pressure) {
  // Check if still connected to Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  WiFiClientSecure client;
  client.setInsecure(); // Use this if you don't want to verify the SSL certificate

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection to server failed");
    return;
  }

  String url = "/api/measurements";
  String postData = "{\"temperature\":" + String(temperature, 2) +
                    ",\"humidity\":" + String(humidity, 2) +
                    ",\"pressure\":" + String(pressure, 2) + "}";

  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Content-Type: application/json\r\n" +
               "Content-Length: " + postData.length() + "\r\n" +
               "Connection: close\r\n\r\n" +
               postData + "\r\n");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }

  while (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
  }

  client.stop();
  Serial.println("Connection to server closed");
}
