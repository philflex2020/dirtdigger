#include <ArduinoJson.h>


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

//#include <Adafruit_CircuitPlayground.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>

// This one works

#define FILE "esp822Html"
#define VERSION 0.97

#include <Ticker.h>
Ticker flip1;

//#include <DHTesp.h>
//  dht.setup(5, DHTesp::DHT22); // Connect DHT sensor to GPIO 
//float humidity = dht.getHumidity();
//float temperature = dht.getTemperature();

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
  
#define DHTpin   13  //D5 of NodeMCU is GPIO14
DHTesp dht;
String Tval;
String Hval;

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

/* Create a WiFi access point and provide a web server on it. */

#include <stdio.h>
#include <stdlib.h>
//#include <sqlite3.h>
//#include <vfs.h>
//#include <SPI.h>
// SPIFFS lib
#include <FS.h>
//extern "C" {
//#include "user_interface.h"
//}

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

#define PUMP_OFF 0
#define PUMP_ON 1
#define PUMP_DELAY 3
#define ADC_LIMIT1 1400.0F
#define ADC_LIMIT2 400.0F
#define GPIO_PUMP 15
#define GPIO_AUX 16
#define MAX_ON_TIME 15000
#define MAX_DELAY_TIME 2000

/* Set these to your desired credentials. */
//const char *ssid = APSSID;
//const char *password = APPSK;
String ledState = "OFF";

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

void writeTestFile(String fname, String sbuf);
int readTestFile(String fname, String &sbuf);

