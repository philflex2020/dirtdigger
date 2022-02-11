/*
 * http://www.martyncurrey.com/esp8266-and-the-arduino-ide-part-9-websockets/
 * Sketch: ESP8266_Part9_02_Websocket_LED_2Way
 * Intended to be run on an ESP8266
 * time sync here
 * https://www.geekstips.com/arduino-time-sync-ntp-server-esp8266-udp/
 */

String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

String html_1 = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
  <meta charset='utf-8'>
  <style>
    body     { font-size:120%;} 
    #main    { display: table; width: 300px; margin: auto;  padding: 10px 10px 10px 10px; border: 3px solid blue; border-radius: 10px; text-align:center;} 
    #BTN_LED { width:200px; height:40px; font-size: 110%;  }
    p        { font-size: 75%; }
  </style>
  <title>Websockets</title>
</head>
<body>
  <div id='main'>
    <h3>LED CONTROL</h3>
    <div id='content'>
      <p id='LED_status'>LED is off</p>
      <button id='BTN_LED'class="button">Turn on the LED</button>
    </div>
    <p>Recieved data = <span id='rd'>---</span> </p>
    <br />
   </div>
</body>

 <script>
  var Socket;
  function init() 
  {
    Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
    Socket.onmessage = function(event) { processReceivedCommand(event); };
  }
 
 
function processReceivedCommand(evt) 
{
    document.getElementById('rd').innerHTML = evt.data;
    if (evt.data ==='0') 
    {  
        document.getElementById('BTN_LED').innerHTML = 'Turn on the LED';  
        document.getElementById('LED_status').innerHTML = 'LED is off';  
    }
    if (evt.data ==='1') 
    {  
        document.getElementById('BTN_LED').innerHTML = 'Turn off the LED'; 
        document.getElementById('LED_status').innerHTML = 'LED is on';   
    }
}
 
 
  document.getElementById('BTN_LED').addEventListener('click', buttonClicked);
  function buttonClicked()
  {   
    var btn = document.getElementById('BTN_LED')
    var btnText = btn.textContent || btn.innerText;
    if (btnText ==='Turn on the LED') { btn.innerHTML = 'Turn off the LED'; document.getElementById('LED_status').innerHTML = 'LED is on';  sendText('1'); }  
    else                              { btn.innerHTML = 'Turn on the LED';  document.getElementById('LED_status').innerHTML = 'LED is off'; sendText('0'); }
  }
 
  function sendText(data)
  {
    Socket.send(data);
  }
 
 
  window.onload = function(e)
  { 
    init();
  }
</script>
 
 
</html>
)=====";
// timelib from https://github.com/PaulStoffregen/Time.git
// Hack copy the Adafruit_ADS1015.h .cpp into the local dir
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "Adafruit_ADS1015.h"
#include "TimeLib.h"

WiFiServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
 
byte pin_led = D3;
byte pin_switch = D6;
 
boolean LEDstatus = LOW;
boolean oldSwitchState = LOW;
boolean newSwitchState1 = LOW;
boolean newSwitchState2 = LOW;
boolean newSwitchState3 = LOW;
  
char ssid[] = "Butter2.4";
char pass[]= "Butt3r12";

unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServerIP;
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];
WiFiUDP udp;

