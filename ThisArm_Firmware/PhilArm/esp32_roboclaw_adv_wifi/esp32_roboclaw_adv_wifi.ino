
#include <Wire.h>
#include "FS.h"

#include <ArduinoJson.h>

#ifdef ESP32

#include <ArduinoOTA.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "SPIFFS.h"
//#include <Wire.h>


#else

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "ESP8266OTA.h"
#include <ESP8266HTTPUpdateServer.h>

#endif

#include <WiFiClient.h>
//#include <EthernetClient.h>
//#include <WebSocketsServer.h>
//#include "LiquidCrystal_I2C.h"
#include "webSock.h"


//https://github.com/Links2004/arduinoWebSockets
// 
//#include <Update.h>
#include "RoboClaw.h"
#include "sockMenu.h"
#include "home_page.h"
#include "home_page2.h"

extern String wsdata; 

//void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length);
//void webSockSetup();

//int webSockSend(char *sp);
//void webSockLoop();
/*
 * banggood motor
 * https://www.banggood.com/
 * 12V-320rpm12V-107rpm6V-160rpm-DC-Gear-Motor-Encoder-Motor-with-Mounting-Bracket-and-Wheel-p-1059966.html
 * 
 *   red motor +
 *   black enc ground
 *   yellow encoder a phase
 *   green encoder b phase
 *   blue encoder 5v
 *   white motor -
 *   Roboclaw
 *         X X    lb + -
 *         x x    enc power + -
 *         x x    enc power + -
 *         x x    m1 enc a  b
 *         x x    m2 enc a  b
 *       x x x    S1 Serial Rx  
 *       x x x    S2 Serial Tx  
 *       x x x    S3 EStop  
 *       x x x    S4 m1 home/limit   
 *       x x x    S5 m2 home/limit   
 */
 
/* 
 *   
 *  esp 32s
 *            3v3               gnd
 *            en                gpio23 - vspimosi
 *   adc0 gpio36                gpio22 
 *   adc3 gpio39                gpio1 -  txd  (S1) 
 *   adc6 gpio34                gpio3 -  rxd  (S2)
 *   adc7 gpio35                gpio21    
 *   adc4 gpio32                gnd 
 *   adc5 gpio33                gpio19 - vspimiso 
 *  adc18 gpio25                gpio18 - vspi sck 
 *  adc19 gpio26                gpio5  
 *  adc17 gpio27                gpio17  - i2c sda
 *  adc16 gpio14                gpio16  - i2c scl
 *  adc15 gpio12                gpio4 -  adc10 
 *           gnd                gpio0 -  adc11 
 *  adc14 gpio13                gpio2 -  adc12 
 *  fl d2 gpio9                 gpio15-  adc13
 *  fl d3 gpio10                gpio8 -  fl d1
 *  flmcd gpio11                gpio7 -  fl d0
 *        vin 5v                gpio6 -  fl clk
 */
//#define RC_SDA 17
//#define RC_SCL 16 

//

void show_lcd(int r, int c, String msg);

void setup_lcd();

RoboClaw roboclaw(&Serial, 10000);

//const char pw[] = "basicmicro";
//const char ap_name[] = "esp8266_test";
const char host[] = "philRc2esp2";
const char ssid[] = "Butter2.4";
const char pass[] = "Butt3r12";

//WebSocketsServer webSocket = WebSocketsServer(81);
//ESP8266WebServer server(80);
WebServer server(80);
WiFiServer wserv(8090);
//ESP8266WebServer httpServer(8080);
//ESP8266HTTPUpdateServer httpUpdater;

