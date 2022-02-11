#include <ArduinoJson.h>
#include <Wire.h>

#include <WiFi.h>
#include <WebServer.h>
#include "RoboClaw.h"
#include <WebSocketsServer.h>
#include "LiquidCrystal_I2C.h"
#include "home_page.h"
//#include "h_p.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DataDefs.h"
#include "ioVars.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

RoboClaw roboclaw(&Serial1, 10000);

const char* pw = "basicmicro";
const char* ap_name = "esp32motor";

//https://www.pjrc.com/teensy/td_libs_SSD1306.html
int lcdColumns = 20;
int lcdRows = 4;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

const int blueLED = LED_BUILTIN; 

int bluestate = 0;

void show_lcd(int r, int c, String msg)
{
  lcd.setCursor(r, c);
  // print message
  lcd.print(msg);
}

void setup_lcd(){
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
}

void setup_display(){
  
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  Serial.println(F("SSD1306 allocation passed"));
  display.setRotation(2);
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  //display.setTextColor(BLUE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("CoVent Controller");
  display.display();
}

WebServer server(80);
//WiFiServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void handleHome(){
  String page = home_page;
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

void handleTemp() {
  uint16_t temp = 0;
  temp = 450;
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

uint16_t g_count = 0;
float g_volt = 12.5;
float g_temp = 48.7;
#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif

void handleCount() {
  unsigned short temp;
  roboclaw.ReadTemp(0x80, temp);
  g_volt = (float)(roboclaw.ReadMainBatteryVoltage(0x80)/10);
  g_temp = (float)temp/10;
  g_count++;
  //doc2["count"]= g_count;
  //g_temp = (temprature_sens_read() - 32) / 1.8;
  String count_str = "\"count\":" +String(g_count);
  String volt_str = "\"volt\":" +String(g_volt);
  String temp_str = "\"temp\":" +String(g_temp);
  String send_str = "{" + count_str + "," + volt_str + "," + temp_str+"}";
  //serializeJson(doc2, send_str);
  server.send(200, "text/plain", send_str);
  Serial.print(F("HandleCount... temp:"));
  Serial.println(temp);
  //show_lcd(0, 0, send_str);

}
  
char ssid[] = "Butter2.4";
char pass[]= "Butt3r12";
int g_a0 = 0;

void setup() {
  
  Serial.begin(115200);
  while (!Serial) continue;
  Serial1.println(F("Testing..."));

  Serial.println(F("Starting..."));
  Serial.println();
  Serial.print("File:");Serial.println(__FILE__);

  Serial.println();
  Serial.println("Serial started at 115200");
  Serial.println();
  setup_display();
  //addmdata();

  // Connect to a WiFi network
  Serial.print(F("Connecting to "));  Serial.println(ssid);
  WiFi.setHostname("philRcesp");
  WiFi.begin(ssid,pass);
 
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
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
 
  //Serial1.begin(115200,SERIAL_8N1, 12, 13);    //Baud rate, parity mode, RX, TX
  //Serial1.println("Serial1  started");
  //delay(10);
  //while (Serial1.available()) {
   // Serial.print(char(Serial1.read()));
  //}
  

  //while (1);
  roboclaw.begin(38400,12,13);
  //WiFi.softAP(ap_name, pw);

  server.on("/", handleHome);
  server.on("/readEncoder_1", handleEncoder_1);
  server.on("/readEncoder_2", handleEncoder_2);
  server.on("/readTemp", handleTemp);
  server.on("/readVoltage", handleVoltage);
  server.on("/handleMotor", handleMotor);
  server.on("/readCount", handleCount);

  server.begin();
  setupioVar ("ADC_0", AN_IN, ADC1_0);
  setupioVar ("ADC_1", AN_IN, ADC1_1);
  setupioVar ("ADC_2", AN_IN, ADC1_2);
  setupioVar ("ADC_3", AN_IN, ADC1_3);

  int g_a0  = ioAdc(0);
  Serial.print("A0in:");Serial.println(g_a0);
  scan_i2c();
//  while(1);
  setup_lcd();
  
  pinMode(blueLED, OUTPUT); // For LED feedback
  setup_timer();
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

void scan_i2c() {
  byte error, address;
  int nDevices;
  Serial.println("I2C Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
//  delay(5000);          
}

int lcount = 0;

int g_a[4];
int g_ad[4];

//volatile int interruptCounter;
//int totalInterruptCounter;

volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;

hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter++;
  lastIsrAt = millis();
  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  //xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output
  toggleLed(blueLED, bluestate);
}

void setup_timer() {
 
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
 
}


void loop() {

  // If Timer has fired
  if (0) {
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE){
    uint32_t isrCount = 0, isrTime = 0;
    // Read the interrupt count and time
    portENTER_CRITICAL(&timerMux);
    isrCount = isrCounter;
    isrTime = lastIsrAt;
    portEXIT_CRITICAL(&timerMux);
    // Print it
    Serial.print("onTimer no. ");
    Serial.print(isrCount);
    Serial.print(" at ");
    Serial.print(isrTime);
    Serial.println(" ms");
  }
  }
  lcount++;
  String lcdOut;  
  server.handleClient();
  delay(1);
  if (lcount%10 == 0) {
    if (readAdc()) {

      for (int i = 0; i < 4 ; i++) {
  
         g_a[i]  = getAdc(i);
         g_ad[i]  = getAdcDiff(i);
         if (i ==0) Serial.print("0,");
         Serial.print(g_a[i]);    
         if (i < 3) Serial.print(",");
         else Serial.print(",4000");
         lcdOut = " Ain:" + String(i) +":"+ String(g_a[i],DEC) +"   ";
         show_lcd(0,i,lcdOut);
         //Serial.print(",");
         //Serial.print(g_ad[i]);
      }
      Serial.println();
      
    }
  }
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
          //digitalWrite(pin_led, LOW);
          //LEDstatus = LOW;
          Serial.println("LED=off");        
      }
      else if (payload[0] == '1')
      {
          //digitalWrite(pin_led, HIGH);
          //LEDstatus = HIGH;
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
