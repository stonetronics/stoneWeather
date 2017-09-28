#include "HP03S.h"

#include <Arduino.h>
#include <Wire.h>

#define EEADDR 0x50
#define ADADDR 0x77
#define EESTART 0x10
#define PRESSURE 0xF0
#define TEMPERATURE 0xE8
#define MODE_AIR 0
#define MODE_TEMP 1

HP03S::HP03S(int xclr, int mclk)
{
    _xclr = xclr;
    _mclk = mclk;
}


void HP03S::begin()
{
    // init pins
    pinMode(_xclr, OUTPUT);
    pinMode(_mclk, OUTPUT);
    analogWriteFreq(32768); // set freq of pwm to 32.768 kHz
    analogWrite(_mclk, 127); //clock out on mclk
    digitalWrite(_xclr, HIGH);
    _getParameters();
}


void HP03S::printParameters()
{
  int x = 0;

  for(int c = 0; c < 7; c ++)  {
    Serial.println((_readParams[c + x] << 8) + _readParams[c + 1 + x]);
    x = c + 1;
  }
  for(int c = 0; c < 4; c ++)  {
    Serial.println(_readParams[c + 14]);
  }
}

float HP03S::getPressure()
{
    return _pressure;
}


float HP03S::getPressureAtSeaLevel(double height)
{
    double gardient = 0.0065;
    double tempAtSea = 15.0;
    double p = getPressure();
    tempAtSea += 273.15;  // °C to K
    return p / pow((1 - gardient * height / tempAtSea), (0.03416 / gardient));
}


float HP03S::getTemperature()
{
    return _temperature;
}

void HP03S::measureAndCalcData()
{
  
 float offset = 0, sens = 0, x = 0;
 float pres = 0, temp = 0, dut = 0;
 float D1 = 0, D2 = 0, C1 = 0, C2 = 0, C3 = 0, C4 = 0, C5 = 0, C6 = 0, C7 = 0;
 float A = 0, B = 0;
 float C = 0, D = 0;

  _getParameters();
  
  C1 = (unsigned short)((_readParams[0] << 8) + _readParams[1]);
  C2 = (unsigned short)((_readParams[2] << 8) + _readParams[3]);
  C3 = (unsigned short)((_readParams[4] << 8) + _readParams[5]);
  C4 = (unsigned short)((_readParams[6] << 8) + _readParams[7]);
  C5 = (unsigned short)((_readParams[8] << 8) + _readParams[9]);
  C6 = (unsigned short)((_readParams[10] << 8) + _readParams[11]);
  C7 = (unsigned short)((_readParams[12] << 8) + _readParams[13]);
  
  A = (float)_readParams[14];
  B = (float)_readParams[15];
  C = (float)_readParams[16];
  D = (float)_readParams[17];
  
  D1 = _readAD(MODE_AIR);
  D2 = _readAD(MODE_TEMP);   
 
  
  if(D2 >= C5) {
    dut = D2 - C5 - ((D2 - C5) / 128.0) * ((D2 - C5) / 128.0) * A / pow(2,C);
  }
  else if(D2 < C5)  {
    dut = D2 - C5 - ((D2 - C5) / 128.0) * ((D2 - C5) / 128.0) * B / pow(2,C);
  }

  offset = (C2 + (C4 - 1024) * dut / 16384) * 4;
  sens = C1 + C3 * dut / 1024;
  x = sens * (D1 - 7168) / 16384 - offset;
  
  pres = x * 10 / 32 + C7;
  _pressure = pres / 10.0;

  temp = 250 + dut * C6 / 65536 - dut / pow(2,D);
  _temperature = (temp / 10.0);

}



void HP03S::_getParameters()
{
  int a = 0; 
  digitalWrite(_xclr, LOW);
  delay(4);
  Wire.beginTransmission(EEADDR);
  Wire.write(EESTART);
  Wire.endTransmission();
  delay(5);
  Wire.requestFrom(EEADDR, 18);
  while(Wire.available())  {
    _readParams[a] = Wire.read();
    a ++;
  }  
  
  digitalWrite(_xclr, HIGH);
}

float HP03S::_readAD(short Mode)  {
  byte ADValue[2], d = 0;
  unsigned int AD = 0;
    
  switch(Mode)  {
  
  case MODE_AIR:
    digitalWrite(_xclr, HIGH);
    Wire.beginTransmission(ADADDR);
    Wire.write(0xFF);
    Wire.write(PRESSURE);
    Wire.endTransmission();
    delay(45);

    Wire.beginTransmission(ADADDR);
    Wire.write(0xFD);
    Wire.endTransmission();
  delay(5);
    Wire.requestFrom(ADADDR, 2);
    while(Wire.available())  {
      ADValue[d] = Wire.read(); 
      d ++;
    }
    digitalWrite(_xclr, LOW);
    break;

  case MODE_TEMP:
    digitalWrite(_xclr, HIGH);
    Wire.beginTransmission(ADADDR);
    Wire.write(0xFF);
    Wire.write(TEMPERATURE);

    Wire.endTransmission();
    delay(45);

    Wire.beginTransmission(ADADDR);
    Wire.write(0xFD);
    Wire.endTransmission();
  delay(5);
    Wire.requestFrom(ADADDR, 2);
    while(Wire.available())  {
      ADValue[d] = Wire.read(); 
      d ++;
    }
    digitalWrite(_xclr, LOW);
    break;
  }
  
  AD = (ADValue[0] << 8) | ADValue[1];
  return (float) AD;
}
