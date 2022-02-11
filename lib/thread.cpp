// thread setup


#include <iostream>
#include <string.h>
#include <string>
#include <sstream>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#include "array.h"
#include "tests.h"
#include "servo.h"
#include "thread.h"
#include "vars.h"

using namespace std;

extern void *g_tdest;
Array *g_threadArray = NULL;
int g_thread_id = 0;

static int t_sleepmS(struct timespec *now, int ms)
{
  clock_gettime(CLOCK_MONOTONIC, now);
  if (ms < 0) {
 
    return 0;
  }
  while (ms > 1000) {
    ms -= 1000;
    now->tv_sec++;
  }
  // Add the time you want to sleep
  now->tv_nsec += (1000000 * ms);
  
  // Normalize the time to account for the second boundary
  while(now->tv_nsec >= 1000000000) {
      now->tv_nsec -= 1000000000;
      now->tv_sec++;
    }
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, now, NULL);
  return 0;
  
}
// commands are :
// set per <per>
// run <nruns>
// enable
// disable
// add item <name> 
// del item <name> 
// show items <NULL>
// show config
// run  <nruns>
// on run <function>  TODO
// on quit <function>  TODO
//
//VarItem *findVarItem(string iname);

int setupThreadCmd(void *tdest, Thread *s, Array *cA)
{
    stringstream sout;
    SockThread *st = (SockThread *)tdest;
    int rc=0;
    int sval;
    char *ckey;
    char *c0 = (char *)cA->getId(0);

    rc = cA->size();
    sout << "Thread::setupCmd name [" << s->name
	 << "] We got Setup "
	 << " c0 [" << c0 <<"]"
	 << " rc " << rc <<  endl;
    st->send(sout);
    if(rc > 0) {
      if (string(c0) == "setup") {
	ckey = findKeyItem(cA, (char *) "per");
	if(ckey) {
	  sscanf(ckey, "%d", &sval);
	  sout << "Thread per [" << ckey << "] (" << sval << ")"<< endl;
	  s->per = sval;
	}
	ckey = findKeyItem(cA, (char *) "run");
	if(ckey) {
	  sscanf(ckey, "%d", &sval);
	  sout << "Thread run [" << ckey << "] (" << sval << ")"<< endl;
	  //	  s->run = sval;
	  //s->setUpRun(tdest, sval);

	}
	
      }
      
      st->send(sout);
    }
    
    return 0;
};

  

