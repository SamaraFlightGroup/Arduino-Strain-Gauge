// #include "HX711.h"
//
//// HX711 circuit wiring
// const int LOADCELL_DOUT_PIN = 2;
// const int LOADCELL_SCK_PIN = 3;
//
// HX711 scale;
//
// void setup()
//{
//   Serial.begin(115200);
//   scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
// }
//
// void loop()
//{
//
//   if (scale.wait_ready_timeout(1000))
//   {
//     long reading = scale.read();
//     Serial.print("HX711 reading: ");
//     Serial.println(reading - 111600);
//   }
//   else
//   {
//     Serial.println("HX711 not found.");
//   }
//
//   delay(50);
// }

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
