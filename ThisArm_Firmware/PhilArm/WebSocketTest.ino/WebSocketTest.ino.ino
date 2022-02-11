// new code
/*
 * Sketch: ESP8266_Part9_01_Websocket_LED
 * Intended to be run on an ESP8266
 */
 
String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
 
String html_1 = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
  <meta charset='utf-8'>
  <style>
    body    { font-size:120%;} 
    #main   { display: table; width: 300px; margin: auto;  padding: 10px 10px 10px 10px; border: 3px solid blue; border-radius: 10px; text-align:center;} 
    .button { width:200px; height:40px; font-size: 110%;  }
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
    <br />
   </div>
</body>
 
<script>
  var Socket;
  function init() 
  {
    Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
  }
 
  document.getElementById('BTN_LED').addEventListener('click', buttonClicked);
  function buttonClicked()
  {   
    var btn = document.getElementById('BTN_LED')
    var btnText = btn.textContent || btn.innerText;
    if (btnText ==='Turn on the LED') { btn.innerHTML = "Turn off the LED"; document.getElementById('LED_status').innerHTML = 'LED is on';  sendText('1'); }  
    else                              { btn.innerHTML = "Turn on the LED";  document.getElementById('LED_status').innerHTML = 'LED is off'; sendText('0'); }
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
 
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
 
WiFiServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
 
byte pin_led = D3;
 
char ssid[] = "Butter2.4";  // use your own network ssid and password
char pass[] = "Butt3r12";
 
void setup()
{
  pinMode(pin_led, OUTPUT);
  digitalWrite(pin_led,LOW);
 
  Serial.begin(115200);
  Serial.print("File: ");  Serial.println(__FILE__);


  Serial.println();
  Serial.println("Serial started at 115200");
  Serial.println();
 
  // Connect to a WiFi network
  Serial.print(F("Connecting to "));  Serial.println(ssid);
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
}
 
void loop()
{
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
  if(type == WStype_TEXT)
  {
      if (payload[0] == '0')
      {
          digitalWrite(pin_led, LOW);
          Serial.println("LED=off");        
      }
      else if (payload[0] == '1')
      {
          digitalWrite(pin_led, HIGH);
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

#if 0
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
 
WiFiServer server(80);
WebSocketsServer webSocketServer(81);
 
const char* ssid = "Butter2.4";
const char* password =  "Butt3r12";
 
void handleReceivedMessage(String message){
 
  //StaticJsonBuffer<500> JSONBuffer;                     //Memory pool
  StaticJsonDocument<500> parsed;                     //Memory pool
  //JsonObject& parsed = JSONBuffer.parseObject(message); //Parse message
  auto error = deserializeJson(parsed,message); //Parse message
 
  //if (!parsed.success()) {   //Check for errors in parsing
  if (error) {   //Check for errors in parsing
 
    Serial.println("Parsing failed");
    Serial.println(error.c_str());
    return;
 
  }
 
  const char * sensorType = parsed["sensor"];           //Get sensor type value
  const char * sensorIdentifier = parsed["identifier"]; //Get sensor type value
  const char * timestamp = parsed["timestamp"];         //Get timestamp
  int value = parsed["value"];                          //Get value of sensor measurement
  Serial.println();
  Serial.println("----- NEW DATA FROM CLIENT ----");
 
  Serial.print("Sensor type: ");
  Serial.println(sensorType);
 
  Serial.print("Sensor identifier: ");
  Serial.println(sensorIdentifier);
 
  Serial.print("Timestamp: ");
  Serial.println(timestamp);
 
  Serial.print("Sensor value: ");
  Serial.println(value);
 
  Serial.println("------------------------------");
}
 
void setup() {
 
  Serial.begin(115200);
 
  delay(2000);
  Serial.println("File : %s", __FILE__);
 
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
 
  server.begin();
  delay(100);
}
 
void loop() {
 
  WiFiClient client = server.available();
 
  if (client.connected() && webSocketServer.handshake(client)) {
 
    String data;      
 
    while (client.connected()) {
 
      data = webSocketServer.getData();
 
      if (data.length() > 0) {
         handleReceivedMessage(data);
         webSocketServer.sendData(data);
      }
 
      delay(10); // Delay needed for receiving the data correctly
   }
 
   Serial.println("The client disconnected");
   delay(100);
  }
 
  delay(100);
}
#endif
