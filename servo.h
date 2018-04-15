#ifndef MSERVO_H
#define MSERVO_H

#include <avr/io.h>
#include <avr/interrupt.h>

class Servo
{
private:
  enum {servoLimit = 12, servoLimitHalf = 6, minPin = 0, maxPin = 13, registerSize = 8, prescaler = 8};
  static int servoNumber;            //total number of active servos
  static Servo *servos[servoLimit];          //array of pointers to every available servo - max 12 servos can be active at time
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
  static void setPinState(int p, bool state);

  int pulseToAngle(int p);
  int angleToPulse(int a);
  
public:
  enum atmega328p
  { //used to convert arduino digital pins to avr just for convenience
    _PD0_ = 0,
    _PD1_ = 1,
    _PD2_ = 2,
    _PD3_ = 3,
    _PD4_ = 4,
    _PD5_ = 5,
    _PD6_ = 6,
    _PD7_ = 7,
    _PB0_ = 8,
    _PB1_ = 9,
    _PB2_ = 10,
    _PB3_ = 11,
    _PB4_ = 12,
    _PB5_ = 13
  };
  
  Servo();
  ~Servo();
  static void init();

  static void ISRpulseA(); //manage pulses for servos in ISR
  static void ISRpulseB();

  bool activate(int p, int a);
  bool isActive();
  void deactivate();

  void setMinAngle(int a);
  void setMaxAngle(int a);
  void setMinPulse(int p);
  void setMaxPulse(int p);
  void setUsableMinAngle(int a);
  void setUsableMaxAngle(int a);
  void setAngle(int a);

  static int getServoNumber();
  int getPin();
  int getMinAngle();
  int getMaxAngle();
  int getMinPulse();
  int getMaxPulse();
  int getUsableMinAngle();
  int getUsableMaxAngle();
  int getAngle();
  int getPulse();
};

#endif
