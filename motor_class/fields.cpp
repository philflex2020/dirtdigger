
// MOtor Fields
#include <iostream>
#include <fstream>
#include <string>
#include <cjson/cJSON.h>
#include "fields.h"


Field::Field(string n, string t) : name(n), type(t), value("0"),next(NULL) {
  cout<<" created new field ["<< name<<"]"<<endl;
}

string Field::getField()
{
  string res;
  res = name + "(" + type + ") -> [" + value + "]"; 
  return res;
  
}

string Field::jsonField()
{
  string res;
  res = "\"" +name + "\":\"" + value + "\""; 
  return res;
  
}

string Field::wwwRange(string fname)
{
  string res = "";
  string id = fname; id += "_";
  id += getName();
  
  res += " id =\""; 
  res += id; 
  res += "\"";
  //form_name_disp.value
  res +=" oninput=\""; res +=id; res+= "_disp.value = ";
  res += id; res += ".value\"";
  
  res += ">\n"; 
  res += " <output id=\"";res += id; res += "_disp\">";
  //fs+=" value=\"";
  res += getValue();
  res +="</output";
  return res;
}

string Field::wwwField(string fname)
{
  string res;
  res = name + ":";
  res +="<input type=\""; res += getType(); 
  res +="\" name=\""      ; res += getName(); 
  res +="\" value=\""     ; res+=getValue(); res +="\"";
  if(getType() =="range") {
    res += wwwRange(fname);
  }
  
  res += "><br>"; 
  res +="\n";
  return res;
}

// helper functions

string showFields(Field *f)
{
  string res = "";
  while (f) {
    res += "   " + f->getField() + "\n";
    f = f->getNext();
  }
  return res;
  
}
string jsonFields(Field *f)
{
  string res = "";
  while (f) {
    res += f->jsonField();
    f = f->getNext();
    if(f) res += ",";
  }
  return res;
}

string wwwFields(Field *f, string name)
{
  string res = "";
  while (f) {
    res += f->wwwField("motor_"+name);
    f = f->getNext();
    //if(f) res += ",";
  }
  return res;
}
  
