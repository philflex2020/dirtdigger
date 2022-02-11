// Digital Input

// addVar D0 DigIn "a Digital Input" key D0
// send D0 setup pin 12 run 10 init

// this command sets up the pin number (12) and attaches to a thread
// gThread_10 which it creates if needed
// the variables D0_val and D0_count can be read at any time

//// "send D0 stop" will stop scanning D0
//// "send D0 start" will start scanning D0

// the following are todo items 
//// "send D0 on set xx" will run thread xx when d0 changes from 0 to 1
//// "send D0 on clear xy" will run thread xy when d0 changes from 1 to 0
//// "send D0 on change xz" will run thread xz when d0 changes 

// The threads mentioned can have actions attached to run functions etc 

// addVar D1 DigOut "a Digital Ouput" key D1
// send D1 setup pin 13 val 1 run 10 init

// this command sets up the pin number (13) and attaches to a thread
// gThread_10 which it creates if needed
// the variables D1_val and D1_count can be read at any time
// the variables D1_val can be written at any time

//// "send D1 stop" will stop output of D1
//// "send D1 start" will start output D1
//// "send D1 val 0" will output 0 on D1 immedately


#include <iostream>
#include <string.h>
#include <string>
#include <sstream>
#include <time.h>

#include "array.h"
#include "tests.h"
#include "digital.h"
#include "thread.h"

using namespace std;



Array *g_digitalArray = NULL;


int g_runDigitalDone = 0;
extern void *g_tdest;
int g_digital_id = 0;

#define DIGITAL_RATE_MS  10


int ShowDigitalOutput(int pin, int val, int count)
{
  cout << "digital pin " << pin<<" new val " << val << "count " << count << endl;
  return 0;
}

int SetDigitalOutput(int pin, int val, int vtime)
{
  cout << "digital pin " << pin<<" new val " << val << "time" << vtime << endl;
  return 0;
}

Thread *Digital::setUpRun(void *tdest, int run)
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

#if 0

// note vi can be null, we'll just look it up later
// that is why we have the tfun 
ThreadFun *tfun = new ThreadFun(t, vi, th );
if(vi) {
  tfun->ftype = vi->vtype;
 }
if(!th->funArray) {
  th->funArray = new Array(16,4,"ThreadFuns");
 }
th->funArray->addItem(tfun);

sout << "Thread add item   name [" << t << "]"<< endl;
	//      Thread *
#endif

