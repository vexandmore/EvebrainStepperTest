#include "Arduino.h"
#include "Evebrain-Stepper.h"
#include "lib/ShiftStepper.h"

ShiftStepper rightMotor(1);
ShiftStepper leftMotor(0);



Evebrain::Evebrain(){
}

void Evebrain::begin(unsigned char v){
  version(v);

  // Initialise the steppers
  ShiftStepper::setup(SHIFT_REG_DATA, SHIFT_REG_CLOCK, SHIFT_REG_LATCH);
}

void Evebrain::begin(){
  begin(3);
}

void Evebrain::enableSerial(){
  // Set up the commands
  // Set up Serial and add it to be processed
  // Add the output handler for responses
  if(hwVersion == 1){
    Serial.begin(57600);
  }else if(hwVersion == 2){
    Serial.begin(57600);
  }else if(hwVersion == 3){
    Serial.begin(230400);
    Serial.setTimeout(1);
    Serial.print("{\"status\":\"notify\",\"id\":\"boot\",\"msg\":\"");
    Serial.print(versionStr);
    Serial.println("\"}");
  }
}

void Evebrain::takeUpSlack(byte rightMotorDir, byte leftMotorDir){
  
}

void Evebrain::forward(int distance){
  takeUpSlack(FORWARD, BACKWARD);
  rightMotor.turn(distance * steps_per_mm, FORWARD);
  leftMotor.turn(distance * steps_per_mm, BACKWARD);
}

void Evebrain::back(int distance){
  takeUpSlack(BACKWARD, FORWARD);
  rightMotor.turn(distance * steps_per_mm, BACKWARD);
  leftMotor.turn(distance * steps_per_mm, FORWARD);
}

void Evebrain::left(int angle){
  takeUpSlack(FORWARD, FORWARD);
  rightMotor.turn(angle * steps_per_degree, FORWARD);
  leftMotor.turn(angle * steps_per_degree, FORWARD);
}

void Evebrain::right(int angle){
  takeUpSlack(BACKWARD, BACKWARD);
  rightMotor.turn(angle * steps_per_degree, BACKWARD);
  leftMotor.turn(angle * steps_per_degree, BACKWARD);
}


void Evebrain::stop(){
  rightMotor.stop();
  leftMotor.stop();
}

void Evebrain::leftMotorForward(int distance){
  takeUpSlack(BACKWARD, FORWARD);
  leftMotor.turn(distance * steps_per_mm, FORWARD);
}


void Evebrain::rightMotorForward(int distance){
  takeUpSlack(FORWARD, BACKWARD);
  rightMotor.turn(distance * steps_per_mm, FORWARD);
}

void Evebrain::leftMotorBackward(int distance){
  takeUpSlack(FORWARD, BACKWARD);
  leftMotor.turn(distance * steps_per_mm, BACKWARD);
}

void Evebrain::rightMotorBackward(int distance){
  takeUpSlack(BACKWARD, FORWARD);
  rightMotor.turn(distance * steps_per_mm, BACKWARD);
}

void Evebrain::speedMove(int leftDistance, float leftSpeed, int rightDistance, float rightSpeed){
  //Serial.printf("ldist %d lspeed %f rdist %d rspeed %f\n", leftDistance, leftSpeed, rightDistance, rightSpeed);

  byte rightMotorDir = rightDistance > 0 ? FORWARD : BACKWARD, leftMotorDir = leftDistance > 0 ? BACKWARD : FORWARD;
  rightMotor.setRelSpeed(rightSpeed);
  leftMotor.setRelSpeed(leftSpeed);
  takeUpSlack(rightMotorDir, leftMotorDir);
  if (rightDistance != 0) {
    rightMotor.turn(abs(rightDistance) * plotter_steps_per_mm, rightMotorDir);
  }
  if (leftDistance != 0) {
    leftMotor.turn(abs(leftDistance) * plotter_steps_per_mm, leftMotorDir);
  }
}


// This allows for runtime configuration of which hardware is used
void Evebrain::version(char v){
  hwVersion = v;
  sprintf(versionStr, "%d.%s", hwVersion, Evebrain_SUB_VERSION);
  steps_per_mm = STEPS_PER_MM_V2;
  steps_per_degree = STEPS_PER_DEGREE_V2;
  plotter_steps_per_mm = PLOTTER_STEPS_PER_MM;
  wheel_distance = WHEEL_DISTANCE_V2;
}

boolean Evebrain::ready(){
  return (rightMotor.ready() && leftMotor.ready());
}

byte nextStep(byte currentStep, byte _dir){
  switch(currentStep){
    case B0000:
    case B0001:
      return (_dir == FORWARD ? B0011 : B1001);
    case B0011:
      return (_dir == FORWARD ? B0010 : B0001);
    case B0010:
      return (_dir == FORWARD ? B0110 : B0011);
    case B0110:
      return (_dir == FORWARD ? B0100 : B0010);
    case B0100:
      return (_dir == FORWARD ? B1100 : B0110);
    case B1100:
      return (_dir == FORWARD ? B1000 : B0100);
    case B1000:
      return (_dir == FORWARD ? B1001 : B1100);
    case B1001:
      return (_dir == FORWARD ? B0001 : B1000);
    default:
      return B0000;
  }
}

void shift(byte currentBits) {
  shiftOut(SHIFT_REG_DATA, SHIFT_REG_CLOCK, MSBFIRST, currentBits);
  digitalWrite(SHIFT_REG_LATCH, HIGH);
  digitalWrite(SHIFT_REG_DATA,  LOW);
  digitalWrite(SHIFT_REG_CLOCK, LOW);
  digitalWrite(SHIFT_REG_LATCH, LOW);
}

void newRM(int mm, byte _dir) {
  int steps = mm * STEPS_PER_MM_V2;
  unsigned long timeStart = micros();
  byte currentBits = B0001;
  for (int i = 0; i < steps; i++) {
    while (micros() - timeStart < i * 1500) {}
    currentBits = nextStep(currentBits, _dir);
    shift(currentBits << 4);
  }
}

void newMove(int mm, byte _dir) {
  int steps = mm * STEPS_PER_MM_V2;
  unsigned long timeStart = micros();
  byte currentBits = B0001;
  for (int i = 0; i < steps; i++) {
    while (micros() - timeStart < i * 1500) {}
    currentBits = nextStep(currentBits, _dir);
    shift(currentBits | currentBits << 4);
  }
}

void Evebrain::loop()
{
  if (Serial.available()) {
    char in = Serial.read();
    switch(in) {
      case 'F':
        newMove(100, FORWARD);
        break;
      case 'f':
        forward(100);
        break;
      case 'B':
        newMove(100, BACKWARD);
        break;
      case 'b':
        back(100);
        break;
      case 'R':
      {
        while (!Serial.available()) {}
        char in2 = Serial.read();
        if (in2 == 'F') {
          newRM(100, FORWARD);
        } else if (in2 == 'B') {
          newRM(100, BACKWARD);
        } else {
          Serial.println("cannot parse");
        }
      }
        break;
      case 'r':
      {
        while (!Serial.available()) {}
        char in2 = Serial.read();
        if (in2 == 'F') {
          rightMotorForward(100);
        } else if (in2 == 'B') {
          rightMotorBackward(100);
        } else {
          Serial.println("cannot parse");
        }
      }
      case '\r':
      case '\n':
      case ' ':
        break;
      default:
        Serial.println("Cannot parse.");
    }
  }
}
