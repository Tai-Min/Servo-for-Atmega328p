#ifndef MSERVO_H
#define MSERVO_H

#include <avr/io.h>
#include <avr/interrupt.h>

class Servo
{
private:
  enum {servoLimit = 12, servoLimitHalf = 6, minPin = 0, maxPin = 13, registerSize = 8, prescaler = 8};
  static uint8_t servoNumber;            //total number of active servos
  static Servo *servos[servoLimit];          //array of pointers to every available servo - max 12 servos can be active at time
  volatile static uint8_t currentServoA; //current servo in query to send pulse in ISR
  volatile static uint8_t currentServoB;

  int8_t pin = -1;
  int8_t index = -1;

  int16_t minAngle = 0;
  int16_t maxAngle = 180;
  uint16_t minPulse = 600;  //in microseconds
  uint16_t maxPulse = 2600; //in microseconds

  int16_t usableMinAngle = minAngle; //user can limit movement of the servo without modifying
  int16_t usableMaxAngle = maxAngle; //possible angle and pulse of the actual servo
  uint16_t usableMinPulse = minPulse;
  uint16_t usableMaxPulse = maxPulse;

  int16_t angle = minAngle; //angle acts as x in linear function that transforms angle to pulse and vice versa
  uint16_t pulse = minPulse; //pulse acts as y
  uint16_t counts;  //pulse in counts for timer1

  double linearA = 0; //for function to convert pulse to angle and vice versa
  double linearB = 0;
  void computeLinearConstants();

  static uint16_t pulseToCounts(uint16_t p);
  static void setPinState(int8_t p, bool state);

  int16_t pulseToAngle(uint16_t p);
  uint16_t angleToPulse(int16_t a);
  
public:
  Servo();
  ~Servo();
  static void init();

  static void ISRpulseA(); //manage pulses for servos in ISR
  static void ISRpulseB();

  bool activate(int8_t p, int16_t a);
  bool isActive();
  void deactivate();

  void setMinAngle(int16_t a);
  void setMaxAngle(int16_t a);
  void setMinPulse(uint16_t p);
  void setMaxPulse(uint16_t p);
  void setUsableMinAngle(int16_t a);
  void setUsableMaxAngle(int16_t a);
  void setAngle(int16_t a);

  static uint8_t getServoNumber();
  int8_t getPin();
  int16_t getMinAngle();
  int16_t getMaxAngle();
  uint16_t getMinPulse();
  uint16_t getMaxPulse();
  int16_t getUsableMinAngle();
  int16_t getUsableMaxAngle();
  int16_t getAngle();
  uint16_t getPulse();
};

#endif
