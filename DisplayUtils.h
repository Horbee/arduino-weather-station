#ifndef DISPLAYUTILS_H
#define DISPLAYUTILS_H

#include <Adafruit_SSD1306.h>

class DisplayUtils {
  private:
    Adafruit_SSD1306 display;

  public:
    // Constructor
    DisplayUtils(uint8_t w, uint8_t h, TwoWire *twi = &Wire, int8_t rst_pin = -1);

    // Initialize the display
    void initDisplay();

    // Methods to draw text
    void drawText(const String &text, int x, int y);
    void drawText(const String &text, int x, int y, int textSize);

    // Method to clear the display
    void clearDisplay();

    // Method to update the display
    void updateDisplay();
};

#endif // DISPLAYUTILS_H
