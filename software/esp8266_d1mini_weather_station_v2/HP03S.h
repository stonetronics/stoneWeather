#ifndef HP03S_H
#define HP03S_H

#include <Arduino.h>
#include <Wire.h>

// #define HP03S_H_DEBUG



class HP03S
{
  public:
    HP03S(int xclr, int mclk);
    void begin();

    void printParameters();

    float getPressure();
    float getPressureAtSeaLevel(double height);
    float getTemperature();

    void measureAndCalcData();


  private:

    int _xclr;
    int _mclk;
    unsigned char _readParams[18];

    float _pressure;
    float _temperature;

    void _getParameters();
    float _readAD(short Mode);
};

#endif
