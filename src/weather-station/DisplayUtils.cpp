#include "DisplayUtils.h"

#define SCREEN_ADDRESS 0x3C

// Constructor
DisplayUtils::DisplayUtils(uint8_t w, uint8_t h, TwoWire *twi, int8_t rst_pin) 
  : display(w, h, twi, rst_pin) {}

// Initialize the display
void DisplayUtils::initDisplay() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();
}

// Method to draw text with default size
void DisplayUtils::drawText(const String &text, int x, int y) {
  drawText(text, x, y, 1);
}

// Method to draw text with specified size
void DisplayUtils::drawText(const String &text, int x, int y, int textSize) {
  display.setTextSize(textSize);             
  display.setTextColor(WHITE);        
  display.setCursor(x, y);             
  display.println(text);
}

// Method to clear the display
void DisplayUtils::clearDisplay() {
  display.clearDisplay();
}

// Method to update the display
void DisplayUtils::updateDisplay() {
  display.display();
}