#include "palettes.h"

CRGBPalette16 currentPalette;
TBlendType currentBlending = LINEARBLEND;

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
  0xFFFFAF,
  0x7F7F7F,
  0xAFAFFF,

  0xFFFFFF,
  0xFFFFAF,
  0x7F7F7F,
  0xAFAFFF,

  0xFFFFFF,
  0xFFFFAF,
  0x7F7F7F,
  0xAFAFFF,

  0xFFFFFF,
  0xFFFFAF,
  0x7F7F7F,
  0xAFAFFF
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
