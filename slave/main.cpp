#include <Arduino.h>
#include <FastLED.h>
#include <Wire.h>

#include "palettes.h"

#include "common.h"

// floating input pin for randomSeed
const uint8_t RANDOM_PIN = 0;

#define LED_PIN 2
#define NUM_LEDS 21
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
const uint8_t UPDATES_PER_SECOND = 100;
CRGB leds[NUM_LEDS];

ControlData data = {
  0,
  CRGB::White,
  SOLID,
  false,
  false,
  false,
  255,
  6
};

uint8_t modeBrightness(const int i)
{
  switch (data.mode)
  {
    case FADE_THREE:
      return cubicwave8((millis() / 5000.0f + (3.0f * i) / NUM_LEDS) * 255);
    case FADE_SEVEN:
      return cubicwave8((millis() / 5000.0f + (7.0f * i) / NUM_LEDS) * 255);
    case JUMP_THREE:
      return (((millis() / 300) + i) % 2) * 255;
    case JUMP_SEVEN:
      return (((millis() / 700) + i) % 2) * 255;
    default:
      return 255;
  }
}

void twinklePalette(uint8_t colorIndex)
{
  static uint8_t twinkle[NUM_LEDS] = {255};

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = ColorFromPalette(getPalette(data.palette), colorIndex, twinkle[i], LINEARBLEND);
    twinkle[i] -= 16;
    colorIndex += 3;
  }

  if (random(3) == 0)
    twinkle[random(NUM_LEDS)] = 255;
}

void paletteLEDs(uint8_t colorIndex)
{
  if (data.mode == TWINKLE)
  {
    twinklePalette(colorIndex);
    return;
  }

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = ColorFromPalette(getPalette(data.palette), colorIndex, modeBrightness(i), LINEARBLEND);
    colorIndex += 3;
  }
}

void solidLEDs()
{
  if (data.mode == TWINKLE)
  {
    // fade all
    for (int i = 0; i < NUM_LEDS; ++i)
      leds[i].fadeToBlackBy(8);

    // brighten some
    if (random(2) == 0)
    {
      for (int i = 0; i < 1; ++i)
        leds[random(NUM_LEDS)] = data.colour;
    }
    return;
  }

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = data.colour;
    leds[i] %= modeBrightness(i);
  }
}

void updateLEDs()
{
  if (data.usePalette)
  {
    static uint16_t startIndex = 0;
    if (!data.paused)
      startIndex = startIndex + data.speed;

    paletteLEDs(startIndex / 10);
  }
  else
  {
    solidLEDs();
  }

  if (data.lightsOn)
    FastLED.show();
  else
    FastLED.clear();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void receiveEvent(int bytes)
{
  uint8_t* ptr = reinterpret_cast<uint8_t*>(&data);

  for (int i = 0; i < bytes; ++i)
    ptr[i] = Wire.read();

  FastLED.setBrightness(data.brightness);
}

void setup()
{
  delay(3000); // power up safety delay, apparently

  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);
  fill_solid(leds, NUM_LEDS, CRGB::White);

  randomSeed(analogRead(RANDOM_PIN));
}

void loop()
{
  updateLEDs();
}