//get data from dht
String readTemp(void)
{
  String s;
  float tmp  = dht.getTemperature();
  float temp = dht.toFahrenheit(tmp);
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

void readADF(void)
{
  for (int i = 0 ; i < 4 ; i++) {
    adc[i] = ads.readADC_SingleEnded(i);
    adcf[i]=adc[i] * multiplier; /* 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */
  }
}
String readADCs(void)
{
  String s;
  s = "";
  for (int i = 0 ; i < 4 ; i++) {
    //adc[i] = ads.readADC_SingleEnded(i);
    //adcf[i]=adc[i] * multiplier; /* 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */

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
char *getUptime(char *tmp){
   int sec = millis() / 1000;
   int min = sec / 60;
   int hr = min / 60;
   snprintf( tmp, 128,"%02d:%02d:%02d"
    , hr, min %60, sec % 60
   );
  return tmp;
}
String getLocalIP() {
  IPAddress IP =WiFi.localIP();
  return String(IP[0])+"."+String(IP[1])+"."+String(IP[2])+"."+String(IP[3]);
}

//mkFunction("Temp")
String mkFunction(String Topic) {
  String s;
  s+="function get"+Topic+"() {";
  s+=" var xhttp = new XMLHttpRequest();";
  s+=" xhttp.onreadystatechange = function() {";
  s+="  if (this.readyState == 4 && this.status == 200) {";
  s+="   document.getElementById(\""+Topic+"\").innerHTML =";
  s+="  this.responseText;";
  s+="}";
  s+="};";
  s+="xhttp.open(\"GET\", \"read"+Topic+"\", true);";
  s+="xhttp.send();";
  s+="}";
  return s;
}
void handleRoot() {
    String s;
    s = "<html>";
   s+="<head>";
   s+="<title>Pump Controller</title>";
   s+= "<style>";
   s+= "body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }";
   s+= "</style>";
   s+= "</head>";
   s+= "<body>";
   s += "<div id=\"head\">";
   s+= "<h1>Phil's Pump Controller</h1>";
 
   s +="<p><a href=ajax> Try ajax</a><br></p>";
   s +="<p><a href=update> Try UPDATE</a><br></p>";
   s +="<p><a href=form> set up values</a><br></p>";
   s+= "</div>";

   s += "<div id=\"demo\">";
   s +="<h1>Demo Dynamic web page </h1>";
   s +=" <button type=\"button\" onclick=\"sendData(1)\">LED ON</button>";
   s+= " <button type=\"button\" onclick=\"sendData(0)\">LED OFF</button><br>";
   s+= "</div>";
 
   s+= "<div id=data>";
   //s+= "ADC Value is : <span id=\"ADCValue\">0</span><br>";
   s+= "LED State: <span id=\"LEDState\">"+ledState+"</span><br>";
   s+= "ADCS: <span id=\"Adcs\">1:2 2:2 3:3 :4</span><br>";
   s+= "Temp: <span id=\"Temp\">0</span><br>";
   s+= "Humid: <span id=\"Humid\">0</span><br>";
   s+= "Uptime: <span id=\"Uptime\">0</span><br>";
   //s+= "More Ticks are : <span id=\"TICKVal\">0</span>";
   s+= "<form method=\"post\" enctype=\"multipart/form-data\">";
   s+= " <input type=\"file\" name=\"name\">";
   s+= "<input class=\"button\" type=\"submit\" value=\"Upload\">";
   s+= "</form>";
   s+= "</div>";
   s+="<script>";

  s+="function sendData(led) {";
  s+="   var xhttp = new XMLHttpRequest();";
  s+="   xhttp.onreadystatechange = function() {";
  s+="     if (this.readyState == 4 && this.status == 200) {";
  s+="       document.getElementById(\"LEDState\").innerHTML =";
  s+="           this.responseText;";
  s+="      }";
  s+="   };";
  
  s+="   xhttp.open(\"GET\", \"setLED?LEDstate=\"+led, true);";
  s+="   xhttp.send();";
  s+="}";
  s+="setInterval(function() {";
  // Call a function repetatively with 5 Second interval
 //s+=" getData();";
 //s+=" getTicks();";
  s+=" getAdcs();";
  s+=" getTemp();";
  s+=" getHumid();";
  s+=" getUptime();";

  s+="}, 1000);"; //5000mSeconds update rate

  s+= mkFunction("Temp");
  s+= mkFunction("Humid");
  s+= mkFunction("Uptime");
  s+= mkFunction("Adcs");


  s+= "</script>";
  //s+= "<br><br><a href=\"https://circuits4you.com\">Circuits4you.com</a>";
  s+= "</body>";
  s+= "</html>";
 
  server.send(200, "text/html", s.c_str());
}
float adc_on = 1450.0;
float adc_off = 0450.0;

void handleForm() {
   char utime[128];
   String a_on = String(adc_on); 
   String a_off = String(adc_off); 
   String s; 
   s = "<html>";
   s+="<head>";
   s+="<title>Pump Controller</title>";
   s+= "<style>";
   s+= "body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }";
   s+= "</style>";
   s+= "</head>";
   s+= "<body>";
   s+= "<h1>Phil's Pump Controller</h1>";
   s+= "<p>Uptime: " + String(getUptime(utime)) + "</p>";
   s+= "<p>MyIP : " + getLocalIP() + "</p>";
   s+= "<form action='submit' method='get'>";
   s+= "F_name: <input type='text' name='fname' value='foo'><br>";
   s+= "ADC_ON: <input type='text' name='adc_on' value='";
   s+=a_on;
   s+="'><br>";
   s+= "ADC_OFF: <input type='text' name='adc_off' value='";
   s+=a_off;
   s+="'><br>";
   s+= "LCD1: <input type='text' name='LCD1'><br>";
   s+= "LCD2: <input type='text' name='LCD2'><br>";
   s+= "<input type='submit' value='Submit'>";
   s+="</form>";
   s+= "<br><br><a href=\"/\">home</a>";

   s+="</body>";
   s+="</html>";
  
  server.send(200, "text/html", s.c_str());
}


void setAdc_off(String val) {
  
  writeTestFile("/adc_off", val);
  adc_off = atof(val.c_str());
}

void setAdc_on(String val) {
  writeTestFile("/adc_on", val);
  adc_on = atof(val.c_str());
  
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
    else if (server.argName(i) == "adc_on") {
      setAdc_on(server.arg(i));
    }
    else if (server.argName(i) == "adc_off") {
      setAdc_off(server.arg(i));
    }

  }
  s+= "<br><br><a href=\"/\">home</a>";
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

#if 1
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>
 
<div id="demo">
<h1>ESP8266 Dynamic web page </h1>
  <button type="button" onclick="sendData(1)">LED ON</button>
  <button type="button" onclick="sendData(0)">LED OFF</button><BR>
</div>
 
<div id=data>
  ADC Value is : <span id="ADCValue">0</span><br>
  LED State is : <span id="LEDState">NA</span><br>
  ADCS     are : <span id="ADCS">1:2 2:2 3:3 :4</span><br>
  Temp is: <span id="Temp">0</span><br>
  Humid is: <span id="Humid">0</span><br>
  More Ticks are : <span id="TICKVal">0</span>
</div>
<script>

function sendData(led) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LEDState").innerHTML =
      this.responseText;
    }
  };
  
  xhttp.open("GET", "setLED?LEDstate="+led, true);
  xhttp.send();
}
 
setInterval(function() {
  // Call a function repetatively with 5 Second interval
  getData();
  getTicks();
  getAdcs();
  getTemp();
  getHumid();
}, 5000); //5000mSeconds update rate

