#ifndef AUTO_H_INCLUDE
#define AUTO_H_INCLUDE

#include <stdint.h>
#include <stdlib.h>

class AutoHandler
{
public:
  struct Entry
  {
    uint32_t onTime;
    uint32_t offTime;
  };

  AutoHandler();

  inline const Entry& operator[](const uint8_t idx) const { return entries[idx]; }
  inline Entry& operator[](const uint8_t idx) { return entries[idx]; }
  inline uint8_t size() const { return count; }

  void erase(const uint8_t idx);

  bool add(const Entry entry);

  bool on() const;

private:
  Entry* entries;
  uint8_t count;
};

#endif
