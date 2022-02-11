void append_page_header() {
  webpage  = F("<!DOCTYPE html><html>");
  webpage += F("<head>");
  webpage += F("<title>Crawl Pump Controller</title>"); // NOTE: 1em = 16px
  webpage += F("<meta name='viewport' content='user-scalable=yes,initial-scale=1.0,width=device-width'>");
  webpage += F("<style>");
  webpage += F("body{max-width:65%;margin:0 auto;font-family:arial;font-size:105%;text-align:center;color:blue;background-color:#F7F2Fd;}");
  webpage += F("ul{list-style-type:none;margin:0.1em;padding:0;border-radius:0.375em;overflow:hidden;background-color:#dcade6;font-size:1em;}");
  webpage += F("li{float:left;border-radius:0.375em;border-right:0.06em solid #bbb;}last-child {border-right:none;font-size:85%}");
  webpage += F("li a{display: block;border-radius:0.375em;padding:0.44em 0.44em;text-decoration:none;font-size:85%}");
  webpage += F("li a:hover{background-color:#EAE3EA;border-radius:0.375em;font-size:85%}");
  webpage += F("section {font-size:0.88em;}");
  webpage += F("h1{color:white;border-radius:0.5em;font-size:1em;padding:0.2em 0.2em;background:#558ED5;}");
  webpage += F("h2{color:orange;font-size:1.0em;}");
  webpage += F("h3{font-size:0.8em;}");
  webpage += F("table{font-family:arial,sans-serif;font-size:0.9em;border-collapse:collapse;width:85%;}"); 
  webpage += F("th,td {border:0.06em solid #dddddd;text-align:left;padding:0.3em;border-bottom:0.06em solid #dddddd;}"); 
  webpage += F("tr:nth-child(odd) {background-color:#eeeeee;}");
  webpage += F(".rcorners_n {border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:20%;color:white;font-size:75%;}");
  webpage += F(".rcorners_m {border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:50%;color:white;font-size:75%;}");
  webpage += F(".rcorners_w {border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:70%;color:white;font-size:75%;}");
  webpage += F(".column{float:left;width:50%;height:45%;}");
  webpage += F(".row:after{content:'';display:table;clear:both;}");
  webpage += F("*{box-sizing:border-box;}");
  webpage += F("footer{background-color:#eedfff; text-align:center;padding:0.3em 0.3em;border-radius:0.375em;font-size:60%;}");
  webpage += F("button{border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:20%;color:white;font-size:130%;}");
  webpage += F(".buttons {border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:15%;color:white;font-size:80%;}");
  webpage += F(".buttonsm{border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:9%; color:white;font-size:70%;}");
  webpage += F(".buttonm {border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:15%;color:white;font-size:70%;}");
  webpage += F(".buttonw {border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:40%;color:white;font-size:70%;}");
  webpage += F("a{font-size:75%;}");
  webpage += F("p{font-size:75%;}");
  webpage += F("</style></head>");
 
  //webpage += F("<body onload=\"javascript:start();\">    ");
  webpage += F("<body>");

  webpage += F("<h1><span id=\"NTPtime\">xxx</span>   Crawl Pump Controller   "); 
  webpage += String(ServerVersion);
  webpage += " Uptime: <span id=\"Uptime\">0</span>"; 
  webpage += "</h1>"; 

}

void mkWebSock(String &s, String Topic)
{
  s += F("<script>");
  s += F("var websock;");
  s += F("function start() {");
  s += F("websock = new WebSocket('ws://' + window.location.hostname + ':81/');");
  s += F("websock.onopen = function(evt) { console.log('websock open'); };");
  s += F("websock.onclose = function(evt) { console.log('websock close'); };");
  s += F("websock.onerror = function(evt) { console.log(evt); };");
  s += F("websock.onmessage = function(evt) {");
  s += F("  console.log(evt);");
  s += F("  var e = document.getElementById('ledstatus');");
  s += F("  if (evt.data === 'ledon') { ");
  s += F("    e.style.color = 'red';");
  s += F("  }");
  s += F("  else if (evt.data === 'ledoff') {");
  s += F("   e.style.color = 'black';");
  s += F(" }");
  s += F("  else {");
  s += F("    console.log('unknown event');");
  s += F("  }" );
  s += F("  };"  );
  s += F("}"   );
  //s += F("function buttonclick(e) {");
  //s += F("  websock.send(e.id);");
  //s += F("}");
  s += F("</script>");
}
//String  mkFunction(String Topic);
void mkFunction(String &s, String Topic);
void mkSlider(String &s, String Topic);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void append_page_script(){ // Saves repeating many lines of code for HTML page footers
  webpage += F("<script>");
  webpage +="setInterval(function() {";
  // Call a function repetatively with 5 Second interval
  //webpage +=" getData();";
  //webpage +=" getTicks();";
  //webpage +=" getAdcs();";
  //webpage +=" getTemp();";
  //webpage +=" getHumid();";
  webpage +=" getUptime();";
  webpage +=" getNTPtime();";

  webpage +="}, 1000);"; //5000mSeconds update rate
  mkFunction(webpage, "Uptime");
  mkFunction(webpage, "NTPtime");
  webpage += F("</script>");
  mkSlider(webpage,"Analog1");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void append_page_footer(){ // Saves repeating many lines of code for HTML page footers
  webpage += F("<ul>");
  webpage += F("<li><a href='/'>Home</a></li>"); // Lower Menu bar command entries
  webpage += F("<li><a href='/download'>Download</a></li>"); 
  webpage += F("<li><a href='/upload'>Upload</a></li>"); 
  webpage += F("<li><a href='/stream'>Stream</a></li>"); 
  webpage += F("<li><a href='/delete'>Delete</a></li>"); 
  webpage += F("<li><a href='/dir'>Directory</a></li>");
  webpage += F("</ul>");
  webpage += F("<footer>&copy;");
  webpage += F("sdcs 2019");
  //  webpage += +String(char(byte(0x40>>1)))+String(char(byte(0x88>>1)))+String(char(byte(0x5c>>1)))+String(char(byte(0x98>>1)))+String(char(byte(0x5c>>1)));
//  webpage += String(char((0x84>>1)))+String(char(byte(0xd2>>1)))+String(char(0xe4>>1))+String(char(0xc8>>1))+String(char(byte(0x40>>1)));
//  webpage += String(char(byte(0x64/2)))+String(char(byte(0x60>>1)))+String(char(byte(0x62>>1)))+String(char(0x70>>1))+"</footer>";
  webpage += F("</footer>");
  webpage += F("</body></html>");
}
