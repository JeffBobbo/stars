#include <Arduino.h>

#ifdef USE_EEPROM_STORAGE
#include <EEPROM.h>
#endif
#include <IRLremote.h>
#include <FastLED.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "RTClib.h"

#include "controller_mapping.h"
#include "common.h"
#include "edit.h"
#include "global.h"

Adafruit_SSD1306 display(128, 64);

const int IR_PIN = 2;
CNec IRLremote;

// overrides auto mode
bool autoPowerOff = false;

void buttonColour(const uint8_t button)
{
  switch (button)
  {
    case BUTTON_RED:
      data.colour = CRGB::Red;
    break;
    case BUTTON_GREEN:
      data.colour = CRGB::Lime;
    break;
    case BUTTON_BLUE:
      data.colour = CRGB::Blue;
    break;
    case BUTTON_WHITE:
      data.colour = CRGB::White;
    break;

    case BUTTON_DARK_ORANGE:
      data.colour = CRGB::OrangeRed;
    break;
    case BUTTON_LIME:
      data.colour = CRGB::ForestGreen;
    break;
    case BUTTON_LIGHT_BLUE:
      data.colour = CRGB::DodgerBlue;
    break;

    case BUTTON_ORANGE:
      data.colour = CRGB::DarkOrange;
    break;
    case BUTTON_TURQUIOSE:
      data.colour = CRGB::DarkTurquoise;
    break;
    case BUTTON_MAUVE:
      data.colour = CRGB::Sienna;
    break;

    case BUTTON_LIGHT_ORANGE:
      data.colour = CRGB::Orange;
    break;
    case BUTTON_TEAL:
      data.colour = CRGB::Teal;
    break;
    case BUTTON_PURPLE:
      data.colour = CRGB::Purple;
    break;

    case BUTTON_YELLOW:
      data.colour = CRGB::Yellow;
    break;
    case BUTTON_DARK_GREEN:
      data.colour = CRGB::DarkGreen;
    break;
    case BUTTON_PINK:
      data.colour = CRGB::DeepPink;
    break;
    default:
      data.colour = CRGB::Black;
    break;
  }

  data.usePalette = false;
  data.mode = SOLID;
}

void buttonMode(const uint32_t button)
{
  switch (button)
  {
    case BUTTON_FLASH:
      data.mode = TWINKLE;
    break;
    case BUTTON_JUMP_THREE:
      data.mode = JUMP_THREE;
    break;
    case BUTTON_JUMP_SEVEN:
      data.mode = JUMP_SEVEN;
    break;
    case BUTTON_FADE_THREE:
      data.mode = FADE_THREE;
    break;
    case BUTTON_FADE_SEVEN:
      data.mode = FADE_SEVEN;
    break;
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
      data.palette = mappedPalette[diy(button)];
    break;
    default:
      data.usePalette = false;
      //fill_solid(leds, NUM_LEDS, CRGB::Black);
    break;
  }
}

void handleDIY(const uint32_t button)
{
  data.usePalette = true;
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

void handleIR(const uint8_t value)
{
  // filter out anything unknown
  if (!value)
    return;

  if (editMode)
  {
    editIR(value);
    return;
  }

  switch (value)
  {
    case BUTTON_BRIGHTNESS_INC:
      if (data.brightness < MAX_BRIGHTNESS)
        data.brightness += INC_BRIGHTNESS;
    break;
    case BUTTON_BRIGHTNESS_DEC:
      if (data.brightness > MIN_BRIGHTNESS)
        data.brightness -= INC_BRIGHTNESS;
    break;
    case BUTTON_PLAYPAUSE:
      data.paused = !data.paused;
    break;
    case BUTTON_POWER:
      if (autoMode && autoHandler.on())
        autoPowerOff = !autoPowerOff;
      else
        powerOn = !powerOn;      
      //if (powerOn && !usePalette)
        //fill_solid(leds, NUM_LEDS, colour);
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
      buttonColour(value);
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
      if (data.speed < MAX_SPEED)
        ++data.speed;
    break;
    case BUTTON_SLOW:
      if (data.speed > MIN_SPEED)
        --data.speed;
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
    default:
     break;
  }
}

uint8_t getIRCode()
{
  if (IRLremote.available())
  {
    auto data = IRLremote.read();

    if (data.address == 0xFF00)
      return data.command;
  }
  return 0;
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

  if (!editMode)
  {
    uint8_t brightness = ((data.brightness+1) / 256.0f) * 100;
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print(F("B:"));
    display.print(brightness);
  }

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
    if (editState == TIME)
      display.setCursor((128 - x2) / 2, 16);//(48 - y2) / 2 + 16);
    else
      display.setCursor((128 - x2) / 2, (48 - y2) / 2 + 16);
    display.print(timestamp.c_str());

    if (editState == TIME)
    {
      String seconds;
      if (now.second() < 10)
        seconds += "0";
      seconds += now.second();
      display.setTextSize(2);
      display.getTextBounds(seconds.c_str(), 0, 0, &x1, &y1, &x2, &y2);
      display.setCursor((128 - x2) / 2, 48);
      display.print(seconds.c_str());
    }
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
          display.print(paletteSelect + 1);
        else
          display.print("Select");
        if (paletteSelect != 6)
        {
          display.setCursor(0, 32);
          display.print(F("Palette: "));

          String palette(mappedPalette[paletteSelect]);
          display.getTextBounds(palette.c_str(), 0, 0, &x1, &y1, &x2, &y2);
          display.setCursor(128 - x2, 32);
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

  // start IR receiver
  if (!IRLremote.begin(IR_PIN))
  {
    while (1)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    }
  }

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
}

void loop()
{
  handleIR(getIRCode());

  data.lightsOn = lightsOn();

  Wire.beginTransmission(SLAVE_ADDRESS);
  uint8_t* ptr = reinterpret_cast<uint8_t* >(&data);
  for (size_t i = 0; i < sizeof(data); ++i)
    Wire.write(ptr[i]);
  Wire.endTransmission();

  screen();

  delay(100);
}
