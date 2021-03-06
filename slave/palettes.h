#ifndef PALETTES_H_INCLUDE
#define PALETTES_H_INCLUDE

#include <Arduino.h>

#include <FastLED.h>

// custom palettes
extern const TProgmemPalette16 pinkPurpleWhite_p PROGMEM;
extern const TProgmemPalette16 offwhite_p PROGMEM;
extern const TProgmemPalette16 pinkBlue_p PROGMEM;

// available palettes
CRGBPalette16 getPalette(const uint8_t idx);

#endif