// number direction [speed [ dist]]
void motorRun(struct wConn &wc, void *data) {
  uint32_t speed = 0;
  uint32_t dist = 0;
  String v_str = "";
  if (wc.nwords > 2) {
    speed = atoi((const char *)wc.swords[2]);
  }
  
  if (wc.nwords > 3) {
    dist = atoi((const char *)wc.swords[3]);
  }
  
  v_str ="Speed:"+ String(speed)+ " Dist:"+ String(dist);
  
  roboclaw.SetEncM1(0x80, 0);
  if((speed>0) && (dist>0)) {
     roboclaw.SpeedDistanceM1(0x80, speed, dist);

  } else if((speed>0)) {
    roboclaw.SpeedM1(0x80, speed);
  }
  v_str +="\n";

  writeString(v_str, wc);
  
}
// number direction [speed [ dist]]
// speed accel  dist
void motorSac(struct wConn &wc, void *data) {
  uint32_t speed = 0;
  uint32_t accel = 0;
  uint32_t dist = 0;
  String v_str = "";
  if (wc.nwords > 1) {
    speed = atoi((const char *)wc.swords[1]);
  }
  if (wc.nwords > 2) {
    accel = atoi((const char *)wc.swords[2]);
  }
  
  if (wc.nwords > 3) {
    dist = atoi((const char *)wc.swords[3]);
  }
  v_str ="Speed:"+ String(speed) + " Accel:"+ String(accel) + " Dist:"+ String(dist);
  
  if((speed>0) && (dist>0) &&(accel > 0)) {
     roboclaw.SetEncM1(0x80, 0);
     roboclaw.SpeedAccelDeccelPositionM1(0x80, speed, accel, accel, dist, 0);

  }
  v_str +="\n";
  writeString(v_str, wc);
  
}

void motorVolt(struct wConn &wc, void *data) {
  int voltage = 0;
  voltage = roboclaw.ReadMainBatteryVoltage(0x80);
  float mainvoltage_f = (float)voltage/10;
  voltage = roboclaw.ReadLogicBatteryVoltage(0x80);
  float logicvoltage_f = (float)voltage/10;
  String v_str = " Main:"+ String(mainvoltage_f)+" Logic:"+ String(logicvoltage_f) + "\n";
  writeString(v_str, wc);

}
void scan_i2c(struct wConn &wc, void *data) {
  byte error, address;
  int nDevices;
  String v_str = "I2C Scanning...\n";
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      v_str += "I2C device found at address 0x";
      if (address<16) {
        v_str+= "0";
      }
      v_str += String(address,HEX);
      v_str+= "\n";

      nDevices++;
    }
    else if (error==4) {
      v_str += "Unknow error at address 0x";
      if (address<16) {
        v_str += "0";
      }
      v_str += String(address,HEX);
      v_str+= "\n";

    }    
  }
  if (nDevices == 0) {
    v_str += "No I2C devices found\n";
  }
  else {
    v_str += "done\n";
  }
  writeString(v_str, wc);

//  delay(5000);          
}

void handleHome(){
  String page = home_page;
  server.send(200, "text/html",page);
}
void handleHome2(){
  String page = home_page2;
  server.send(200, "text/html",page);
}


void handleEncoder_1(){
  int enc_value = roboclaw.ReadEncM1(0x80);
  String value = String(enc_value);
  server.send(200, "text/plain", value);
    
}

void handleEncoder_2(){
  int enc_value = roboclaw.ReadEncM2(0x80);
  String value = String(enc_value);
  server.send(200, "text/plain", value);
    
}

void handleMotor(){
  String motor_channel = server.arg("motorChannel");
  String motor_state = server.arg("motorState");
  if(motor_channel == "1") {
    if(motor_state == "1") {
      roboclaw.ForwardM1(0x80, 64);
    }
    else if(motor_state == "0") {
      roboclaw.ForwardM1(0x80, 0);
    }
    else if(motor_state == "-1") {
      roboclaw.BackwardM1(0x80, 64);
    }

  }

  else if(motor_channel == "2") {
     if(motor_state == "1") {
      roboclaw.ForwardM2(0x80, 64);
    }
    else if(motor_state == "0") {
      roboclaw.ForwardM2(0x80, 0);
    }
  }
  
}
// webSock.cpp
int spiffsRead(String &fname, String &fdata);

void handleGetConfig() {
  String fdata = "";
  String fname = "/config.json";
  int rc = spiffsRead(fname, fdata);
  if (rc <= 0) {
     fdata  = "{\"hostname\":\"mrfoo\",\"SSID\":\"Butter2.4\",\"Passwd\":\"myPasswd\"}";
  }  
  server.send(200, "text/plain", fdata);
}

void handleSetConfig() {

  String cs = "{";
  for (int i = 0; i < server.args(); i++) {
    if (i > 0) {
       cs+= ",";
    }
    //cs += "Arg num " + (String)i + " –> ";
    cs  += "\""+server.argName(i) + "\":\"";
    cs += server.arg(i) + "\"";

  } 
  cs += "}";
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "OK\r\n");
  //server.send(200, "text/plain", cs);
  webSockSend((char *)cs.c_str());
    
  //String temp_str = "{\"hostname\":\"mrfoo\",\"SSID\":\"Butter2.4\",\"Passwd\":\"myPasswd\"}";
  //server.send(200, "text/plain", temp_str);
}

