//#include <Adafruit_CircuitPlayground.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
// not working yet
//#include <DHTesp.h>
//  dht.setup(5, DHTesp::DHT22); // Connect DHT sensor to GPIO 17
//float humidity = dht.getHumidity();
//  float temperature = dht.getTemperature();

//Serial.print(dht.getStatusString());
//  Serial.print("\t");
//  Serial.print(humidity, 1);
//  Serial.print("\t\t");
//  Serial.print(temperature, 1);
//  Serial.print("\t\t");
//  Serial.print(dht.toFahrenheit(temperature), 1);
//  Serial.print("\t\t");
//  Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
//  Serial.print("\t\t");
//  Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);

Adafruit_ADS1115 ads;

int16_t adc[4];
float adcf[4];

// +/- 6.144v 1 bit = 3mV/ADS1015 , 0.1875mV/ADS1115

  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
//adc[0]=ads.readADC_SingleEnded(0);

// ADC
// VDD 3.3v
// D1 SCL
// D2 SDA
// G GND

#include <DHTesp.h>  //DHT11 Library for ESP
  
#define DHTpin   5  //D5 of NodeMCU is GPIO14
DHTesp dht;

// Standard for ESP8266 with 4-SDA 5-SCL or ARDUINO with A4-SDA A5-SCL and LCD 16x2 display
#include <LiquidCrystal_I2C.h>
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);


  /* Be sure to update this value based on the IC and the gain settings! */
//  float   multiplier = 3.0F;    /* ADS1015 @ +/- 6.144V gain (12-bit results) */
float multiplier = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */

//  results = ads.readADC_Differential_0_1();  
// results*multiplier = mV
//  Serial.println("Comparator Threshold: 1000 (3.000V)");
//ads.begin();
  
  // Setup 3V comparator on channel 0
  //ads.startComparator_SingleEnded(0, 1000);
// Comparator will only de-assert after a read
// uses the alrt/rdy pin
//  adc0 = ads.getLastConversionResults();

/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//#ifndef APSSID
//#define APSSID "ESPap"
//#define APPSK  "thereisnospoon"
//#endif

/* Set these to your desired credentials. */
//const char *ssid = APSSID;
//const char *password = APPSK;

const char* ssid = "Butter2.4";
const char* password = "Butt3r12";
ESP8266WebServer server(80);
//WiFi.localIP());
//WiFi.localIP());
  //char *myIP = NULL;
IPAddress myIP;
/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
//get data from dht
String readTemp(void)
{
  String s;
  float tmp = dht.getTemperature();
  float temp =dht.toFahrenheit(tmp);
  s = String(temp);
  return s;
}

String readHumid(void)
{
  String s;
  float hum = dht.getHumidity();
  s = String(hum);
  return s;
}

String readADCs(void)
{
  String s;
  s = "";
  for (int i = 0 ; i < 4 ; i++) {
    adc[i] = ads.readADC_SingleEnded(i);
    adcf[i]=adc[i] * multiplier; /* 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */

    s += String(i)+":["+String(adcf[i])+"] ";
  }
  return s;
}

int setLCD(int line, String s)
{
  lcd.setCursor(0,line);
  lcd.print("                ");
  lcd.setCursor(0,line);
  lcd.print(s.c_str());
  return line;
}


void handleSubmit() {
   String s; 
   s = "<html>";
   s+="<head>";
   s+="<title>ESP8266 Submit</title>";
   s+= "<style>";
   s+= "body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }";
   s+= "</style>";
  s+= "</head>";
  s+="<body>";
  s+=" <h1>Submit from ESP8266!</h1>";
  s+= "<p>Args: " + String(server.args())+"</p>";
  //int i = 0;
  for (int i = 0 ; i < server.args(); i++) {
    s+="<p> Arg " + String(i)+ " Name [" +server.argName(i) + " value [" + server.arg(i)+"]  ";
    if (server.argName(i) == "LCD1") {
      setLCD(0, server.arg(i));
    }
    else if (server.argName(i) == "LCD2") {
      setLCD(1, server.arg(i));
    }

  }
  s+="</body>";
  s+="</html>";
  server.send(200, "text/html", s.c_str());

}

int runOTA = 0;

