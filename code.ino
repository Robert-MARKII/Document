#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

  
#include <Servo.h>
#include "EMGFilters.h"
#define TIMING_DEBUG 1
#define SensorInputPin A0 

Servo servo_pin_9;
Servo servo_pin_10;
EMGFilters myFilter;
int sampleRate = SAMPLE_FREQ_1000HZ;
int humFreq = NOTCH_FREQ_50HZ;
int Value;
int DataAfterFilter;
int envlope;
int data[10]={1000,1000,1000,1000,1000,1000,
              1000,1000,1000,1000};
int data_10=1000;
int turnNum=1;
static int Threshold = 0;

unsigned long timeStamp;
unsigned long timeBudget;
bool flag=false;

void setup() {
    myFilter.init(sampleRate, humFreq, true, true, true);
    Serial.begin(115200);
    timeBudget = 1e6 / sampleRate;
    servo_pin_9.attach(9);
    servo_pin_10.attach(10);
    servo_pin_9.write(90);
    servo_pin_10.write(90); 
}

void collectData()
{
  //timeStamp = micros();
  Value = analogRead(SensorInputPin);
  DataAfterFilter = myFilter.update(Value);
  envlope = sq(DataAfterFilter);
  envlope = (envlope > Threshold) ? envlope : 0;
  envlope=abs(envlope);
  //timeStamp = micros() - timeStamp;
  if (TIMING_DEBUG) {
      Serial.print("Squared Data: ");
      Serial.println(envlope);
  }
  delay(5);
  }

void processData()
{
  int x;
  long ave_10=0;
  for(x=0;x<9;x++)
  {
    data[x]=data[x+1];
    }
    data[x]=abs(envlope);
  
  for(x=0;x<10;x++)
  {
    ave_10=ave_10+data[x];
    }
    data_10=int(ave_10/10);
  }

void judgeData()
{
  if(data_10>=30)
  {
    turnNum=1;
    }
  else
  {
    turnNum=2;
    }
  }

void turnServe()
{
  Serial.println(turnNum);
    switch(turnNum)
    {
      case 1:servo_pin_9.write(90);servo_pin_10.write(90);delay_func();
             //servo_pin_9.write(120);servo_pin_10.write(60);delay_func();
             break;
      case 2:servo_pin_9.write(10);servo_pin_10.write(170);delay_func();break;
      default:break;
      }
  }

void delay_func()
{
  int x=0;
  for(x=0;x<100;x++)
  {
    delayMicroseconds(500);
    collectData();
    processData();
    judgeData();
    }
  }
void loop() {
    collectData();
    processData();
    judgeData();
    turnServe();
}
