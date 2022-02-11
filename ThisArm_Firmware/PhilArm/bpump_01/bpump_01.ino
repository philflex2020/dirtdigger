/* Example from Sandeep Mistry 
 * With mods from AliExpress/LilyGo docs
 * For TTGo ESP32 LoRa-OLED board
 * http://www.lilygo.cn/down_view.aspx?TypeId=11&Id=78&Fid=t14:11:14
 * Based on SX1276 LoRa Radio
 * http://www.semtech.com/apps/product.php?pn=SX1276
 * RMB 29Nov2017
 */

 /*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
*/


#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiMulti.h>

WiFiMulti WiFiMulti;

#include <WiFiAP.h>

#include <SPI.h>
#include <LoRa.h>       // https://github.com/sandeepmistry/arduino-LoRa
#include <U8g2lib.h>   // https://github.com/olikraus/U8g2_Arduino
// #include <U8x8lib.h>
#include <esp32-hal.h>
#include "Task.h"
int g_debug = 0;
#include "Pump.h"

// Set these to your desired credentials.
const char *ssid = "yourAP";
const char *password = "yourPassword";

const char *version ="0.0.3";

WiFiServer server(80);

#define OFF 0   // For LED
#define ON 1

// SPI LoRa Radio
#define LORA_SCK 5        // GPIO5 - SX1276 SCK
#define LORA_MISO 19     // GPIO19 - SX1276 MISO
#define LORA_MOSI 27    // GPIO27 - SX1276 MOSI
#define LORA_CS 18     // GPIO18 - SX1276 CS
#define LORA_RST 14   // GPIO14 - SX1276 RST
#define LORA_IRQ 26  // GPIO26 - SX1276 IRQ (interrupt request)

// I2C OLED Display works with SSD1306 driver
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16

/* Pick One. Hardware I2C does NOT work! This article helped: https://robotzero.one/heltec-wifi-kit-32/ 
* TTGo boards similar to Heltec boards, LED_BUILTIN = 2 instead of pin 25
* Some OLED displays don't handle ACK correctly so SW I2C works better. Thank you Olikraus!
* TTGo OLED has pin 16 reset unlike other OLED displays
*/

// UNCOMMENT one of the constructor lines below
//U8X8_SSD1306_128X64_NONAME_SW_I2C Display(/* clock=*/ OLED_SCL, /* data=*/ OLED_SDA, /* reset=*/ OLED_RST); // Unbuffered, basic graphics, software I2C
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C Display(U8G2_R0, /* clock=*/ OLED_SCL, /* data=*/ OLED_SDA, /* reset=*/ OLED_RST); // Page buffer, SW I2C
U8G2_SSD1306_128X64_NONAME_F_SW_I2C Display(U8G2_R0, /* clock=*/ OLED_SCL, /* data=*/ OLED_SDA, /* reset=*/ OLED_RST); // Full framebuffer, SW I2C

const int blueLED = LED_BUILTIN; 
const int redLED = 13; 
const int testLED = 12; 
const int delLED = 2; 


String rssi = "";
String packet = "";

uint8_t adc0 = 36;
int redLow = 0;
int testLow = 0;
int delLow = 0;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile int fastCounter =0;
volatile int interruptCounter;
int totalInterruptCounter;

int toggleLed(uint8_t pin, int &state);
uint16_t readAdc(uint8_t pin);




void IRAM_ATTR onTimer()
{
   toggleLed(testLED, testLow);
   toggleLed(redLED, redLow);

  portENTER_CRITICAL_ISR(&timerMux);
  fastCounter++;
 
  if ( fastCounter >= 10000) {
    fastCounter = 0;
    interruptCounter++;
  }
  
  portEXIT_CRITICAL_ISR(&timerMux);
  toggleLed(redLED, redLow);
  uint16_t aval = readAdc(adc0); // up to 4096
  toggleLed(testLED, testLow);
  dacWrite(26, interruptCounter % 255);  
  toggleLed(testLED, testLow);
  toggleLed(delLED, delLow);
  int del = aval/256;
  if (del > 75) del = 75 ;
  delayMicroseconds(del);
  toggleLed(delLED, delLow);
}

