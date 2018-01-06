#ifndef MSERVO_H
#define MSERVO_H

#include <avr/io.h>
#include <avr/interrupt.h>

class Servo
{
  private:
    static const int prescaler;
    static int servoNumber;//total number of active servos
    static Servo *servos[6];//array of pointers to every available servo - max 6 servos can be active at time
    volatile static int currentServo;//current servo in query to send pulse in ISR

    int index = -1;
    int pin = -1;

    int minAngle = 0;
    int maxAngle = 180;
    int minPulse = 600;//in us
    int maxPulse = 2600;//in us

    int usableMinAngle = minAngle;//user can limit movement of the servo without modifying 
    int usableMaxAngle = maxAngle;//possible angle and pulse of the actual servo
    int usableMinPulse = minPulse;
    int usableMaxPulse = maxPulse;

    int angle = minAngle;//angle acts as x in linear function that transforms angle to pulse and vice versa
    int pulse = minPulse;//pulse acts as y
    unsigned int counts;//pulse in counts for timer1
    
    double linearA = 0;//for function to convert pulse to angle and vice versa
    double linearB = 0;
    void computeLinearConstants();
    
    static unsigned int pulseToCounts(int p);

    static void init();
    static void setPinState(int pin, bool state);

  protected:
    int pulseToAngle(int p);
    int angleToPulse(int a);

  public:
    enum PIN {//used to convert arduino digital pins to avr just for convenience
      avrPD0 = 0,
      avrPD1 = 1,
      avrPD2 = 2,
      avrPD3 = 3,
      avrPD4 = 4,
      avrPD5 = 5,
      avrPD6 = 6,
      avrPD7 = 7,
      avrPB0 = 8,
      avrPB1 = 9,
      avrPB2 = 10,
      avrPB3 = 11,
      avrPB4 = 12,
      avrPB5 = 13
    };

    Servo();
    ~Servo();
    static void ISRpulse();//manage pulses for servos in ISR
    static void ISRreset();//prepare servos for next cycle
    
    bool activate(int p);
    void deactivate();
    bool isActive();
    
    void setMinAngle(int a);
    void setMaxAngle(int a);
    void setMinPulse(int p);
    void setMaxPulse(int p);
    void setUsableMinAngle(int a);
    void setUsableMaxAngle(int a);
    void setUsableMinPulse(int p);
    void setUsableMaxPulse(int p);
    void setAngle(int a);
    void setPulse(int p);

    static int getServoNumber();
    int getServoIndex();
    int getPin();
    int getMinAngle();
    int getMaxAngle();
    int getMinPulse();
    int getMaxPulse();
    int getUsableMinAngle();
    int getUsableMaxAngle();
    int getUsableMinPulse();
    int getUsableMaxPulse();
    int getAngle();
    int getPulse();  
};

#endif