#include "global.h"

#include <EEPROM.h>

const uint8_t UPDATES_PER_SECOND = 100;
CRGB leds[NUM_LEDS];

//const int INDICATOR_R = 2;
//const int INDICATOR_G = 3;

Mode mode;

bool editMode = false;
bool autoMode = false;
bool powerOn = true;
bool usePalette = false;
bool paused = false;

uint32_t colour = CRGB::White;

const uint8_t MAX_BRIGHTNESS = 255;
const uint8_t MIN_BRIGHTNESS = 31;
const uint8_t INC_BRIGHTNESS =  32;
uint8_t brightness = MAX_BRIGHTNESS;

const uint8_t MAX_SPEED = 24;
const uint8_t MIN_SPEED = 6;
uint8_t speed = MAX_SPEED / 2;

uint8_t mappedPalette[5] = {0, 5, 10, 1, 2};

RTC_DS1307 RTC;

AutoHandler autoHandler;

// data storage stuff
const uint16_t ADDRESS_CHECKSUM = 0; 
const uint16_t ADDRESS_BRIGHTNESS = 4;
const uint16_t ADDRESS_SPEED  = 5;
const uint16_t ADDRESS_PALETTE = 6;
const uint16_t ADDRESS_FLAGS = 11;

const uint8_t BIT_PAUSED = 0x01;
const uint8_t BIT_AUTO = 0x02;

const uint32_t WEEK = 7 * 86400UL;

uint32_t EEPROM_crc()
{
  const uint32_t table[] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  uint32_t crc = ~0L;

  for (size_t i = 0; i < EEPROM.length(); ++i)
  {
    crc = table[(crc ^ EEPROM[i]) & 0x0f] ^ (crc >> 4);
    crc = table[(crc ^ (EEPROM[i] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }

  return crc;
}


const char SUNDAY[] PROGMEM = "Sun";
const char MONDAY[] PROGMEM = "Mon";
const char TUESDAY[] PROGMEM = "Tue";
const char WEDNESDAY[] PROGMEM = "Wed";
const char THURSDAY[] PROGMEM = "Thur";
const char FRIDAY[] PROGMEM = "Fri";
const char SATURDAY[] PROGMEM = "Sat";
const char* const DAY_TABLE[] PROGMEM = {
  SUNDAY,
  MONDAY,
  TUESDAY,
  WEDNESDAY,
  THURSDAY,
  FRIDAY,
  SATURDAY
};

const char* dayName(const uint8_t idx)
{
  static char buff[5];
  strcpy_P(buff, (char*)pgm_read_word(&(DAY_TABLE[idx])));
  return buff;
}