void handleTemp() {
  uint16_t temp = 0;
  roboclaw.ReadTemp(0x80, temp);
  float temp_f = (float)temp/10;
  String temp_str = String(temp_f);
  server.send(200, "text/plain", temp_str);
}

void handleVoltage() {
  int voltage = 0;
  voltage = roboclaw.ReadMainBatteryVoltage(0x80);
  float voltage_f = (float)voltage/10;
  String voltage_str = String(voltage_f);
  server.send(200, "text/plain", voltage_str);
}

int g_spiffsOK;
void setup_SPIFFS() {
  g_spiffsOK = SPIFFS.begin();
}

String getSpiffsInfo(int fmt) {
   String rstr = "";
   if(fmt) {
       SPIFFS.format() ;
       rstr +="File system formatted.\n";
   }
  // Get all information of your SPIFFS
#ifndef ESP32
    FSInfo fs_info;
    SPIFFS.info(fs_info);
 
    rstr +="File system info.\n";
    rstr +="Total space:      " + String(fs_info.totalBytes) + " bytes\n";
    rstr +="Total space used: " + String(fs_info.usedBytes)  + " bytes\n";
 
    rstr +="Block size: " + String(fs_info.blockSize)  + " bytes\n";
    rstr +="Page size : " + String(fs_info.totalBytes)  + " bytes\n";
    rstr +="Max open files:" + String(fs_info.maxOpenFiles)  + "\n";
    rstr +="Max PATH LENGTH:" + String(fs_info.maxPathLength)  + " bytes\n";
#endif
    File testFile = SPIFFS.open(F("/testCreate.txt"), "r");
 
    if (testFile){
        rstr +="Read file content!\n";
        //testFile.print("Here the test text!!");
        testFile.close();
 
    }else{
      rstr += "Problem with read file!\n";
      testFile = SPIFFS.open(F("/testCreate.txt"), "w");
    
      if (testFile){
        testFile.print("Here the test text!!");
        testFile.close();
        rstr +="Able to write file content!\n";
      } else {
        rstr += "Problem with write file!\n";      
      }
    }
#ifdef ESP32
    File root = SPIFFS.open("/","r");
    if (root) {
      // Cycle all the content
      if (root.isDirectory()) {
        File file = root.openNextFile();
        while (file) {
          // get filename
          rstr += (String)file.name();
          rstr += " - ";

          // If element have a size display It else write 0
          if(file.size()) {
          
             rstr += String(file.size());

          }else{
              rstr += "0";
          }
        
          rstr += "\n";
          file = root.openNextFile();
        }
      } else {
        rstr += "Root not a  directory \n";
  
      }
    } else {
        rstr += "Failed to open directory \n";
    }


#else
    // Open dir folder
    Dir dir = SPIFFS.openDir("/");
    // Cycle all the content
    while (dir.next()) {
        // get filename
        rstr += dir.fileName() + " - ";

        // If element have a size display It else write 0
        if(dir.fileSize()) {
          
           File f = dir.openFile("r");
           rstr += String(f.size());

            f.close();
        }else{
            rstr += "0";
        }
        rstr += "\n";
    }
#endif    
    return rstr;
}

void spiffsInfo(struct wConn &wc, void *data) {
  int fmt = 0;
  if (wc.nwords > 1) fmt = 1;
  String v_str = getSpiffsInfo(fmt);
  writeString(v_str, wc);
}
int readAdc(void);
int getAdc(int idx);

void adcInfo(struct wConn &wc, void *data) {
  String v_str = " ADC input values :\n";
  readAdc();
  
  for (int i = 0; i < 4 ; i++) {
      int val = getAdc(i); 
      v_str += String(i) + " ->";
      v_str += String(val);
      v_str += "\n";
  }
  writeString(v_str, wc);
}

void wsSend(struct wConn &wc, void *data) {
  if (wc.nwords > 1) {
    //String v_str = String(wc.swords[1]);
    webSockSend((char *)wc.swords[1]);
//    webSocket.broadcastTXT((char *)wc.swords[1]);
    writeString((char *)wc.swords[1], wc);
  }
}

