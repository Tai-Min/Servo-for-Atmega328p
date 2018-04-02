#ifndef MSERVO_H
#define MSERVO_H

#include <avr/io.h>
#include <avr/interrupt.h>

class Servo
{
private:
  static const int prescaler;
  static int servoNumber;            //total number of active servos
  static Servo *servos[12];          //array of pointers to every available servo - max 12 servos can be active at time
  volatile static int currentServoA; //current servo in query to send pulse in ISR
  volatile static int currentServoB;

  int pin = -1;
  int index = -1;

  int minAngle = 0;
  int maxAngle = 180;
  int minPulse = 600;  //in microseconds
  int maxPulse = 2600; //in microseconds

  int usableMinAngle = minAngle; //user can limit movement of the servo without modifying
  int usableMaxAngle = maxAngle; //possible angle and pulse of the actual servo
  int usableMinPulse = minPulse;
  int usableMaxPulse = maxPulse;

  int angle = minAngle; //angle acts as x in linear function that transforms angle to pulse and vice versa
  int pulse = minPulse; //pulse acts as y
  unsigned int counts;  //pulse in counts for timer1

  double linearA = 0; //for function to convert pulse to angle and vice versa
  double linearB = 0;
  void computeLinearConstants();

  static unsigned int pulseToCounts(int p);

  
  static void setPinState(int pin, bool state);

protected:
  int pulseToAngle(int p);
  int angleToPulse(int a);

public:
  enum atmega328p
  { //used to convert arduino digital pins to avr just for convenience
    _PD0 = 0,
    _PD1 = 1,
    _PD2 = 2,
    _PD3 = 3,
    _PD4 = 4,
    _PD5 = 5,
    _PD6 = 6,
    _PD7 = 7,
    _PB0 = 8,
    _PB1 = 9,
    _PB2 = 10,
    _PB3 = 11,
    _PB4 = 12,
    _PB5 = 13
  };
  
  Servo();
  ~Servo();
  static void init();

  static void ISRpulseA(); //manage pulses for servos in ISR
  static void ISRpulseB();

  bool activate(int p);
  void deactivate();
  bool isActive();

  bool setMinAngle(int a);
  bool setMaxAngle(int a);
  bool setMinPulse(int p);
  bool setMaxPulse(int p);
  bool setUsableMinAngle(int a);
  bool setUsableMaxAngle(int a);
  bool setAngle(int a);

  static int getServoNumber();
  int getPin();
  int getMinAngle();
  int getMaxAngle();
  int getMinPulse();
  int getMaxPulse();
  int getUsableMinAngle();
  int getUsableMaxAngle();
  int getAngle();
};

#endif
