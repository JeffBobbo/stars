#ifndef GLOBAL_H_INCLUDE
#define GLOBAL_H_INCLUDE

#include <FastLED.h>
#include <stdint.h>

#include "RTClib.h"

extern const int UPDATES_PER_SECOND;
#define LED_PIN 6
#define NUM_LEDS 16
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
extern CRGB leds[NUM_LEDS];

extern const int INDICATOR_R;
extern const int INDICATOR_G;
//extern const int INDICATOR_B;

enum Mode {
  SOLID = 0,
  FADE,
  JUMP,
  TWINKLE
};
extern Mode mode;

extern bool editMode;
extern bool autoMode;
extern bool powerOn;

extern uint32_t colour;
extern bool usePalette;

extern const uint8_t MAX_BRIGHTNESS;
extern const uint8_t MIN_BRIGHTNESS;
extern const uint8_t INC_BRIGHTNESS;
extern uint8_t brightness;

extern const uint8_t MAX_SPEED;
extern const uint8_t MIN_SPEED;
extern uint8_t speed;
extern bool paused;


extern uint8_t mappedPalette[5];

extern RTC_DS1307 RTC;


// data storage stuff
extern const size_t ADDRESS_CHECKSUM;
extern const size_t ADDRESS_BRIGHTNESS;
extern const size_t ADDRESS_SPEED;
extern const size_t ADDRESS_PALETTE;

extern const uint8_t BIT_PAUSED;
extern const uint8_t BIT_AUTO;

uint32_t EEPROM_crc();

#endif
