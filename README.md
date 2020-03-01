# Servo-for-Atmega328p

Simple library to control up to 12 servos on Atmega328p microcontroller. </br> </br>

This library uses timer 1.

## Usage

#### Configure timer1 for Servo objects:
```cpp
static void begin();
```

***
#### ISR functions:
```cpp
static void ISRpulseA();
static void ISRpulseB();
```
ISRpulseA() is in TIMER1_COMPA_vect routine<br/>
ISRpulseB() is in TIMER1_COMPB_vect routine

***
#### Activate servo:
```cpp
void activate(int8_t & port, int8_t & pin, int16_t angle);
```
+ **port:** i.e PORTB
+ **pin** i.e PB2
+ **angle:** Angle the servo will go to after activation

Servo will not activate if there are 12 active servos or if other servo uses given port and pin combination.
***
#### Check whether servo is active:
```cpp
bool isActive();
```

***
#### Deactivate servo:
```cpp
void deactivate();
```

***
#### Set hardware limits of the servo:
```cpp
void setMinAngle(int16_t a);
void setMaxAngle(int16_t a);
void setMinPulse(uint16_t p);
void setMaxPulse(uint16_t p);
```

***
#### Set imposed limits of the servo:
```cpp
void setUsableMinAngle(int16_t a);
void setUsableMaxAngle(int16_t a);
```

***
#### Set current angle/pulse:
```cpp
void setAngle(int16_t a);
void setPulse(uint16_t p);
```

***
#### Get number of active servos:
```cpp
static uint8_t getServoNumber();
```
+ **returns:** Number of servos that are active
***
#### Get servo's parameters:
```cpp
int16_t getMinAngle();
int16_t getMaxAngle();
uint16_t getMinPulse();
uint16_t getMaxPulse();
int16_t getUsableMinAngle();
int16_t getUsableMaxAngle();
int16_t getAngle();
uint16_t getPulse();
```

***
