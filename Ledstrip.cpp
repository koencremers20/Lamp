#include "Ledstrip.h"

uint8_t redValue, blueValue, greenValue = 0;
CRGB leds[NUM_LEDS];

void setupLedstrip() {
  LEDS.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  LEDS.setBrightness(50);
  Serial.println("Ledstrip initialized");
  powerOffLedstrip();
  updateLedstrip();
}
void updateBrigtness(double brightness) {
  LEDS.setBrightness(brightness);
}
void setRGBLedstrip(uint8_t red, uint8_t green, uint8_t blue) {
  for (int j = 0; j < NUM_LEDS; j++) {
    leds[j].setRGB(red, green, blue);
  }

  if(redValue != red || blueValue != blue || greenValue != green) {
    redValue = red;
    blueValue = blue;
    greenValue = green;
    updateLedstrip();
  }
}

void powerOffLedstrip() {
  for (int j = 0; j < NUM_LEDS; j++) {
     leds[j].nscale8( 250);
  }
  updateLedstrip();
}

void setRedValue(int position, uint8_t red) {
  redValue = red;
  leds[position].setRGB(red, 0, 0);
}

void setBlueValue(int position, uint8_t blue) {
  blueValue = blue;
  leds[position].setRGB(0, 0, blue);
}

void setGreenValue(int position, uint8_t green) {
  greenValue = green;
  leds[position].setRGB(0, green, 0);
}

void updateLedstrip() {
  FastLED.show();
}
