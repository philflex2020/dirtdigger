// basic servo movement


#include <iostream>
#include <string.h>
#include <string>
#include <sstream>
#include <time.h>

#include "array.h"
#include "tests.h"
#include "servo.h"

using namespace std;
Array *g_servoArray = NULL;

pthread_t runServopth;
int g_runServoDone = 0;
extern void *g_tdest;
int g_servo_id = 0;

#define SERVO_RATE_MS  10


int ShowServoOutput(int pin, int pos, int count)
{
  cout << "servo pin " << pin<<" new pos " << pos << "count " << count << endl;
  return 0;
}

int SetServoOutput(int pin, int pos, int count)
{
  cout << "servo pin " << pin<<" new pos " << pos << "count " << count << endl;
  return 0;
}

int sleepmS(struct timespec *now, int ms)
{
  if (ms < 0) {
      return 0;
    }
  clock_gettime(CLOCK_MONOTONIC, now);
  while(ms >=1000) {
      ms -= 1000;
      now->tv_sec++;
  }
    // Add the time you want to sleep
  now->tv_nsec += (1000000 * ms);
  
  // Normalize the time to account for the second boundary
  if(now->tv_nsec >= 1000000000) {
      now->tv_nsec -= 1000000000;
      now->tv_sec++;
    }
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, now, NULL);
  return 0;
  
}

int setServoVal(string name, char *spos, char *stime)
{
  int idx= 0;
  Servo *servo= NULL;
  if(g_servoArray) {
    while (g_servoArray->findNext((void**)&servo, idx, 0) ) {
      if(servo->name == name ) {
	
	servo->setVal(spos, stime);
	return 0;
      }
    }
  }
  return 0;
} 



int showServoVal(string name)
{
  int idx= 0;
  Servo *servo= NULL;
  if(g_servoArray) {
    while (g_servoArray->findNext((void**)&servo, idx, 0) ) {
      if(servo->name == name ) {
	
	servo->pc+=1;
	return 0;
      }
    }
  }
  return 0;
} 

int runSingleServo(Servo *s)
{
  //  struct timespec now;
  //int idx;
  //Servo *servo = (Servo *)data;

  

  if((s->pos == s->rpos) 
     && s->movArray && (s->movArray->size()> 0)) {
    s->nextMove();
  }
  
  if(s->pos != s->rpos) {
    if(s->pos > s->rpos) {
      s->pos -= s->incr;
      if(s->pos < s->rpos) {
	s->pos = s->rpos;
	s->time = 0;
      }
    }
    if(s->pos < s->rpos) {
      s->pos += s->incr;
      if(s->pos > s->rpos) {
	s->pos = s->rpos;
	s->time = 0;
      }
      
    }
    s->time -= SERVO_RATE_MS;
    s->count++;
    if (s->pc > 0) {
      s-> pc--;
      ShowServoOutput(s->pin, s->pos, s->count);
    }
    SetServoOutput(s->pin, s->pos, s->count);
  }
  return 0;
}

void *runServo(void *data)
{
  struct timespec now;

  int idx;
  Servo *servo;

  sleepmS(&now, -1);
  
  while (!g_runServoDone) {
    sleepmS(&now, SERVO_RATE_MS);
    

    idx = 0;
    servo = NULL;
    if(g_servoArray) {
      while (g_servoArray->findNext((void**)&servo, idx, 0) ) {
	runSingleServo(servo);
      }
    }
  }
  return NULL;
  
}

int Servo::init(int xpin)
{
  if(g_servoArray == NULL) {
    g_servoArray = new Array(16, 16, "Servos");
    //pthread_create(&runServopth, NULL, runServo, NULL);
   }
  pin = xpin;
  pos = 0;
  time = 0;
  //g_servoArray->addId(pin,(void *)this);
  count = 0;
  pc = 0;
  movArray = NULL;
  id = g_servo_id++;
  return 0;
}

Servo::Servo(int xpin)
{
  init(xpin);
}

