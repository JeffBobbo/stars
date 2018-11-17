#include <FastLED.h>
#include <IRremote.h>
#include <Wire.h>

#include "RTClib.h"

#define LED_PIN 5
#define NUM_LEDS 3
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

const int UPDATES_PER_SECOND 100

const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;

enum Button
{
  BUTTON_PLAYPAUSE = 0xFF827D,
  BUTTON_POWER = 0xFF02FD,

  BUTTON_BRIGHTNESS_INC = 0xFF3AC5,
  BUTTON_BRIGHTNESS_DEC = 0xFFBA45,

  BUTTON_RED = 0xFF1AE5,
  BUTTON_GREEN = 0xFF9A65,
  BUTTON_BLUE = 0xFFA25D,
  BUTTON_WHITE = 0xFF22DD,

  BUTTON_DARK_ORANGE = 0x2AD5,
  BUTTON_LIME = 0xFFAA55,
  BUTTON_LIGHT_BLUE = 0xFF926D,
  BUTTON_PINK_A = 0xFF12ED,

  BUTTON_ORANGE = 0xFF0AF5,
  BUTTON_TURQUIOSE = 0xFF8A75,
  BUTTON_MAUVE = 0xFFB24D,
  BUTTON_PINK_B = 0xFF32CD,

  BUTTON_LIGHT_ORANGE = 0xFF38C7,
  BUTTON_TEAL = 0xFFB847,
  BUTTON_PURPLE = 0xFF7887,
  BUTTON_BLUE_A = 0xFFF807,

  BUTTON_YELLOW = 0xFF18E7,
  BUTTON_DARK_GREEN = 0xFF9867,
  BUTTON_PINK = 0xFF58A7,
  BUTTON_BLUE_B = 0xFFD827,

  BUTTON_RED_UP = 0xFF28D7,
  BUTTON_GREEN_UP = 0xFFA857,
  BUTTON_BLUE_UP = 0xFF6897,
  BUTTON_RED_DOWN = 0xFF08F7,
  BUTTON_GREEN_DOWN = 0xFF8877,
  BUTTON_BLUE_DOWN = 0xFF48B7,
  BUTTON_DIY_ONE = 0xFF30CF,
  BUTTON_DIY_TWO = 0xFFB04F,
  BUTTON_DIY_THREE = 0xFF708F,
  BUTTON_DIY_FOUR = 0xFF10EF,
  BUTTON_DIY_FIVE = 0xFF906F,
  BUTTON_DIY_SIX = 0xFF50AF,

  BUTTON_QUICK = 0xFFE817,
  BUTTON_SLOW = 0xFFC837,

  BUTTON_AUTO = 0xFFF00F,
  BUTTON_FLASH = 0xFFD02F,
  BUTTON_JUMP_THREE = 0xFF20DF,
  BUTTON_JUMP_SEVEN = 0xFFA05F,
  BUTTON_FADE_THREE = 0xFF609F,
  BUTTON_FADE_SEVEN = 0xFFE01F,

  BUTTON_HOLD = 0xFFFFFFFF
};

enum Mode {
  SOLID = 0,
  PALETTE
};

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
const uint8_t MAX_SPEED = 12;
uint8_t speed = MAX_SPEED / 2;
bool paused = false;
uint8_t brightness = 127;
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 pinkPurpleWhite;
extern const TProgmemPalette16 pinkPurpleWhite_p PROGMEM;

void buttonPalette(const uint32_t button)
{
  switch (button)
  {
    case BUTTON_DIY_ONE:
      currentPalette = pinkPurpleWhite_p;
    break;
    case BUTTON_DIY_TWO:
      currentPalette = PartyColors_p;
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
      mode = SOLID;
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
      mode = SOLID;
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
    case BUTTON_DIY_SIX:
      mode = PALETTE;
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
    case BUTTON_HOLD:
    break;
    default:
      Serial.println(results.value, HEX);
     break;
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
  currentBlending = LINEARBLEND;
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

  if (mode == PALETTE)
  {
    static uint8_t startIndex = 0;
    if (paused == false)
      startIndex = startIndex + speed; /* motion speed */

    FillLEDsFromPaletteColors(startIndex);
  }

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void FillLEDsFromPaletteColors(uint8_t colorIndex)
{
    for (int i = 0; i < NUM_LEDS; ++i)
    {
        leds[i] = ColorFromPalette(currentPalette, colorIndex, 255, currentBlending);
        colorIndex += 3;
    }
}


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;

    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;

}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;

    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


const TProgmemPalette16 pinkPurpleWhite_p PROGMEM =
{
  CRGB::White,
  CRGB::Pink,
  CRGB::Purple,
  CRGB::White,

  CRGB::Pink,
  CRGB::Purple,
  CRGB::White,
  CRGB::Pink,

  CRGB::Purple,
  CRGB::White,
  CRGB::Pink,
  CRGB::Purple,

  CRGB::White,
  CRGB::Pink,
  CRGB::Purple,
  CRGB::White
};

// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,

    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,

    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};



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
