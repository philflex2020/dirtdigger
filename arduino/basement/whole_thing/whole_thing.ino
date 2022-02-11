#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHTesp.h>
//#include <Adafruit_BMP085.h>

#define ssid      "Butter2.4"       // WiFi SSID
#define password  "Butt3r12"  // WiFi password
#define DHTTYPE   DHTesp::DHT22       // DHT type (DHT11, DHT22)
#define DHTpin    5          // Broche du DHT / DHT Pin
const uint8_t GPIOPIN[4] = {6,7,8};  // Led
float   t = 0 ;
float   h = 0 ;
float   p = 0;
String  etatGpio[4] = {"OFF","OFF","OFF","OFF"};

// Création des objets / create Objects
DHTesp dht;
//Adafruit_BMP085 bmp;
ESP8266WebServer server ( 80 );

String getPage(){
  String page = "<html lang='fr'><head><meta http-equiv='refresh' content='60' name='viewport' content='width=device-width, initial-scale=1'/>";
  page += "<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'><script src='https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js'></script><script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script>";
  page += "<title>ESP8266 Demo - www.projetsdiy.fr</title></head><body>";
  page += "<div class='container-fluid'>";
  page +=   "<div class='row'>";
  page +=     "<div class='col-md-12'>";
  page +=       "<h1>Demo Webserver ESP8266 + Bootstrap</h1>";
  page +=       "<h3>Mini station m&eacute;t&eacute;o</h3>";
  page +=       "<ul class='nav nav-pills'>";
  page +=         "<li class='active'>";
  page +=           "<a href='#'> <span class='badge pull-right'>";
  page +=           t;
  page +=           "</span> Temp&eacute;rature</a>";
  page +=         "</li><li>";
  page +=           "<a href='#'> <span class='badge pull-right'>";
  page +=           h;
  page +=           "</span> Humidit&eacute;</a>";
  page +=         "</li><li>";
  page +=           "<a href='#'> <span class='badge pull-right'>";
  page +=           p;
  page +=           "</span> Pression atmosph&eacute;rique</a></li>";
  page +=       "</ul>";
  page +=       "<table class='table'>";  // Tableau des relevés
  page +=         "<thead><tr><th>Capteur</th><th>Mesure</th><th>Valeur</th><th>Valeur pr&eacute;c&eacute;dente</th></tr></thead>"; //Entête
  page +=         "<tbody>";  // Contenu du tableau
  page +=           "<tr><td>DHT22</td><td>Temp&eacute;rature</td><td>"; // Première ligne : température
  page +=             t;
  page +=             "&deg;C</td><td>";
  page +=             "-</td></tr>";
  page +=           "<tr class='active'><td>DHT22</td><td>Humidit&eacute;</td><td>"; // 2nd ligne : Humidité
  page +=             h;
  page +=             "%</td><td>";
  page +=             "-</td></tr>";
  page +=           "<tr><td>BMP180</td><td>Pression atmosph&eacute;rique</td><td>"; // 3ème ligne : PA (BMP180)
  page +=             p;
  page +=             "mbar</td><td>";
  page +=             "-</td></tr>";
  page +=       "</tbody></table>";
  page +=       "<h3>GPIO</h3>";
  page +=       "<div class='row'>";
  page +=         "<div class='col-md-4'><h4 class ='text-left'>D14 ";
  page +=           "<span class='badge'>";
  page +=           etatGpio[0];
  page +=         "</span></h4></div>";
  page +=         "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='D14' value='1' class='btn btn-success btn-lg'>ON</button></form></div>";
  page +=         "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='D14' value='0' class='btn btn-danger btn-lg'>OFF</button></form></div>";
  page +=         "<div class='col-md-4'><h4 class ='text-left'>D6 ";
  page +=           "<span class='badge'>";
  page +=           etatGpio[1];
  page +=         "</span></h4></div>";
  page +=         "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='D6' value='1' class='btn btn-success btn-lg'>ON</button></form></div>";
  page +=         "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='D6' value='0' class='btn btn-danger btn-lg'>OFF</button></form></div>";
  page +=         "<div class='col-md-4'><h4 class ='text-left'>D7 ";
  page +=           "<span class='badge'>";
  page +=           etatGpio[2];
  page +=         "</span></h4></div>";
  page +=         "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='D7' value='1' class='btn btn-success btn-lg'>ON</button></form></div>";
  page +=         "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='D7' value='0' class='btn btn-danger btn-lg'>OFF</button></form></div>";
  page +=         "<div class='col-md-4'><h4 class ='text-left'>D8 ";
  page +=           "<span class='badge'>";
  page +=           etatGpio[3];
  page +=         "</span></h4></div>";
  page +=         "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='D8' value='1' class='btn btn-success btn-lg'>ON</button></form></div>";
  page +=         "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='D8' value='0' class='btn btn-danger btn-lg'>OFF</button></form></div>";
  page +=       "</div>";
  page +=     "<br><p><a href='http://www.projetsdiy.fr'>www.projetsdiy.fr</p>";
  page += "</div></div></div>";
  page += "</body></html>";
  return page;
}
void handleRoot(){ 
  if ( server.hasArg("D14") ) {
    handleD14();
  } else if ( server.hasArg("D6") ) {
    handleD6();
  } else if ( server.hasArg("D7") ) {
    handleD7();
  } else if ( server.hasArg("D8") ) {
    handleD8();
  } else {
    server.send ( 200, "text/html", getPage() );
  }  
}

