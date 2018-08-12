#include "servo.h"

//private
Servo *Servo::servos[servoLimit] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
uint8_t Servo::servoNumber = 0;
volatile uint8_t Servo::currentServoA = 0;
volatile uint8_t Servo::currentServoB = 0;

void Servo::computeLinearConstants()
{
  if (maxAngle == minAngle)
    linearA = ((double)maxPulse - (double)minPulse) / 0.0000001;
  else
    linearA = ((double)maxPulse - (double)minPulse) / ((double)maxAngle - (double)minAngle);
  linearB = (double)minPulse - linearA * (double)minAngle;
}

uint16_t Servo::pulseToCounts(uint16_t p)
{
  return (double)F_CPU / (double)prescaler * (double)p / (double)1000000;
}

void Servo::setPinState(bool state)
{
  if (state)
    *port |= 1 << *pin;
  else
    *port &= ~(1 << *pin);
}

int16_t Servo::pulseToAngle(uint16_t p)
{
  return ((double)p - linearB) / linearA;
}

uint16_t Servo::angleToPulse(int16_t a)
{
  return (double)a * linearA + linearB;
}

//public
Servo::Servo()
{
  computeLinearConstants();
  counts = pulseToCounts(pulse);
}

Servo::~Servo()
{
  deactivate();
}

void Servo::begin()
{
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 = 0;

  TIMSK1 |= (1 << OCIE1A) | (1 << OCIE1B); //interrupt on compare w/ OCR1A and OCR1B
  TCCR1B |= (1 << CS11);                   // F_CPU/8 prescaler
  sei();
}

void Servo::ISRpulseA()
{
  static bool flag = 0;

  if (currentServoA >= servoLimitHalf)
  {
    OCR1A = 0;
    currentServoA = 0;
    return;
  }
  else if (servos[currentServoA] == nullptr)
  {
    OCR1A = 0;
    currentServoA = 0;
    return;
  }

  flag = !flag;

  if (flag)
  {
    servos[currentServoA]->setPinState(1);
    OCR1A = TCNT1 + servos[currentServoA]->counts;
  }
  else
  {
    servos[currentServoA]->setPinState(0);
    currentServoA++;
    OCR1A = TCNT1 + pulseToCounts(500);
  }
}

void Servo::ISRpulseB()
{
  static bool flag = 0;

  if (currentServoB >= servoLimitHalf)
  {
    OCR1B = 0;
    currentServoB = 0;
    return;
  }
  else if (servos[currentServoB + servoLimitHalf] == nullptr)
  {
    OCR1B = 0;
    currentServoB = 0;
    return;
  }

  flag = !flag;

  if (flag)
  {
    servos[currentServoB + servoLimitHalf]->setPinState(1);
    OCR1B = TCNT1 + servos[currentServoB + servoLimitHalf]->counts;
  }
  else
  {
    servos[currentServoB + servoLimitHalf]->setPinState(0);
    currentServoB++;
    OCR1B = TCNT1 + pulseToCounts(500);
  }
}

ISR(TIMER1_COMPA_vect)
{
  Servo::ISRpulseA();
}

ISR(TIMER1_COMPB_vect)
{
  Servo::ISRpulseB();
}

//setters
void Servo::activate(int8_t &_port, int8_t &_pin, int16_t a)
{
  if (isActive() || servoNumber >= servoLimit) //check if selected pin is not in available pins and if yes then do not activate servo
  {
    return;
  }
  for (int i = 0; i < servoNumber; i++) //check if other servo uses selected pin and if yes then do not activate servo
  {
    if (&_pin == servos[i]->pin && &_port == servos[i]->port)
      return;
  }
  setAngle(a);
  pin = &_pin;
  port = &_port;
  index = servoNumber;
  servos[index] = this; //add servo to the end of array of active servos
  servoNumber++;
}

bool Servo::isActive()
{
  if (pin != nullptr)
    return 1;
  return 0;
}

void Servo::deactivate()
{
  if (!isActive())
    return;

  for (int i = index; i < servoNumber - 1; i++) //decrease index of every servo by 1 starting from selected servo+1 to the last not nullpointer element
  {                                             //the index+1 element will override selected servo and remove it from the array
    servos[i] = servos[i + 1];
  }
  servos[servoNumber - 1] = nullptr;
  pin = nullptr;
  port = nullptr;
  index = -1;
  servoNumber--;
}

void Servo::setMinAngle(int16_t a)
{
  minAngle = a;
  computeLinearConstants();
}

void Servo::setMaxAngle(int16_t a)
{
  maxAngle = a;
  computeLinearConstants();
}

void Servo::setMinPulse(uint16_t p)
{
  minPulse = p;
  computeLinearConstants();
}

void Servo::setMaxPulse(uint16_t p)
{
  maxPulse = p;
  computeLinearConstants();
}

void Servo::setUsableMinAngle(int16_t a)
{
  usableMinAngle = a;
}

void Servo::setUsableMaxAngle(int16_t a)
{
  usableMaxAngle = a;
}

void Servo::setAngle(int16_t a)
{
  if (a > maxAngle || a < minAngle || a > usableMaxAngle || a < usableMinAngle)
    return;

  angle = a;
  pulse = angleToPulse(angle);
  counts = pulseToCounts(pulse);
}

void Servo::setPulse(uint16_t p)
{
  int16_t a = pulseToAngle(p);
  if (a > maxAngle || a < minAngle || a > usableMaxAngle || a < usableMinAngle)
    return;

  angle = a;
  pulse = p;
  counts = pulseToCounts(pulse);
}

//getters
uint8_t Servo::getServoNumber() { return servoNumber; }

int16_t Servo::getMinAngle() { return minAngle; }

int16_t Servo::getMaxAngle() { return maxAngle; }

uint16_t Servo::getMinPulse() { return minPulse; }

uint16_t Servo::getMaxPulse() { return maxPulse; }

int16_t Servo::getUsableMinAngle() { return usableMinAngle; }

int16_t Servo::getUsableMaxAngle() { return usableMaxAngle; }

int16_t Servo::getAngle() { return angle; }

uint16_t Servo::getPulse() { return pulse; }