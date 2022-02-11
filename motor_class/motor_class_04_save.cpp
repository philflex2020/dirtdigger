// simple motor class demo
#include <iostream>
#include <fstream>
#include <string>
#include <cjson/cJSON.h>
#include "fields.h"
#include "server.h"

// g++ -o mc motor_class_01.cpp

using namespace std;


class Motor {
public:
  Motor(string n);
  ~Motor(){};
  void addMotor(Motor *motor);
  void showMotor();
  Motor *getNext();
  string getName(){ return name;};
  string jsonMotor();
  string wwwMotor();
  void addField(string n, string t);


private:
  string name;
  Field *fields;
  Motor *next;
};

Motor::Motor(string n) : name(n), next(NULL), fields(NULL) {
  cout<<" created new motor ["<< name<<"]"<<endl;
}

void Motor::addMotor(Motor*motor)
{
  Motor *last = this;
  while(last->next) last = last->next;
  last->next = motor;
}

Motor *Motor::getNext()
{
  return next;
}

void Motor::addField(string n, string t)
{
  Field *field = new Field(n,t);
  if (fields == NULL) {
    fields = field;
  } else {
    Field *last = fields;
    while (last->getNext()) last - last->getNext();
    last->setNext(field);
  }
}

void Motor::showMotor()
{
  cout << " Motor ["<< name <<"]"<<endl;
  string res = showFields(fields);
  cout << res << endl;

}

string Motor::jsonMotor()
{
  string res;
  res = "\"" + name + "\":{";
  res += jsonFields(fields);
  res += "}";
  
  return res;
}

string Motor::wwwMotor()
{
  string res;
  res = "<h2>" + name + "</h2>";
  res += "<form action =\"/motor_" + getName() + "\">\n";
  res += wwwFields(fields, getName());
  res += "<input type=\"submit\" value=\"Set\">";
  res += "</form>\n";

  return res;
}


Motor *g_motors = NULL;

void addMotor(Motor *motor)
{
  if (g_motors == NULL) {
    g_motors = motor;
    cout << " set up g_motor" << endl;
  } else {
    g_motors->addMotor(motor);
    cout << " added motor to  g_motor" << endl;
  }
}

Motor *newMotor( string name)
{
  Motor *m = new Motor(name);
  cout << " motor ["<<name<<"] created" << endl;
  addMotor(m);
  m->addField("speed","range");
  m->addField("time","range");
  return m;
}

void newMotor()
{
  string name;
  cout << " Enter name" << endl;
  cin >> name;
  newMotor(name);
}



void showMotors()
{
  Motor *motor = g_motors;
  while (motor) {
    motor->showMotor();
    motor = motor->getNext();
  }
}

string jsonMotors()
{
  string res = "{\"motors\":{";
  Motor *motor = g_motors;
  while (motor) {
    res += motor->jsonMotor();
    motor = motor->getNext();
    if (motor) res += ",";
  }
  res += "}}";
  return res;
}

void jMotors()
{
  string res = jsonMotors();
  cout << res << endl;
  cJSON *cj = cJSON_Parse(res.c_str());
  if(cj) {
  //cJSON_AddItemToObject(cj, name.c_str(), cJSON_CreateString(name.c_str()));
    res = cJSON_Print(cj);
    cJSON_Delete(cj);
    cout << " JSON Format" << endl;
    cout << res << endl;
  } else {
    cout << " Error in JSON " << endl;
  }
}

string getHttpRequest()
{
  string res = "Motor Speed<div id=\"json\"></div>\n";
  res += "<div id=\"demo\"></dev>\n";
  res += "<div id=\"demo1\">\n";
  res +=
    "<h2>JSON Updater</h2>\n\
    <button type=\"button\" onclick=loadDoc()>Get Data</button>\n\
    </div>\n\
    <script>\n\
    function loadDoc() {\n\
      var xhttp = new XMLHttpRequest();\n\
      xhttp.onreadystatechange = function() {\n\
        if (this.readyState == 4 && this.status == 200) {\n\
          var obj=JSON.parse(this.responseText);\n\
          document.getElementById(\"demo\").innerHTML =this.responseText;\n\
          document.getElementById(\"json\").innerHTML =obj.motors.right_speed;\
          if(document.getElementById(\"motors_right_speed\")) { \n\
            document.getElementById(\"motors_right_speed\").value = \n\
                         obj.motors_right.speed;\n\
            document.getElementById(\"motors_right_speed_disp\").value =\n\
                         obj.motors_right.speed;\n\
            } \n\
          var x;\n\
          for (x in obj) {\n\
          console.log(x);\n\
            var motor = obj[x]; \n\
            var y;\n\
            for ( y in motor) {\n\
               var ele = x + \"_\" + y;\n\
               console.log(ele);\n\
               if(document.getElementById(ele)) {\n\
                 document.getElementById(ele).value = motor[y];\n\
                 ele += \"_disp\";\n\
                 document.getElementById(ele).value = motor[y];\n\
               }\n\
            }\n\
          }\n\
        }\n\
      };\n\
      \n\
      xhttp.open(\"GET\", \"get_motor.txt\", true);\n\
      xhttp.send();\n\
    }\n\
    </script>\n";
  return res;
}

