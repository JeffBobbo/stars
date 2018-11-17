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
