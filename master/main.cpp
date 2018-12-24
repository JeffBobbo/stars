#include <Arduino.h>

#include <FastLED.h>
#include <Wire.h>

#ifdef USE_EEPROM_STORAGE
#include <EEPROM.h>
#endif

#include "RTClib.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64);

//#include "../common.h"
const uint8_t SLAVE_ADDRESS = 9;
const size_t DATA_SIZE = sizeof(uint32_t);

#include "global.h"
#include "controller_mapping.h"
#include "palettes.h"
#include "edit.h"

// overrides auto mode
bool autoPowerOff = false;

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
      mode = JUMP_THREE;
    case BUTTON_JUMP_SEVEN:
      mode = JUMP_SEVEN;
    break;
    case BUTTON_FADE_THREE:
      mode = FADE_THREE;
    case BUTTON_FADE_SEVEN:
      mode = FADE_SEVEN;
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
      currentPalette = getPalette(mappedPalette[diy(button)]);
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

bool lightsOn()
{
  if (autoMode)
  {
    if (autoHandler.on())
    {
      if (autoPowerOff) // if we've turned off, stay off
        return false;

      if (powerOn)
        powerOn = false;
      return true;
    }
    else // drop override when we drop out of auto
    {
      autoPowerOff = false;
    }
  }

  return powerOn;
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
      if (autoMode && autoHandler.on())
        autoPowerOff = !autoPowerOff;
      else
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
      mode = SOLID;
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
      autoPowerOff = false;
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
    case FADE_THREE:
      return cubicwave8((millis()/25+i*25) % 255);
    case FADE_SEVEN:
      return cubicwave8((millis()/100+i*25) % 255);
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
    leds[i] = ColorFromPalette(currentPalette, colorIndex, twinkle[i], currentBlending);
    twinkle[i] -= 16;
    colorIndex += 3;
  }

  if (random(3) == 0)
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