void runHttpReq()
{
  string res = getHttpRequest();
  cout << " Http request code " << endl;
  cout << res << endl;

}

// www format 
void wwwMotors(int mode)
{
  string res = "<!DOCTYPE html>\n";
  res += "<html>\n";
  res += "<title> HTML MOTORS</title>\n";
  res += "<body>\n";
  res += "<h1>motors</h1>\n";
  Motor *motor = g_motors;
  while (motor) {
    //jsonMotor
    res += motor->wwwMotor();
    res += "\n";
    motor = motor->getNext();
    //if (motor) res += ",";
  }
  if (mode ==1 )  res += getHttpRequest();
  res += "</body>\n";
  res += "</html>\n";

  //res += "}}";
  ofstream outf;
  outf.open("motor_class.html");
  outf<<res<<endl;
  outf.close();
  cout << " HTML Format" << endl;
  cout << res << endl;
}


void setupMotors()
{

  newMotor("right");
  newMotor("left");
}


void menuHelp()
{
  cout << " Hello Menu help:" << endl;
  cout << " <?> show help" << endl;
  cout << " <q> quit" << endl;
  cout << " <t> setup test list of motors" << endl;
  cout << " <(m)otor> create new motor" << endl;
  cout << " <(s)how_motors> show list of motors" << endl;
  cout << " <(j)son_motors> show json list of motors" << endl;
  cout << " <(w)ww_motors> show www list of motors" << endl;
  cout << " <r> get http request" << endl;
  cout << " <(w1)ww_motors> show www list of motors plus req" << endl;
  cout << " <server> start www server" << endl;

}


Server *g_wserv = NULL;

void runFoo(int client, const char *file, int cgi)
{
  return;
}

string serveMotor(int client, char *buf, int cgi)
{
  if (cgi) {
    string mLine = buf;
    //decodeForm(mLine);
  }
  // TODO
  string mres =  "";//getForms();
  //mres += getJson1();
  
  //cout << mres << endl;
 //cout << "length :" << mres.length()<<endl;
  return mres;
}


string serveAjax(int client, const char *file, int cgi)
{
  if (cgi) {
   //string mLine = file;
   //decodeForm(mLine);
    printf(" file [%s]\n", file);
  }
  
  //string mres =  getForms();
  //mres += getJson1();
  
  //cout << mres << endl;
  //cout << "length :" << mres.length()<<endl;
  string mres ="{\"name\":\"John\", \"age\":30, \"city\":\"New York\"}";
  //"here is some content";
  //mres = getMotorsJson();
  return mres;
}

int wwwServer(int port)
{
  Server *wserv = new Server(port);
  wserv->on("/foo", (void *)runFoo);
  wserv->on("/motor", (void *)serveMotor);
  wserv->on("/get_json.txt", (void *)serveAjax);
  wserv->start();
  g_wserv = wserv;
}

void menu()
{
  string input;
  int done = 0;
  while (!done) {
    cout << " Hello Menu <?= help>" << endl;
    cin >> input;
    
    cout << " You entered [" << input <<"]"<< endl;
    if (input == "?") menuHelp();
    else if (input == "q") done = 1;
    else if (input == "m") newMotor();
    else if (input == "motor") newMotor();
    else if (input == "s") showMotors();
    else if (input == "show_motors") showMotors();
    else if (input == "j") jMotors();
    else if (input == "json_motors") jsonMotors();
    else if (input == "w") wwwMotors(0);
    else if (input == "www_motors") wwwMotors(0);
    else if (input == "t") setupMotors();
    else if (input == "r") runHttpReq();
    else if (input == "w1") wwwMotors(1);
    else if (input == "server") wwwServer(8080);
  }
  cout << " quitting " << endl;
}


int main() {  
  menu();
  if (g_wserv)
    g_wserv->stop();

  return 0;
}
