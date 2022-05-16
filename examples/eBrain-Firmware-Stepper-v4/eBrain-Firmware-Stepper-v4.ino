#include <Evebrain-Stepper.h>

Evebrain evebrain;

void setup(){
  evebrain.begin();
  evebrain.enableSerial();
  Serial.println("Running custom stepper firmware");
}

void loop(){
  evebrain.loop();
}
