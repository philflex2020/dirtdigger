#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "sockMenu.h"

extern String wsdata;

/*
 * Wemos Mini Pins 
 *     RST                            GPIO 1
 *     ADC0                           GPIO 3  
 *     GPIO 16                        GPIO 5
 *     GPIO 14                        GPIO 4  
 *     GPIO 12                        GPIO 0
 *     GPIO 13                        GPIO 2 --led
 *     GPIO 15                        GND
 *     3.3 V                          5.0V
 *     
 *     
 */

#ifndef STASSID
#define STASSID "Butter2.4"
#define STAPSK  "Butt3r12"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char *host ="ESP8266OTA";
int led_pin = D4;
int t1pin = D3;
int t2pin = D2;

#define N_WLS 3
int wl_pin[] = {D1, D3, D5};
int wifiServ = 5009;

WiFiServer wserv(wifiServ);

void seeBuf(struct wConn &wc, void *data) {
  String s_str  = " Showing buffers:";
  writeString(s_str, wc);
}


void setup() {
  Serial.begin(115200);
  Serial.print("Booting :");
  Serial.println(__FILE__);

  pinMode(led_pin, OUTPUT);
  pinMode(t1pin, OUTPUT);
  pinMode(t2pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  digitalWrite(t1pin, LOW);
  delay(1000);
  digitalWrite(led_pin, HIGH);
  digitalWrite(t1pin, HIGH);
  digitalWrite(t2pin, HIGH);
  
  WiFi.mode(WIFI_STA);
  WiFi.hostname(host);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }  
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  //ArduinoOTA.setHostname("ESPOTA");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  setupWserv(wserv);
  addWcmd("sb", "See Buffers",seeBuf,NULL);
  
}


int stime = millis();
int ledon = 0;
int ton = 0;
int sint = 10;
int sled = led_pin;
int scount = 0;
int oncount = 0;

int sense_pin = A0;
int val;
int maxval = 0;

int pcount = 0;
int pnum = 0;
#define NUM_PINS 3
int pins[NUM_PINS] = {led_pin, t1pin, t2pin};
float vals [NUM_PINS];
float maxvals[NUM_PINS] = {0,0,0};
float lvals [NUM_PINS];
float maxlvals[NUM_PINS] = {0,0,0};

int handleLed()
{
  if ((millis() -stime) > sint) {
    val = analogRead(sense_pin);
    if (val > maxval) maxval=val;
    maxval = maxval - (maxval /10);
    if(digitalRead(sled) == HIGH) {
      vals[pnum] = val;
      if (val > maxvals[pnum]) maxvals[pnum]+=(val-maxvals[pnum])/10.0;
      maxvals[pnum] = maxvals[pnum] - (maxvals[pnum] /100.0);
    } 
    else {
      lvals[pnum] = val;
      if (val > maxlvals[pnum]) maxlvals[pnum]+=(val-maxlvals[pnum])/10.0;
      maxlvals[pnum] = maxlvals[pnum] - (maxlvals[pnum] /100.0);
      
    }
    stime = millis();
    scount++;
    if(scount>200) {
       scount = 0;
       digitalWrite(sled, LOW);
       ledon = 0;
       pnum ++;
       if (pnum>=NUM_PINS) pnum = 0;
       digitalWrite(sled, (LOW));
       sled = pins[pnum];
       digitalWrite(sled, (LOW));
 
    }
    oncount++;
    if(oncount >50) {
      oncount = 0;   
      digitalWrite(sled, (digitalRead(sled) == LOW));
    }
    pcount++;
    if(pcount > 10) {
      pcount = 0;
      for (int ix = 0; ix < NUM_PINS; ix++ ) {
         if(ix>0) 
            Serial.print(",");  
      
         Serial.printf("%2.3f,%2.3f", maxvals[ix] , vals[ix]);
      

      }
      Serial.print("\n");
    }
  }
}

#include <WiFiUdp.h>

int bcastMillis = 0;
int bcastDel = 3000;

WiFiUDP Udp;
unsigned int localUdpPort = 4211;  // local port to listen on
//char incomingPacket[255];  // buffer for incoming packets
IPAddress IP_Bcast(192, 168, 1, 255);
int port_Bcast= 4511;
int bcnt = 21;

void handleBcast()
{
  int nowM = millis();
  if (bcastMillis == 0) {
    bcastMillis = nowM;
    return;
  }
  
  if((nowM - bcastMillis)  > bcastDel) {
    char buf_Bcast[64];
    snprintf(buf_Bcast,64,"{\"msg\":\"Hi\",\"id\":%d,\"ip\":\"%s\",\"port\":%d}\n",bcnt++, WiFi.localIP().toString().c_str(), wifiServ);
    bcastMillis = nowM;
    Udp.beginPacket(IP_Bcast, port_Bcast); // subnet Broadcast IP and port
    Udp.write(buf_Bcast, strlen(buf_Bcast));
    Udp.endPacket();

  }

}
void loop() {
  ArduinoOTA.handle();
  handleLed();
  handleWserv(wserv);
  handleBcast();
}
