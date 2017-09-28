#include <ESP8266WiFi.h>
#include <stdio.h>
 
const char* ssid = "XXXXXXXXXXXXXXXXXX";//type your ssid
const char* password = "XXXXXXXXXXXXXXXXXX";//type your password

unsigned char stringIndex;
char inputText[255];
 
int ledPin = 2; // GPIO2 of ESP8266
WiFiServer server(80);//Service Port

void connectWifi(){
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    digitalWrite(ledPin, LOW);
    delay(250);
    digitalWrite(ledPin, HIGH);
  }

  // Start the server
  server.begin();
}

void requestData(){
  Serial.write('t');
}
 
void setup() {
  Serial.begin(115200);
  delay(10);
 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  stringIndex = 0;
   
  connectWifi();

  //indicate successfull startup
  digitalWrite(ledPin, HIGH);
 
  // Print the IP address
/*  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");*/
}
 
void loop() {
  
  char inputChar;
  char temperature[20];
  char pressure[20];
  char tokenizeIndex;
  char pressureStartIndex;
  
  //check uart
  if(Serial.available()>0){
    inputChar = Serial.read();
    
    inputText[stringIndex++] = inputChar;
    if(inputChar == '\n')
    {
      inputText[stringIndex] = '\0';
      stringIndex = 0;
      tokenizeIndex = 0;
      while((inputText[tokenizeIndex] != '\t') && (inputText[tokenizeIndex] != '\0') && (inputText[tokenizeIndex] != '\n')) {
        temperature[tokenizeIndex] = inputText[tokenizeIndex];
        tokenizeIndex++;
      }
      temperature[tokenizeIndex] = '\0';
      tokenizeIndex++;
      pressureStartIndex = tokenizeIndex;
      while((inputText[tokenizeIndex] != '\t') && (inputText[tokenizeIndex] != '\0') && (inputText[tokenizeIndex] != '\n')) {
        pressure[tokenizeIndex - pressureStartIndex] = inputText[tokenizeIndex];
        tokenizeIndex++;
      }
      pressure[tokenizeIndex - pressureStartIndex] = '\0';
      
    }
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
   
 
  int value = LOW;
  if (request.indexOf("/GETDATA=TRUE") != -1) {
    requestData();
  } 
 
  //Set ledPin according to the request
  //digitalWrite(ledPin, value);
   
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><title>|=| STONEWEATHER |=|</title></head>");
  client.println("<body>");
  /*  
  client.println("<br><h1>STONEWEATHER DEV</h1><br>");
  client.println("Click <a href=\"/GETDATA=TRUE\">here</a> request data<br>");
  client.print("Input Text = \" ");
  client.print(inputText);*/

  client.println("<h1>Test site</h1><br>");
  client.print("Temperatur: <div id = \"temperature\">");
  client.printf("%s", temperature);
  client.println("</div> degree C <br><br>");
  client.print("Luftdruck: <div id = \"pressure\">");
  client.printf("%s", pressure);
  client.println("</div> hPa <br>");
  client.println("</body>");
  client.println("</html>");
}
