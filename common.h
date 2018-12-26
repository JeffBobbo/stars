#ifndef COMMON_H_INCLUDE
#define COMMON_H_INCLUDE

const uint8_t SLAVE_ADDRESS = 9;

enum Mode {
  SOLID = 0,
  FADE_THREE,
  FADE_SEVEN,
  JUMP_THREE,
  JUMP_SEVEN,
  TWINKLE
};

struct ControlData
{
  uint8_t palette;
  uint32_t colour;
  Mode mode;
  bool usePalette : 1;
  bool paused : 1;
  bool lightsOn : 1;
  uint8_t brightness;
  uint8_t speed;
};

#include <Arduino.h>
static void printData(const ControlData& data)
{
  if (data.lightsOn)
    Serial.println(F("LIGHTS ON"));
  if (data.usePalette)
  {
    Serial.print(F("Palette "));
    Serial.println(data.palette);
  }
  else
  {
    Serial.print(F("Colour "));
    Serial.println(data.colour, HEX);
  }

  Serial.print(data.brightness);
  Serial.print(" ");
  Serial.print(data.speed);

  if (data.paused)
    Serial.print(F(" PAUSED"));
  Serial.println();
}

#endif
