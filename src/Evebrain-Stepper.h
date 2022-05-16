#ifndef Evebrain_h
#define Evebrain_h

#include "Arduino.h"

#define FORCE_SETUP 1
#define SERIAL_BUFFER_LENGTH 180

// The steppers have a gear ratio of 1:63.7 and have 32 steps per turn. 32 x 63.7 = 2038.4
#define STEPS_PER_TURN    2038.0f

#define CIRCUMFERENCE_MM_V2  254.4f
#define WHEEL_DISTANCE_V2    108.5f
#define PENUP_DELAY_V2 2000
#define PENDOWN_DELAY_V2 1100
#define STEPS_PER_MM_V2      STEPS_PER_TURN / CIRCUMFERENCE_MM_V2
// the steps per degree of turn when steppers are acting as wheels in the bot
#define STEPS_PER_DEGREE_V2  ((WHEEL_DISTANCE_V2 * 3.1416) / 360) * STEPS_PER_MM_V2

#define PLOTTER_CIRCUMFERENCE_MM (3.1416 * 25.0f)
#define PLOTTER_STEPS_PER_MM STEPS_PER_TURN / PLOTTER_CIRCUMFERENCE_MM

#define Evebrain_SUB_VERSION "2.1"

#define hmc5883l_address  0x1E

#define EEPROM_OFFSET 0
#define MAGIC_BYTE_1 0xF0
#define MAGIC_BYTE_2 0x0D
#define SETTINGS_VERSION 1

#define SERVO_PULSES 30
#define DHTPIN 16 
#define TRIGPIN 5
#define ECHOPIN 4
#define SPEAKER_PIN 5
#define SERVO_PIN 10
#define SERVO_PIN_TWO 16
#define SHIFT_REG_DATA  12
#define SHIFT_REG_CLOCK 13
#define SHIFT_REG_LATCH 14
#define RESET 13

#define STATUS_LED_PIN 15
#define LED_PULSE_TIME 6000.0
#define LED_COLOUR_NORMAL 0xFFFFFF

#define PCF8591_ADDRESS B1001000
#define I2C_DATA  0
#define I2C_CLOCK 2


class Evebrain {
  public:
    Evebrain();
    void begin();
    void begin(unsigned char);
    void enableSerial();
    void forward(int);
    void back(int);
    void right(int);
    void left(int);
    void stop();
    void leftMotorForward(int);
    void rightMotorForward(int);
    void leftMotorBackward(int);
    void rightMotorBackward(int);
    void speedMove(int leftDistance, float leftSpeed, int rightDistance, float rightSpeed);
    boolean ready();
    void loop();
    void calibrateSlack(unsigned int);
    void calibrateMove(float);
    void calibrateTurn(float);
    char hwVersion;
    char versionStr[9];
  private:
    void wait();
    void version(char);
    Evebrain& self() { return *this; }
    void takeUpSlack(byte, byte);
    float steps_per_mm;
    float steps_per_degree;
    float plotter_steps_per_mm;
    int wheel_distance;
    long timeTillComplete;
    
};
#endif
