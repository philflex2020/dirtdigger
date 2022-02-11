#include <iostream>
#include <string>
#include <string.h>

// Serial.print 

using namespace std;

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
      input_t *input = &form->inputs[j];
      input->name[0]=0;    
      input->value[0]=0;    
      input->type[0]=0;    
    }
  }
  return 0;
}

int setInputToForm(int fnum, int inum, string name, string value, string type)
{
  form_t *form = &forms[fnum];
  input_t *input = &form->inputs[inum];
  snprintf(input->name, 32, "%s",name.c_str());
  snprintf(input->value, 32, "%s",value.c_str());
  snprintf(input->type, 32, "%s",type.c_str());
  return 0;
}

int setUpForm(int fnum, string name)
{
  form_t *form = &forms[fnum];
  snprintf(form->name, 32, "%s",name.c_str());
  return 0;
}

string getForm(int fnum)
{
  string fs="";
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
        string id = form->name; id += "_"; id += input->name;
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
      fs += "><br>"; 
      fs +="\n";

    }
  }
  
  fs += "<input type=\"submit\" value=\"Submit\">";
  fs += "</form>\n";

  return fs;
}

int setUpMForm(int num, string name)
{
  setUpForm(num, name);
  setInputToForm(num, 0, 
		 "name", "f0value", "text");
  setInputToForm(num, 1, "speed", "20", "range");
  setInputToForm(num, 2, "accel", "30", "range");
  setInputToForm(num, 3, "time", "90", "range");
  return num;
}

string getForms()
{
  string fs = "";
  for (int i = 0 ; i< MAX_FORMS; i++ ) {
    form_t *form = &forms[i];
    if(form->name[0] == 0) continue;
    string  ff = getForm(i);
    fs += ff;
  }
  return fs;
}

//Referer: http://192.168.4.1/motor0?name=f0value&speed=20&accel=30&time=90
//Referer: http://192.168.4.1/H
string getValue(int jdx, string mLine) {
   
  string rstr = "";
  //rstr.concat(mLine.charAt(jdx));
  while((mLine[jdx+1] != '&') 
     && (mLine[jdx+1] != '\n')
     && (mLine[jdx+1] != ' ') 
	&& (jdx+1) < (int)mLine.length()) {
    rstr += mLine[jdx+1];
    jdx += 1;
  }
  return rstr;
}

int getInput(input_t *input, string mLine) {
  if(input->name[0] == 0) return 0;
  string istr = input->name;
  istr += "=";
  //int jdx = mLine.indexOf(istr);
  int jdx = mLine.find(istr);
  if(jdx < 0) return 0;
  jdx += strlen(input->name);
  string rstr = getValue(jdx, mLine);
  
  cout <<"Found value: [";
  cout <<rstr;
  cout <<"]\n";
  snprintf(input->value, 32, "%s",rstr.c_str());
  return 0;
}
    
int decodeForm(string mLine);

int decodeForms(string mLine, char c)
{
  //if (mLine.endsWith("GET /H")) {
    //digitalWrite(LED_BUILTIN, HIGH);               // GET /H turns the LED on
  //}
  //else if (mLine.endsWith("GET /L")) {
    //digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
  //} else {
  //}
  if(c != '\r') return 0;
  int ret = decodeForm(mLine);
  return ret;
}

int decodeForm(string mLine)
{
  //if (mLine.indexOf("GET /") != 0) return 0;
  if (mLine.find("GET /") != 0) return 0;
  for (int i = 0 ; i< MAX_FORMS; i++ ) {
    form_t *form = &forms[i];
    
    //int idx = mLine.indexOf(form->name);
    int idx = mLine.find(form->name);
    if (idx <= 0) continue;
    cout <<"\nChecking form :[";
    cout <<form->name;
    cout <<"]\n";

    if (mLine[idx-1] != '/') return 0;
    if (mLine[idx+strlen(form->name)] != '?') return 0;
    for (int j = 0 ; j< MAX_INPUTS; j++ ) {
         input_t * input = &form->inputs[j];
         getInput(input, mLine);

    }
  }
  return 0;

}
#if 0
int main()
{
  cout <<" Hi there" << endl;
  setUpMForm(4, "motors");
  cout <<getForms()<< endl;

  return 0;
}
#endif
