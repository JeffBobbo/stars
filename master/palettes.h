#ifndef PALETTES_H_INCLUDE
#define PALETTES_H_INCLUDE

#include <Arduino.h>

#include <FastLED.h>

extern CRGBPalette16 currentPalette;
extern TBlendType currentBlending;

// custom palettes
extern const TProgmemPalette16 pinkPurpleWhite_p PROGMEM;
extern const TProgmemPalette16 offwhite_p PROGMEM;
extern const TProgmemPalette16 pinkBlue_p PROGMEM;

// available palettes
extern const CRGBPalette16 availablePalettes[] PROGMEM;

#endif
