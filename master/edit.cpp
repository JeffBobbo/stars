#include "edit.h"

#include "controller_mapping.h"
#include "global.h"
#include "palettes.h"

EditMode editState;

uint8_t paletteSelect = 6;
void changePalette(const uint32_t button)
{
  const uint8_t selection = diy(button);
  if (paletteSelect == selection)
    mappedPalette[paletteSelect] = (mappedPalette[paletteSelect] + 1) % 11;
  else
    paletteSelect = selection;
  if (paletteSelect != 6)
    currentPalette = getPalette(mappedPalette[paletteSelect]);
}

void editPalette(const uint32_t value)
{
  switch (value)
  {
    case BUTTON_DIY_ONE:
    case BUTTON_DIY_TWO:
    case BUTTON_DIY_THREE:
    case BUTTON_DIY_FOUR:
    case BUTTON_DIY_FIVE:
      changePalette(value);
    break;
    case BUTTON_DIY_SIX:
      editState = NONE;
    default:
    break;
  }
}

uint8_t autoSelect = 0;
bool autoEditOn = true;
void editAuto(const uint32_t value)
{
  switch (value)
  {
    case BUTTON_RED_UP:
      autoSelect = (autoSelect + 1) % autoHandler.size();
    break;
    case BUTTON_RED_DOWN:
      autoSelect = (autoSelect - 1 + autoHandler.size()) % autoHandler.size();
    break;
    case BUTTON_GREEN_UP:
      if (autoEditOn)
        autoHandler[autoSelect].onTime = (autoHandler[autoSelect].onTime + WEEK + 3600UL) % WEEK;
      else
        autoHandler[autoSelect].offTime = (autoHandler[autoSelect].offTime + WEEK + 3600UL) % WEEK;
    break;
    case BUTTON_GREEN_DOWN:
      if (autoEditOn)
        autoHandler[autoSelect].onTime = (autoHandler[autoSelect].onTime + WEEK - 3600UL) % WEEK;
      else
        autoHandler[autoSelect].offTime = (autoHandler[autoSelect].offTime + WEEK - 3600UL) % WEEK;
    break;
    case BUTTON_BLUE_UP:
      if (autoEditOn)
        autoHandler[autoSelect].onTime = (autoHandler[autoSelect].onTime + WEEK + 600UL) % WEEK;
      else
        autoHandler[autoSelect].offTime = (autoHandler[autoSelect].offTime + WEEK + 600UL) % WEEK;
    break;
    case BUTTON_BLUE_DOWN:
      if (autoEditOn)
        autoHandler[autoSelect].onTime = (autoHandler[autoSelect].onTime + WEEK - 600UL) % WEEK;
      else
        autoHandler[autoSelect].offTime = (autoHandler[autoSelect].offTime + WEEK - 600UL) % WEEK;
    break;
    case BUTTON_DIY_ONE:
      if (autoEditOn)
        autoHandler[autoSelect].onTime = (autoHandler[autoSelect].onTime + WEEK + 86400UL) % WEEK;
      else
        autoHandler[autoSelect].offTime = (autoHandler[autoSelect].offTime + WEEK + 86400UL) % WEEK;
    break;
    case BUTTON_DIY_TWO:
      if (autoEditOn)
        autoHandler[autoSelect].onTime = (autoHandler[autoSelect].onTime + WEEK - 86400UL) % WEEK;
      else
        autoHandler[autoSelect].offTime = (autoHandler[autoSelect].offTime + WEEK - 86400UL) % WEEK;
    break;
    case BUTTON_DIY_THREE:
      autoEditOn = !autoEditOn;
    break;
    case BUTTON_DIY_FOUR:
      if (autoHandler.add({0, 0}))
        autoSelect = autoHandler.size() - 1;
    break;
    case BUTTON_DIY_FIVE:
      autoHandler.erase(autoSelect);
      autoSelect = 0;
    break;
    case BUTTON_DIY_SIX:
      editState = NONE;
    break;
  }
}

void editTime(const uint32_t value)
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
    case BUTTON_DIY_SIX:
      editState = NONE;
    default:
    break;
  }
}

void editDate(const uint32_t value)
{
  DateTime now = RTC.now();

  switch (value)
  {
    case BUTTON_RED_UP:
      RTC.adjust(DateTime(now.year(), now.month(), now.day()+1, now.hour(), now.minute(), now.second()));
    break;
    case BUTTON_RED_DOWN:
      RTC.adjust(DateTime(now.year(), now.month(), now.day()-1, now.hour(), now.minute(), now.second()));
    break;
    case BUTTON_GREEN_UP:
      RTC.adjust(DateTime(now.year(), now.month()+1, now.day(), now.hour(), now.minute(), now.second()));
    break;
    case BUTTON_GREEN_DOWN:
      RTC.adjust(DateTime(now.year(), now.month()-1, now.day(), now.hour(), now.minute(), now.second()));
    break;
    case BUTTON_BLUE_UP:
      RTC.adjust(DateTime(now.year()+1, now.month(), now.day(), now.hour(), now.minute(), now.second()));
    break;
    case BUTTON_BLUE_DOWN:
      RTC.adjust(DateTime(now.year()-1, now.month(), now.day(), now.hour(), now.minute(), now.second()));
    break;
    case BUTTON_DIY_SIX:
      editState = NONE;
    default:
    break;
  }
}


void editIR(const uint32_t value)
{
  if (editState)
  {
    switch (editState)
    {
      case PALETTE:
        editPalette(value);
      break;
      case AUTO:
        editAuto(value);
      break;
      case TIME:
        editTime(value);
      break;
      case DATE:
        editDate(value);
      default:
      break;
    }
    return;
  }

  static bool oldPalette;
  static Mode oldMode; 
  switch (value)
  {
    case BUTTON_DIY_ONE:
    {
      oldPalette = usePalette;
      oldMode = mode;

      editState = PALETTE;
      usePalette = true;
      mode = SOLID;
    }
    break;
    case BUTTON_DIY_TWO:
      editState = AUTO;
    break;
    case BUTTON_DIY_THREE:
      editState = TIME;
    break;
    case BUTTON_DIY_FOUR:
      editState = DATE;
    break;
    case BUTTON_DIY_SIX:
      usePalette = oldPalette;
      mode = oldMode;
      editMode = false;
    default:
    break;
  }
}
