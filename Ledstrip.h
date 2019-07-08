#ifndef LEDSTRIP_H_
#define LEDSTRIP_H_

#include <stdint.h>
#include <FastLED.h>

#define NUM_LEDS 240
#define DATA_PIN 13

void setupLedstrip();
void updateBrigtness(double brigtness);
void setRedValue(int position, uint8_t red);
void setBlueValue(int position, uint8_t blue);
void setGreenValue(int position, uint8_t green);
void setRGBLedstrip(uint8_t red, uint8_t green, uint8_t blue);
void powerOffLedstrip();

void updateLedstrip();




#endif