void wifiScan()
{
  int n = WiFi.scanNetworks();

  if (n == 0) {
    Display.clearDisplay();
    Display.setCursor(0,0); Display.print("Search");
    Display.sendBuffer();
    
    //Display.drawString(0, 0, "Searching networks.");
    Serial.println("Searching networks . . .");
  } else {
    Display.clearDisplay();
    Display.setCursor(0,0); Display.print("Found");
    Display.sendBuffer();
    //Display.sendBuffer();

    //Display.drawString(0, 0, "Networks found: ");
    Serial.println("Networks found:");  
    for (int i = 0; i < n; ++i) {
      // Print SSID for each network found
      char currentSSID[64];
      WiFi.SSID(i).toCharArray(currentSSID, 64);
      Display.setCursor(0,i+1); Display.print(currentSSID);
      //Display.drawString(0, i + 1, currentSSID);
      Display.sendBuffer();

      Serial.println(currentSSID);
    }
    
  }
}

void wifiStaMode(void)
{
   // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

int wifiRunning  = 0;
int wifiConnected = 0;

int wifiMultiSetup()
{
    // We start by connecting to a WiFi network
    WiFiMulti.addAP("Butter2.4", "Butt3r12");

    Serial.print("Waiting for WiFi... ");
    int imax = 10;
    int i = 0;
    int sta = 0;
    while((i < imax)) {
      sta = WiFiMulti.run();
      if( sta != WL_CONNECTED) {
        i++;
        Serial.print(".");
        delay(500);
      }
      else {
        i = imax;
      }
    } 
    if (sta == WL_CONNECTED) {
      wifiConnected = 1;
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());

    } else {
      wifiConnected = 0;
      Serial.println("");
      Serial.println("WiFi NOT connected");
    
    }
    return wifiConnected;

}

//  WifiSend("Hello from me\n");

int wifiTest(void)
{
  if (wifiRunning == 0) {
    if(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        //delay(500);
        return 0;
    }
    
    wifiRunning = 1;
    wifiConnected = 1;
    
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    return 1;
  }
  return 1;
}

