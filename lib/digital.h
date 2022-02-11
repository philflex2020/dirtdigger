//// servo class driver

#ifndef __DIGITAL_H
#define __DIGITAL_H

#include "sock.h"
#include "array.h"
#include "tests.h"
#include "vars.h"
#include "thread.h"


using namespace std;
struct Digital;

int sendDigitalCmd(void *tdest, Digital *s, Array *cmd);
int setupDigitalCmd(void *tdest, Digital *s, Array *cmd);
int runSingleDigital(Digital *d);

#if 0
struct Move {
  Move(int xpin, int xdest, int xtime) {
    pin = xpin;
    dest =  xdest;
    time = xtime;
  };
  ~Move(){};
  int pin;
  int dest;
  int time;
};

#endif

struct Digital : public Var {
  Digital(int pin, int dir);
  Digital(char *sname, char *spin, char *sdir, char *spos);
  ~Digital();
  int init(int pin, int dir);
  
  void setVal(int val, int time);
  void setVal(char *spos, char *stime);
  //void setMove(char *spos, char *stime);

  //int addMove(Move *mv) {
  // if(movArray == NULL) {
  //  movArray = new Array(16,16, "Moves");
  //}
  //movArray->pushItem(mv);
  //return 0;
  //};

  //int nextMove(void) {
  // Move *mv = (Move *)movArray->popItem();
  //if (mv) {
  //  setVal(mv->dest, mv->time);
  //  delete mv;
  //}
  //return 0;
  //}

  int sendCmd(void *tdest, Array *cA) {  
    return sendDigitalCmd(tdest, this, cA);
  }

  Thread *setUpRun(void *tdest, int run);

  string name;

  int pin;

  int pos;
  int base;
  int rpos;
  int time;
  int incr;
  int pc;
  int val;
  int dir;


  int on;
  int off;
  int count;
  //Array *movArray;
  int id;
  
};






#endif