function getTemp() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("Temp").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readTemp", true);
  xhttp.send();
}

function getHumid() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("Humid").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readHumid", true);
  xhttp.send();
}

function getAdcs() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ADCS").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readADCS", true);
  xhttp.send();
}


function getTicks() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("TICKVal").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readTick", true);
  xhttp.send();
}

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ADCValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readADC", true);
  xhttp.send();
}

</script>
<br><br><a href="https://circuits4you.com">Circuits4you.com</a>
</body>
</html>
)=====";
#endif

void runTemp(void);

void handleAjax() {
   String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleDpage() {
   String s = MAIN_page; //Read HTML contents
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

//int tick = 0;
//String Tval;
void handleTemp() {
 //String value = readTemp();
 //String tmplcd="Tick: "+ String(tick) + " :" +value;
 //setLCD(0, tmplcd);
 //tick+= 1;
 server.send(200, "text/plane", Tval); //Send ADC values only to client ajax request
 runTemp();
}

void handleHumid() {
  
 //String value = readHumid();
 //String humlcd="Humid: "+value;
 //setLCD(1, humlcd);
 
 server.send(200, "text/plane", Hval); //Send ADC values only to client ajax request
}

void handleAdcs() {
   String adcValue = readADCs();
  server.send(200, "text/plane", adcValue); //Send ADC values only to client ajax request
}

void handleUptime() {
  char utime[128];
  server.send(200, "text/plane", getUptime(utime)); //Send Uptime
}

int tickval = 0;

void handleTick()
{
  tickval += 2;
  String tickState = String(tickval);
  server.send(200, "text/plane", tickState); //Send web page
}
  
void handleLED() {
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

void handleAUX() {
 String ledState = "OFF";
 String t_state = server.arg("LEDstate"); //Refer  xhttp.open("GET", "setAUX?LEDstate="+led, true);
 Serial.println(t_state);
 if(t_state == "1")
 {
  digitalWrite(GPIO_AUX,LOW); //LED ON
  ledState = "ON"; //Feedback parameter
 }
 else
 {
  digitalWrite(GPIO_AUX,HIGH); //LED OFF
  ledState = "OFF"; //Feedback parameter  
 }
 
 server.send(200, "text/plane", ledState); //Send web page
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
int fcount =0;
int distate = 0;


void runTemp()
{
  fcount += 1;
  String msg = "PumpSys:"+String (fcount%1000) + " " + String(distate);
  setLCD(0, msg);
  if ((fcount%5)== 4 ) {
    distate++;
  }
  
  switch (distate) {
    
  case 0:
    msg = "T: " + Tval + " H: " + Hval;
    setLCD(1, msg);
    break;
    
  case 1:
    //WiFi.localIP()
    msg = "IP:" + getLocalIP();
    setLCD(1, msg);
    break;
    
  case 2:
   char utime[128];
   //uptime
   msg = "Uptime:" +String(getUptime(utime));
   setLCD(1, msg);
   break;
   
  default:
   distate = 0;
   break;
  }
}


unsigned long ltime;

int pstate = PUMP_OFF;
unsigned long g_ptime;

void runADCTest()
{
     switch (pstate) {
    
    case PUMP_OFF:
    if (adcf[3] > adc_on) {
      pstate = PUMP_ON;
      digitalWrite(GPIO_PUMP,HIGH); //LED ON
      digitalWrite(LED_BUILTIN,LOW); //LED ON
      g_ptime = millis();
      Serial.println("Turning pump on level");
    }
    break;
    
    case PUMP_ON:
    if (adcf[3] < adc_off) {
      pstate = PUMP_DELAY;
      digitalWrite(GPIO_PUMP,LOW); //LED ON
      digitalWrite(LED_BUILTIN,HIGH); //LED OFF
      g_ptime = millis();      
      Serial.println("Turning pump off; level drop");
    }
    if ((millis()-g_ptime) > MAX_ON_TIME) {
      pstate = PUMP_DELAY;
      digitalWrite(GPIO_PUMP,LOW); //LED ON
      digitalWrite(LED_BUILTIN,HIGH); //LED ON      digitalWrite(GPIO_PUMP,LOW); //LED ON
      g_ptime = millis();      
      Serial.println("Turning pump off time out");
    }
    break;
    
    default:
    case PUMP_DELAY:
    if ((adcf[3] > ADC_LIMIT1) || ((millis()-g_ptime) > MAX_DELAY_TIME)) {
      pstate = PUMP_OFF;
      g_ptime = millis();      
      Serial.println("End of pump DELAY");

    }
    break;
  }
  
}

File fsUploadFile;              // a File object to temporarily store the received file

void handleFileUpload(){ // upload a new file to the SPIFFS
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
    filename = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile) {                                    // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
      server.sendHeader("Location","/success.html");      // Redirect the client to the success page
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}


int readTestFile(String fname, String &sbuf)
{
  char buf[1024];
  File fs;              // a File object to temporarily store the received file
  int rc =-1;
   
  fs = SPIFFS.open(fname, "r");

  if(fs) {
      rc = fs.read((uint8_t*)buf, 1023); // Read the received bytes to the file
      fs.close();                               // Close the file again
      Serial.print("F Size: "); Serial.println(rc);
      if(rc > 0) {
        buf[rc] = 0;
      }
      sbuf = String(buf);
  } else {
      Serial.println("FS open failed"); 
      
  }
  return rc;
}

void writeTestFile(String fname, String sbuf)
{
  File fs;              // a File object to temporarily store the received file 
  fs = SPIFFS.open(fname, "w");

  if(fs) {
      fs.write((const uint8_t*)sbuf.c_str(), sbuf.length()); // Write the received bytes to the file
      fs.close();                               // Close the file again
      Serial.print("F Size: "); Serial.println(sbuf.length());
  } else {
      Serial.println("FS open failed"); 
  }
  
}

void setup() {
  
  //sqlite3 * db1;
  //sqlite3 * db2;
  
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  pinMode(GPIO_PUMP, OUTPUT);     // Initialize the GPIO_PUMP pin as an output
  pinMode(GPIO_AUX, OUTPUT);     // Initialize the GPIO_AUX pin as an output
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
  Serial.print(FILE);
  Serial.print(" version:");
  Serial.println(VERSION);
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
  
   if (!SPIFFS.begin()) {
       Serial.println("Failed to mount file system");
//       return;
   } else {
    writeTestFile("/Test2", "This is a test file");
   // list SPIFFS contents
   Serial.printf("SPIFFS files\n");

   Dir dir = SPIFFS.openDir("/");
   while (dir.next()) {    
       String fileName = dir.fileName();
       size_t fileSize = dir.fileSize();
       Serial.printf("FS File: %s, size: %ld\n", fileName.c_str(), (long) fileSize);
   }
   Serial.printf("End of files\n");
   }
   String sbuf="nothing";
   int rc = readTestFile("/Test2", sbuf);
   Serial.print("Read file rc =");
   Serial.print(rc);
   Serial.print(" sbuf =[");
   Serial.print(sbuf);
   Serial.println("]");

  //flip1.attach(5.0,flip);
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
  server.on("/setAUX", handleAUX);  
  server.on("/readADC", handleADC);  
  server.on("/readADCS", handleADCS);  
  server.on("/dpage", handleDpage);
  server.on("/readTick", handleTick);  
  server.on("/readTemp", handleTemp);  
  server.on("/readTemp", handleTemp);  
  server.on("/readHumid", handleHumid);  
  server.on("/readUptime", handleUptime);  
  server.on("/readAdcs", handleAdcs);  
  server.on("/update", handleOTA);  
  server.on("/upload", HTTP_POST,                       // if the client posts to the upload page
    [](){ server.send(200); },                          // Send status 200 (OK) to tell the client we are ready to receive
    handleFileUpload                                    // Receive and save the file
  );
  rc = readTestFile("/adc_off", sbuf);
  if(rc > 0) {
    setAdc_off(sbuf);
  }
  rc = readTestFile("/adc_on", sbuf);
  if(rc > 0) {
    setAdc_on(sbuf);
  }

  server.begin();
  Serial.println("HTTP server started");
  ltime = millis();
  
}

void loop() {
  
  unsigned long ftime = millis();
  unsigned long ftime1;
  unsigned long ftime2;
  unsigned long ftime3;
  unsigned long ftime4;
  
  if ((ftime - ltime ) > 2000) {
    ftime1 = millis();
    Tval = readTemp();
    ftime2 = millis();
    Hval = readHumid();
    ftime3 = millis();
    readADF();
    ftime4 = millis();
    Serial.print("ltime delta:");
    Serial.print( ftime-ltime);
    Serial.print(" temp time:");
    Serial.print( ftime2-ftime1);
    Serial.print(" hum time:");
    Serial.print( ftime3-ftime2);
    Serial.print(" adc time:");
    Serial.print( ftime4-ftime3);
    Serial.print(" adc val:");
    Serial.print( adcf[3] );
    Serial.println("..");

    ltime = ftime; 
    runTemp();
    runADCTest();

  }
  
 if(runOTA == 0){
  server.handleClient();
 } else {
   ArduinoOTA.handle();
 }
}

