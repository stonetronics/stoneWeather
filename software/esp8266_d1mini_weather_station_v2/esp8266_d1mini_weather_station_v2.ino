#include <ESP8266WiFi.h>
#include <Wire.h>
#include "SparkFunHTU21D.h"
#include "HP03S.h"

#define UPDATEINTERVAL_MS 2000
 
const char* ssid = "XXXXXXXXXXXXXXXXXXX";//type your ssid
const char* password = "XXXXXXXXXXXXXXXXXXX";//type your password

int millisPrev;

float hp03sTemperature;
float hp03sPressure;
float hp03sPressureAtSeaLevel;
float htu21dTemperature;
float htu21dHumidity;
 
int ledPin = 2; // GPIO2 of ESP8266
WiFiServer server(80);//Service Port

// XCLR       MCLK
//   |______    |
//          |   |
HP03S hp03s(14, 16);

HTU21D htu21d;

void connectWifi(){
  hp03s.begin();
  htu21d.begin();
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    digitalWrite(ledPin, HIGH);
    delay(250);
    digitalWrite(ledPin, LOW);
  }

  // Start the server
  server.begin();
}

 
void setup() {
  millisPrev = millis();
  Serial.begin(115200);
  delay(10);
 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
   
  connectWifi();

  //indicate successfull startup
  digitalWrite(ledPin, LOW);
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}
 
void loop() {

  int millisNow = millis();

  //overflow handling for milliseconds counter  
  if(millisNow < millisPrev){
    millisPrev = millisNow;
  }

  //measurement every UPDATEINTERVAL_MS
  if(millisNow > (millisPrev + UPDATEINTERVAL_MS))
  {
    hp03s.measureAndCalcData();
    hp03sTemperature = hp03s.getTemperature();
    hp03sPressure = hp03s.getPressure();
    hp03sPressureAtSeaLevel = hp03s.getPressureAtSeaLevel(366);

    htu21dTemperature = htu21d.readTemperature();
    htu21dHumidity = htu21d.readHumidity();
  }

  //check wifi status
  if(WiFi.status() != WL_CONNECTED){
    connectWifi();
  }
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
   
  // Wait until the client sends some data
  while(!client.available()){
    delay(1);
  }
   
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  client.flush();
   
 /* if (request.indexOf("/GETDATA=TRUE") != -1) {
    requestData();
  } */

   
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  client.println("<title>|=| STONEWEATHER |=|</title>");
  //client.println("<link rel=\"icon\" type=\"image/png\" href=\"http://*.*.*.*/stonetronicsLogo.PNG\"");
  client.println(" /></head>");
  client.println("<body>");

  client.println("<h1>|=| STONEWEATHER |=|</h1><hr>");
  client.println("<br><h2> HP03s readings: </h2>");
  client.print("Temperature: <div id = \"hp03sTemperature\">");
  client.print(hp03sTemperature);
  client.println("</div> degree C <br><br>");
  client.print("Pressure: <div id = \"hp03sPressure\">");
  client.print(hp03sPressure);
  client.println("</div> hPa <br><br>");
  client.print("Pressure at Sea Level: <div id = \"hp03sPressureAtSeaLevel\">");
  client.print(hp03sPressureAtSeaLevel);
  client.println("</div> hPa <br>");
  client.println("<br><h2> HTU21D readings: </h2>");
  client.print("Temperatur: <div id = \"htu21dTemperature\">");
  client.print(htu21dTemperature);
  client.println("</div> degree C <br><br>");
  client.print("Humidity: <div id = \"htu21dHumidity\">");
  client.print(htu21dHumidity);
  client.println("</div> % <br>");
  client.println("<hr>");
  //client.println("<img src = \"http://*.*.*.*/stonetronicsLogo.PNG\" width = \"20%\" height = \"20%\">");
  client.println("</body>");
  client.println("</html>");
}
