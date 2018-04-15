#include "servo.h"

//private
Servo *Servo::servos[servoLimit] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
int Servo::servoNumber = 0;
volatile int Servo::currentServoA = 0;
volatile int Servo::currentServoB = 0;

void Servo::computeLinearConstants()
{
  if (maxAngle == minAngle)
    linearA = ((double)maxPulse - (double)minPulse) / 0.0000001;
  else
    linearA = ((double)maxPulse - (double)minPulse) / ((double)maxAngle - (double)minAngle);
  linearB = (double)minPulse - linearA * (double)minAngle;
}

unsigned int Servo::pulseToCounts(int p)
{
  return (double)F_CPU / (double)prescaler * (double)p / (double)1000000;
}

void Servo::setPinState(int p, bool state)
{
  if (p < registerSize)
  {
    DDRD |= (1 << p);
    if (state)
      PORTD |= (1 << p);
    else
      PORTD &= ~(1 << p);
  }
  else
  {
    p -= registerSize;
    DDRB |= (1 << p);
    if (state)
      PORTB |= (1 << p);
    else
      PORTB &= ~(1 << p);
  }
}

int Servo::pulseToAngle(int p)
{
  return ((double)p - linearB) / linearA;
}

int Servo::angleToPulse(int a)
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

void Servo::init()
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
    setPinState(servos[currentServoA]->pin, 1);
    OCR1A = TCNT1 + servos[currentServoA]->counts;
  }
  else
  {
    setPinState(servos[currentServoA]->pin, 0);
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
    setPinState(servos[currentServoB + servoLimitHalf]->pin, 1);
    OCR1B = TCNT1 + servos[currentServoB + servoLimitHalf]->counts;
  }
  else
  {
    setPinState(servos[currentServoB + servoLimitHalf]->pin, 0);
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
bool Servo::activate(int p, int a)
{
  if (isActive() || servoNumber >= servoLimit || p > maxPin || p < minPin) //check if selected pin is not in available pins and if yes then do not activate servo
  {
    return 0;
  }
  for (int i = 0; i < servoNumber; i++) //check if other servo uses selected pin and if yes then do not activate servo
  {
    if (p == servos[i]->pin)
      return 0;
  }
  setAngle(a);
  pin = p;
  index = servoNumber;
  servos[index] = this; //add servo to the end of array of active servos
  servoNumber++;
  return 1;
}

bool Servo::isActive()
{
  if (pin != -1)
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
  pin = -1;
  index = -1;
  servoNumber--;
}

void Servo::setMinAngle(int a)
{
  minAngle = a;
  computeLinearConstants();
}

void Servo::setMaxAngle(int a)
{
  maxAngle = a;
  computeLinearConstants();
}

void Servo::setMinPulse(int p)
{
  minPulse = p;
  computeLinearConstants();
}

void Servo::setMaxPulse(int p)
{
  maxPulse = p;
  computeLinearConstants();
}

void Servo::setUsableMinAngle(int a)
{
  usableMinAngle = a;
}

void Servo::setUsableMaxAngle(int a)
{
  usableMaxAngle = a;
}

void Servo::setAngle(int a)
{
  if (a > maxAngle || a < minAngle || a > usableMaxAngle || a < usableMinAngle)
    return;

  angle = a;
  pulse = angleToPulse(angle);
  counts = pulseToCounts(pulse);
}

//getters
int Servo::getServoNumber() { return servoNumber; }

int Servo::getPin() { return pin; }

int Servo::getMinAngle() { return minAngle; }

int Servo::getMaxAngle() { return maxAngle; }

int Servo::getMinPulse() { return minPulse; }

int Servo::getMaxPulse() { return maxPulse; }

int Servo::getUsableMinAngle() { return usableMinAngle; }

int Servo::getUsableMaxAngle() { return usableMaxAngle; }

int Servo::getAngle() { return angle; }

int Servo::getPulse() { return pulse; }