void setup()
{
  pinMode(pin_led, OUTPUT);
  digitalWrite(pin_led,LEDstatus);
 
  pinMode(pin_switch, INPUT);
 
  Serial.begin(115200);
  Serial.println();
  Serial.print("File:");Serial.println(__FILE__);

  Serial.println();
  Serial.println("Serial started at 115200");
  Serial.println();
 
  // Connect to a WiFi network
  Serial.print(F("Connecting to "));  Serial.println("ssid");
  WiFi.hostname("philWs1");
  WiFi.begin(ssid,pass);
 
 
 
 
  // connection with timeout
  int count = 0; 
  while ( (WiFi.status() != WL_CONNECTED) && count < 17) 
  {
      Serial.print(".");  delay(500);  count++;
  }
 
  if (WiFi.status() != WL_CONNECTED)
  { 
     Serial.println("");  Serial.print("Failed to connect to ");  Serial.println(ssid);
     while(1);
  }
 
  Serial.println("");
  Serial.println(F("[CONNECTED]"));   Serial.print("[IP ");  Serial.print(WiFi.localIP()); 
  Serial.println("]");
 
  // start a server
  server.begin();
  Serial.println("Server started");
 
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  udp.begin(localPort);

  WiFi.hostByName(ntpServerName, timeServerIP); 
  sendNTPpacket(timeServerIP);
  //delay(1000);
  displayCurrentTime();
}
 
 
void loop()
{
  
  int cb = udp.parsePacket();
  if (cb) {
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;
    Serial.print("UNX");
    Serial.println(epoch);
    decodeTime(epoch);
    displayCurrentTime();
    
  }
    checkSwitch();
 
    webSocket.loop();
 
    WiFiClient client = server.available();     // Check if a client has connected
    if (!client)  {  return;  }
 
    client.flush();
    client.print( header );
    client.print( html_1 ); 
    Serial.println("New page served");
 
    delay(5);
}
 
 
void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length)
{
  const size_t capacity = 1024;//JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);
  if(type == WStype_TEXT)
  {
      Serial.println("got Text");
      //for (int i = 0; i < length;i++) {
      //  Serial.print(payload[i]);        
      //}
      Serial.print("WS payload = ");
      for(int i = 0; i < length; i++) { Serial.print((char) payload[i]); }
      Serial.println();
      DeserializationError error = deserializeJson(doc, (char *)payload);
      if (error) {
         Serial.print(F("deserializeJson() failed: "));
         Serial.println(error.c_str());
         //return;
      } else {
         Serial.println(F("deserializeJson() OK: "));
         webSocket.broadcastTXT(doc["sensor"].as<char *>());

      }


      if (payload[0] == '0')
      {
          digitalWrite(pin_led, LOW);
          LEDstatus = LOW;
          Serial.println("LED=off");        
      }
      else if (payload[0] == '1')
      {
          digitalWrite(pin_led, HIGH);
          LEDstatus = HIGH;
          Serial.println("LED=on");        
      }
  }
 
  else 
  {
    Serial.print("WStype = ");   Serial.println(type);  
    Serial.print("WS payload = ");
    for(int i = 0; i < length; i++) { Serial.print((char) payload[i]); }
    Serial.println();
 
  }
}
 
void checkSwitch()
{
    newSwitchState1 = digitalRead(pin_switch);    delay(1);
    newSwitchState2 = digitalRead(pin_switch);    delay(1);
    newSwitchState3 = digitalRead(pin_switch);
 
    // if all 3 values are the same we can continue
    if (  (newSwitchState1==newSwitchState2) && (newSwitchState1==newSwitchState3) )
    {
        if ( newSwitchState1 != oldSwitchState )
        {
 
           // toggle the LED when the button switch is pressed rather than released
           if ( newSwitchState1 == HIGH )
           {
              LEDstatus = ! LEDstatus;
 
              if ( LEDstatus == HIGH ) { digitalWrite(pin_led, HIGH);  webSocket.broadcastTXT("1"); Serial.println("LED is ON"); }
              else                     { digitalWrite(pin_led, LOW);   webSocket.broadcastTXT("0"); Serial.println("LED is OFF"); }
           }
           oldSwitchState = newSwitchState1;
        }
   }
}

unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  udp.beginPacket(address, 123);
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void decodeTime(unsigned long epoch)
{
  setTime(epoch);
}
void displayCurrentTime() {
  time_t t = now();
  Serial.print(year(t));
  Serial.print("-");
  Serial.print(month(t));
  Serial.print("-");
  Serial.print(day(t));
  Serial.print("->");
  Serial.print(hour(t));
  Serial.print(":");
  Serial.print(minute(t));
  Serial.print(":");
  Serial.print(second(t));
  Serial.println();
  //delay(1000);

}
