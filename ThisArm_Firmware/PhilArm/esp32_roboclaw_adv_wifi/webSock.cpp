#include "webSock.h"
#include "SPIFFS.h"


WebSocketsServer webSocket = WebSocketsServer(81);
//void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length);

String wsdata = "";

void webSockSetup()
{
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

int webSockSend(char *sp) {
  int rc = webSocket.broadcastTXT((char *)sp);
  return rc;
}

void webSockLoop() {
  webSocket.loop();
}


int spiffsWrite(String &fname, String &fdata)
{
  int rc = -1;
  File file = SPIFFS.open(fname, FILE_WRITE);
  rc = file.print(fdata);
  file.close();
  return rc;
}

int spiffsRead(String &fname, String &fdata)
{
  int rc = -1;
  fdata = "";
  File file = SPIFFS.open(fname, FILE_READ);
  if (file) {
    rc = 0;
    while(file.available()){
      fdata += char(file.read());
      rc++;
    }
  }
  file.close();
  return rc;
}

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
      for(int i = 0; i < length; i++) { wsdata += (char) payload[i]; }
      wsdata += "\n";
      DeserializationError error = deserializeJson(doc, (char *)payload);
      if (error) {
         wsdata += "deserializeJson() failed: ";
         wsdata += error.c_str();
         //return;
      } else {
         wsdata +="deserializeJson() OK: \n";
         //webSocket.broadcastTXT(doc["sensor"].as<char *>());

      }
      String cmd=doc["cmd"];
      if (cmd == "filewrite") {
         wsdata +="FILEWRITE:";
         String fname = doc["name"];
         String fdata = doc["data"];
         wsdata +="name:";
         wsdata +=fname;
         wsdata +="\n";
         int rc = spiffsWrite(fname, fdata);
         if (rc > 0 ) {
            wsdata +=" spifs write  OK: \n";
         } else {
            wsdata +=" spifs write  FAIL: \n";
         }
      }
      
      // now look for cmd = file act = write name = xxx data = 

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