void handleOTA() {
  ArduinoOTA.onStart([]() {
    Serial.println("OTA Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
   ArduinoOTA.begin();
   Serial.println("OTA Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  runOTA = 1;
  String s ="<H1> Running OTA</H1>";
  server.send(200, "text/html", s.c_str());

}



String getMainPage(void)
{
  String s;
  

  s ="<!DOCTYPE html><html><body> <div id=\"demo\">";
  s+="<h1>ESP8266 Dynamic web page </h1>";
  s+="<button type=\"button\" onclick=\"sendData(1)\">LED ON</button>";
  s+="<button type=\"button\" onclick=\"sendData(0)\">LED OFF</button><BR>";
  s+= "</div>";
 
  s+="<div> ADC Value is : <span id=\"ADCValue\">0</span><br>";
  s+="  LED State is : <span id=\"LEDState\">NA</span><br>";
  s+="  ADCS     are : <span id=\"ADCS\">1:2 2:2 3:3 :4</span><br>";
  s+="  Temp is: <span id=\"Temp\">0</span><br>";
  s+="  Humid is: <span id=\"Humid\">0</span><br>";
  s+="  More Ticks are : <span id=\"TICKVal\">0</span>";
  s+="</div>";
  
  s+="<script>";

  s+="function sendData(led) {";
  s+= "var xhttp = new XMLHttpRequest();";
  s+= "xhttp.onreadystatechange = function() {";
  s+= "   if (this.readyState == 4 && this.status == 200) {";
  s+= "     document.getElementById(\"LEDState\").innerHTML = this.responseText; }";
  
  s+="  xhttp.open(\"GET\", \"setLED?LEDstate=\"+led, true);";
  s+="  xhttp.send();";
  s+="}";
 
  s+="setInterval(function() {";
  // Call a function repetatively with 2 Second interval
  s+="  getData();";
  s+="  getTicks();";
  //s+="  getAdcs();";
  //s+="  getTemp();";
  //s+="  getHumid();";

  s+="}, 2000);"; //2000mSeconds update rate

  s+="function getTemp() {";
  s+="  var xhttp = new XMLHttpRequest();";
  s+="  xhttp.onreadystatechange = function() {";
  s+="    if (this.readyState == 4 && this.status == 200) {";
  s+="      document.getElementById(\"Temp\").innerHTML = ";
  s+="      this.responseText;";
  s+="    }";
  s+="  };";
  s+="  xhttp.open(\"GET\", \"readTemp\", true);";
  s+="  xhttp.send();";
  s+="}";

  s+="function getHumid() {";
  s+="  var xhttp = new XMLHttpRequest();";
  s+="  xhttp.onreadystatechange = function() {";
  s+="    if (this.readyState == 4 && this.status == 200) {";
  s+="      document.getElementById(\"Humid\").innerHTML =";
  s+="          this.responseText;";
  s+="    }";
  s+="  };";
  s+="  xhttp.open(\"GET\", \"readHumid\", true);";
  s+="  xhttp.send();";
  s+="}";

  s+="function getAdcs() {";
  s+="  var xhttp = new XMLHttpRequest();";
  s+="  xhttp.onreadystatechange = function() {";
  s+="    if (this.readyState == 4 && this.status == 200) {";
  s+="      document.getElementById(\"ADCS\").innerHTML =";
  s+="            this.responseText;";
  s+="    }";
  s+="  };";
  s+="  xhttp.open(\"GET\", \"readADCS\", true);";
  s+="  xhttp.send();";
  s+="}";


  s+="function getTicks() {";
  s+="  var xhttp = new XMLHttpRequest();";
  s+="  xhttp.onreadystatechange = function() {";
  s+="    if (this.readyState == 4 && this.status == 200) {";
  s+="      document.getElementById(\"TICKVal\").innerHTML =";
  s+="           this.responseText;";
  s+="    }";
  s+="  };";
  s+="  xhttp.open(\"GET\", \"readTick\", true);";
  s+="  xhttp.send();";
  s+="}";

  s+="function getData() {";
  s+="  var xhttp = new XMLHttpRequest();";
  s+="  xhttp.onreadystatechange = function() {";
  s+="    if (this.readyState == 4 && this.status == 200) {";
  s+="      document.getElementById(\"ADCValue\").innerHTML =";
  s+="      this.responseText;";
  s+="    }";
  s+="  };";
  s+="  xhttp.open(\"GET\", \"readADC\", true);";
  s+="  xhttp.send();";
  s+="}";

  s+="</script>";
  s+="<br><br><a href=\"https://circuits4you.com\">Circuits4you.com</a>";
  s+="</body>";
  s+="</html>";
  return s;
}


void handleAjax() {
   String s = getMainPage(); //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleDpage() {
   String s = getMainPage(); //Read HTML contents
   server.send(200, "text/html", s); //Send web page
}
void handleADC() {
 int a = analogRead(A0);
 String adcValue = String(a);
 
 server.send(200, "text/plane", adcValue); //Send ADC value only to client ajax request
}

void handleADCS() {
 String adcValue = readADCs();
 server.send(200, "text/plane", adcValue); //Send ADC values only to client ajax request
}

void handleTemp() {
 String value = readTemp();
 server.send(200, "text/plane", value); //Send ADC values only to client ajax request
}

void handleHumid() {
 String value = readHumid();
 server.send(200, "text/plane", value); //Send ADC values only to client ajax request
}

int tickval = 0;

void handleTick()
{
  tickval += 2;
  String tickState = String(tickval);
  server.send(200, "text/plane", tickState); //Send web page
}
  
void handleLED() {
 String ledState = "OFF";
 String t_state = server.arg("LEDstate"); //Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
 Serial.println(t_state);
 if(t_state == "1")
 {
  digitalWrite(LED_BUILTIN,LOW); //LED ON
  ledState = "ON"; //Feedback parameter
 }
 else
 {
  digitalWrite(LED_BUILTIN,HIGH); //LED OFF
  ledState = "OFF"; //Feedback parameter  
 }
 
 server.send(200, "text/plane", ledState); //Send web page
}

void handleForm() {
  int sec = millis() / 1000;
   int min = sec / 60;
   int hr = min / 60;
   char temp[1420];
   snprintf ( temp, 1400,
  "<html>\
  <head>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <p>MyIP :%s  </p>\
    <form action='submit' method='get'>\
    F_name: <input type='text' name='fname'><br>\
    LCD1: <input type='text' name='LCD1'><br>\
    LCD2: <input type='text' name='LCD2'><br>\
    <input type='submit' value='Submit'>\
</form>\
  </body>\
</html>",
      hr, min % 60, sec % 60, String(myIP).c_str()
   );
  server.send(200, "text/html", temp);
}

void handleRoot() {
  String s = "<h1>You are connected</h1>";
  s +="<a href=ajax> Try ajax</a><br>";
  s +="<a href=update> Try UPDATE</a><br>";
  s +="<a href=form> run form</a><br>";
 
  server.send(200, "text/html", s.c_str());
}

void handleFoo() {
  if (digitalRead(LED_BUILTIN) == 1) {   // Turn the LED on (Note that LOW is the voltage level
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
    server.send(200, "text/html", "<h1>You are connected to foo op - off</h1>");
  } else {
    digitalWrite(LED_BUILTIN, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    server.send(200, "text/html", "<h1>You are connected to foo op - on</h1>");

  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  //Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  //WiFi.softAP(ssid, password);

  //IPAddress myIP = WiFi.softAPIP();

  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  myIP=WiFi.localIP();
  
  Serial.println(WiFi.localIP());
  
  //Serial.print("AP IP address: ");
  //Serial.println(myIP);
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  
  //lcd.begin(0, 2); //ESP8266-01 I2C with pin 0-SDA 2-SCL
  lcd.begin(0,2);
  lcd.backlight();
  
  lcd.setCursor(0, 0);
//#if defined(ESP8266)
  lcd.print("ESP8266");
  Serial.println("LCD started");
  // use the same as lcd 
  //ads.begin();
  // use pin 
  dht.setup(DHTpin, DHTesp::DHT22); // Connect DHT sensor to GPIO 17
//float humidity = dht.getHumidity();
//  float temperature = dht.getTemperature();

  server.on("/", handleRoot);
  server.on("/foo", handleFoo);  
  server.on("/form", handleForm);  
  server.on("/submit", handleSubmit);  
  server.on("/ajax", handleAjax);  
  server.on("/setLED", handleLED);  
  server.on("/readADC", handleADC);  
  server.on("/readADCS", handleADCS);  
  server.on("/dpage", handleDpage);
  server.on("/readTick", handleTick);  
  server.on("/readTemp", handleTemp);  
  server.on("/readTemp", handleTemp);  
  server.on("/readHumid", handleHumid);  
  server.on("/update", handleOTA);  

  server.begin();
  Serial.println("HTTP server started");
}


void loop() {
  
 if(runOTA == 0){
  server.handleClient();
 } else {
   ArduinoOTA.handle();
 }
}

