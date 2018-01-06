#include "servo.h"

//private
const int Servo::prescaler = 8;
Servo *Servo::servos[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
int Servo::servoNumber = 0;
volatile int Servo::currentServo = 0;

void Servo::computeLinearConstants()
{
  linearA = ((double)maxPulse - (double)minPulse) / ((double)maxAngle - (double)minAngle);
  linearB = (double)minPulse - linearA * (double)minAngle;
}

unsigned int Servo::pulseToCounts(int p)
{
  return (double)F_CPU / (double)prescaler * (double)p / (double)1000000 /*- (double)1*/;
}

void Servo::init()
{
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 = 0;

  OCR1A = pulseToCounts(20000);            //20ms / 50Hz
  TCCR1B |= (1 << WGM12);                  //CTC on OCR1A
  TIMSK1 |= (1 << OCIE1A) | (1 << OCIE1B); //interrupt on compare w/ OCR1A and OCR1B
  TCCR1B |= (1 << CS11);                   // F_CPU/8 prescaler
  sei();
}

void Servo::setPinState(int pin, bool state)
{
  switch (pin)
  {
  case avrPD2:
    DDRD |= (1 << DDD2);
    if (state)
      PORTD |= (1 << PD2);
    else
      PORTD &= ~(1 << PD2);
    break;
  case avrPD3:
    DDRD |= (1 << DDD3);
    if (state)
      PORTD |= (1 << PD3);
    else
      PORTD &= ~(1 << PD3);
    break;
  case avrPD4:
    DDRD |= (1 << DDD4);
    if (state)
      PORTD |= (1 << PD4);
    else
      PORTD &= ~(1 << PD4);
    break;
  case avrPD5:
    DDRD |= (1 << DDD5);
    if (state)
      PORTD |= (1 << PD5);
    else
      PORTD &= ~(1 << PD5);
    break;
  case avrPD6:
    DDRD |= (1 << DDD6);
    if (state)
      PORTD |= (1 << PD6);
    else
      PORTD &= ~(1 << PD6);
    break;
  case avrPD7:
    DDRD |= (1 << DDD7);
    if (state)
      PORTD |= (1 << PD7);
    else
      PORTD &= ~(1 << PD7);
    break;
  case avrPB0:
    DDRB |= (1 << DDB0);
    if (state)
      PORTB |= (1 << PB0);
    else
      PORTB &= ~(1 << PB0);
    break;
  case avrPB1:
    DDRB |= (1 << DDB1);
    if (state)
      PORTB |= (1 << PB1);
    else
      PORTB &= ~(1 << PB1);
    break;
  case avrPB2:
    DDRB |= (1 << DDB2);
    if (state)
      PORTB |= (1 << PB2);
    else
      PORTB &= ~(1 << PB2);
    break;
  case avrPB3:
    DDRB |= (1 << DDB3);
    if (state)
      PORTB |= (1 << PB3);
    else
      PORTB &= ~(1 << PB3);
    break;
  case avrPB4:
    DDRB |= (1 << DDB4);
    if (state)
      PORTB |= (1 << PB4);
    else
      PORTB &= ~(1 << PB4);
    break;
  case avrPB5:
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
  init();
  computeLinearConstants();
  counts = pulseToCounts(pulse);
}

Servo::~Servo()
{
  deactivate();
}

void Servo::ISRpulse()
{
  static bool flag = 0;

  if (currentServo >= 6)
  {
    OCR1B = 0;
    flag = 0;
    return;
  }

  if(servos[currentServo] == nullptr)
  {
    currentServo++;
    OCR1B = TCNT1 + pulseToCounts(200);
    return;
  }

  flag = !flag;

  if (flag)
  {
    setPinState(servos[currentServo]->pin, 1);
    OCR1B = TCNT1 + servos[currentServo]->counts;
  }
  else
  {
    setPinState(servos[currentServo]->pin, 0);
    currentServo++;
    OCR1B = TCNT1 + pulseToCounts(200);
  }
}

void Servo::ISRreset()
{
  currentServo = 0;
}

ISR(TIMER1_COMPA_vect)
{
  Servo::ISRreset();
}

ISR(TIMER1_COMPB_vect)
{
  Servo::ISRpulse();
}

//setters
bool Servo::activate(int p)
{
  if (servoNumber >= 6 || p > 13 || p < 2)//check if selected pin is not in available pins and if yes then do not activate servo
  {
    return 0;
  }
  for (int i = 0; i < servoNumber; i++)//check if other servo uses selected pin and if yes then do not activate servo
  {
    if (p == servos[i]->pin)
      return 0;
  }
  for(int i = 0; i < 6; i++)//check if slot is empty and if so set index to it's position in array
  {
    if(servos[i] == nullptr)
    {
      index = i;
      break;
    }
      
  }
  pin = p;
  //index = servoNumber;
  servoNumber++;
  servos[index] = this;//add servo to array of active servos
  return 1;
}

void Servo::deactivate()
{
  /*for (int i = index; i < servoNumber - 1; i++)//remove servo from array of active servos
  {
    servos[i] = servos[i + 1];
  }*/
  //servos[servoNumber - 1] = nullptr;
  servo[index] = nullptr;
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
  if (a > maxAngle)
    a = maxAngle;
  else if(a < 0)
    a = 0;
  minAngle = a;
  computeLinearConstants();
  setUsableMinAngle(getUsableMinAngle()); //refresh usable angles just in case they are out of min-max angle boundaries
  setUsableMaxAngle(getUsableMaxAngle());
  minPulse = angleToPulse(a);
}

void Servo::setMaxAngle(int a)
{
  if (a < minAngle)
    a = minAngle;
  maxAngle = a;
  computeLinearConstants();
  setUsableMaxAngle(getUsableMaxAngle()); //refresh usable angles just in case they are out of min-max angle boundaries
  setUsableMinAngle(getUsableMinAngle());
  maxPulse = angleToPulse(a);
}

void Servo::setMinPulse(int p)
{
  if (p > maxPulse)
    p = maxPulse;
  else if (p < 0)
    p = 0;
  minPulse = p;
  computeLinearConstants();
  setUsableMinPulse(getUsableMinPulse()); //refresh usable pulses just in case they are out of min-max pulse boundaries
  setUsableMaxPulse(getUsableMaxPulse());
  minAngle = pulseToAngle(p);
}

void Servo::setMaxPulse(int p)
{
  if (p < minPulse)
    p = minPulse;
  maxPulse = p;
  computeLinearConstants();
  setUsableMaxPulse(getUsableMaxPulse()); //refresh usable pulses just in case they are out of min-max pulse boundaries
  setUsableMinPulse(getUsableMinPulse());
  maxAngle = pulseToAngle(p);
}

void Servo::setUsableMinAngle(int a)
{
  if (a > usableMaxAngle)
    a = usableMaxAngle;
  else if (a < minAngle)
    a = minAngle;
  usableMinAngle = a;
  usableMinPulse = angleToPulse(a);
  setAngle(getAngle());//refresh current angle in case it is out of usable min-max angle boundaries
}

void Servo::setUsableMaxAngle(int a)
{
  if (a < usableMinAngle)
    a = usableMinAngle;
  else if (a > maxAngle)
    a = maxAngle;
  usableMaxAngle = a;
  usableMaxPulse = angleToPulse(a);
  setAngle(getAngle());//refresh current angle in case it is out of usable min-max angle boundaries
}

void Servo::setUsableMinPulse(int p)
{
  if (p > usableMaxPulse)
    p = usableMaxPulse;
  else if (p < minPulse)
    p = minPulse;
  usableMinPulse = p;
  usableMinAngle = pulseToAngle(p);
  setPulse(getPulse());//refresh current pulse in case it is out of usable min-max pulse boundaries
}

void Servo::setUsableMaxPulse(int p)
{
  if (p < usableMinPulse)
    p = usableMinPulse;
  else if (p > maxPulse)
    p = maxPulse;
  usableMaxPulse = p;
  usableMaxAngle = pulseToAngle(p);
  setPulse(getPulse());//refresh current pulse in case it is out of usable min-max pulse boundaries
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

void Servo::setPulse(int p)
{
  if (p > usableMaxPulse)
    p = usableMaxPulse;
  else if (p < usableMinPulse)
    p = usableMinPulse;

  pulse = p;
  angle = pulseToAngle(p);
  counts = pulseToCounts(p);
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

int Servo::getUsableMinPulse() { return usableMinPulse; }

int Servo::getUsableMaxPulse() { return usableMaxPulse; }

int Servo::getAngle() { return angle; }

int Servo::getPulse() { return pulse; }