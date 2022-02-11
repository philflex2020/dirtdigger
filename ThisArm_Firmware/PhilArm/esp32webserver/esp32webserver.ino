/*
 * ESP32webserver
 * for WeMosWiFi&Bluetooth Battery
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
#include <WiFiAP.h>

#define LED_BUILTIN 22   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

// Set these to your desired credentials.
const char *ssid = "yourAP";
const char *password = "yourPassword";

WiFiServer server(80);
#define MAX_FORMS 8
#define MAX_INPUTS 8

typedef struct inputs_s {
  char name[32];
  char value[32];
  char type[32];
} input_t;

typedef struct form_s {
  char name[32];
  int num_inputs;
  input_t inputs[MAX_INPUTS];
}  form_t;

int num_forms = 0;
form_t forms[MAX_FORMS];

int setupForms()
{
  int i;
  for (i=0 ; i < MAX_FORMS; i++){
    form_t *form = &forms[i];
    form->name[0]=0;
    for (int j = 0 ; j < MAX_INPUTS; j++) {
      input_t * input = &form->inputs[j];
      input->name[0]=0;    
      input->value[0]=0;    
      input->type[0]=0;    
    }
  }
}

int setInputToForm(int fnum, int inum, char *name, char *value, char * type)
{
  form_t *form = &forms[fnum];
  input_t * input = &form->inputs[inum];
  snprintf(input->name, 32, "%s",name);
  snprintf(input->value, 32, "%s",value);
  snprintf(input->type, 32, "%s",type);
  return 0;
}

int setUpForm(int fnum, char *name)
{
  form_t *form = &forms[fnum];
  snprintf(form->name, 32, "%s",name);
  return 0;
}

String getForm(int fnum)
{
  String fs="";
  form_t *form = &forms[fnum];
  fs += "<h1>"; fs += form->name; fs += "</h1>";
  fs += "<form action=\"/"; fs += form->name;  fs+="\">\n";
  for (int i = 0 ; i < MAX_INPUTS; i++) {
    input_t * input = &form->inputs[i];
    if(input->name[0] != 0) {
      fs += "Enter "; fs += input->name; 
      fs +=": <input type=\""; fs +=input->type; 
      fs +="\" name=\""; fs+=input->name; 
      fs +="\" value=\""; fs+=input->value; fs +="\"";
      if( strcmp(input->type,"range") == 0) {
        String id = form->name; id += "_"; id += input->name;
        fs += " id =\""; 
        fs += id; 
        fs += "\"";
        //form_name_disp.value
        fs+=" oninput=\""; fs +=id; fs+= "_disp.value = ";
        fs += id; fs += ".value\"";
        //fs+=" onload=\""; fs +=id; fs+= "_disp.value = \"";
        //fs+=input->value; fs +="\"";
        //fs += id; fs += ".value\"";

        fs +=">\n"; 
        fs += " <output id=\"";fs += id; fs += "_disp\">";
        //fs+=" value=\"";
        fs += input->value;
        fs +="</output";
      }
      fs += ">"; 
      fs +="\n";

    }
  }
  
  fs += "<input type=\"submit\" value=\"Submit\">";
  fs += "</form>\n";

  return fs;
}
int setUpMForm(int num, char * name)
{
  setUpForm(num, name);
  setInputToForm(num, 0, "name", "f0value", "text");
  setInputToForm(num, 1, "speed", "20", "range");
  setInputToForm(num, 2, "accel", "30", "range");
  setInputToForm(num, 3, "time", "90", "range");
  return num;
}

String getForms()
{
  String fs = "";
  for (int i = 0 ; i< MAX_FORMS; i++ ) {
    form_t *form = &forms[i];
    if(form->name[0] == 0) continue;
    String  ff = getForm(i);
    fs += ff;
  }
  return fs;
}

//Referer: http://192.168.4.1/motor0?name=f0value&speed=20&accel=30&time=90
//Referer: http://192.168.4.1/H
String getValue(int jdx, input_t *input, String mLine) {
   
  String rstr = "";
  //rstr.concat(mLine.charAt(jdx));
  while((mLine.charAt(jdx+1) != '&') 
     && (mLine.charAt(jdx+1) != '\n')
     && (mLine.charAt(jdx+1) != ' ') 
     && (jdx+1) < mLine.length()) {
    rstr += mLine.charAt(jdx+1);
    jdx += 1;
  }
  return rstr;
}

int getInput(input_t *input, String mLine) {
  if(input->name[0] == 0) return 0;
  String istr = input->name;
  istr += "=";
  int jdx = mLine.indexOf(istr);
  if(jdx < 0) return 0;
  jdx += strlen(input->name);
  String rstr = getValue(jdx, input, mLine);
  
  Serial.print("Found value: [");
  Serial.print(rstr);
  Serial.print("]\n");
  snprintf(input->value, 32, "%s",rstr.c_str());
  return 0;
}
    

int decodeForms(String mLine, char c)
{
  if (mLine.endsWith("GET /H")) {
     digitalWrite(LED_BUILTIN, HIGH);               // GET /H turns the LED on
  }
  else if (mLine.endsWith("GET /L")) {
     digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
  } else {
  }
  if(c != '\r') return 0;
  
  if (mLine.indexOf("GET /") != 0) return 0;
  for (int i = 0 ; i< MAX_FORMS; i++ ) {
    form_t *form = &forms[i];
    
    int idx = mLine.indexOf(form->name);
    if (idx <= 0) continue;
    Serial.print("\nChecking form :[");
    Serial.print(form->name);
    Serial.print("]\n");

    if (mLine[idx-1] != '/') return 0;
    if (mLine[idx+strlen(form->name)] != '?') return 0;
    for (int j = 0 ; j< MAX_INPUTS; j++ ) {
         input_t * input = &form->inputs[j];
         getInput(input, mLine);

    }
  }
  return 0;

}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  setupForms();
  setUpMForm(0, "motor0");
  setUpMForm(1, "motor1");
  setUpMForm(2, "motor2");
 
  String f0 = getForm(0);
  Serial.println();
  Serial.println("Test Form");
  Serial.println(f0);

  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");
}

void loop() {
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
            client.print("<form action=\"/run_form\">");
            client.print("Enter value: <input type=\"text\" name=\"fvalue\"><br>");
             client.print("Enter speed: <input type=\"range\" name=\"fspeed\"><br>");
            client.print("<input type=\"submit\" value=\"Submit\">");
            client.print("</form>");
            String f0 = getForms();
            client.print(f0);

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

        decodeForms(currentLine, c);
        // Check to see if the client request was "GET /H" or "GET /L":
        //if (currentLine.endsWith("GET /H")) {
        //  digitalWrite(LED_BUILTIN, HIGH);               // GET /H turns the LED on
        //}
        //if (currentLine.endsWith("GET /L")) {
          //digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
        //}
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

