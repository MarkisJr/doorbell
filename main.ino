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
int isOn = 0;

void setup()
{
  Serial.begin(9600); // initialise serial bus

  // interrupt code
  cli();
  PCICR |= 0b00000100;
  PCMSK2 |= 0b00010000;
  sei();
  
  // initialise pins for circuit control
  pinMode(vIn, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);
}

// converts analogue reading to double precision voltage between 0-5V
double getVoltage()
{
  return ((double)(analogRead(vIn)) / 1023) * 5.0;
}

double movingAverage(double average)
{
  average -= average / N;
  average += getVoltage() / N;
  return average;
}

void profile1()   // dark
{
  
}

void profile2()   // morning/afternoon
{
}

void profile3()   // midday
{
}

void alert()
{
  if (isOn == 0)
  {
    isOn = 1;
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
}

// the loop function runs over and over again forever
void loop()
{
  avg = movingAverage(avg);

  if (avg<1.2)
  {
    alert();
  }
}

ISR(PCINT2_vect)
{
  alert();
}