//String wsdata="";

void wsData(struct wConn &wc, void *data) {
  writeString(wsdata, wc);
  wsdata = "";
}

int setupAdc();
void setupIRQ(int rate);

char * hostp = (char *)host;

void setup() {
  Wire.begin(17, 16);
  setup_SPIFFS();
  roboclaw.begin(38400);
  //Serial.swap();
  // Connect to a WiFi network
  //Serial.print(F("Connecting to "));  Serial.println(ssid);
  setupAdc();
  setup_lcd();
  show_lcd(1,1,"Hello LCD");
  setupIRQ(10000);

#ifdef ESP32
  hostp ="philRcesp32";
  WiFi.setHostname(hostp);
#else
  WiFi.hostname(hostp);
#endif
  WiFi.begin(ssid,pass);
  //WiFi.softAP(ap_name, pw);

  server.on("/", handleHome);
  server.on("/2", handleHome2);

  server.on("/readEncoder_1", handleEncoder_1);
  server.on("/readEncoder_2", handleEncoder_2);
  server.on("/readTemp", handleTemp);
  server.on("/readVoltage", handleVoltage);
  server.on("/handleMotor", handleMotor);
  server.on("/config", HTTP_GET, handleGetConfig);
  server.on("/config", HTTP_POST, handleSetConfig);

  server.begin();

  MDNS.begin(host);

  //httpUpdater.setup(&httpServer);
  //httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);

  setupWserv(wserv);
  addWcmd("mv", "Main Volts", motorVolt, NULL);
  addWcmd("mr", "run motor mr 1 fwd speed dist", motorRun, NULL);
  addWcmd("msac", "run speed accel dist", motorSac, NULL);
  addWcmd("sp", "spiffs Info", spiffsInfo, NULL);
  addWcmd("adc", "adc Info", adcInfo, NULL);
  addWcmd("wss", "send arg 1 to ws", wsSend, NULL);
  addWcmd("wsd", "show ws data", wsData, NULL);
  addWcmd("i2c", "show i2c bus", scan_i2c, NULL);

#if 1
  if(1) {
  //ArduinoOTA.setHostname("8622Robo");
  ArduinoOTA.onStart([]() {
    if(0)Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    if(0)Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    if(0)Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    if(0)Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) if(0)Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) if(0)Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) if(0)Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) if(0)Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) if(0)Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  }
#endif
  webSockSetup();
  //webSocket.begin();
  //webSocket.onEvent(webSocketEvent);
}

//String wsdata="";
#if 0
void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length)
{
  const size_t capacity = 1024;//JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);
  if(type == WStype_TEXT)
  {
      wsdata ="got Text";
      //for (int i = 0; i < length;i++) {
      //  Serial.print(payload[i]);        
      //}
      wsdata +="WS payload = ";
      for(int i = 0; i < length; i++) { Serial.print((char) payload[i]); }
      Serial.println();
      DeserializationError error = deserializeJson(doc, (char *)payload);
      if (error) {
         wsdata += "deserializeJson() failed: ";
         wsdata += error.c_str();
         //return;
      } else {
         wsdata +="deserializeJson() OK: ";
         //webSocket.broadcastTXT(doc["sensor"].as<char *>());

      }


      if (payload[0] == '0')
      {
          //digitalWrite(pin_led, LOW);
          //LEDstatus = LOW;
          wsdata += "LED=off";        
      }
      else if (payload[0] == '1')
      {
          //digitalWrite(pin_led, HIGH);
          //LEDstatus = HIGH;
          //Serial.println("LED=on");
          wsdata += "LED=on";        
      }        
      
  }
 
  else 
  {
    wsdata += "WStype = ";
    wsdata += String(type);  
    wsdata += "WS payload = ";
    for(int i = 0; i < length; i++) { wsdata += payload[i]; wsdata += "\n"; }
    //Serial.println();
 
  }
  webSocket.broadcastTXT((char *)"Hello from ws");
}
#endif

void loop() {
  handleWserv(wserv);

  server.handleClient();
  //httpServer.handleClient();
#ifdef ESP32
#else
  MDNS.update();
#endif
  //webSocket.loop();
  webSockLoop();
  
#if 1 
  ArduinoOTA.handle();
#endif

}
