// variables
// type > 16 is a data block ( min size 16)
// int,double or block
// strings are in blocks
// have a list and data groups


// as soon as a data item is declared we add access functions to the main socket
//
// actions before read and after set
// groups have actions as well.

#ifndef __VARS_H
#define __VARS_H

#include <string>
#include "array.h"

using namespace std;

struct VarItem;

typedef int (varSetup_t)(VarItem *vi);

struct Var {

  Var();
  ~Var();
  Var(char *vname, char *vdesc, char *vtype);

public:
  int id;
  int key;
  int vartype;
  string name;
  string desc;
  string vtype;
  string skey;
  string sonset;
  string sonclear;
  string sonchange;
  string srun;
  int run;
  void *th;
  
private:
  int pin;
  int valint;
  double valdouble;
  void *vardata;
  Array *varGroupArray;
};

struct VarGroup {
  VarGroup(){};
  ~VarGroup(){};
public:
  int group_id;
  string name;
  string desc;
  Array *varGroupArray;
};

struct VarType {
  VarType (string iname, string idesc,  int (*ifcn) ( VarItem * vi), int ivcode) {
    vname = iname;
    vdesc = idesc;
    //vtype = ivtype;
    fcn  = ifcn;
    icode = ivcode;
    cmdArray =  NULL;
  };
  VarType(){};

  int setup(VarItem *v) {
    //typedef int (varSetup_t)(VarItem *vi);
    int rc = fcn(v);
    return rc;
  };

public:
  string vname;
  string vdesc;
  string vtype;
  int icode;
  int (*fcn)(VarItem *v);
  Array *cmdArray;

};

struct VarTypeCmd {

  VarTypeCmd (string ikey, string idesc, int (*ifcn) ( void *tdest, char *msg, VarItem *vi), VarType *ivt, int ivcode) {
    vkey = ikey;
    vdesc = idesc;
    vt = ivt;
    fcn  = ifcn;
    icode = ivcode;
    if(vt->cmdArray == NULL) { vt->cmdArray = new Array(16,4,"vTCmds");};
    vt->cmdArray->addId(ivcode, this);
  };
  
  ~VarTypeCmd(){};
  
  //  int setup(VarItem *v) {
    ////typedef int (varSetup_t)(VarItem *vi);
  //fcn(v);
  //};

public:
  string vkey;
  string vdesc;
  VarType *vt;
  int icode;
  int (*fcn)(void *tdest, char* msg, VarItem *v);


};

// this goes in the list of vars
// it also has a list of items to consider
struct VarItem {
  VarItem (string iname, string itype, string idata);

  VarItem();
  void addVarItem(VarItem *vi);
  int sendCmd(void *tdest, Array *cmd);
  int setupCmd(void *tdest, Array *cmd);
  
public:
  string vname;
  string vtype;
  string vdesc;
  string vdata;
  string vkey;

  void *itype;
  Array *viArray;
  VarType *vt;
  void *item;
  int pin;
  
};

VarType *findVarType (string vtype);
VarItem *findVarItem(string iname);


#endif

