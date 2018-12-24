#ifndef EDIT_H_INCLUDE
#define EDIT_H_INCLUDE

#include <stdint.h>

enum EditMode {
  NONE,
  PALETTE,
  TIME,
  DATE,
  AUTO
};
extern EditMode editState;

// used for changing palettes
extern uint8_t paletteSelect;

extern uint8_t autoSelect;
extern bool autoEditOn;

void editIR(const uint32_t value);

#endif
