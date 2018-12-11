#include <Arduino.h>

#include <FastLED.h>
#include <Wire.h>
#include <EEPROM.h>

#include "RTClib.h"

//#include "../common.h"
const uint8_t SLAVE_ADDRESS = 9;
const size_t DATA_SIZE = sizeof(uint32_t);

#include "global.h"
#include "controller_mapping.h"
#include "palettes.h"
#include "edit.h"

uint32_t buttonColour(const uint32_t button)
{
  switch (button)
  {
    case BUTTON_RED:
      return CRGB::Red;
    case BUTTON_GREEN:
      return CRGB::Lime;
    case BUTTON_BLUE:
      return CRGB::Blue;
    case BUTTON_WHITE:
      return CRGB::White;

    case BUTTON_DARK_ORANGE:
      return CRGB::OrangeRed;
    case BUTTON_LIME:
      return CRGB::ForestGreen;
    case BUTTON_LIGHT_BLUE:
      return CRGB::DodgerBlue;

    case BUTTON_ORANGE:
      return CRGB::DarkOrange;
    case BUTTON_TURQUIOSE:
      return CRGB::DarkTurquoise;
    case BUTTON_MAUVE:
      return CRGB::Sienna;

    case BUTTON_LIGHT_ORANGE:
      return CRGB::Orange;
    case BUTTON_TEAL:
      return CRGB::Teal;
    case BUTTON_PURPLE:
      return CRGB::Purple;

    case BUTTON_YELLOW:
      return CRGB::Yellow;
    case BUTTON_DARK_GREEN:
      return CRGB::DarkGreen;
    case BUTTON_PINK:
      return CRGB::DeepPink;
    default:
      return CRGB::Black;
  }
}

void buttonMode(const uint32_t button)
{
  switch (button)
  {
    case BUTTON_FLASH:
      mode = TWINKLE;
    break;
    case BUTTON_JUMP_THREE:
    case BUTTON_JUMP_SEVEN:
      mode = JUMP;
    break;
    case BUTTON_FADE_THREE:
    case BUTTON_FADE_SEVEN:
      mode = FADE;
    break;
  }
}

void buttonPalette(const uint32_t button)
{
  switch (button)
  {
    case BUTTON_DIY_ONE:
    case BUTTON_DIY_TWO:
    case BUTTON_DIY_THREE:
    case BUTTON_DIY_FOUR:
    case BUTTON_DIY_FIVE:
      currentPalette = availablePalettes[mappedPalette[diy(button)]];
    break;
    default:
      usePalette = false;
      fill_solid(leds, NUM_LEDS, CRGB::Black);
    break;
  }
}

void handleDIY(const uint32_t button)
{
  usePalette = true;
  buttonPalette(button);
}

void handleIR(const uint32_t value)
{
  // filter out anything unknown
  if ((value & 0xFFFF0000) != 0x00FF0000)
    return;

  if (editMode)
  {
    editIR(value);
    return;
  }

  switch (value)
  {
    case BUTTON_BRIGHTNESS_INC:
      if (brightness < MAX_BRIGHTNESS)
        brightness += INC_BRIGHTNESS;
      FastLED.setBrightness(brightness);
    break;
    case BUTTON_BRIGHTNESS_DEC:
      if (brightness > MIN_BRIGHTNESS)
        brightness -= INC_BRIGHTNESS;
      FastLED.setBrightness(brightness);
    break;
    case BUTTON_PLAYPAUSE:
      paused = !paused;
    break;
    case BUTTON_POWER:
      powerOn = !powerOn;
      if (powerOn && !usePalette)
        fill_solid(leds, NUM_LEDS, colour);
    break;
    case BUTTON_RED:
    case BUTTON_GREEN:
    case BUTTON_BLUE:
    case BUTTON_WHITE:
    case BUTTON_DARK_ORANGE:
    case BUTTON_LIME:
    case BUTTON_LIGHT_BLUE:
    case BUTTON_ORANGE:
    case BUTTON_TURQUIOSE:
    case BUTTON_MAUVE:
    case BUTTON_LIGHT_ORANGE:
    case BUTTON_TEAL:
    case BUTTON_PURPLE:
    case BUTTON_YELLOW:
    case BUTTON_DARK_GREEN:
    case BUTTON_PINK:
      usePalette = false;
      fill_solid(leds, NUM_LEDS, colour = buttonColour(value));
    break;
    case BUTTON_DIY_ONE:
    case BUTTON_DIY_TWO:
    case BUTTON_DIY_THREE:
    case BUTTON_DIY_FOUR:
    case BUTTON_DIY_FIVE:
      handleDIY(value);
    break;
    case BUTTON_DIY_SIX:
      editMode = !editMode;
    break;
    case BUTTON_QUICK:
      if (speed < MAX_SPEED)
        ++speed;
    break;
    case BUTTON_SLOW:
      if (speed > MIN_SPEED)
        --speed;
    break;
    case BUTTON_AUTO:
      autoMode = !autoMode;
    break;
    case BUTTON_FLASH:
    case BUTTON_JUMP_THREE:
    case BUTTON_JUMP_SEVEN:
    case BUTTON_FADE_THREE:
    case BUTTON_FADE_SEVEN:
      buttonMode(value);
    break;
    case BUTTON_HOLD:
    break;
    default:
     break;
  }
}

