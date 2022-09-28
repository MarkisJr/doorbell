#include <iostream>

using namespace std;

double N = 500.0;

double getVoltage()
{
    return (double)(rand()%(3));
}

double rollingAverage (double average)
{
  average -= average/N;
  average += getVoltage()/N;
  return average;
}

int main()
{
    double a = 2.5;
    srand(time(NULL));
    for(long i=0; i<1000000; i++)
    {
        a = rollingAverage(a);
        cout << a << endl;
    }
    
}