int sendThreadCmd(void *tdest, Thread *th, Array *cA)
{
    stringstream sout;
    SockThread *st = (SockThread *)tdest;

    int rc=0;
    char *c = (char *)cA->getId(0);
    char *n = (char *)cA->getId(1);
    char *t = (char *)cA->getId(2);
    //char *d = (char *)cA->getId(3);
    //char *e = (char *)cA->getId(4);

    rc = cA->size();
    sout << "Thread::sendCmd name [" << th->name
	 << "] We got a Thread Cmd [" << c << "]"
	 << " n [" << n <<"]"
	 << " t [" << t <<"]"
	 << " rc " << rc <<  endl;
    st->send(sout);
    if(rc > 0) {
      // set per
      if ((string(c) == "set") && (string(n) == "per")) {
	int sper;
	sscanf(t, "%d", &sper);
	sout << "Thread set per [" << t << "] (" << sper << ")"<< endl;
	
	th->per = sper;
      }
      // show per
      else if ((string(c) == "show") && (string(n) == "per")) {
	sout << " " << th->name << " per [" << th->per << "]" << endl;
	
      }
      // show count
      else if ((string(c) == "show") && (string(n) == "count")) {
	sout << " " << th->name << " count [" << th->count << "]" << endl;
	
      }
      // enable
      else if ((string(c) == "enable") /*&& (string(n) == "count")*/) {
	sout << " " << th->name << " enabled" << endl;
	th->enable = 1;
      }
      // disable
      else if ((string(c) == "disable") /*&& (string(n) == "count")*/) {
	sout << " " << th->name << " disabled" << endl;
	th->enable = 0;
      }
      
      //add item  the name is the varItem name
      else if ((string(c) == "add") && (string(n) == "item")) {
	//int spos;
	//int stime;
	//sscanf(t, "%d", &spos);
	//sscanf(d, "%d", &stime);
	//s->addMove(new Move(s->pin, spos, stime));
	VarItem *vi = findVarItem(string (t));
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
	
      }
      //del item
      else if ((string(c) == "del") && (string(n) == "item")) {
	//int spos;
	//int stime;
	//sscanf(t, "%d", &spos);
	////sscanf(d, "%d", &stime);
	//s->addMove(new Move(s->pin, spos, stime));
	sout << "Thread del item  [" << " TODO" << "] name [ " << t << "]"<< endl;
	
      }
      // show items
      else if ((string(c) == "show") && (string(n) == "items")) {
	if(th->funArray) {
	  ThreadFun *tfun = NULL;
	  int idx = 0;
	  while(th->funArray->findNext((void **)&tfun, idx, 0)){
	    cout << " Item [" << idx
		 << "] fun->name [" << tfun->name
		 << "] fun->ftype [" << tfun->ftype<< "] "
		 << endl;
	    
	    st->send(sout);
	  }
	}
	sout << " End of items " << endl; 
	st->send(sout);
      }
      // show config
      else if ((string(c) == "show") && (string(n) == "config")) {

	sout << "send " << th->name << " per " << th->per <<endl;
	sout << "send " << th->name << " enabled " << th->enable <<endl;
	sout << "send " << th->name << " count " << th->count <<endl;
	
	st->send(sout);
	  
	  
      }
      // attach thread <name> (or none for detach) TODO
      // TODO
      else if ((string(c) == "attach") && (string(n) == "item")) {
	
	sout << "send " << th->name << " attach item " << t <<endl;
      }
      // run  <nruns>
      // TODO
      else if ((string(c) == "run")) {
	
	sout << "run " << th->name << " num " << n << endl;
	th->run(atoi(n));
      }
      
      // on step <function>  TODO
      else if ((string(c) == "on") && (string(n) == "step")) {
	
	sout << "send " << th->name << " on step " << n <<endl;
      }
      // on move <function>  TODO
      else if ((string(c) == "on") && (string(n) == "move")) {
	
	sout << "send " << th->name << " on move " << n <<endl;
      }
      else if ((string(c) == "help")) {
	
	sout <<	  "help, commands " << endl;
	sout <<   "set per <per>" << endl;
	sout <<   "enable" << endl;
	sout <<   "disable" << endl;
	sout <<   "show per" << endl;
	sout <<   "show count" << endl;
	sout <<   "add item <item>" << endl;
	sout <<   "del item <item>" << endl;
	sout <<   "show items" << endl;
	sout <<   "show config" << endl;
	sout <<   "run  <nruns>" << endl;
	//sout <<   "attach thread <name> (or none for detach) TODO" << endl;
	sout <<   "on run <function>  TODO" << endl;
	sout <<   "on quit <function>  TODO" << endl;

      }
      st->send(sout);
    }
    
    return 0;
};


// a ThreadFun can be a pointer to a varItem
// a Varitem can be any object
// lets get away from Threadfun's if we can
//

// for examle we want to add Servo1 (varItem to this list ) but want to keep
// away from a cross linked structure
// so the varItem->run command is in vars.cpp and not in here
// we really need to register a run function with the varItem type
// we associate a varItem with a thread like this
// send <Thread->name> add item <varItem->name>
// this will create the ThreadFun object and set its action to the VarItem and its type to "VarItem"
// the ThreadRunVar command is in vars.cpp

int threadRunVar(Thread *t, ThreadFun *tfun);

