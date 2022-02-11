// test helper functions
//
// type > 16 is a data block ( min size 16)
// int,double or block
// strings are in blocks
// have a list and data groups


// as soon as a data item is declared we add access functions to the main socket
//
// actions before read and after set
// groups have actions as well.
#ifndef __TEST_H
#define __TEST_H


#include <iostream>
#include <string.h>
#include <string>
#include <sstream>

#include "array.h"


using namespace std;

int showTests(void *tdest, Array *cmd, void *data);

int promptTests(void *tdest, Array *cmd, void *data);

char *findKeyItem(Array *cA, char *key);
int delCmdArray(Array *cA);

extern Array *g_testArray;


struct Test {

  Test(string tkey, string tdesc, int (*taction) (void *tdest, Array *tcmd, void *data), void*tdata, void *tdest);

  ~Test(){
    if(g_testArray)g_testArray->_delItem(this);
  };

public:

  int match(char *tkey) {
    if (strcmp(tkey, key.c_str()) == 0 ) return 1;
    return 0;
  };

  string key;
  string desc;
  void *data;
  void *tdest;
  int (*action) (void *tdest, Array *cmd, void *data);

};



int runTest(void *tdest, char *cmd);

int addTest(string tkey, string tdesc, int (*taction) (void *tdest, Array*tcmd, void *data), void*tdata, void *tdest);

int initTests(void);
int delTests(void);

#define showTest \
  do { \
    cout << endl << " ******* Running test <"	\
	 << __FUNCTION__\
	 << ">  ***********"\
	 << endl;\
} while (0);
#define endTest \
  do { \
    cout << endl << " ******* Endog test <"	\
	 << __FUNCTION__\
	 << ">  ***********"\
	 << endl;\
} while (0);

#endif