void handleD14() {
  String D14Value; 
  //updateGPIO(0,server.arg("D14")); 
}

void handleD6() {
  String D6Value; 
  updateGPIO(0,server.arg("D6")); 
}

void handleD7() {
  String D7Value; 
  updateGPIO(1,server.arg("D7")); 
}

void handleD8() {
  String D8Value; 
  updateGPIO(2,server.arg("D8")); 
}

void updateGPIO(int gpio, String DxValue) {
  Serial.println("");
  Serial.println("Update GPIO "); Serial.print(GPIOPIN[gpio]); Serial.print(" -> "); Serial.println(DxValue);
  
  if ( DxValue == "1" ) {
    digitalWrite(GPIOPIN[gpio], HIGH);
    etatGpio[gpio] = "On";
    server.send ( 200, "text/html", getPage() );
  } else if ( DxValue == "0" ) {
    digitalWrite(GPIOPIN[gpio], LOW);
    etatGpio[gpio] = "Off";
    server.send ( 200, "text/html", getPage() );
  } else {
    Serial.println("Err Led Value");
  }  
}

void setup() {
  for ( int x = 0 ; x < 3 ; x++ ) { 
    pinMode(GPIOPIN[x],OUTPUT);
  }  
  Serial.begin ( 115200 );
  
  // Initialisation du BMP180 / Init BMP180
  //if ( !bmp.begin() ) {
  //  Serial.println("BMP180 KO!");
  //  while(1);
  //} else {
  //  Serial.println("BMP180 OK");
  //}
  dht.setup(DHTpin, DHTesp::DHT22); // Connect DHT sensor to GPIO 
  WiFi.begin ( ssid, password );
  // Attente de la connexion au réseau WiFi / Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 ); Serial.print ( "." );
  }
  // Connexion WiFi établie / WiFi connexion is OK
  Serial.println ( "" ); 
  Serial.print ( "Connected to " ); Serial.println ( ssid );
  Serial.print ( "IP address: " ); Serial.println ( WiFi.localIP() );

  // On branche la fonction qui gère la premiere page / link to the function that manage launch page 
  server.on ( "/", handleRoot );

  server.begin();
  Serial.println ( "HTTP server started" );
  
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  t = dht.getTemperature();
  h = dht.getHumidity();
  //p = bmp.readPressure() / 100.0F;

  delay(1000);
}