void screen()
{
  display.clearDisplay();

  int16_t x1, y1;
  uint16_t x2, y2;

  /*
  if (!editMode)
  {
    display.setTextSize(2);
    display.setCursor(0, 0);
    if (autoMode)
      display.print("A");
    if (powerOn)
      display.print("P");
    if (autoPowerOff)
      display.print("O");
  }
  */

  if (!editMode || editState == TIME)
  {
    const DateTime now = RTC.nowDST();
    String timestamp;
    if (now.hour() < 10)
      timestamp += "0";
    timestamp += now.hour();
    timestamp += now.second() % 2 ? ":" : " ";
    if (now.minute() < 10)
      timestamp += "0";
    timestamp += now.minute();

    display.setTextSize(4);
    display.getTextBounds(timestamp.c_str(), 0, 0, &x1, &y1, &x2, &y2);
    display.setCursor((128 - x2) / 2, (48 - y2) / 2 + 16);
    display.print(timestamp.c_str());
  }

  if (editMode)
  {
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print(F("Edit"));

    switch (editState)
    {
      case NONE:
      break;
      case PALETTE:
        display.setTextSize(2);
        display.getTextBounds(F("DIY"), 0, 0, &x1, &y1, &x2, &y2);
        display.setCursor(128 - x2, 0);
        display.print(F("DIY"));

        display.setCursor(0, 16);
        display.print(F("DIY: "));
        if (paletteSelect != 6)
          display.print(paletteSelect);
        else
          display.print("Select");
        if (paletteSelect != 6)
        {
          display.setCursor(0, 32);
          display.print(F("Palette: "));
          display.print(mappedPalette[paletteSelect]);
        }
      break;
      case AUTO:
        display.setTextSize(2);
        display.getTextBounds(F("Auto"), 0, 0, &x1, &y1, &x2, &y2);
        display.setCursor(128 - x2, 0);
        display.print(F("Auto"));

        if (autoSelect < autoHandler.size())
        {
          display.setCursor(0, 16);
          uint32_t onAt = autoHandler[autoSelect].onTime;
          uint32_t offAt = autoHandler[autoSelect].offTime;

          uint32_t onDay = onAt / 86400;
          uint32_t offDay = offAt / 86400;
          uint8_t onHour = (onAt % 86400) / 3600;
          uint8_t offHour = (offAt % 86400) / 3600;
          uint8_t onMinute = (onAt % 3600) / 60;
          uint8_t offMinute = (offAt % 3600) / 60;

          display.setCursor(0, 16);
          if (autoEditOn)
            display.print(">");
          display.print(dayName(onDay));
          display.setCursor(0, 32);
          if (!autoEditOn)
            display.print(">");
          display.print(dayName(offDay));


          String str;
          if (onHour < 10)
            str += "0";
          str += onHour;
          str += ":";
          if (onMinute < 10)
            str += "0";
          str += onMinute;
          display.getTextBounds(str.c_str(), 0, 0, &x1, &y1, &x2, &y2);
          display.setCursor(128 - x2, 16);
          display.print(str.c_str());

          str = "";
          if (offHour < 10)
            str += "0";
          str += offHour;
          str += ":";
          if (offMinute < 10)
            str += "0";
          str += offMinute;
          display.getTextBounds(str.c_str(), 0, 0, &x1, &y1, &x2, &y2);
          display.setCursor(128 - x2, 32);
          display.print(str.c_str());
        }

        display.setCursor(0, 48);
        if (autoHandler.size() == 0)
        {
          display.print("No entries");
        }
        else
        {
          display.print(autoSelect + 1);
          display.print(" of ");
          display.print(autoHandler.size());
        }
      break;
      case TIME:
        display.setTextSize(2);
        display.getTextBounds(F("Time"), 0, 0, &x1, &y1, &x2, &y2);
        display.setCursor(128 - x2, 0);
        display.print(F("Time"));
      break;
      case DATE:
        display.setTextSize(2);
        display.getTextBounds(F("Date"), 0, 0, &x1, &y1, &x2, &y2);
        display.setCursor(128 - x2, 0);
        display.print(F("Date"));

        const DateTime now = RTC.nowDST();
        String dmString(now.day());
        dmString += "/";
        dmString += now.month();
        String yString(now.year());

        display.setTextSize(3);
        display.getTextBounds(dmString.c_str(), 0, 0, &x1, &y1, &x2, &y2);
        display.setCursor((128 - x2) / 2, 16);
        display.print(dmString.c_str());

        display.setTextSize(2);
        display.getTextBounds(yString.c_str(), 0, 0, &x1, &y1, &x2, &y2);
        display.setCursor((128 - x2) / 2, 48);
        display.print(yString.c_str());
      break;
    }
  }
  else
  {
    if (autoMode)
    {
      display.setTextSize(2);
      display.getTextBounds(F("Auto"), 0, 0, &x1, &y1, &x2, &y2);
      display.setCursor(128 - x2, 0);
      display.print(F("Auto"));
    }
  }

  display.display();
}

void setup()
{
  delay(3000); // power up safety, apparently

  // start I2C
  Wire.begin();

  // start display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    while (1);

  display.clearDisplay();
  display.setTextColor(WHITE); // only options are white and black
  display.setTextWrap(false); // don't wrap text
  display.dim(1); // dim the display

  // start RTC
  RTC.begin();
  // Check to see if the RTC is keeping time.  If it is, load the time from your computer.
  if (!RTC.isrunning())
    RTC.adjust(DateTime(__DATE__, __TIME__));


  #ifdef USE_EEPROM_STORAGE
    uint32_t checksum;
    EEPROM.get(ADDRESS_CHECKSUM, checksum);
    if (checksum != EEPROM_crc()) // data corrupted, restore defaults
    {
      Serial.prinln(F("Restoring EEPROM from defaults"));
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
      Serial.prinln(F("Loading data from EEPROM"));

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

void updateLEDs()
{
  if (usePalette)
  {
    static uint16_t startIndex = 0;
    if (paused == false)
      startIndex = startIndex + speed;

    paletteLEDs(startIndex / 10);
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

void loop()
{
  handleIR(getIRCode());

  screen();

  updateLEDs();
}
