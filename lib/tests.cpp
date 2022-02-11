// tests
// Sets up a test framework.
// But todo take it down
// Phil Wilshire 2018

#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <sstream>

#include "array.h"
#include "sock.h"
#include "tests.h"

using namespace std;
int readFile(void *tdest, Array *cmd, void *data);

Array *g_testArray = NULL;
void *g_tdest = NULL;

Array *getCmdArray(char *spc)
{
  char *sp1;
  char *sp2;
  char *spend;
  char *spx;
  int q;
  int len;
  int done = 0;
  Array *cA = new Array(16,4, "Cmds");
  sp1 = spc;
  sp2 = spc;
  spend = spc + strlen(spc);
  while (!done) {
    q = 0;
    sp1 = sp2;
    while (*sp1 && (*sp1 == ' ') && (sp1 < spend)) sp1++;
    if(*sp1 == '"') {q = 1 ; sp1++;}
    
    sp2 = sp1;
    if (q) {
      while ((sp2[0] != 0) && (*sp2 != '"')&& (sp2 < spend)) sp2++;
    } else {
      while ((sp2[0] != 0) && (*sp2 != ' ')&& (sp2 < spend)) sp2++;
    }
    
    len = sp2 - sp1;
    //cout << " Len " << len << endl;
    if(len > 1) {
      spx = (char *)malloc(len);
      int i = 0;
      while (i < len) {
	spx[i] = *sp1++;
	i++;
      }
      spx[i] = 0;
      cout << " spx [" << spx << "]" << endl;

      cA->addItem(spx);
      sp2++;	
    } else {
      done = 1;
    }
    if (sp2 >= spend) done = 1;
  }
  {
    spx = NULL;
    int idx = 0;
    while(cA->findNext((void **)&spx, idx, 0)) {
      cout << " cmds [" << idx-1<<"] spx ["<<spx<<"]" << endl;
    }
  }
  return cA;
}

int delCmdArray(Array *cA)
{
  char *spx = NULL;
  int idx = 0;
  while(cA->findNext((void **)&spx, idx, 0)) {
    free (spx);
    spx =  NULL;
  }
  delete cA;
  return 0;
}

char *findKeyItem(Array *cA, char *key)
{
  char *spx = NULL;
  int idx = 0;
  while(cA->findNext((void **)&spx, idx, 0)) {
    if (strcmp(spx,key) == 0) {
      return (char *)cA->getId(idx);
    }
  }
  return NULL;
}


Test::Test(string tkey, string tdesc, int (*taction) (void * tdest, Array *tcmd, void *data), void* tdata, void *itdest) {
    key = tkey;
    desc = tdesc;
    action = taction;
    data = tdata;
    tdest = itdest;
    if(!g_testArray) {
      g_testArray = new Array(16,16,"Tests");
      addTest("help", "Show tests", showTests, NULL, tdest);
      addTest("prompt", "Run prompt", promptTests, NULL, tdest);
      addTest("read", "Read file ", readFile, NULL, tdest);
    }
    g_testArray->addItem(this);

};

int runTest(void *tdest, char *cmd)
{

  int rc = 0;
  stringstream sout;
  SockThread *st = (SockThread *)tdest;
  if((cmd) && (strlen(cmd) > 1)) {
    Array *cA  = getCmdArray(cmd);
    char *key = (char *)cA->getId(0);
    if (key) {
      int idx = 0;
      Test *test =  NULL;
      while( g_testArray->findNext((void **)&test, idx , 0)) {
	if (test->match(key)) {
	  rc =  test->action(tdest, cA, test->data);
	  break;
	} 
      }
      delCmdArray(cA);
      if(idx == 0) {
	sout << " Test [" << key << "] not found" << endl;
	st->send (sout);
      }
    }
    
  }
  return rc;
}

int showTests(void *tdest, Array *cmd, void *data)
{
  SockThread *st = (SockThread *)tdest;
  stringstream sout;
  int idx=0;
  Test *test =  NULL;
  
  while( g_testArray->findNext((void **)&test, idx , 0)) {
    sout << " Test [" << test->key << "] desc ["<< test->desc<<" ]" << endl;
  }
  st->send (sout);
  return 0;
}


int promptTests(void *tdest, Array *cA, void *data)
{

  int done = 0;
  string request;
  stringstream sout;

  SockThread *st = (SockThread *)tdest;
  sout << " Entering prompt mode done or exit to quit " << endl;
  st->send (sout);
  
  while (! done) {
    sout << " Enter a test (help OK)" << endl;
    st->send (sout);

    getline (cin ,request);
    sout << " you entered  ["<< request <<"]" << endl;
    st->send (sout);
    if (request == "done") done = 1;
    else if (request == "exit") done = 1;
    else if (request == "quit") done = 1;
    else {
      runTest(tdest, (char *)request.c_str());
    }
  }
  return 0;
}

int readFile(void *tdest, Array *cA, void *data)
{

  char *n;
  char *sp;

  char fdata[1024];
  int rc = 0;
  ifstream in;
  n = (char *)cA->getId(1);
  in.open(n);
  while (in.getline(fdata, 1024)) {
    sp = fdata;

    cout << " we read ["<<fdata<<"]" <<endl;
    
    while (*sp && *sp == ' ')sp++;
    cout << " we processed  ["<<sp<<"]" <<endl;
    runTest(tdest, sp);
  }
  in.close();
  return rc;
}

int addTest(string tkey, string tdesc, int (*taction) (void *tdest, Array *tcmd, void *data), void* tdata, void *tdest)
{
  if (tdest == NULL) {
    SockThread *st = new SockThread(1, NULL);
      g_tdest = (void *) st;
      tdest =  g_tdest;
  }  
  Test *t = new Test(tkey, tdesc, taction, tdata, tdest);
  if(0) cout << " Created test " << t->key << endl;
  return 0;
}

int delTests(void)
{
  int idx = 0;
  Test *test =  NULL;
  if(g_testArray) {
    while( g_testArray->findNext((void **)&test, idx , 0)) {
      delete test;
      test=NULL;
    } 
    delete g_testArray;
    g_testArray = NULL;
  }
  return 0;
}
	    

int initTests(void)
{
  if (g_tdest == NULL) {
    SockThread *st = new SockThread(1, NULL);
    g_tdest = (void *)st;
  }
  return 0;
}
