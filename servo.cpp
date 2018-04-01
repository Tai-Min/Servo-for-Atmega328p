#include "servo.h"

//private
const int Servo::prescaler = 8;
Servo *Servo::servos[12] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
int Servo::servoNumber = 0;
volatile int Servo::currentServoA = 0;
volatile int Servo::currentServoB = 0;

void Servo::computeLinearConstants()
{
  linearA = ((double)maxPulse - (double)minPulse) / ((double)maxAngle - (double)minAngle);
  linearB = (double)minPulse - linearA * (double)minAngle;
}

unsigned int Servo::pulseToCounts(int p)
{
  return (double)F_CPU / (double)prescaler * (double)p / (double)1000000;
}

void Servo::setPinState(int pin, bool state)
{
  switch (pin)
  {
  case _PD0:
    DDRD |= (1 << DDD0);
    if (state)
      PORTD |= (1 << PD0);
    else
      PORTD &= ~(1 << PD0);
    break;
  case _PD1:
    DDRD |= (1 << DDD1);
    if (state)
      PORTD |= (1 << PD1);
    else
      PORTD &= ~(1 << PD1);
    break;
  case _PD2:
    DDRD |= (1 << DDD2);
    if (state)
      PORTD |= (1 << PD2);
    else
      PORTD &= ~(1 << PD2);
    break;
  case _PD3:
    DDRD |= (1 << DDD3);
    if (state)
      PORTD |= (1 << PD3);
    else
      PORTD &= ~(1 << PD3);
    break;
  case _PD4:
    DDRD |= (1 << DDD4);
    if (state)
      PORTD |= (1 << PD4);
    else
      PORTD &= ~(1 << PD4);
    break;
  case _PD5:
    DDRD |= (1 << DDD5);
    if (state)
      PORTD |= (1 << PD5);
    else
      PORTD &= ~(1 << PD5);
    break;
  case _PD6:
    DDRD |= (1 << DDD6);
    if (state)
      PORTD |= (1 << PD6);
    else
      PORTD &= ~(1 << PD6);
    break;
  case _PD7:
    DDRD |= (1 << DDD7);
    if (state)
      PORTD |= (1 << PD7);
    else
      PORTD &= ~(1 << PD7);
    break;
  case _PB0:
    DDRB |= (1 << DDB0);
    if (state)
      PORTB |= (1 << PB0);
    else
      PORTB &= ~(1 << PB0);
    break;
  case _PB1:
    DDRB |= (1 << DDB1);
    if (state)
      PORTB |= (1 << PB1);
    else
      PORTB &= ~(1 << PB1);
    break;
  case _PB2:
    DDRB |= (1 << DDB2);
    if (state)
      PORTB |= (1 << PB2);
    else
      PORTB &= ~(1 << PB2);
    break;
  case _PB3:
    DDRB |= (1 << DDB3);
    if (state)
      PORTB |= (1 << PB3);
    else
      PORTB &= ~(1 << PB3);
    break;
  case _PB4:
    DDRB |= (1 << DDB4);
    if (state)
      PORTB |= (1 << PB4);
    else
      PORTB &= ~(1 << PB4);
    break;
  case _PB5:
    DDRB |= (1 << DDB5);
    if (state)
      PORTB |= (1 << PB5);
    else
      PORTB &= ~(1 << PB5);
    break;
  }
}

//protected
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

  TIMSK1 |= (1 << OCIE1A) | (1 << OCIE1B) /*| (1 << TOIE0)*/; //interrupt on compare w/ OCR1A and OCR1B and overflow
  TCCR1B |= (1 << CS11);                                      // F_CPU/8 prescaler
  sei();
}

void Servo::ISRpulseA()
{
  static bool flag = 0;

  if (currentServoA >= 6)
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

  if (currentServoB >= 6)
  {
    OCR1B = 0;
    currentServoB = 0;
    return;
  }
  else if (servos[currentServoB + 6] == nullptr)
  {
    OCR1B = 0;
    currentServoB = 0;
    return;
  }

  flag = !flag;

  if (flag)
  {
    setPinState(servos[currentServoB + 6]->pin, 1);
    OCR1B = TCNT1 + servos[currentServoB + 6]->counts;
  }
  else
  {
    setPinState(servos[currentServoB + 6]->pin, 0);
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
bool Servo::activate(int p)
{
  if (isActive())
    return 0;
  if (servoNumber >= 12 || p > 13 || p < 0) //check if selected pin is not in available pins and if yes then do not activate servo
  {
    return 0;
  }
  for (int i = 0; i < servoNumber; i++) //check if other servo uses selected pin and if yes then do not activate servo
  {
    if (p == servos[i]->pin)
      return 0;
  }
  pin = p;
  index = servoNumber;
  servos[index] = this; //add servo to the end of array of active servos
  servoNumber++;
  return 1;
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

bool Servo::isActive()
{
  if (pin != -1)
    return 1;
  return 0;
}

void Servo::setMinAngle(int a)
{
  if (a >= maxAngle)
    a = maxAngle - 1;
  else if (a < 0)
    a = 0;
  minAngle = a;
  computeLinearConstants();
  setUsableMinAngle(getUsableMinAngle()); //refresh usable angles just in case they are out of min-max angle boundaries
  setUsableMaxAngle(getUsableMaxAngle());
}

void Servo::setMaxAngle(int a)
{
  if (a <= minAngle)
    a = minAngle + 1;
  maxAngle = a;
  computeLinearConstants();
  setUsableMaxAngle(getUsableMaxAngle()); //refresh usable angles just in case they are out of min-max angle boundaries
  setUsableMinAngle(getUsableMinAngle());
}

void Servo::setMinPulse(int p)
{
  if (p > maxPulse)
    p = maxPulse - 1;
  else if (p < 0)
    p = 0;
  minPulse = p;
  computeLinearConstants();
}

void Servo::setMaxPulse(int p)
{
  if (p < minPulse)
    p = minPulse + 1;
  maxPulse = p;
  computeLinearConstants();
}

void Servo::setUsableMinAngle(int a)
{
  if (a > usableMaxAngle)
    a = usableMaxAngle - 1;
  else if (a < minAngle)
    a = minAngle;
  usableMinAngle = a;
  setAngle(getAngle()); //refresh current angle in case it is out of usable min-max angle boundaries
}

void Servo::setUsableMaxAngle(int a)
{
  if (a < usableMinAngle)
    a = usableMinAngle + 1;
  else if (a > maxAngle)
    a = maxAngle;
  usableMaxAngle = a;
  setAngle(getAngle()); //refresh current angle in case it is out of usable min-max angle boundaries
}

void Servo::setAngle(int a)
{
  if (a > usableMaxAngle)
    a = usableMaxAngle;
  else if (a < usableMinAngle)
    a = usableMinAngle;

  angle = a;
  pulse = angleToPulse(a);
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
