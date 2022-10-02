/*
  Doorbell Control Code (DCC)
  Written by Noah West 620683
  September/October UTAS 2022
  Group xxxxx
  License: MIT
*/

// pin definition
#define vIn A0
#define LED 5
#define BUTTON 4

// data typed required for rolling average
double fastN = 300.0;
double slowN = 2000.0;
double fastMovingAvg = 2.5; // dope the starting value for the algorithm
double slowMovingAvg = 0;

// alert trigger
volatile bool isOn = false;

bool hasDoped = false;

// initialises pins and other board functionality
void setup()
{
  Serial.begin(9600); // initialise serial bus

  // initialise pins for circuit control
  pinMode(vIn, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);

    
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

double movingAverage(double average, double N)
{
  average -= average / N;
  average += getVoltage() / N;
  return average;
}

// function that blinks light every 300ms over 10s by use of a time delta
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
}

// the loop function runs over and over again forever
void loop()
{
  // if statement dopes the slow moving average to current light level such that functionality is ensured when device is powered on
  if (!hasDoped)
  {
    hasDoped = false;
    slowMovingAvg = getVoltage();
  }
  
  fastMovingAvg = movingAverage(fastMovingAvg, fastN);
  slowMovingAvg = movingAverage(slowMovingAvg, slowN);
  Serial.println(slowMovingAvg);
}

ISR(PCINT2_vect)
{
  if (isOn == 0)
  {
    isOn++;
    alert();
  }
}