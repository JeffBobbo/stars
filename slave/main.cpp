#include <Arduino.h>
#include <IRremote.h>
#include <Wire.h>

//#include "../common.h"
const uint8_t SLAVE_ADDRESS = 9;

const int IR_PIN = 7;
IRrecv irrecv(IR_PIN);
decode_results results;
uint32_t current = 0;

const uint8_t NO_DATA[] = {0xFF, 0xFF, 0xFF, 0xFF};
const size_t DATA_SIZE = sizeof(uint32_t);

void requestEvent()
{
  if (current)
  {
    Wire.write(reinterpret_cast<byte*>(&current), DATA_SIZE);
    current = 0;
  }
  else
  {
    Wire.write(NO_DATA, DATA_SIZE);
  }
}

void setup()
{
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);

  irrecv.enableIRIn();
  irrecv.blink13(true);
}

void loop()
{
  if (irrecv.decode(&results))
  {
    const uint32_t value = results.value;
    if ((value & 0xFFFF0000) == 0x00FF0000)
      current = value;
    irrecv.resume();
  }
  delay(100);
}
