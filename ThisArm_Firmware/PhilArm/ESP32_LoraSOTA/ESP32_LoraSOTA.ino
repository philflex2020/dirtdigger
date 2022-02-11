#include <Bridge.h>
#include <BridgeClient.h>
#include <BridgeServer.h>
#include <BridgeSSLClient.h>
#include <BridgeUdp.h>
#include <Console.h>
#include <FileIO.h>
#include <HttpClient.h>
#include <Mailbox.h>
#include <Process.h>
#include <YunClient.h>
#include <YunServer.h>

/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/ttgo-lora32-sx1276-arduino-ide/

gnd              --       gnd
5v               --       5v
3v3              --       3v3
adc1-0 gpio36    --        gnd
adc1-1 gpio37    --        rx
adc1-2 gpio38    --        tx
adc1-3 gpio39    --        rst
adc1-6 gpio34    --             gpio0 -adc2_1 button
adc1-7 gpio35    --        scl  gpio22
adc1-4 gpio32    --             gpio19       lora mosi
adc1-5 gpio33    --             gpio23
LED    gpio25    --             gpio18       lora cs
LORA IRQ  gpio26    --          gpio5      lora sclk
LORA MOSI gpio27    --          gpio15     OLED SCL
LORA RST  gpio14    --          gpio2    
adc2_5    gpio12    --          gpio4      OLED SDA
adc2_4    gpio13    --          gpio17     u2 txd
sda       gpio21    --          gpio16     OLED RST


*********/
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "sockMenu.h"
#include "udpbcast.h"
#include "TM1638.h"

extern String wsdata;
const char *version ="0.0.1";
const char *project = "ESP32_LoraSOTA";
int wifiServ = 5009;

WiFiServer wserv(wifiServ);

// led pin
#define MY_LED 2
// adc pin

#define myADC0 36
#define myADC1 37
#define gpioADC 17

#ifdef ESP32

#endif

const char* ssid = "Butter2.4";
const char* password = "Butt3r12";

void seeBuf(struct wConn &wc, void *data) {
  String s_str  = " Showing buffers:\n";
  digitalWrite(MY_LED, (digitalRead(MY_LED) == LOW));
  writeString(s_str, wc);
}
//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 915E6

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//packet counter
int counter = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

int adcMillis = 0;
int adcDel = 50;
int adcval[2] = {0,0};
int gpioVal = 0;
int gpioTrig = 0;

void readADC()
{
  int nowM = millis();
  if (adcMillis == 0) {
    adcMillis = nowM;
    //return;
  }
  else 
  {
    if ((nowM - adcMillis)  > adcDel) {
       
       adcval[0] = analogRead(myADC0);
       adcval[1] = analogRead(myADC1);
       adcMillis = nowM;
       if(gpioTrig > 0) {
          if (digitalRead(MY_LED)==HIGH) 
          {
            gpioVal = 4095;
           } else{
            gpioVal = 0;
          }
          gpioTrig = 0;
          digitalWrite(gpioADC, (digitalRead(gpioADC) == LOW));
          digitalWrite(MY_LED, (digitalRead(MY_LED) == LOW));          
       }
       Serial.print(gpioVal);
       Serial.print(","); 
       Serial.print(adcval[0]);
       Serial.print(",");
       Serial.println(adcval[1]);

    }
  }    
}


void seeAdc(struct wConn &wc, void *data) {
  String s_str  = " ADC 1,2: [";
  s_str += String(gpioVal);
  s_str+= ",";
  s_str += String(adcval[0]);
  s_str+= ",";
  s_str += String(adcval[1]);
  s_str+= "]\n";
  writeString(s_str, wc);
  //Serial.print(gpioVal);
  //Serial.print(","); 
  //Serial.print(adcval[0]);
  //Serial.print(",");
  //Serial.println(adcval[1]);
  //digitalWrite(MY_LED, (digitalRead(MY_LED) == LOW));
  //digitalWrite(gpioADC, (digitalRead(gpioADC) == LOW));
  gpioTrig = 1;
}
// define a module on data pin 15, clock pin 23 and strobe pin 19
TM1638 module(15, 23, 19);

void setup() {
 Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  //const int TLMstrobe = 19;
  //const int TLMclock = 23;
  //const int TLMdata = 15;
  module.setDisplayToHexNumber(0x1234ABCD, 0xF0);
  //byte keys = module.getButtons();

  // light the first 4 red LEDs and the last 4 green LEDs as the buttons are pressed
  //module.setLEDs(((keys & 0xF0) << 8) | (keys & 0xF));
  pinMode(MY_LED, OUTPUT);
  digitalWrite(MY_LED, LOW);
  pinMode(gpioADC, OUTPUT);
  digitalWrite(gpioADC, LOW);

  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
  digitalWrite(MY_LED, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA SENDER ");
  display.display();
  
  
  
  Serial.println(__FILE__);
  Serial.println("ESP32 LoRa Sender Test");

  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");
  display.setCursor(0,10);
  display.print("LoRa Initializing OK!");
  display.display();
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  char localIP[32];
  IPAddress ip = WiFi.localIP();
  snprintf(localIP,32,"%d.%d.%d.%d"
       , ip[0]
       , ip[1]
       , ip[2]
       , ip[3]
       );
  Serial.println(localIP);
  
  //snprintf(localIP,32,"%s", "not known");
  udpbcastIP(localIP);
  //TLMsetup();
  //TLMloop();
  
  setupWserv(wserv);
  addWcmd("sb", "See Buffers", seeBuf, NULL);
  addWcmd("sa", "read adc", seeAdc, NULL);

  delay(2000);
}

int loraMillis = 0;
int loraDel = 5000;

void handleLora() {
  
  int nowM = millis();
  if (loraMillis == 0) {
    loraMillis = nowM;
    //return;
  }
  else {
    if ((nowM - loraMillis)  > loraDel) {
      loraMillis = nowM;
    
      //Serial.print("Sending packet: ");
      //Serial.println(counter);

      //Send LoRa packet to receiver
      LoRa.beginPacket();
      LoRa.print("hello ");
      LoRa.print(counter);
      LoRa.endPacket();
  
      display.clearDisplay();
      display.setCursor(0,0);
      display.println("LORA SENDER");
      display.setCursor(0,20);
      display.setTextSize(1);
      display.print("LoRa packet sent.");
      display.setCursor(0,30);
      display.print("Counter:");
      display.setCursor(50,30);
      display.print(counter);      
      display.display();

      counter++;
   }
  }
} //handle lora

void loop() {
   ArduinoOTA.handle();
   handleWserv(wserv);
  //handleWserv(wserv);
  handleBcast();
  readADC();
  handleLora(); 
}
