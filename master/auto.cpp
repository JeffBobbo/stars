#include "auto.h"

#include "global.h"

const uint8_t capacity = 7;
AutoHandler::Entry _entries[capacity] = {{0, 0}};

AutoHandler::AutoHandler() : entries(nullptr), count(4)
{
  entries = _entries;
  entries[0] = {
    3UL * 86400UL + 22UL * 3600UL,
    4UL * 86400UL
  };
  entries[1] = {
    5UL * 86400UL + 22UL * 3600UL,
    6UL * 86400UL + 1800UL
  };
  entries[2] = {
    6UL * 86400UL + 22UL * 3600UL,
    1800UL
  };
  entries[3] = {
    22UL * 3600UL,
    23UL * 3600UL + 1800UL
  };
}


void AutoHandler::erase(const uint8_t idx)
{
  if (idx >= count)
    return;

  // copy everything up one
  for (uint8_t i = idx; i < count-1; ++i)
  {
    entries[i].onTime = entries[i+1].onTime;
    entries[i].offTime = entries[i+1].offTime;
  }

  // decrement the count
  --count;
}

bool AutoHandler::add(const Entry entry)
{
  if (count >= capacity)
    return false;

  entries[count++] = entry;
  return true;
}

bool AutoHandler::on() const
{
  const DateTime now = RTC.nowDST();
  const uint32_t s = now.dayOfWeek() * 86400UL + now.hour() * 3600UL + now.minute() * 60UL + now.second();

  for (uint8_t i = 0; i < count; ++i)
  {
    const Entry& e = entries[i];
    if (e.offTime < e.onTime) // wraps around the week
    {
      if (s > e.onTime || s < e.offTime)
      {
        Serial.print(F("on with wrap for "));
        Serial.println(i);
        Serial.print(F("on: "));
        Serial.print(e.onTime);
        Serial.print(F(", off: "));
        Serial.print(e.offTime);
        Serial.print(F(", at: "));
        Serial.print(now.dayOfWeek());
        Serial.print(F(" "));
        Serial.print(now.hour());
        Serial.print(F(":"));
        Serial.print(now.minute());
        Serial.print(F(":"));
        Serial.println(now.second());
        return true;
      }
    }
    else if (s > e.onTime && s < e.offTime)
    {
      Serial.print(F("on for "));
      Serial.println(i);
      Serial.print(F("on: "));
      Serial.print(e.onTime);
      Serial.print(F(", off: "));
      Serial.print(e.offTime);
      Serial.print(F(", at: "));
      Serial.print(now.dayOfWeek());
      Serial.print(F(" "));
      Serial.print(now.hour());
      Serial.print(F(":"));
      Serial.print(now.minute());
      Serial.print(F(":"));
      Serial.println(now.second());
      return true;
    }
    /*
    if (s > e.onTime && (e.onTime < e.offTime ? s < e.offTime : s < e.offTime + WEEK))
      return true;
    */
  }
  return false;
}
