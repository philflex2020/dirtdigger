/*
 * OTAWebUpdater.ino Example from ArduinoOTA Library
 * Rui Santos 
 * Complete Project Details http://randomnerdtutorials.com
 */

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

const char* host = "esp32";
const char* ssid = "Butter2.4";
const char* password = "Butt3r12";

WebServer server(80);

/*
 * GpiosIndex Page
 */
String output26State = "ON";
String output27State = "ON";
int myID = 27;
/*
 * Login page
 */

 /*
  * <script>
function loadDoc() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("demo").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "ajax_info.txt", true);
  xhttp.send();
}
  */
const char* gpiosIndex = 
    "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
    ".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;"
    "text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}"
    ".button2 {background-color: #555555;}</style>"
    "<h1> Pump Status </h1>"
     "<p>GPIO 26 - State OFF </p>"
     "<p><a href='/gpio/26/on'><button class='button'>ON</button></a></p>"
     "<p>GPIO 27 - State ON </p>"
     "<p><a href='/gpio/27/off'><button class='button button2'>OFF</button></a></p>"
     "<div id='demo'> <h2>The XMLHttpRequest Object</h2>"
     "<button type='button' onclick='loadDoc()'>Change Content</button>"
    "</div>"
     "<script> setInterval (loadDoc, 5000) ; function loadDoc() { var xhttp = new XMLHttpRequest();"
     "xhttp.onreadystatechange = function() { "
     "if (this.readyState == 4 && this.status == 200) {"
      "document.getElementById('demo').innerHTML ="
      "this.responseText;}};"
     "xhttp.open('GET', 'ajax_info.txt', true);"
     "xhttp.send();} </script>"
    "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>Pump Control Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<td>Gpios:</td>"
        "<td><input type='text' size=25 name='userid'><br><a href='/gpio/26'> 26</a></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
"</script>";

const char* loginIndex = 
 "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>Pump Login Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<td>Username:</td>"
        "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
"</script>";
 
/*
 * GpiosIndex Page
 */
//String output26State = "ON";
//String output27State = "ON";
//int myID = 27;
 
const char *xxgpiosIndex()
{
  String gpiosIndexStr = "";
    //"<!DOCTYPE html><html>";
  //gpiosIndexStr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  //gpiosIndexStr += "<link rel=\"icon\" href=\"data:,\">";


// CSS to style the on/off buttons 
// Feel free to change the background-color and font-size attributes to fit your preferences
gpiosIndexStr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}";

gpiosIndexStr +=".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;";
gpiosIndexStr +="text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}";
gpiosIndexStr +=".button2 {background-color: #555555;}</style></head>";
//            
// Web Page Heading
gpiosIndexStr +="<body><h1>ESP32 Pump Web Server (";
gpiosIndexStr +=myID;
gpiosIndexStr +=") </h1>";
//            
// Display current state, and ON/OFF buttons for GPIO 26  
gpiosIndexStr +="<p>GPIO 26 - State " + output26State + "</p>";
// If the output26State is off, it displays the ON button       
gpiosIndexStr +="<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>";
//"<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>";
//              
// Display current state, and ON/OFF buttons for GPIO 27  
gpiosIndexStr +="<p>GPIO 27 - State " + output27State + "</p>";
// If the output27State is off, it displays the ON button       
//
gpiosIndexStr +="<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>";
//"<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>"
//
gpiosIndexStr +="</body>";
//</html>";


   return  (const char *)gpiosIndexStr.c_str();
}
/*
 * Server Index Page
 */
 
const char* serverIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')" 
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";

/*
 * setup function
 */
char stuff[256];

void setup(void) {
  Serial.begin(115200);

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  server.on("/gpios", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", gpiosIndex);
  });
  server.on("/gpio/26", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/data", "gpio26");
  });
  server.on("/ajax_info.txt", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    //char stuff[256];
    snprintf(stuff , 256, " Time is %d " , millis());
    server.send(200, "text/data", stuff);
  });
    

  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
}

void loop(void) {
  server.handleClient();
  delay(1);
}



