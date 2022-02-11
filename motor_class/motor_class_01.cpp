// simple motor class demo
#include <iostream>
#include <fstream>
#include <string>
#include <cjson/cJSON.h>

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


private:
  string name;
  Motor *next;
};

Motor::Motor(string n) : name(n), next(NULL) {
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

string Motor::showMotor()
{
  string res;
  res = " Motor ["+ name +"]\n";
  return res;
}



string Motor::jsonMotor()
{
  string res;
  res = "\"" + name + "\":{";
  res += "}";
  return res;
}

string Motor::wwwMotor()
{
  string res;
  res = "<h2>" + name + "</h2>";

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

// www format 
string wwwMotors()
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
  return res;
}

string menu( string input)
{
  string res;
  //int done = 0;

    
    cout << " You entered [" << input <<"]"<< endl;
    if (input == "?") res = menuHelp();
    else if (input == "q") {res = " quitting\n"; done = 1; }
    else if (input == "m") res = newMotor();
    else if (input == "motor") res = newMotor();
    else if (input == "s") res = showMotors();
    else if (input == "show_motors") res = showMotors();
    else if (input == "j") jMotors();
    else if (input == "json_motors") res = jsonMotors();
    else if (input == "w") res = wwwMotors();
    else if (input == "www_motors") res = wwwMotors();
    else if (input == "t") res = setupMotors();
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
