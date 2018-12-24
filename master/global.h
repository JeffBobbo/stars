#ifndef GLOBAL_H_INCLUDE
#define GLOBAL_H_INCLUDE

#include <FastLED.h>
#include <stdint.h>

#include "RTClib.h"
#include "auto.h"

extern const uint8_t UPDATES_PER_SECOND;
#define LED_PIN 6
#define NUM_LEDS 15
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
extern CRGB leds[NUM_LEDS];

enum Mode {
  SOLID = 0,
  FADE_THREE,
  FADE_SEVEN,
  JUMP_THREE,
  JUMP_SEVEN,
  TWINKLE
};
extern Mode mode;

extern bool editMode;
extern bool autoMode;
extern bool powerOn;
extern bool usePalette;
extern bool paused;

extern uint32_t colour;

extern const uint8_t MAX_BRIGHTNESS;
extern const uint8_t MIN_BRIGHTNESS;
extern const uint8_t INC_BRIGHTNESS;
extern uint8_t brightness;

extern const uint8_t MAX_SPEED;
extern const uint8_t MIN_SPEED;
extern uint8_t speed;


extern uint8_t mappedPalette[5];

extern RTC_DS1307 RTC;

extern AutoHandler autoHandler;

// data storage stuff
extern const uint16_t ADDRESS_CHECKSUM;
extern const uint16_t ADDRESS_BRIGHTNESS;
extern const uint16_t ADDRESS_SPEED;
extern const uint16_t ADDRESS_PALETTE;

extern const uint8_t BIT_PAUSED;
extern const uint8_t BIT_AUTO;

extern const uint32_t WEEK;

uint32_t EEPROM_crc();

const char* dayName(const uint8_t idx);

#endif
