
#include <stdlib.h>
#include <Arduino.h>

class Task {
public:
Task(){ 
  
  count = 0; 
  ticks = 0; 
  fcn = NULL; 
  data = NULL;
  runs = 0;
  state = 0;
  next = NULL;
  name = "";
  };
~Task(){};

int runTask() {
  ticks ++;
  if (ticks > count) {
    ticks = 0;
    if (fcn) {
      //void (*myfcn) (void *, void *);
      //myfcn = (void (*)(void *, void *))fcn;
      fcn(this, data);   
     }
     
  }
  return 0;
}

int addTask(Task * ntask) {
  if (next != NULL) {
    ntask->next = next;
  }
  next=ntask;
  }

int count;
int ticks;
void (* fcn)(void *, void *);
void * data;
int runs;
int state;
String name;
Task * next;

};
Task *setupTask(int count ,void (*myfcn) (void *, void *));