Servo::Servo(char *sname, char *spin, char *spos)
{
  name = (string)sname;

  pin = atoi(spin);
  init(pin);
  
  if(spos) {
    pos = atoi(spos);
  } else {
    pos = 0;
  }
}

Servo::~Servo()
{
  g_servoArray->delId(pin, this);
  if(movArray) {
    int idx = 0;
    Move *mv = NULL;
    while(movArray->findNext((void **)&mv, idx, 0)) {
      delete mv;
      mv = NULL;
    }
  }
}


void Servo::setVal(int val, int itime)
{
  rpos = val;
  time = itime;
  if(itime) {
    int steps = (itime / SERVO_RATE_MS) + 1 ;
    int dist = rpos - pos;
    if (dist < 0) dist = -dist;
    incr = dist / steps;
  } else {
    pos = val;
    time = 0;
  }
}

void Servo::setVal(char *spos, char *stime)
{
  int xtime = 0;
  int xpos = atoi(spos);
  if(stime)
    xtime = atoi(stime);
  setVal(xpos, xtime);
}

void Servo::setMove(char *spos, char *stime)
{

  int xtime = 0;
  int xpos = atoi(spos);
  if(stime)
    xtime = atoi(stime);
  Move *mv = new Move(pin, xpos, xtime);
  addMove(mv);
}

Thread *Servo::setUpRun(void *tdest, int run)
{
  char tname[1024];
  stringstream sout;
  SockThread *st = (SockThread *)tdest;
  snprintf(tname, sizeof(tname), "G_RUN_TH_%04d", run);
  Thread*th = findThread(tdest, tname);
  if (th == NULL){
    th = new Thread(string(tname), run, NULL, tdest);
  }
  VarItem *vi = findVarItem(name);
  ThreadFun *tfun = new ThreadFun(name, vi, th );
  if(vi) {
    tfun->ftype = vi->vtype;
  }
  if(!th->funArray) {
    th->funArray = new Array(16,4,"ThreadFuns");
  }
  th->funArray->addItem(tfun);
  
  sout << "Thread add item   name [" << name << "]"<< endl;
  st->send(sout);
  return (Thread *)th;
}

int setupServoCmd(void *tdest, Servo *s, Array *cA)
{
    stringstream sout;
    SockThread *st = (SockThread *)tdest;
    int rc=0;
    int sval;
    char *ckey;
    char *c0 = (char *)cA->getId(0);

    rc = cA->size();
    sout << "Servo::setupCmd name [" << s->name
	 << "] We got Setup "
	 << " c0 [" << c0 <<"]"
	 << " rc " << rc <<  endl;
    st->send(sout);
    if(rc > 0) {
      if (string(c0) == "setup") {
	ckey = findKeyItem(cA, (char *) "pin");
	if(ckey) {
	  sscanf(ckey, "%d", &sval);
	  sout << "Servo pin [" << ckey << "] (" << sval << ")"<< endl;
	  s->pin = sval;
	}
	ckey = findKeyItem(cA, (char *) "pos");
	if(ckey) {
	  sscanf(ckey, "%d", &sval);
	  sout << "Servo pos [" << ckey << "] (" << sval << ")"<< endl;
	  s->pos = sval;
	}
	ckey = findKeyItem(cA, (char *) "base");
	if(ckey) {
	  sscanf(ckey, "%d", &sval);
	  sout << "Servo base [" << ckey << "] (" << sval << ")"<< endl;
	  s->base = sval;
	}
	ckey = findKeyItem(cA, (char *) "max");
	if(ckey) {
	  sscanf(ckey, "%d", &sval);
	  sout << "Servo max TODO[" << ckey << "] (" << sval << ")"<< endl;
	  //s->max = sval;
	}
	ckey = findKeyItem(cA, (char *) "min");
	if(ckey) {
	  sscanf(ckey, "%d", &sval);
	  sout << "Servo min TODO[" << ckey << "] (" << sval << ")"<< endl;
	  //s->max = sval;
	}
	ckey = findKeyItem(cA, (char *) "run");
	if(ckey) {
	  sscanf(ckey, "%d", &sval);
	  sout << "Servo run [" << ckey << "] (" << sval << ")"<< endl;
	  s->srun = string(ckey);
	  s->run = sval;
	  s->setUpRun(tdest, sval);

	}
	
      }
      
      st->send(sout);
    }
    
    return 0;
};

