#include <ESP8266WiFi.h>

class LLS
{
public:
  LLS(unsigned int data_pin, unsigned int power_pin)
  : data_pin(data_pin), power_pin(power_pin) {}

  ~LLS()
  {}

  void begin()
  {
    pinMode(power_pin, OUTPUT);
    digitalWrite(power_pin, LOW);
  }

  float readLiquidLevel()
  {
    digitalWrite(power_pin, HIGH);
    delay(10);
    float w = analogRead(data_pin);
    digitalWrite(power_pin, LOW);;
    return w;
  }

private:
  unsigned int data_pin;
  unsigned int power_pin;
};