uint8_t modeBrightness(const int i)
{
  switch (mode)
  {
    case FADE:
      return cubicwave8((millis()/25+i*25) % 255);
    case JUMP:
      return (((millis() / 250) + i) % 2) * 255;
    default:
      return 255;
  }
}

void twinklePalette(uint8_t colorIndex)
{
  static uint8_t twinkle[NUM_LEDS] = {255};

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, 255, twinkle[i]);
    twinkle[i] -= 8;
    colorIndex += 3;
  }

  if (random(2) == 0)
    twinkle[random(NUM_LEDS)] = 255;
}

void paletteLEDs(uint8_t colorIndex)
{
  if (mode == TWINKLE)
  {
    twinklePalette(colorIndex);
    return;
  }
    
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, modeBrightness(i), currentBlending);
    colorIndex += 3;
  }
}

void solidLEDs()
{
  if (mode == TWINKLE)
  {
    // fade all
    for (int i = 0; i < NUM_LEDS; ++i)
      leds[i].fadeToBlackBy(8);

    // brighten some
    if (random(2) == 0)
    {
      for (int i = 0; i < 1; ++i)
        leds[random(NUM_LEDS)] = colour;
    }
    return;
  }

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = colour;
    leds[i] %= modeBrightness(i);
  }
}

uint32_t getIRCode()
{
  Wire.requestFrom(SLAVE_ADDRESS, DATA_SIZE);

  union {
    uint8_t a[DATA_SIZE];
    uint32_t i;
  } buf;
  buf.i = 0;
  for (size_t i = 0; i < DATA_SIZE && Wire.available(); ++i)
    buf.a[i] = Wire.read();

  return buf.i;
}

bool lightsOn()
{
  if (autoMode)
  {
    DateTime now = RTC.now();

    const uint32_t on = 22 * 3600;
    const uint32_t off = 23 * 3600 + 1800;

    const uint32_t time = now.hour() * 3600 + now.minute() * 30 + now.second();
    if (time > on && time < off)
    {
      if (powerOn)
        powerOn = false;
      return true;
    }
  }

  return powerOn;
}

void indicators()
{
  const bool on = lightsOn();

  digitalWrite(INDICATOR_R, !powerOn);
  digitalWrite(INDICATOR_G, autoMode);
  //digitalWrite(INDICATOR_B, HIGH);
}

void setup()
{
  digitalWrite(INDICATOR_R, HIGH);
  digitalWrite(INDICATOR_G, HIGH);
  //digitalWrite(INDICATOR_B, LOW);

  delay(3000); // power up safety, apparently

  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
  // Check to see if the RTC is keeping time.  If it is, load the time from your computer.
  if (!RTC.isrunning())
  {
    Serial.println("RTC is NOT running!");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  #ifdef USE_EEPROM_STORAGE
    uint32_t checksum;
    EEPROM.get(ADDRESS_CHECKSUM, checksum);
    if (checksum != EEPROM_crc()) // data corrupted, restore defaults
    {
      Serial.prinln("Restoring EEPROM from defaults");
      EEPROM.put(ADDRESS_BRIGHTNESS, brightness);
      EEPROM.put(ADDRESS_SPEED, speed);

      for (size_t i = 0; i < 5; ++i)
        EEPROM.put(ADDRESS_PALETTE + i, mappedPalette[i]);

      // update the checksum for next time
      checksum = EEPROM_crc();
      EEPROM.put(ADDRESS_CHECKSUM, checksum);
    }
    else // data intact, load into memory
    {
      Serial.prinln("Loading data from EEPROM");

      EEPROM.get(ADDRESS_BRIGHTNESS, brightness);
      EEPROM.get(ADDRESS_SPEED, speed);

      for (size_t i = 0; i < 5; ++i)
        EEPROM.get(ADDRESS_PALETTE + i, mappedPalette[i]);
    }
  #endif

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(brightness);
  fill_solid(leds, NUM_LEDS, CRGB::White);
}

int last = -1;
void loop()
{

  handleIR(getIRCode());

  DateTime now = RTC.now();
  const int s = now.second();
  if (s != last)
  {
    last = s;
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print('/');
    Serial.print(now.year(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
  }

  indicators();

  if (usePalette)
  {
    static uint8_t startIndex = 0;
    if (paused == false)
      startIndex = startIndex + speed; /* motion speed */

    paletteLEDs(startIndex);
  }
  else
  {
    solidLEDs();
  }

  if (lightsOn())
    FastLED.show();
  else
    FastLED.clear();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}
