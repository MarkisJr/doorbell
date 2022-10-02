// pin definition
#define vIn A0
#define LED 5
#define BUTTON 4

// data typed required for rolling average
double N = 300.0;
double avg = 2.5;     // doping value to start average

// profile values
double profile1Val = 2.2;

// alert trigger
volatile int isOn = 0;

void setup()
{
  Serial.begin(9600); // initialise serial bus

  // initialise pins for circuit control
  pinMode(vIn, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);

  // interrupt code
  cli();
  PCICR |= 0b00000100;
  PCMSK2 |= 0b00010000;
  sei();
}

// converts analogue reading to double precision voltage between 0-5V
double getVoltage()
{
  return 5.0 - (((double)(analogRead(vIn)) / 1023) * 5.0);
}

double movingAverage(double average)
{
  average -= average / N;
  average += getVoltage() / N;
  return average;
}

void alert()
{
  unsigned long startTime = millis();
  unsigned long currentTime = millis();
  while ((currentTime - startTime) < 10000)
  {
    digitalWrite(LED, HIGH);
    Serial.println("LED is ON");
    delay(300);
    digitalWrite(LED, LOW);
    Serial.println("LED is OFF");
    delay(300);
    currentTime = millis();
  }
  isOn = 0;
}

// the loop function runs over and over again forever
void loop()
{
  avg = movingAverage(avg);
  Serial.println(avg);

  if (avg<0.5)
  {
    alert();
  }
}

void test()
{
  isOn++;
}

ISR(PCINT2_vect)
{
  if (isOn == 0)
  {
    isOn++;
    alert();
  }
}