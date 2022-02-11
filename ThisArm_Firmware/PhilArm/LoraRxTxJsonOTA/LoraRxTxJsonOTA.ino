/* Example from Sandeep Mistry 
 * With mods from AliExpress/LilyGo docs
 * For TTGo ESP32 LoRa-OLED board
 * http://www.lilygo.cn/down_view.aspx?TypeId=11&Id=78&Fid=t14:11:14
 * Based on SX1276 LoRa Radio
 * http://www.semtech.com/apps/product.php?pn=SX1276
 * RMB 29Nov2017
 */
#include <ArduinoJson.h>
#include "EEPROM.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <WiFiMulti.h>


#include <WiFiAP.h>
#include <SPI.h>
#include <LoRa.h>       // https://github.com/sandeepmistry/arduino-LoRa
#include <U8g2lib.h>   // https://github.com/olikraus/U8g2_Arduino
// #include <U8x8lib.h>

#include "OTA_PAGES.h"

#define OFF 0   // For LED
#define ON 1

const char *progname ="RxTxJson :";
const char *version ="0.0.4";
int myID =0;  // this is my ID (255 or  1 == not setup)

// Replace with your network credentials
const char* ssid     = "ESP32-Access-Point";
const char* password = "12345";

// SPI LoRa Radio
#define LORA_SCK 5        // GPIO5 - SX1276 SCK
#define LORA_MISO 19     // GPIO19 - SX1276 MISO
#define LORA_MOSI 27    // GPIO27 -  SX1276 MOSI
#define LORA_CS 18     // GPIO18 -   SX1276 CS
#define LORA_RST 14   // GPIO14 -    SX1276 RST
#define LORA_IRQ 26  // GPIO26 -     SX1276 IRQ (interrupt request)

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
int counter = 0;
#define EEPROM_SIZE 64

byte eepdata[16];
String ipAddr;
String gotPacs[255];

int eeprom_read()
{
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("failed to initialise EEPROM"); delay(1000000);
  }
  Serial.println(" bytes read from Flash . Values are:");
  for (int i = 0; i < 16; i++) {
    eepdata[i]=byte(EEPROM.read(i));
    Serial.print(eepdata[i]); Serial.print(" ");
  }
  Serial.println(); 
  return 0;
}

int eeprom_write(int addr, byte val)
{
  if (addr < EEPROM_SIZE) {
    EEPROM.write(addr, val);
    EEPROM.commit();
  }
}
// Set web server port number to 80
WiFiServer server(80);
WiFiMulti WiFiMulti;

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;
int wifiRunning  = 0;
int wifiConnected = 0;

long baseTime = millis();
long nextTimetoSend = baseTime;

long sendWait = 10000;

void setNextTime()
{
  baseTime += sendWait + random(10);
}

void setBaseTime( int myid, int toid)
{
  baseTime = millis() - (toid * 50);
  nextTimetoSend =  baseTime +(myid * 50);
  //setNextTime();
}

long checkTime(int myid)
{
 return baseTime + (myid  * 50);
}
void wifiScan();

void wifiStaMode(void)
{
   // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

String IP2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}

int wifiMultiSetup()
{
    // We start by connecting to a WiFi network
    WiFiMulti.addAP("Butter2.4", "Butt3r12");
    WiFiMulti.addAP("yourAP", "yourPassword");

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
      ipAddr = IP2String(WiFi.localIP());

    } else {
      wifiConnected = 0;
      Serial.println("");
      Serial.println("WiFi NOT connected");
    
    }
    return wifiConnected;

}

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
    ipAddr = IP2String(WiFi.localIP());
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

//void wifiStaMode(void)
//{
   // Set WiFi to station mode and disconnect from an AP if it was previously connected
//  WiFi.mode(WIFI_STA);
//  WiFi.disconnect();
//  delay(100);
//}


void wifiSetupAP()
{
  Serial.print("Configuring access point...");
  Serial.println(ssid);

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  ipAddr = IP2String(myIP);
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

}
long rnum;
void setup() {
  Serial.begin(115200);
  while (!Serial);
  randomSeed(analogRead(0));
   // Initialize the output variables as outputs
  //pinMode(output26, OUTPUT);
  //pinMode(output27, OUTPUT);
  // Set outputs to LOW
  //digitalWrite(output26, LOW);
  //digitalWrite(output27, LOW);
 //
  // The above line prints:
  // {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}


  Serial.print(progname);
  Serial.println(version);
  wifiScan();
  wifiMultiSetup();
  Serial.print("WifiConnected :");
  Serial.println(wifiConnected);
  
  if (wifiConnected == 0) {
     wifiStaMode();
     wifiSetupAP();
     Serial.print("WifiConnected :");
     Serial.println(wifiConnected);

     IPAddress IP = WiFi.softAPIP();
     Serial.print("AP IP address: ");
     Serial.println(IP);
     ipAddr = IP2String(IP);
  
  } else{
     server.begin();
  }
  
  eeprom_read();
  if (eepdata[0] == 255) {
    myID = 0;
    eeprom_write(0,1);  
    eeprom_write(1,23);
    eeprom_write(2,250);
  
  } else {
    myID = eepdata[0];
  }
  Display.begin();
  Display.enableUTF8Print();    // enable UTF8 support for the Arduino print() function
  Display.setFont(u8g2_font_ncenB10_tr);
  
  // Very important for SPI pin configuration!
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS); 
  
  // Very important for LoRa Radio pin configuration! 
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);         

  pinMode(blueLED, OUTPUT); // For LED feedback

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  // The larger the spreading factor the greater the range but slower data rate
  // Send and receive radios need to be set the same
  LoRa.setSpreadingFactor(12); // ranges from 6-12, default 7 see API docs

  // Change the transmit power of the radio
  // Default is LoRa.setTxPower(17, PA_OUTPUT_PA_BOOST_PIN);
  // Most modules have the PA output pin connected to PA_BOOST, gain 2-17
  // TTGO and some modules are connected to RFO_HF, gain 0-14
  // If your receiver RSSI is very weak and little affected by a better antenna, change this!
  LoRa.setTxPower(14, PA_OUTPUT_RFO_PIN);
  
}