#if 0
static int dsleepmS(struct timespec *now, int ms)
{
  if (ms < 0) {
      clock_gettime(CLOCK_MONOTONIC, now);
      return 0;
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
#endif

int setDigitalVal(string name, char *sval, char *stime)
{
  int idx= 0;
  Digital *dig= NULL;
  if(g_digitalArray) {
    while (g_digitalArray->findNext((void**)&dig, idx, 0) ) {
      if(dig->name == name ) {
	
	dig->setVal(sval, stime);
	return 0;
      }
    }
  }
  return 0;
} 



int showDigitalVal(string name)
{
  int idx= 0;
  Digital *dig= NULL;
  if(g_digitalArray) {
    while (g_digitalArray->findNext((void**)&dig, idx, 0) ) {
      if(dig->name == name) {
	
	dig->pc+=1;
	return 0;
      }
    }
  }
  return 0;
} 

int runSingleDigital(Digital *dig)
{
  //  struct timespec now;
  //int idx;
  //Servo *servo = (Servo *)data;

  

  //  if((s->pos == s->rpos) 
  // && s->movArray && (s->movArray->size()> 0)) {
  //s->nextMove();
  //}
  
  //if(s->pos != s->rpos) {
  //if(s->pos > s->rpos) {
  //  s->pos -= s->incr;
  //  if(s->pos < s->rpos) {
  //	s->pos = s->rpos;
  //	s->time = 0;
  //  }
  //}
  //if(s->pos < s->rpos) {
  //  s->pos += s->incr;
  //  if(s->pos > s->rpos) {
  //	s->pos = s->rpos;
  //	s->time = 0;
  //  }
      
  //}
  //dig->time -= SERVO_RATE_MS;
    dig->count++;
    if (dig->pc > 0) {
      dig->pc--;
      ShowDigitalOutput(dig->pin, dig->val, dig->count);
    }
    SetDigitalOutput(dig->pin, dig->pos, dig->count);
  
  return 0;
}

void *runDigital(void *data)
{
  //struct timespec now;

  //int idx;
  //Servo *servo;

  //dsleepmS(&now, -1);
  
  //while (!g_runServoDone) {
  //sleepmS(&now, SERVO_RATE_MS);
    

  //idx = 0;
  //servo = NULL;
  //if(g_servoArray) {
  //  while (g_servoArray->findNext((void**)&servo, idx, 0) ) {
  //	runSingleServo(servo);
  //  }
  //}
  //}
  return NULL;
  
}

int Digital::init(int xpin, int xdir)
{
  if(g_digitalArray == NULL) {
    g_digitalArray = new Array(16, 16, "Digitals");
    //pthread_create(&runServopth, NULL, runServo, NULL);
   }
  pin = xpin;
  dir = xdir;   // 1 == input o = output
  val = 0;
  time = 0;
  count = 0;
  pc = 0;
  sonset = "none";
  sonclear = "none";
  sonchange = "none";

  //movArray = NULL;
  id = g_digital_id++;
  return 0;
}

Digital::Digital(int xpin , int xdir)
{
  init(xpin, xdir);
}

Digital::Digital(char *sname, char *spin, char *stdir, char *sval)
{
  int mydir = 0;  // output
  string sdir(stdir);
  name = (string)sname;

  pin = atoi(spin);
  if((sdir == "input")
     || (sdir == "in" )
     || (sdir == "i" ) ) {
    mydir = 1; // input
  }
  
  init(pin, mydir);
  
  if(sval) {
    val = atoi(sval);
  } else {
    val = 0;
  }
}

Digital::~Digital()
{
  g_digitalArray->delId(pin, this);
  //  if(movArray) {
  //int idx = 0;
  //Move *mv = NULL;
  //while(movArray->findNext((void **)&mv, idx, 0)) {
  //  delete mv;
  //  mv = NULL;
  //}
  //}
}


void Digital::setVal(int ival, int itime)
{
  val = ival;
  time = itime;
  //  if(itime) {
  // int steps = (itime / SERVO_RATE_MS) + 1 ;
  //int dist = rpos - pos;
  //if (dist < 0) dist = -dist;
  //incr = dist / steps;
  //} else {
  //pos = val;
  //time = 0;
  //}
}

void Digital::setVal(char *sval, char *stime)
{
  int xtime = 0;
  int xval = atoi(sval);
  if(stime)
    xtime = atoi(stime);
  setVal(xval, xtime);
}

//void Servo::setMove(char *spos, char *stime)
//{

//  int xtime = 0;
//  int xpos = atoi(spos);
//  if(stime)
//    xtime = atoi(stime);
//  Move *mv = new Move(pin, xpos, xtime);
//  addMove(mv);
//}



// commands are :
// set pin <pin>
// set val <pos>
// set dir <in>
// show config
// on set <function>  TODO
// on clear <function>  TODO


int sendDigitalCmd(void *tdest, Digital *s, Array *cA)
{
    stringstream sout;
    SockThread *st = (SockThread *)tdest;
    int spin;
    int sdir;
    int sval;
    int srun;
    int rc=0;
    char *ckey;
    char *c0 = (char *)cA->getId(0);
    char *c1 = (char *)cA->getId(1);
    char *c2 = (char *)cA->getId(2);
    char *c3 = (char *)cA->getId(3);
    char *c4 = (char *)cA->getId(4);
    //char *c5 = (char *)cA->getId(5);

    rc = cA->size();
    sout << "Digital::sendCmd name [" << s->name
	 << "] We got Cmds "
	 << " c0 [" << c0 <<"]"
	 << " c1 [" << c1 <<"]"
	 << " c2 [" << c2 <<"]"
	 << " rc " << rc <<  endl;
    st->send(sout);
    sout << "Digital::sendCmd name [" << s->name
	 << "] We got a Digital Cmd [" << c1 << "]"
	 << " c1 [" << c2 <<"]" << " rc " << rc <<  endl;
    st->send(sout);
    if(rc > 0) {
      if (string(c2) == "setup") {
	ckey = findKeyItem(cA, (char *) "pin");
	if(ckey) {
	  sscanf(ckey, "%d", &spin);
	  sout << "Digital pin [" << ckey << "] (" << spin << ")"<< endl;
	  s->pin = spin;
	}
	ckey = findKeyItem(cA, (char *) "dir");
	if(ckey) {
	  sscanf(ckey, "%d", &sdir);
	  sout << "Digital dir [" << ckey << "] (" << sdir << ")"<< endl;
	  s->dir = sdir;
	}
	ckey = findKeyItem(cA, (char *) "val");
	if(ckey) {
	  sscanf(ckey, "%d", &sval);
	  sout << "Digital val [" << ckey << "] (" << sval << ")"<< endl;
	  s->val = sval;
	}
	ckey = findKeyItem(cA, (char *) "run");
	if(ckey) {
	  sscanf(ckey, "%d", &srun);
	  sout << "Digital run [" << ckey << "] (" << srun << ")"<< endl;
	  s->srun = string(ckey);
	  s->run = srun;
	  s->setUpRun(tdest, srun);

	}
	ckey = findKeyItem(cA, (char *) "onset");
	if(ckey) {
	  sout << "Digital onset [" << ckey << endl;
	  s->sonset = string(ckey);
	}
	ckey = findKeyItem(cA, (char *) "onclear");
	if(ckey) {
	  sout << "Digital onclear [" << ckey << endl;
	  s->sonclear = string(ckey);
	}
	ckey = findKeyItem(cA, (char *) "onchange");
	if(ckey) {
	  sout << "Digital onchange [" << ckey << endl;
	  s->sonchange = string(ckey);
	}
	
      }
      // set pin
      else if ((string(c2) == "set") && (string(c3) == "pin")) {
	sscanf(c4, "%d", &spin);
	sout << "Digital set pin [" << c4 << "] (" << spin << ")"<< endl;
	s->pin = spin;
      }
      // set dir
      else if ((string(c2) == "set") && (string(c3) == "dir")) {
	sscanf(c4, "%d", &sdir);
	sout << "Digital set dir [" << c3 << "] (" << sdir << ")"<< endl;
	
	s->dir = sdir;
	
      }      // show val
      else if ((string(c2) == "show") && (string(c3) == "val")) {
	//int sval;
	//sscanf(t, "%d", &spos);
	sout << " " << s->name << " val [" << s->val << "]" << endl;
	
	
      }
      // show config
      else if ((string(c2) == "show") && (string(c3) == "config")) {

	sout << "send " << s->name << "setup "
	     << " pin " << s->pin 
	     << " val " << s->val 
	     << " dir " << s->dir 
	     << " run " << s->srun 
	     << " key " << s->skey 
	     << " onset " << s->sonset 
	     << " onclear " << s->sonclear 
	     << " onchange " << s->sonchange
	     <<endl;
	
	st->send(sout);
	  
	  
      }
      // attach thread <name> (or none for detach) TODO
      // TODO
      else if ((string(c2) == "attach") && (string(c3) == "thread")) {
	
	sout << "send " << s->name << " TODO attach thread " << c4 <<endl;
      }
      // run  <nruns>
      // TODO
      else if ((string(c2) == "run")) {
	
	sout << "run " << s->name << " num " << c3 << endl;
      }
      
      // on set <function>  TODO
      else if ((string(c2) == "on") && (string(c3) == "set")) {
	
	sout << "send " << s->name << " on set " << c4 <<endl;
      }
      // on clear <function>  TODO
      else if ((string(c2) == "on") && (string(c3) == "clear")) {
	
	sout << "send " << s->name << " on move " << c4 <<endl;
      }
      else if ((string(c2) == "help")) {
	
	sout <<	  "help, commands " << endl;
	sout <<   "setup pin <pin> ..." << endl;
	sout <<   "set pin <pin>" << endl;
	sout <<   "set val <val>" << endl;
	sout <<   "set dir <1 input, 0 output>" << endl;
	sout <<   "show val" << endl;
	sout <<   "show dir" << endl;
	sout <<   "show config" << endl;
	sout <<   "run  <nruns>" << endl;
	//sout <<   "attach thread <name> (or none for detach) TODO" << endl;
	sout <<   "on set <function>  TODO" << endl;
	sout <<   "on clear <function>  TODO" << endl;

      }
      st->send(sout);
    }
    
    return 0;
};

int setupDigitalCmd(void *tdest, Digital *s, Array *cA)
{
    stringstream sout;
    SockThread *st = (SockThread *)tdest;
    int rc=0;
    int sval;
    char *ckey;
    char *c0 = (char *)cA->getId(0);

    rc = cA->size();
    sout << "Digital::sendCmd name [" << s->name
	 << "] We got Setup "
	 << " c0 [" << c0 <<"]"
	 << " rc " << rc <<  endl;
    st->send(sout);
    if(rc > 0) {
      if (string(c0) == "setup") {
	ckey = findKeyItem(cA, (char *) "pin");
	if(ckey) {
	  sscanf(ckey, "%d", &sval);
	  sout << "Digital pin [" << ckey << "] (" << sval << ")"<< endl;
	  s->pin = sval;
	}
	ckey = findKeyItem(cA, (char *) "dir");
	if(ckey) {
	  sscanf(ckey, "%d", &sval);
	  sout << "Digital dir [" << ckey << "] (" << sval << ")"<< endl;
	  s->dir = sval;
	}
	ckey = findKeyItem(cA, (char *) "val");
	if(ckey) {
	  sscanf(ckey, "%d", &sval);
	  sout << "Digital val [" << ckey << "] (" << sval << ")"<< endl;
	  s->val = sval;
	}
	ckey = findKeyItem(cA, (char *) "run");
	if(ckey) {
	  sscanf(ckey, "%d", &sval);
	  sout << "Digital run [" << ckey << "] (" << sval << ")"<< endl;
	  s->srun = string(ckey);
	  s->run = sval;
	  s->setUpRun(tdest, sval);

	}
	ckey = findKeyItem(cA, (char *) "onset");
	if(ckey) {
	  sout << "Digital onset [" << ckey << endl;
	  s->sonset = string(ckey);
	}
	ckey = findKeyItem(cA, (char *) "onclear");
	if(ckey) {
	  sout << "Digital onclear [" << ckey << endl;
	  s->sonclear = string(ckey);
	}
	ckey = findKeyItem(cA, (char *) "onchange");
	if(ckey) {
	  sout << "Digital onchange [" << ckey << endl;
	  s->sonchange = string(ckey);
	}
	
      }
      
      st->send(sout);
    }
    
    return 0;
};
  
#ifdef _MAIN

int testDigital(void *tdest, Array *cA, void *data)
{
  // servo name pin [pos]
  stringstream sout;
  SockThread *st = (SockThread *)tdest;
  Digital *s;
  int rc;
  //  char *scmd =  (char *)cA->getId(0);
  char *sname = (char *)cA->getId(1);
  char *spin =  (char *)cA->getId(2);
  char *sdir =  (char *)cA->getId(3);
  char *sval =  (char *)cA->getId(4);
  rc = cA->size();

  if (rc < 3 ) {
    sout <<"Usage digital name pin dir val " << endl;
    st->send(sout);
    return 0;
  }
  if (rc > 3 ) {
    sout <<"digital name " << sname 
	 << " pin " << spin 
	 << " dir " << sdir
	 << " val " << sval
	 << endl;
    s = new Digital(sname, spin, sdir, sval);
  } else {
    sout <<"digital name " << sname 
	 << " pin " << spin 
	 << " dir output"
	 << " val 0"
	 << endl;
    s = new Digital(sname, spin, (char *)"output", (char *)"0");
  }
  if(0) cout<< " Digital Created " << s->pin << endl;
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
  //char *scmd  = (char *)cA->getId(0);
  char *sname = (char *)cA->getId(1);
  char *sdir  = (char *)cA->getId(2);
  char *sval  = (char *)cA->getId(3);
  char *stime = (char *)cA->getId(4);
  rc = cA->size();

  if (rc < 3 ) {
    cout <<"Usage setVal name pos [time] " << endl;
    return 0;
  }
  if (rc > 3 ) {
    sout <<"digital name " << sname 
	 << " dir " << sdir 
	 << " val " << sval 
	 << " time " << stime << endl;
    setDigitalVal((string)sname, sval, stime);
  } else {
    sout <<"digital name " << sname 
	 << " val" << sval 
	 << " time " << "0" << endl;
    setDigitalVal((string)sname, sval, (char *)"0");
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
  //char *scmd  = (char *)cA->getId(0);
  char *sname = (char *)cA->getId(1);
  //char *sval  = (char *)cA->getId(2);
  //char *stime = (char *)cA->getId(3);
  rc = cA->size();

  if (rc < 2 ) {
    sout <<"Usage showVal name " << endl;
    st->send(sout);
    return 0;
  }
  if (rc >= 2 ) {
    sout <<"digital name " << sname << endl;
    st->send(sout);
    showDigitalVal(string(sname));
  } 
  return 0;
}

int main(int argc, char *argv[])
{

  initTests();

  addTest("digital", "digital name pin [dir, val] create digital", testDigital, NULL, g_tdest);
  addTest("setVal", "setval [pos,[time]]", testSetVal, NULL, g_tdest);
  addTest("showVal", "showval [name]", testShowVal, NULL, g_tdest);


  for (int i=1 ; i < argc ; i++ ) {
    runTest(g_tdest, argv[i]);
  }

  delTests();
  return 0;
}

#endif
