

#include <Arduino.h>
#include <HX711.h>

#include "Application.hpp"

Application app;

void setup()
{
  Serial.begin(115200);
}

void loop()
{

  app.runApp();
  // if (Serial.available() > 0)
  //{
  //   // Read the incoming data
  //   String data = Serial.readStringUntil('\n');
  //   // Echo the data back to the sender
  //   Serial.println(data);
  // }
}
