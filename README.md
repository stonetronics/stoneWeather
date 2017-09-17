# stoneweather
Small IOT Weather station using Wifi Module ESP8266, Microcontroller ATMega328, Pressure/Temperature Sensor HP03S, Relative Air Humidity/Temperature Sensor HTU21 LDO AMS1117

Hardware Layout drawn in Eagle 8.0
Header for ESP8266 and GY21-HTU21 breakout boards mounted SMD
all components SMD except for LED and XTAL, but they will be updated as soon as the parts are fixed in SMD

Code for ESP8266 written in Arduino IDE
Reads temperature and air pressure on uart and displays it on a very simple website to be used by a data collector (see github.com/ringo96/weatherstation)

Code for ATMega328 written in Arduino IDE
Measures temperature and air pressure with an HP03S sensor and displays it on uart