void wifiScan()
{
  int n = WiFi.scanNetworks();

  if (n == 0) {
    Serial.println("Searching networks . . .");
  } else {
    Serial.println("Networks found:");  
    for (int i = 0; i < n; ++i) {
      // Print SSID for each network found
      char currentSSID[64];
      WiFi.SSID(i).toCharArray(currentSSID, 64);

      Serial.println(currentSSID);
    }
    
  }
}

void serveWeb()
{
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              //digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              //digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              //digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              //digitalWrite(output27, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Pump Web Server ("+ String(myID) +") </h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>GPIO 26 - State " + output26State + "</p>");
            // If the output26State is off, it displays the ON button       
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>GPIO 27 - State " + output27State + "</p>");
            // If the output27State is off, it displays the ON button       
            if (output27State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
}
}

String rssi= "";

void sendPacket(int toID) {
    String packet = "";
    Serial.print("\nSending packet: ");
    Serial.println(counter);
    DynamicJsonDocument  doc(200);

    // Add values in the document
    //
    doc["myID"]=myID;
    doc["toID"]=toID;
    //doc["ipAddr"]=ipAddr;
    //doc["sensor"] = "gps";
    doc["time"] = millis();

    // Add an array.
    //
    //JsonArray data = doc.createNestedArray("data");
    //data.add(48.756080);
    //data.add(2.302038);
    //JsonArray cdata = doc.createNestedArray("counter");
    //cdata.add(counter);

    // Generate the minified JSON and send it to the Serial port.
    serializeJson(doc, Serial);

    digitalWrite(blueLED, ON);  // Turn blue LED on
    // send packet
    LoRa.beginPacket();
    serializeJson(doc, LoRa);

    //LoRa.print("HeLoRa! ");
    //LoRa.print(counter);
    LoRa.endPacket();
    digitalWrite(blueLED, OFF); // Turn blue LED off

    // Display Info
    Display.clearBuffer();  
    Display.setCursor(0,12); Display.print("RxTxOTA :"+(String)myID);
    Display.setCursor(0,30); Display.print("Sent # " + (String)counter);
    Display.sendBuffer();
    counter++;
}

int getPacket()
{
  int ret = -1;  
  String pack = "";                   // Clear packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.println("LoRa available");
    
    while (LoRa.available()) {
      pack += (char)LoRa.read(); // Assemble new packet
    }
    Serial.println("LoRa pack [" + pack + "]");
    
    rssi = LoRa.packetRssi();
    StaticJsonDocument<240> rdoc;
    DeserializationError error = deserializeJson(rdoc, pack);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        //return;
        //counter = -1;
        ret = 0;
      } else {
        Serial.println("Received packet:"+ pack + "' with RSSI " + rssi);   
        if (rdoc.containsKey("myID") ) {
          ret = rdoc["myID"];       
        } else {
          ret = 1;
        }
      }
  }
  return ret;
}

int timetoSend = 0;
int timetoSync = 0;
//if myID == 1 or myID = 255
// first wait 10 seconds and get packets if any
//
//String gotPacs[255];
String SerIn = "";

void loop() {
  
  int id;
  char inByte;
  if(Serial.available()>0) {
    inByte = Serial.read();
    SerIn += inByte;
    Serial.print(" Got [" + SerIn + "] :");
    Serial.print(inByte, DEC);
    if (inByte == 10) {
      StaticJsonDocument<240> rdoc;
      DeserializationError error = deserializeJson(rdoc, SerIn);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        //return;
        //counter = -1;
        //ret = 0;
      } else {
        if (rdoc.containsKey("id") ) {
          myID = rdoc["id"];       
          eeprom_write(0,myID);  

        }
     }
     SerIn = "";
   }
  }
  serveWeb();
  
  id = getPacket();
  if ((id >  1) && ( id < 255)){
      Serial.print(" Got packet id :");
      Serial.println(id);
      setBaseTime( myID, id);
      //sendWait = 10000;
      //setNextTime();
  }
//    if(timetosend > 10) timetosend = 10;

  if (millis() > checkTime(myID) ) {
    sendPacket(255);
    //timetoSend=0;
    //nextTimetoSend = 500 + random(100);
    setNextTime();

  }
  delay(2);
}


