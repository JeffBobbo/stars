#include "palettes.h"

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

const TProgmemPalette16 offwhite_p PROGMEM =
{
  0xFFFFFF,
  0xFFFF7F,
  0xFFFF7F,
  0x7F7FFF,

  0x7F7FFF,
  0xFFFFFF,
  0xFFFF7F,
  0xFFFF7F,

  0x7F7FFF,
  0xFFFFFF,
  0xFFFF7F,
  0x7F7FFF,

  0xFFFFFF,
  0xFFFF7F,
  0xAFAFFF,
  0xFFFFFF
};

const TProgmemPalette16 pinkBlue_p PROGMEM =
{
  CRGB::Purple,
  CRGB::Blue,
  CRGB::Purple,
  CRGB::Blue,
  CRGB::Purple,
  CRGB::Blue,
  CRGB::Purple,
  CRGB::Blue,
  CRGB::Purple,
  CRGB::Blue,
  CRGB::Purple,
  CRGB::Blue,
  CRGB::Purple,
  CRGB::Blue,
  CRGB::Purple,
  CRGB::Blue
};

CRGBPalette16 getPalette(const uint8_t idx)
{
  CRGBPalette16 p = pinkPurpleWhite_p;
  if (idx == 1)
    p = offwhite_p;
  else if (idx == 2)
    p = pinkBlue_p;
  else if (idx == 3)
    p = CloudColors_p;
  else if (idx == 4)
    p = LavaColors_p;
  else if (idx == 5)
    p = OceanColors_p;
  else if (idx == 6)
    p = ForestColors_p;
  else if (idx == 7)
    p = RainbowColors_p;
  else if (idx == 8)
    p = RainbowStripeColors_p;
  else if (idx == 9)
    p = PartyColors_p;
  else if (idx == 10)
    p = HeatColors_p;
  return p;
}