void *threadFcn(void *data)
{
  Thread *t = (Thread *)data;
  cout << " Thread [" << t->name << "] started" << endl;
  //t_sleepmS(&t->now, -1);
  while (!t->done) {
    if(t->per == 0) {
	sem_wait(&t->t_sem);
    } else {
      t_sleepmS(&t->now, t->per);
      //t_sleepmS(&t->now, -1);
    }
    t->count ++;
    if(!t->done) {
      if(t->funArray) {
	ThreadFun *tfun = NULL;
	int idx = 0;
	while(t->funArray->findNext((void **)&tfun, idx, 0)){
	  cout << " Thread [" << t->name << "] fun ["
	       << tfun<< "] running idx " << idx
	       << " thread count " << t->count << endl;
	  threadRunVar(t, tfun);
	  //tfun->run(tfun->data);
	}
      }
    }
  }
  return NULL;
}

int newThread(void *tdest, Array *cA, void *data)
{
  // thread name [per]
  stringstream sout;
  SockThread *st = (SockThread *)tdest;
  int rc;
  //char *scmd =  (char *)cA->getId(0);
  char *sname = (char *)cA->getId(1);
  char *sper =  (char *)cA->getId(2);
  //char *stime =  (char *)cA->getId(3);
  rc = cA->size();

  //Servo *s;
  Thread *th;
  int per = 0;

  if(rc > 0) {
    if(rc> 1) {
      per = atoi(sper);      
      th = new Thread(string(sname), per, NULL, tdest);
      if(0) cout << " new Thread " << th->name << endl;
    }
  }
  sout << " Thread " << th->name << " created" << endl;
  st->send(sout); 
  return 0;
}

int showThreads(void *tdest, Array *cA, void *data)
{
  // thread name [per]
  stringstream sout;
  SockThread *st = (SockThread *)tdest;
  //Servo *s;
  int rc = cA->size();
  if(rc > 0) {
    Thread * th = NULL;
    int idx = 0;
    if(g_threadArray) {
      while (g_threadArray->findNext((void **)&th, idx, 0)) {
	sout << " Thread [" << th->name << "] count ("
	     << th->count << ")" << endl;
	st->send(sout);
      }
    }
  }
  sout << " End of Threads " << endl;
  st->send(sout); 
  return 0;
}

Thread *findThread(void *tdest, char *name)
{
  // thread name [per]
  stringstream sout;
  SockThread *st = (SockThread *)tdest;
  Thread *th;
  int idx = 0;
  if(g_threadArray) {
    while (g_threadArray->findNext((void **)&th, idx, 0)) {
      if (th->name == (string)name) {
	sout << "Thread " << name << " Found" << endl;
	st->send(sout); 
	return th;
      }
    }
  }

  return NULL;
}

int runThread(void *tdest, Array *cA, void *data)
{
  // thread name [per]
  stringstream sout;
  SockThread *st = (SockThread *)tdest;
  //Servo *s;
  int rc;
  //char *scmd =  (char *)cA->getId(0);
  char *sname = (char *)cA->getId(1);
  char *sper =  (char *)cA->getId(2);
  //char *stime =  (char *)cA->getId(3);

  //Thread *th;
  int runs = 1;

  rc = cA->size();

  if(rc > 1) {
    Thread *th = findThread(tdest,sname);
    if(rc > 2) {
      runs = atoi(sper);
    }
    if(th)
      th->run(runs);
  }
  sout << " End of runThread " << endl;
  st->send(sout); 
  return 0;
}
  
#ifdef _MAIN

int main(int argc, char *argv[])
{
  initTests();
  addTest("newThread", "newThread [name, [per]]", newThread, NULL, g_tdest);
  addTest("showThreads", "showThreads ", showThreads, NULL, g_tdest);
  addTest("runThread", "runThread [name, [count]]", runThread, NULL, g_tdest);


  for (int i=1 ; i < argc ; i++ ) {
    runTest(g_tdest, argv[i]);
  }

  delTests();

  
}
#endif

