
#ifndef __FIELDS_H_
#define __FIELDS_H_
// MOtor Fields
#include <iostream>
#include <fstream>
#include <string>
#include <cjson/cJSON.h>


using namespace std;

class Field {
public:
  Field(string n, string t);
  ~Field(){};
  Field *getNext(){return next;};
  void setNext(Field *f){next=f;};
  string getName(){return name;};
  string getType(){return type;};
  string getValue(){return value;};
  string getField();
  string jsonField();
  string wwwField(string fname);
  string wwwRange(string fname);
  void setValue(string v){value = v;return;};

private:
  string name;
  string type;
  string value;
  Field *next;
  
};
string showFields(Field *f);
string jsonFields(Field *f);
string wwwFields(Field *f, string name);

#endif
