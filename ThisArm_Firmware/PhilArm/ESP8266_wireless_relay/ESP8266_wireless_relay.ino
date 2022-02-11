#include <ESP8266WiFi.h>
#include <aREST.h>
#include <aREST_UI.h>

// create aREST instance
aREST_UI rest = aREST_UI();

// WiFi parameters
const char* ssid = "Butter2.4";
const char* password = "Butt3r12";

// port conncetions TCP 
#define LISTEN_PORT           80

// create a server instance
WiFiServer server(LISTEN_PORT);
void setup(void)
{  
  Serial.begin(115200);
  Serial.print("Running :");
  Serial.println(__FILE__);
  
  // create UI
  rest.title("Relay ");
  rest.button(2);
  rest.button(0);
    
  // set module name and id
  rest.set_id("1");
  rest.set_name("esp8266");
  
  // connecte au wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // initialise the server
  server.begin();
  Serial.println("Server started");
  
  // print the ip address 
  Serial.println(WiFi.localIP());
  
}

void loop() {
  
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while(!client.available()){
    delay(1);
  }
  rest.handle(client);
 
}
