// simple motor class demo
#include <iostream>
#include <fstream>
#include <string>
#include <cjson/cJSON.h>
#include "fields.h"
// g++ -o mc motor_class_01.cpp

using namespace std;


class Motor {
public:
  Motor(string n);
  ~Motor(){};
  void addMotor(Motor *motor);
  string showMotor();
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


string Motor::showMotor()
{
  string res;
  res = " Motor ["+ name +"]\n";
  res += showFields(fields);
  return res;
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

string addMotor(Motor *motor)
{
  string res = "";
  if (g_motors == NULL) {
    g_motors = motor;
    res += " set up base g_motor\n";
  } else {
    g_motors->addMotor(motor);
    res = " added motor to  g_motor\n";
  }
  return res;
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

string newMotor()
{
  string name;
  cout << " Enter name" << endl;
  cin >> name;
  newMotor(name);
  return name;
}

string showMotors()
{
  string res="";
  Motor *motor = g_motors;
  while (motor) {
    res += motor->showMotor();
    motor = motor->getNext();
  }
  return res;
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

string jMotors()
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
  return res;
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

//void runHttpReq()
//{
//  string res = getHttpRequest();
//  cout << " Http request code " << endl;
//  cout << res << endl;
//
//}

// www format 
string wwwMotors(int mode)
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
  return res;
  //cout << " HTML Format" << endl;
  //cout << res << endl;
}


string setupMotors()
{
  Motor * m;
  string res = "";
  m = newMotor("right");
  res += m->showMotor();
  m = newMotor("left");
  res += m->showMotor();

  res += " \nnew Motors setup\n";
  return res;
}

int done  = 0;

string  menuHelp()
{
  string res = " Hello Menu help:\n";
  res += " <?> show help\n";
  res +=" <q> quit\n";
  res += " <t> setup test list of motors\n";
  res +=" <(m)otor> create new motor\n";
  res += " <(s)how_motors> show list of motors\n";
  res += " <(j)son_motors> show json list of motors\n";
  res +=" <(w)ww_motors> show www list of motors\n";
  res += " <r> get http request\n";
  res += " <(w1)ww_motors> show www list of motors plus req\n";
 
  return res;
}

string menu( string input)
{
    string res;
     
    cout << " You entered [" << input <<"]"<< endl;
    if (input == "?") res = menuHelp();
    else if (input == "q") {res = " quitting\n"; done = 1; }
    else if (input == "m") res = newMotor();
    else if (input == "motor") res = newMotor();
    else if (input == "s") res = showMotors();
    else if (input == "show_motors") res = showMotors();
    else if (input == "j") jMotors();
    else if (input == "json_motors") res = jsonMotors();
    else if (input == "w") res = wwwMotors(0);
    else if (input == "www_motors") res = wwwMotors(0);
    else if (input == "t") res = setupMotors();
    else if (input == "r") res = getHttpRequest();
    else if (input == "w1")res =  wwwMotors(1);
    else res = " unknown command [" + input + "]\n";
    //}
    //cout << " quitting " << endl;
    return res;
}


int main() {
  string res;
  string input;
  while (!done) {
    cout << " Hello Menu <?= help>" << endl;
    cin >> input;
    res = menu(input);
    cout << res << endl;
  }
  return 0;
}