// commands are :
// set pin <pin>
// set pos <pos>
// set base <base>
// add move <move>
// show moves <NULL>
// show config
// run  <nruns>
// attach thread <name> (or none for detach) TODO
// on step <function>  TODO
// on move <function>  TODO
//
int sendServoCmd(void *tdest, Servo *s, Array *cA)
{
    stringstream sout;
    SockThread *st = (SockThread *)tdest;

    int rc=0;
    char *c = (char *)cA->getId(0);
    char *n = (char *)cA->getId(1);
    char *t = (char *)cA->getId(2);
    char *d = (char *)cA->getId(3);
    //char *e = (char *)cA->getId(4);

    rc = cA->size();
    sout << "Servo::sendCmd name [" << s->name
	 << "] We got a Servo Cmd [" << c << "]"
	 << " n [" << n <<"]" << " rc " << rc <<  endl;
    st->send(sout);
    if(rc > 0) {
      // set pin
      if ((string(c) == "set") && (string(n) == "pin")) {
	int spin;
	sscanf(t, "%d", &spin);
	sout << "Servo set pin [" << t << "] (" << spin << ")"<< endl;
	
	s->pin = spin;
      }
      // set pos
      else if ((string(c) == "set") && (string(n) == "pos")) {
	int spos;
	sscanf(t, "%d", &spos);
	sout << "Servo set pos [" << t << "] (" << spos << ")"<< endl;
	
	s->pos = spos;
	
      }      // show pos
      else if ((string(c) == "show") && (string(n) == "pos")) {
	int spos;
	sscanf(t, "%d", &spos);
	sout << " " << s->name << " pos [" << s->pos << "]" << endl;
	
	s->pos = spos;
	
      }
      // set base
      else if ((string(c) == "set") && (string(n) == "base")) {
	int sbase;
	sscanf(t, "%d", &sbase);
	sout << "Servo set base [" << t << "] (" << sbase << ")"<< endl;
	
	s->base = sbase;
	
      }
      //add move
      else if ((string(c) == "add") && (string(n) == "move")) {
	int spos;
	int stime;
	sscanf(t, "%d", &spos);
	sscanf(d, "%d", &stime);
	s->addMove(new Move(s->pin, spos, stime));
	sout << "Servo add move [" << t << ", " << d << ")"<< endl;
	
	
      }
      // show moves
      else if ((string(c) == "show") && (string(n) == "moves")) {
	if(s->movArray) {
	  Move *mv = NULL;
	  int idx = 0;
	  while(s->movArray->findNext((void **) &mv, idx, 0)) {
	    sout << "   Move  [" << idx
		 <<"] dest " << mv->dest
		 << " time "<< mv->time
		 << " "<< endl;
	    st->send(sout);
	  }
	}
	sout << " End of moves " << endl; 
	st->send(sout);
      }
      // show config
      else if ((string(c) == "show") && (string(n) == "config")) {

	sout << "send " << s->name << " pin " << s->pin <<endl;
	sout << "send " << s->name << " pos " << s->pos <<endl;
	sout << "send " << s->name << " base " << s->base <<endl;
	
	st->send(sout);
	  
	  
      }
      // attach thread <name> (or none for detach) TODO
      // TODO
      else if ((string(c) == "attach") && (string(n) == "thread")) {
	
	sout << "send " << s->name << " attach thraed " << t <<endl;
      }
      // run  <nruns>
      // TODO
      else if ((string(c) == "run")) {
	
	sout << "run " << s->name << " num " << n << endl;
      }
      
      // on step <function>  TODO
      else if ((string(c) == "on") && (string(n) == "step")) {
	
	sout << "send " << s->name << " on step " << n <<endl;
      }
      // on move <function>  TODO
      else if ((string(c) == "on") && (string(n) == "move")) {
	
	sout << "send " << s->name << " on move " << n <<endl;
      }
      else if ((string(c) == "help")) {
	
	sout <<	  "help, commands " << endl;
	sout <<   "set pin <pin>" << endl;
	sout <<   "set pos <pos>" << endl;
	sout <<   "show pos" << endl;
	sout <<   "set base <base>" << endl;
	sout <<   "add move <move>" << endl;
	sout <<   "show moves <NULL>" << endl;
	sout <<   "show config" << endl;
	sout <<   "run  <nruns>" << endl;
	sout <<   "attach thread <name> (or none for detach) TODO" << endl;
	sout <<   "on step <function>  TODO" << endl;
	sout <<   "on move <function>  TODO" << endl;

      }
      st->send(sout);
    }
    
    return 0;
};
  
