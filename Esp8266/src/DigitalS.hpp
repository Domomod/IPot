#include <ESP8266WiFi.h>

class DigitalS
{
public:
  DigitalS(unsigned int data_pin)
  : data_pin(data_pin) {}

  ~DigitalS()
  {}

  void begin()
  {
    pinMode(data_pin, INPUT_PULLUP);
  }

  float readDigital()
  {
    return (float)digitalRead(data_pin);
  }

private:
  unsigned int data_pin;
};