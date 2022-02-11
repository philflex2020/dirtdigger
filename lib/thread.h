
// thread setup
#ifndef _THREAD_H
#define _THREAD_H


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
#include "vars.h"


using namespace std;

extern Array *g_threadArray;
extern int g_thread_id;

struct Thread;


int setupThreadCmd(void *tdest, Thread *th, Array *cmd);
int sendThreadCmd(void *tdest, Thread *th, Array *cmd);

void *threadFcn(void *data);
Thread *findThread(void *tdest, char *name);

int showThreads(void *tdest, Array *cA, void *data);

struct ThreadFun {

  ThreadFun(string tname, void *taction, void *tdata) {
    name = tname;
    action = taction;
    data = tdata;
  };
  ~ThreadFun(){};

  int run(void *data) {
    cout << " Thread Fun ["<< name << "] running " << endl;
    return 0;
  };
  void *action;
  string name;
  string ftype;
  void *data;
  
};

struct Thread {

  Thread(string tname, int tper, void *tdata , void *tdest) {
    name = tname;
    per = tper;
    data = tdata;
    funArray =  NULL;
    done = 0;
    sem_init(&t_sem, 0, 0);
    count = 0;
    if(g_threadArray == NULL) {
      g_threadArray = new Array (16,4,"Thread Array");
    }
    pthread_create(&thread, NULL, threadFcn,(void *)this);
    g_threadArray->addItemOnce(this);
    id = g_thread_id++;
    
  };
  
  ~Thread(){
    done = 1;
    sem_post(&t_sem);
    pthread_join(thread, NULL);
    
    g_threadArray->_delItem(this);
    if (g_threadArray->size() == 0) {
      delete g_threadArray;
      g_threadArray =  NULL;
    }
      
  };
  
  int run(int num) {
    if(per==0) {
      while (num--) {
	sem_post(&t_sem);
      }
    }
    return 0;
  };
  
  pthread_t thread;
  struct timespec now;
  int done;
  int count;
  string name;
  int per;
  Array *funArray;
  sem_t t_sem;
  void *data;
  int id;
  int enable;
};



#endif

