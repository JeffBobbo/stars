#include <Arduino.h>

#include <FastLED.h>
#include <IRremote.h>
#include <Wire.h>

#include "RTClib.h"

#include "controller_mapping.h"
#include "palettes.h"

const int LED_PIN = 5;
#define NUM_LEDS 3
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

const int UPDATES_PER_SECOND = 100;

const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;

enum Mode {
  SOLID = 0,
  FADE,
  JUMP,
  TWINKLE
};

bool editMode = false;
bool usePalette = false;
bool autoMode = false;

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
      return CRGB::LimeGreen;
    case BUTTON_LIGHT_BLUE:
      return CRGB::DeepSkyBlue;

    case BUTTON_ORANGE:
      return CRGB::OrangeRed;
    case BUTTON_TURQUIOSE:
      return CRGB::Turquoise;
    case BUTTON_MAUVE:
      return CRGB::Maroon;

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
      return CRGB::Pink;
    default:
      return CRGB::Black;
  }
}

Mode mode;
void buttonMode(const uint32_t button)
{
  switch (button)
  {
    case BUTTON_FLASH:
      mode = TWINKLE;
    break;
    case BUTTON_JUMP_THREE:
      mode = JUMP;
    break;
    case BUTTON_JUMP_SEVEN:
      mode = JUMP;
    break;
    case BUTTON_FADE_THREE:
      mode = FADE;
    break;
    case BUTTON_FADE_SEVEN:
      mode = FADE;
    break;
  }
}

const uint8_t MAX_SPEED = 12;
uint8_t speed = MAX_SPEED / 2;
bool paused = false;
uint8_t brightness = 127;

void buttonPalette(const uint32_t button)
{
  switch (button)
  {
    case BUTTON_DIY_ONE:
      currentPalette = pinkPurpleWhite_p;
    break;
    case BUTTON_DIY_TWO:
      currentPalette = CloudColors_p;
      //currentPalette = OceanColors_p;
    break;
    case BUTTON_DIY_THREE:
      currentPalette = PartyColors_p;
    case BUTTON_DIY_FOUR:
      currentPalette = offwhite_p;
    break;
    case BUTTON_DIY_FIVE:
      // ??
    break;
    case BUTTON_DIY_SIX:
      editMode = !editMode;
    break;
    default:
      currentPalette = RainbowColors_p;
    break;
  }
}


extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

RTC_DS1307 RTC;

void handleIR(const uint32_t value)
{
  // filter out anything unknown
  if ((value & 0xFFFF0000) != 0x00FF0000)
    return;

  DateTime now = RTC.now();
  switch (value)
  {
    case BUTTON_BRIGHTNESS_INC:
      if (brightness < 255)
        brightness = (brightness << 2) | 3;
      FastLED.setBrightness(brightness);
    break;
    case BUTTON_BRIGHTNESS_DEC:
      if (brightness > 0)
        brightness >>= 2;
      FastLED.setBrightness(brightness);
    break;
    case BUTTON_PLAYPAUSE:
      paused = !paused;
    break;
    case BUTTON_POWER:
      usePalette = false;
      fill_solid(leds, NUM_LEDS, CRGB::Black);
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
      fill_solid(leds, NUM_LEDS, buttonColour(value));
    break;
    case BUTTON_RED_UP:
      RTC.adjust(DateTime(now.year(), now.month(), now.day(), now.hour()+1, now.minute(), now.second()));
    break;
    case BUTTON_RED_DOWN:
      RTC.adjust(DateTime(now.year(), now.month(), now.day(), now.hour()-1, now.minute(), now.second()));
    break;
    case BUTTON_GREEN_UP:
      RTC.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), now.minute()+1, now.second()));
    break;
    case BUTTON_GREEN_DOWN:
      RTC.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), now.minute()-1, now.second()));
    break;
    case BUTTON_BLUE_UP:
      RTC.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second()+1));
    break;
    case BUTTON_BLUE_DOWN:
      RTC.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second()-1));
    break;
    case BUTTON_DIY_ONE:
    case BUTTON_DIY_TWO:
    case BUTTON_DIY_THREE:
    case BUTTON_DIY_FOUR:
    case BUTTON_DIY_FIVE:
    // case BUTTON_DIY_SIX:
      usePalette = true;
      buttonPalette(value);
    break;
    case BUTTON_QUICK:
      if (speed < MAX_SPEED)
        ++speed;
    break;
    case BUTTON_SLOW:
      if (speed > 0)
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
      Serial.println(results.value, HEX);
     break;
  }
}

void fillLEDs(uint8_t colorIndex)
{
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    const uint8_t brightness =
      mode == FADE ?
        cubicwave8(millis() / 250 + i)
      :
      mode == JUMP ?
        (millis() / 250) % 2
      :
      mode == TWINKLE ?
        cubicwave8(millis() / 250 + i) + cubicwave8(millis() / 785 - i)
      :
        255;
    leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

void setup()
{
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

  irrecv.enableIRIn();
  irrecv.blink13(true);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(brightness);

  currentPalette = RainbowColors_p;
}

int last = 0;
void loop()
{
  DateTime now = RTC.now();

  if (irrecv.decode(&results))
  {
    handleIR(results.value);
    irrecv.resume();
  }

  const int s = now.second();
  if (s == last)
    return;
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

  if (usePalette)
  {
    static uint8_t startIndex = 0;
    if (paused == false)
      startIndex = startIndex + speed; /* motion speed */

    fillLEDs(startIndex);
  }

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

// Additionl notes on FastLED compact palettes:
//
// Normally, in computer graphics, the palette (or "color lookup table")
// has 256 entries, each containing a specific 24-bit RGB color.  You can then
// index into the color palette using a simple 8-bit (one byte) value.
// A 256-entry color palette takes up 768 bytes of RAM, which on Arduino
// is quite possibly "too many" bytes.
//
// FastLED does offer traditional 256-element palettes, for setups that
// can afford the 768-byte cost in RAM.
//
// However, FastLED also offers a compact alternative.  FastLED offers
// palettes that store 16 distinct entries, but can be accessed AS IF
// they actually have 256 entries; this is accomplished by interpolating
// between the 16 explicit entries to create fifteen intermediate palette
// entries between each pair.
//
// So for example, if you set the first two explicit entries of a compact
// palette to Green (0,255,0) and Blue (0,0,255), and then retrieved
// the first sixteen entries from the virtual palette (of 256), you'd get
// Green, followed by a smooth gradient from green-to-blue, and then Blue.
