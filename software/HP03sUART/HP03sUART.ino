/* 
HP03S ---> Arduino Board
Vss   ---> GND
Vdd   ---> 3.3V
MCLK  ---> pin 11 Frequencytimer2_pin 32.767 khz
XCLR  ---> pin 3 pin low(EE-Mode) high(AD-Mode)
SDA   ---> SDA
SCLK  ---> SCL
*/
#include <FrequencyTimer2.h>
#include <Wire.h>

#define EEADDR 0x50
#define ADADDR 0x77
#define EESTART 0x10
#define PRESSURE 0xF0
#define TEMPERATURE 0xE8
#define MODE_AIR 0
#define MODE_TEMP 1
#define XCLR 3

unsigned char readParams[18];
int count = 0, x = 0;

float airPressure;
float temperature;

void setup() {
  pinMode(FREQUENCYTIMER2_PIN, OUTPUT);
  pinMode(XCLR, OUTPUT);
 // pinMode(A4, HIGH);
//  pinMode(A5, HIGH);
  digitalWrite(XCLR, HIGH);

  Serial.begin(115200);
  Wire.begin();
  FrequencyTimer2::setPeriod(32);
  FrequencyTimer2::enable();  

  readEE(readParams);
 // showParams();
}

void loop() { 
  delay(5000);
  calcData();
  Serial.print(temperature);
  Serial.print("\t");
  Serial.println(airPressure);
 /* if(Serial.available()>0){
    if(Serial.readString() == "t")
    {
      //Serial.println("Performing Measurement");
      calcData();
      //Serial.print("airPressure = ");
      Serial.print(airPressure);
      //Serial.print(" hpa; temperature = ");
      Serial.print("\t");
      Serial.println(temperature);
      //Serial.println(" °C");
//      showParams();
    }
  }*/
  

  
}

int readEE(unsigned char Parameter[])  {
  int a = 0; 
  digitalWrite(XCLR, LOW);
  delay(4);
  Wire.beginTransmission(EEADDR);
  Wire.write(EESTART);
  Wire.endTransmission();
  delay(5);
  Wire.requestFrom(EEADDR, 18);
  while(Wire.available())  {
    Parameter[a] = Wire.read();
    a ++;
  }  
  
  digitalWrite(XCLR, HIGH);
  return a;
}
/*
void showParams(void)  {
  int x = 0;
  count = readEE(readParams);

  for(int c = 0; c < 7; c ++)  {
    Serial.println((readParams[c + x] << 8) + readParams[c + 1 + x]);
    x = c + 1;
  }
  for(int c = 0; c < 4; c ++)  {
    Serial.println(readParams[c + 14]);
  }
}*/

 word readAD(short Mode)  {
  byte ADValue[2], d = 0;
  unsigned int AD = 0;
    
  switch(Mode)  {
  
  case MODE_AIR:
    digitalWrite(XCLR, HIGH);
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
    digitalWrite(XCLR, LOW);
    break;

  case MODE_TEMP:
    digitalWrite(XCLR, HIGH);
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
    digitalWrite(XCLR, LOW);
    break;
  }
  
  AD = (ADValue[0] << 8) | ADValue[1];
  return AD;
}

float calcData()  {
  
 float offset = 0, sens = 0, x = 0;
 float pressure = 0, temp = 0, dut = 0;
 float D1 = 0, D2 = 0, C1 = 0, C2 = 0, C3 = 0, C4 = 0, C5 = 0, C6 = 0, C7 = 0;
 float A = 0, B = 0;
 float C = 0, D = 0;
  
  readEE(readParams);
  
  C1 = (unsigned short)((readParams[0] << 8) + readParams[1]);
  C2 = (unsigned short)((readParams[2] << 8) + readParams[3]);
  C3 = (unsigned short)((readParams[4] << 8) + readParams[5]);
  C4 = (unsigned short)((readParams[6] << 8) + readParams[7]);
  C5 = (unsigned short)((readParams[8] << 8) + readParams[9]);
  C6 = (unsigned short)((readParams[10] << 8) + readParams[11]);
  C7 = (unsigned short)((readParams[12] << 8) + readParams[13]);
  
  A = (float)readParams[14];
  B = (float)readParams[15];
  C = (float)readParams[16];
  D = (float)readParams[17];
  
  D1 = readAD(MODE_AIR);
  D2 = readAD(MODE_TEMP);   
 
  
  if(D2 >= C5) {
    dut = D2 - C5 - ((D2 - C5) / 128.0) * ((D2 - C5) / 128.0) * A / pow(2,C);
  }
  else if(D2 < C5)  {
    dut = D2 - C5 - ((D2 - C5) / 128.0) * ((D2 - C5) / 128.0) * B / pow(2,C);
  }

  offset = (C2 + (C4 - 1024) * dut / 16384) * 4;
  sens = C1 + C3 * dut / 1024;
  x = sens * (D1 - 7168) / 16384 - offset;
  
  pressure = x * 10 / 32 + C7;
  airPressure = pressure / 10.0;

  temp = 250 + dut * C6 / 65536 - dut / pow(2,D);
  temperature = (temp / 10.0);


}




