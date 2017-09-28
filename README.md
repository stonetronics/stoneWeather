# stoneweather
Small IOT Weather station using Wifi Module WeMos D1 Mini, Pressure/Temperature Sensor HP03S, Relative Air Humidity/Temperature Sensor HTU21 LDO AMS1117

Newer version using WeMos D1 Mini, kicking out the ATmega328 to reduce complexity

Hardware Layout drawn in Eagle 8.0
coming soon!

Code for WeMos D1 Mini written in Arduino IDE (esp8266_d1mini_weather_station_v2)
-Measures temperature and air pressure using HP03s (custom .cpp and .h mixed up of "github.com/philippG777/HP03S" and "codebender.cc/sketch:42077#HP03S.ino"
-Measures temperature and relative air humidity using HTU21 (sparkfun Library)
-Displays all Data on a very simple Website to be used by a data collector (see github.com/ringo96/weatherstation)


Old Version using ATmega328 and ESP8266:

Hardware Layout drawn in Eagle 8.0
-Header for ESP8266 and GY21-HTU21 breakout boards mounted SMD
-all components SMD except for LED and XTAL

Code for ESP8266 written in Arduino IDE (esp8266displayUart)
-Reads temperature and air pressure on uart and displays it on a very simple website to be used by a data collector (see github.com/ringo96/weatherstation)

Code for ATMega328 written in Arduino IDE (HP03sUART)
-Measures temperature and air pressure with an HP03S sensor and displays it on uart
