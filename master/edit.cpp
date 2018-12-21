#include "edit.h"

#include "controller_mapping.h"
#include "global.h"
#include "palettes.h"

enum EditMode {
  NONE,
  DIY,
  DATE,
  TIME,
  AUTO
} state;

void editDIY(const uint32_t button)
{
  static uint8_t select = 6;
  const uint8_t selection = diy(button);
  if (select == selection)
    mappedPalette[select] = (mappedPalette[select] + 1) % 11;
  else
    select = selection;
  if (select != 6)
    currentPalette = availablePalettes[mappedPalette[select]];
}

void editIR(const uint32_t value)
{
  DateTime now = RTC.now();
  switch (value)
  {
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
      editDIY(value);
    break;
  }
}