void WifiSend(char * msg)
{
    const uint16_t port = 8081;
    //const char * host = "192.168.1.1"; // ip or dns
    const char * host = "pibot02"; // ip or dns

    Serial.print("Connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;

    if (!client.connect(host, port)) {
        Serial.println("Connection failed.");
        Serial.println("Waiting 5 seconds before retrying...");
        delay(5000);
        return;
    }

    // This will send a request to the server
    client.print(msg);

    //read back one line from the server
    String line = client.readStringUntil('\r');
    client.println(line);

    Serial.println("Closing connection.");
    client.stop();

    //Serial.println("Waiting 5 seconds before restarting...");

}

void setupTimer()
{
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  // one second
  //timerAlarmWrite(timer, 1000000, true);
  timerAlarmWrite(timer,      100, true);
  timerAlarmEnable(timer);
}

int toggleLed(uint8_t pin, int &state)
{
   if (state) {
       state = 0;         
       digitalWrite(pin, HIGH);               // GET /H turns the LED on
   } else {
       state = 1;         
       digitalWrite(pin, LOW);               // GET /H turns the LED on
    
   }
}

uint16_t readAdc(uint8_t pin)
{

  uint16_t val = analogRead(36);
  //Serial.print (" Blue pin = ");
  //Serial.println (blueLED);
  //Serial.print (" Read Done 36 val = ");
  //Serial.println (val);
  //val = analogRead(37);
  //Serial.print (" Read Done 37 val = ");
  //Serial.println (val);
  //togleLed(redLED, redLow);
  return val;
  /*
 * Attach pin to ADC (will also clear any other analog mode that could be on)
 * */
//bool adcAttachPin(uint8_t pin);
  if (adcAttachPin(pin))
  {
    Serial.println (" Attached Pin");
  
  }


/*
 * Start ADC conversion on attached pin's bus
 * */
//bool adcStart(uint8_t pin);
  if (adcStart(pin))
  {
    Serial.println (" Pin Started");
  
  }

/*
 * Check if conversion on the pin's ADC bus is currently running
 * */
//bool adcBusy(uint8_t pin);
  while (adcBusy(pin))
  {
    Serial.println (" Pin Busy");
  
  }

/*
 * Get the result of the conversion (will wait if it have not finished)
 * */
//uint16_t adcEnd(uint8_t pin);
  val = adcEnd(pin);
  
   Serial.print (" Pin Done val =");
   Serial.println (val);
  return val;
  

}


void setupAP()
{
  Serial.print("Configuring access point...");
  Serial.println(ssid);

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

}

Task * g_tasks = NULL;

void testTimer(){
   if (interruptCounter > 0) {
 
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
 
    totalInterruptCounter++;
    if(g_tasks) g_tasks->runTask();
    Serial.print("An interrupt as occurred. Total number: ");
    Serial.println(totalInterruptCounter);
 
  }
}

void tfcn(void * ct, void * cd)
{  
  Task  *t = (Task *)ct;
  Serial.print(" Task running ");
  Serial.println(t->runs);
  t->runs++;
}

int wifiWait = 0;


void setup() {
  Serial.begin(115200);
  while (!Serial);

  
  Serial.println("bpump_01 - basic test code for bpump");
  Serial.print("   version :");
  Serial.println(version);
  
    
  Serial.println("LoRa Receiver");

  Display.begin();
  Display.enableUTF8Print();    // enable UTF8 support for the Arduino print() function
  Display.setFont(u8g2_font_ncenB10_tr);
  //Display.setFont(u8x8_font_chroma48medium8_r);
  //wifiStaMode();
  wifiScan();

  wifiMultiSetup();
  if (wifiConnected == 0) {
     wifiStaMode();
     setupAP();
     
  }
  
  //if(wifiRunning) {
  //  WifiSend("Hello from me\n");
    
  //}
  //setupAP();

  // Very important for SPI pin configuration!
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS); 
  
  // Very important for LoRa Radio pin configuration! 
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);         

  pinMode(redLED, OUTPUT); // For LED feedback
  pinMode(blueLED, OUTPUT); // For LED feedback
  pinMode(testLED, OUTPUT); // For LED feedback
  pinMode(delLED, OUTPUT); // For LED feedback
  
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  Serial.println("Starting LoRa looks OK");
  // The larger the spreading factor the greater the range but slower data rate
  // Send and receive radios need to be set the same
  LoRa.setSpreadingFactor(12);  // ranges from 6-12, default 7 see API docs
   g_tasks = setupTask(5 ,tfcn);

   setupTimer();
   Serial.println("After setupTimer");
   
}

#if 1
void wifiClient() {
   WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
#endif

void loop() {
  //wifiTest();
  //if( wifiConnected == 1) {
    // WifiSend("Hello from me\n");
     //wifiConnected = 0;
   //}

  testTimer();
  
  wifiClient();
  
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    
    Serial.print("Received packet '");

    digitalWrite(blueLED, ON);  // Turn blue LED on

    // read packet
    packet = "";                   // Clear packet
    while (LoRa.available()) {
      packet += (char)LoRa.read(); // Assemble new packet
    }
    rssi = LoRa.packetRssi();

    // Display Info
    Display.clearBuffer();  
    Display.setCursor(0,12); Display.print("LoRa Receiver");
    Display.setCursor(0,26); Display.print("Received packet:");
    Display.setCursor(0,42); Display.print("    '" + packet + "'");
    Display.setCursor(0,58); Display.print("RSSI " + rssi);
    Display.sendBuffer();

    digitalWrite(blueLED, OFF); // Turn blue LED off
    
    Serial.println(packet + "' with RSSI " + rssi);     
    //readAdc(adc0);
    
  }
}

