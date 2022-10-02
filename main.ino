/*
  Doorbell Control Code (DCC)
  Written by Noah West 620683
  September/October UTAS 2022
  Group DPS
  License: MIT
*/

// pin definition
#define vIn A0
#define LED 5
#define BUTTON 4

// rolling average configuration
double fastN = 300.0;
double slowN = 1000.0;
double fastMovingAvg = 0;
double slowMovingAvg = 0;
bool hasDoped = false;

// general configuration
double sensitivity = 0.1;

// alert triggered by button
volatile bool isOn = false;

// cooldown period of light censor
unsigned long startTime = 0;
unsigned long currentTime = 501;
unsigned long coolDown = 500;

// initialises pins and other board functionality
void setup()
{
  Serial.begin(9600); // initialise serial bus

  // initialise pins for circuit control
  pinMode(vIn, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);

  /*
    here the the Pin Change Interrupt Control Registry (PCICR) is set such that port d is enabled

    as to make sure only digital pin 4 (D4) triggers the interrupt, a mask is applied for Pin Change Interrupt 20 (PCINT20 aka D4)
    via setting the Pin Change Mask 2 register bit 5 to 1
  */
  cli();
  PCICR |= 0b00000100;
  PCMSK2 |= 0b00010000;
  sei();
}

// function is called when vector is triggered (button push)
ISR(PCINT2_vect)
{
  isOn = true;
}

/*
  calculates the current voltage drop across the LDR
  @returns the voltage drop across the LDR
*/
double getVoltage()
{
  return 5.0 - (((double)(analogRead(vIn)) / 1023) * 5.0);
}

/*
  algorithm that calculates the moving average - credit: https://stackoverflow.com/a/16757630
  @param average current double average
  @param N number of samples to be included in the moving average
  @returns the new moving average value
*/
double movingAverage(double average, double N)
{
  average -= average / N;
  average += getVoltage() / N;
  return average;
}

/*
  function that blinks light every 300ms over 10s by use of a time delta
*/
void alert()
{
  unsigned long alertStartTime = millis();
  unsigned long alertCurrentTime = millis();
  while ((alertCurrentTime - alertStartTime) < 10000)
  {
    digitalWrite(LED, HIGH);
    delay(300);
    digitalWrite(LED, LOW);
    delay(300);
    alertCurrentTime = millis();
  }
  isOn = false;
}

// the loop function runs over and over again forever
void loop()
{
  // if ISR has triggered due to button press then alert
  if (isOn) alert();
  
  // if statement dopes the moving averages to current light level such that functionality is ensured as soon as the device is powered on
  if (!hasDoped)
  {
    hasDoped = true;
    slowMovingAvg = getVoltage();
    fastMovingAvg = getVoltage();
  }
  
  // updates moving averages with movingAverage algorithm
  fastMovingAvg = movingAverage(fastMovingAvg, fastN);
  slowMovingAvg = movingAverage(slowMovingAvg, slowN);
  // TODO: remove code below
  // TODO: make graphs
  Serial.println((slowMovingAvg-fastMovingAvg)/slowMovingAvg);

  /*
    if the fast moving average deviates more than sensitivity % from the slow moving average, alert is called
    
    there is a short cooldown period between each time the sensor is checked so that the moving average algorithm
    has time to return to the environments average if the obstruction is removed
  */  
  if ((currentTime - startTime) > coolDown)
  {
    // by checking if the difference % is < -sensitivity, we can only check for dips in brightness, not increases in brightness
    if (-sensitivity > (slowMovingAvg-fastMovingAvg)/slowMovingAvg)
    {
      alert();
      startTime = millis();
    }
  }
  currentTime = millis();
}