#ifdef _MAIN

int testServo(void *tdest, Array *cA, void *data)
{
  // servo name pin [pos]
  stringstream sout;
  SockThread *st = (SockThread *)tdest;
  Servo *s;
  int rc;
  // char *scmd =  (char *)cA->getId(0);
  char *sname = (char *)cA->getId(1);
  char *spin =  (char *)cA->getId(2);
  char *spos =  (char *)cA->getId(3);
  rc = cA->size();

  if (rc < 3 ) {
    sout <<"Usage servo name pin [pos] " << endl;
    st->send(sout);
    return 0;
  }
  if (rc > 3 ) {
    sout <<"servo name " << sname 
	 << " pin " << spin 
	 << " pos " << spos << endl;
    s = new Servo(sname, spin, spos);
  } else {
    sout <<"servo name " << sname 
	 << " pin " << spin 
	 << " pos " << "0" << endl;
    s = new Servo(sname, spin, (char *)"0");
  }
  if(0) cout<< " Servo Created " << s->pin << endl;
  st->send(sout);

  return 0;
}

int testSetVal(void *tdest, Array *cA, void *data)
{
  // servo name val  [time]
  stringstream sout;
  SockThread *st = (SockThread *)tdest;
  //Servo *s;
  int rc;
  //char *scmd =  (char *)cA->getId(0);
  char *sname = (char *)cA->getId(1);
  char *spos =  (char *)cA->getId(2);
  char *stime =  (char *)cA->getId(3);
  rc = cA->size();

  if (rc < 3 ) {
    cout <<"Usage setVal name pos [time] " << endl;
    return 0;
  }
  if (rc > 3 ) {
    sout <<"servo name " << sname 
	 << " pos " << spos 
	 << " time " << stime << endl;
    setServoVal((string)sname, spos, stime);
  } else {
    sout <<"servo name " << sname 
	 << " pos" << spos 
	 << " time " << "0" << endl;
    setServoVal((string)sname, spos, (char *)"0");
  }
  st->send(sout);

  return 0;
}



int testShowVal(void *tdest, Array *cA, void *data)
{
  // servo name val  [time]
  stringstream sout;
  SockThread *st = (SockThread *)tdest;
  int rc;
  //char *scmd =  (char *)cA->getId(0);
  char *sname = (char *)cA->getId(1);
  //  char *spos =  (char *)cA->getId(2);
  //char *stime =  (char *)cA->getId(3);
  rc = cA->size();

  if (rc < 2 ) {
    sout <<"Usage showVal name " << endl;
    st->send(sout);
    return 0;
  }
  if (rc >= 2 ) {
    sout <<"servo name " << sname << endl;
    st->send(sout);
    showServoVal((string)sname);
  } 
  return 0;
}

int main(int argc, char *argv[])
{

  initTests();

  addTest("servo", "servo name pin [pos] create servo", testServo, NULL, g_tdest);
  addTest("setVal", "setval [pos,[time]]", testSetVal, NULL, g_tdest);
  addTest("showVal", "showval [name]", testShowVal, NULL, g_tdest);


  for (int i=1 ; i < argc ; i++ ) {
    runTest(g_tdest, argv[i]);
  }

  delTests();
  return 0;
}
#endif
