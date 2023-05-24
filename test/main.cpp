#include <Arduino.h>
#include "BiSS.h"
#include <CustomSPI.h>

BiSSAKSIM2<18, false> encoder{CustomSPI, 1000000};
void setup()
{
  Serial.begin(9600);
  /* SPISettings settings_{1000000, MSBFIRST, SPI_MODE3};
  CustomSPI.begin();
  CustomSPI.beginTransaction(settings_);
  while (1)
  {
    for (int i = 1; i <= 32; i++)
    {
      CustomSPI.transfern(0, i); // test every number of clock bits 1-32
      delay(10);
    }
  } */
  encoder.Begin();
  encoder.Zero();
}

void loop()
{
  Serial.print("Position: ");
  Serial.println(encoder.ReadPosition());
  // Serial.print(encoder.GetWarning() ? "\tWARN" : "");
  // Serial.println(encoder.GetError() ? "\tERR" : "");
  delay(1000);
}