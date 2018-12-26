#ifndef RTC_H_INCLUDE
#define RTC_H_INCLUDE
// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!
// modifications made

// Simple general-purpose date/time class (no TZ / DST / leap second handling!)
#include <stdint.h>

class DateTime
{
public:
  DateTime(uint32_t t = 0);
  DateTime(uint16_t year, uint8_t month, uint8_t day,
                uint8_t hour = 0, uint8_t min = 0, uint8_t sec = 0);
  DateTime(const char* date, const char* time);
  uint16_t year() const       { return 2000 + yOff; }
  int8_t month() const       { return m; }
  int8_t day() const         { return d; }
  int8_t hour() const        { return hh; }
  int8_t minute() const      { return mm; }
  int8_t second() const      { return ss; }
  uint8_t dayOfWeek() const;

  uint16_t incYear();
  uint16_t decYear();
  int8_t incMonth();
  int8_t decMonth();
  int8_t incDay();
  int8_t decDay();
  int8_t incHour();
  int8_t decHour();
  int8_t incMinute();
  int8_t decMinute();
  int8_t incSecond();
  int8_t decSecond();

  // 32-bit times as seconds since 1/1/1970
  uint32_t unixtime() const;

  // returns true if this time is in DST time (UK)
  bool isDST() const;

protected:
  uint16_t yOff;
  int8_t m, d, hh, mm, ss;
};

// RTC based on the DS1307 chip connected via I2C and the Wire library
class RTC_DS1307
{
public:
  static uint8_t begin(void);
  static void adjust(const DateTime& dt);
  uint8_t isrunning(void);
  static DateTime now();
  static DateTime nowDST();
};

#endif
