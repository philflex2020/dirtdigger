// variables
// change of plan .. moved across to varitems




// type > 16 is a data block ( min size 16)
// int,double or block
// strings are in blocks
// have a list and data groups


// as soon as a data item is declared we add access functions to the main socket
//
// actions before read and after set
// groups have actions as well.
// send command to an object

// invoke the varitem send command



//varitems have varitems etc


#include <iostream>
#include <fstream>
#include <string.h>
#include <string>

#include "array.h"
#include "sock.h"
#include "vars.h"
#include "tests.h"
#include "servo.h"
#include "thread.h"
#include "digital.h"

using namespace std;

int g_varId = 0;
int g_varGroupId = 0;
extern void *g_tdest;

Array *g_varArray = NULL;
Array *g_varIdArray = NULL;
Array *g_varGArray = NULL;
Array *g_vtArray = NULL;

extern Array *g_threadArray;
extern int g_thread_id;


int threadRunVar(Thread *t, ThreadFun *tfun)
{
  VarItem *vi;
  //FunItem *fi;
  //stringstream sout;
  //SockThread *st = (SockThread *)tdest;
  vi = (VarItem *)tfun->action;
  if(!vi) {
    vi = findVarItem(tfun->name);
    if(vi) {
      tfun->action = vi;
      tfun->ftype = vi->vtype;
    }
  }
  if(vi) {
    if(vi->vtype == "Servo") {
      Servo *s = (Servo *)vi->item;
      cout <<"ThreadRunVar almost running servo [" << s->name << "]" << endl;
      
    } else if(vi->vtype == "DigIn") {
      Digital *s = (Digital *)vi->item;
      cout <<"ThreadRunVar almost running DigIn ["
	   << s->name << "]"
	   << " count [" << s->count++ << "]"
	   << " period [" << t->per << "]"
	   << endl;
     
    } else if(vi->vtype == "DigOut") {
      Digital *s = (Digital *)vi->item;
      cout <<"ThreadRunVar almost running DigOut [" << s->name << "]" << endl;
      
    }
  }
  return 0;
  // tfun->data = whatever
}


int DigInShow(void *tdest, char *msg, VarItem *vi)
{
  cout << " func " << __FUNCTION__
       << " running"
       << endl;
  return 0;
}

int DigOutShow(void *tdest, char *msg, VarItem *vi)
{
  cout << " func " << __FUNCTION__
       << " running"
       << endl;
  return 0;
}

int DigInVar(VarItem *vi)
{
  cout << " running " << __FUNCTION__ << endl;
  cout << " vi->name " << vi->vname<< endl;
  Digital *d = new Digital(vi->pin, 1);
  d->name = vi->vname;
  vi->item = d;
  return 0;

}

int DigOutVar(VarItem *vi)
{
  cout << " running " << __FUNCTION__ << endl;
  cout << " vi->name " << vi->vname<< endl;
  Digital *d = new Digital(vi->pin, 0);
  d->name = vi->vname;
  vi->item = d;
  return 0;

}

int ServoVar(VarItem *vi)
{
  cout << " running " << __FUNCTION__ << endl;
  cout << " vi->name " << vi->vname<< endl;
  Servo *sv = new Servo (vi->pin);
  sv->name = vi->vname;
  vi->item = sv;
  //  vi->vtype = SERVO_TYPE;

  return 0;

}

int NullVar(VarItem *vi)
{
  cout << " running " << __FUNCTION__ << endl;
  cout << " vi->name " << vi->vname<< endl;
  return 0;

}

int ThreadVar(VarItem *vi)
{
  cout << " running " << __FUNCTION__ << endl;
  cout << " vi->name " << vi->vname<< endl;
  Thread *tv = new Thread (vi->vname, 0 , (void *)vi, g_tdest);
  //sv->name = vi->vname;
  vi->item = tv;
  return 0;
}

Var::Var(){
  id = g_varId++;
  vardata =  NULL;
  //if(g_varIdArray)g_varIdArray->addId(id, this);
}

Var::~Var(){

  //int idx = 0;
  //struct VarGroup*gvar=NULL;
  //while (varGroupArray->findNext((void **) &gvar, idx, 0)) {
  // gvar->varGroupArray->_delItem(this);
  // gvar =  NULL;
  //}
  //if(vardata) free(vardata);
}

Var::Var(char *vname, char *vdesc, char *ivtype) {
  id = g_varId++;
  name = (string)vname;
  desc = (string)vdesc;
  vtype = ivtype;
  valint = 0;
  valdouble = 0.0;
  //if(vtype > 4) 
  // valdata = malloc(vtype);
  //else
  vardata = NULL;
   //if(g_varIdArray)g_varIdArray->addId(id, this);
  
  
}

VarItem::VarItem (string iname, string itype, string idata) {
  vname = iname;
  vtype =itype;
  vdata= idata;
  if(g_varArray == NULL )
      g_varArray = new Array(16,16,"VarItems");
  viArray = new Array(16,4, "VItems");
};

VarItem::VarItem(){
  if(g_varArray)g_varArray->_delItem((void *)this);
  //if(g_varIdArray)g_varIdArray->delId(id,this);
}


int VarItem::sendCmd(void *tdest, Array *cA) {

  stringstream sout;
  SockThread *st = (SockThread *)tdest;

  if (vtype == "Servo") {
    sout << " We got a Servo Cmd " <<  endl;
    Servo *s = (Servo *)item;
    sout << " We got a Servo id  [" << s->id << "]" <<  endl;
    int rc = sendServoCmd(tdest, s, cA);
    return rc;
  }
  else if (vtype == "Thread") {
    sout << " We got a Thread Cmd " <<  endl;
    Thread *th = (Thread *)item;
    sout << " We got a Thread id  [" << th->id << "]" <<  endl;
    int rc = sendThreadCmd(tdest, th, cA);
    return rc;
  }
  else if (vtype == "DigIn") {
    sout << " We got a DigIn Cmd " <<  endl;
    Digital *d = (Digital *)item;
    sout << " We got a DigIn id  [" << d->id << "]" <<  endl;
    int rc = sendDigitalCmd(tdest, d, cA);
    return rc;
  }
  else if (vtype == "DigOut") {
    sout << " We got a DigOut Cmd " <<  endl;
    Digital *d = (Digital *)item;
    sout << " We got a DigOut id  [" << d->id << "]" <<  endl;
    int rc = sendDigitalCmd(tdest, d, cA);
    return rc;
    
  } else {
    sout << " UnManaged Type [" <<vtype<<"] requested" << endl;
  }    
  st->send(sout);
  return 0;
}

int VarItem::setupCmd(void *tdest, Array *cA) {

  stringstream sout;
  SockThread *st = (SockThread *)tdest;

  if (vtype == "Servo") {
    sout << " We got a Servo Cmd " <<  endl;
    Servo *s = (Servo *)item;
    sout << " We got a Servo id  [" << s->id << "]" <<  endl;
    int rc = setupServoCmd(tdest, s, cA);
    return rc;
  }
  else if (vtype == "Thread") {
    sout << " We got a Thread Cmd " <<  endl;
    Thread *th = (Thread *)item;
    sout << " We got a Thread id  [" << th->id << "]" <<  endl;
    int rc = setupThreadCmd(tdest, th, cA);
    return rc;
  }
  else if (vtype == "DigIn") {
    sout << " We got a DigIn Cmd " <<  endl;
    Digital *d = (Digital *)item;
    sout << " We got a DigIn id  [" << d->id << "]" <<  endl;
    int rc = setupDigitalCmd(tdest, d, cA);
    return rc;
  }
  else if (vtype == "DigOut") {
    sout << " We got a DigOut Cmd " <<  endl;
    Digital *d = (Digital *)item;
    sout << " We got a DigOut id  [" << d->id << "]" <<  endl;
    int rc = setupDigitalCmd(tdest, d, cA);
    return rc;
    
  } else {
    sout << " UnManaged Type [" <<vtype<<"] requested" << endl;
  }    
  st->send(sout);
  return 0;
}



int addType(string vtype, string desc, int(*fcn)(VarItem *v) , int icode)
{
  VarType *vt = new VarType(vtype, desc, fcn, icode);
  if(g_vtArray == NULL )
    g_vtArray = new Array(16,16,"Vartypes");
  g_vtArray->addId(icode, vt);
  return 0;
}

// these are commands related to the typwe of var
int addTypeCmd(string vtype, string key, string idesc, int(*fcn)( void *tdest, char *msg,VarItem *v) , int icode)
{
  //VarType *vt = findVarType(vtype);
  //VarTypeCmd *vtc = new VarTypeCmd(key, idesc, fcn, vt, icode);
  
  //vt->addTypeCmd(vtc);
  
  return 0;
}

int initVars(void)
{
  addType("DigIn",     "Digital Input",   DigInVar, 1);
  //addTypeCmd("Digin",     "show",   "Show Item", DigInShow, 11);
  addType("DigOut",    "Digital Output",  DigOutVar, 2);
  //addTypeCmd("Digout",     "show",   "Show Item", DigOutShow, 21);
  addType("Analogin",  "Analog Input",    NULL, 3);
  addType("Analogout", "Analog Output",   NULL, 4);
  addType("Servo",     "Servo Output",    ServoVar, 5);
  addType("PWD",       "PWM Output",      NULL, 6);
  addType("Group",     "Group Of Things", NULL, 10);
  addType("List",      "List Of Things",  NULL, 20);
  addType("Func",      "Function",        NullVar, 30);
  addType("Thread",    "Thread",          ThreadVar, 40);
  return 0;
}


VarItem *findVarItem(string iname)
{
  VarItem *vi = NULL;
  int idx = 0;
  while(g_varArray->findNext((void **)&vi, idx, 0)) {
    if(vi->vname == iname) {
      return vi;
    }
  }
  return NULL;

}
  
void VarItem::addVarItem(VarItem *vi)
{
  viArray->addItem(vi);  
}

#ifdef _MAIN


int foutVars(void *tdest, Array *cA, void *data)
{
  stringstream sout;
  //SockThread *st = (SockThread *)tdest;

  int rc=0;


  ofstream out;
  char *n = (char *)cA->getId(1);
  out.open(n);
  int idx=0;
  VarItem *var =  NULL;
  while( g_varArray->findNext((void **)&var, idx , 0)) {
    out << "addVar " << var->vname << " "
	<< var->vtype<<" " 
	<< var->vdata<<" " 
	<< endl;
    int iidx=0;
    VarItem *ivar =  NULL;

    while (var->viArray->findNext((void **) &ivar, iidx, 0)){
    out << "   addVarItem "
	<< var->vname << " "
	<< ivar->vname << " "
	<< ivar->vtype<<" " 
	<< ivar->vdata<<" " 
	<< endl;
    }
  }
  out.close();
  return rc;
}
#if 0
int finVars(void *tdest, char *cmd, void *data)
{
  char c[128];
  char n[128];
  char d[128];
  char t[128];
  char fdata[1024];
  int rc;
  VarItem *var =  NULL;

  rc = sscanf(cmd,"%127s %127s %127s %127s"
	       ,c,n,d,t);
  ifstream in;

  in.open(n);
  while (in.getline(fdata, 1024)) {
    runTest(tdest, fdata);

  }
  in.close();
}
#endif


int showVars(void *tdest, Array *cmd, void *data)
{
  stringstream sout;
  SockThread *st = (SockThread *)tdest;

  int idx=0;
  VarItem *var =  NULL;

  while( g_varArray->findNext((void **)&var, idx , 0)) {
    sout << " VarItem name [" << var->vname
	 << "] type ["<< var->vtype
	 << "] desc ["<< var->vdata
	 << "]" ;
	 
    if(var->vt) {
      sout << " Found type " ;
    } else {
       sout << " NO Type found " ;
    }
    sout << endl;
    st->send(sout);
    int idxI=0;
    VarItem *varI =  NULL;
    while(var->viArray->findNext((void **)&varI, idxI , 0)) {
      sout << "      VarItem name [" << var->vname
	   << "] name [" << varI->vname
	   << "] type ["<< varI->vtype
	   << "] desc ["<< varI->vdata<<"]"
	   << endl;
      st->send(sout);
    }
      
  }
  sout<< "end of vars" << endl;
    
  st->send(sout);
  return 0;
}

int addVar(void *tdest, Array *cA, void *data)
{
  stringstream sout;
  SockThread *st = (SockThread *)tdest;

  //int idx=0;
  VarItem *var =  NULL;
  //char *c=(char  *)cA->getId(0);
  char *name=(char  *)cA->getId(1);
  char *vtype=(char  *)cA->getId(2);
  char *desc=(char  *)cA->getId(3);
  char *key = findKeyItem(cA, (char *)"key");
  int rc = cA->size();
  if (rc > 3) {
    var = new VarItem((char *)name ,(char *)vtype ,(char *)desc);
    if(key)var->vkey = string(key);
    if(g_varArray)g_varArray->addItem(var);
    var->vt = findVarType(vtype);
    if(var->vt) {
      var->vt->setup(var);
    }
  } else {
    sout << " addVar [name] [desc] type "<<rc<<") "<< endl;
    st->send(sout);
  }
  return 0;
}

VarType *findVarType(string vtype)
{
  VarType *vt = NULL;
  int idx = 0;
  cout << " Looking for vartype ["<<vtype<<"]"<<endl;
  
  if(g_vtArray) {
    while (g_vtArray->findNext((void **) &vt, idx, 0)) {
      if(vt->vname == (string)vtype) {
	cout << " Found vartype ["<<vtype<<"]"<<endl;

	return vt;
      }
    }
  }
  return NULL; 
}

int addVarI(void *tdest, Array *cA, void *data)
{
  stringstream sout;
  SockThread *st = (SockThread *)tdest;
  //int idx=0;
  VarItem *var =  NULL;
  int rc;
  char *i  = (char *)cA->getId(1);
  char *n  = (char *)cA->getId(2);
  char *t  = (char *)cA->getId(3);
  char *d  = (char *)cA->getId(4);
  // var 
  rc = cA->size();

  if (rc > 4) {
    VarItem *vi =  findVarItem((string)i);
    if(vi) {
      var = new VarItem((char *)n ,(char *)t ,(char *)d);
      vi->addVarItem(var);
    }
  } else {
    sout << " addVar [name] [desc] type "<<rc<<") "<< endl;
    st->send(sout);
  }
  return 0;
}

// send the rest of the command to a named var
int sendVarCmd(void *tdest, Array *cA, void *data)
{
  stringstream sout;
  SockThread *st = (SockThread *)tdest;
  //int idx=0;
  //VarItem *var =  NULL;
  char *sp;
  int rc;
  char *i  = (char *)cA->getId(1);
  char *n  = (char *)cA->getId(2);
  //char *t  = (char *)cA->getId(3);
  //char *d  = (char *)cA->getId(4);

  // var 
  rc = cA->size();

  if (rc > 2) {
    VarItem *vi =  findVarItem((string)i);
    if(vi) {
      sp = n;//strstr(cmd, i) + strlen(i);
      sout << " sendVar name ["<<i<< "] cmd ["<<sp<<"] "<< endl;
      st->send(sout);
      //var = new VarItem((char *)n ,(char *)t ,(char *)d);
      rc = vi->sendCmd(tdest, cA);
    }
  } else {
    sout << " sendVar [name] [desc] rc ("<<rc<<") "<< endl;
    st->send(sout);
  }
  return 0;
}
// send the rest of the command to a named var
int setupVarCmd(void *tdest, Array *cA, void *data)
{
  stringstream sout;
  SockThread *st = (SockThread *)tdest;
  //int idx=0;
  //VarItem *var =  NULL;
  char *sp;
  int rc;
  char *i  = (char *)cA->getId(1);
  char *n  = (char *)cA->getId(2);
  //char *t  = (char *)cA->getId(3);
  //char *d  = (char *)cA->getId(4);

  // var 
  rc = cA->size();

  if (rc > 2) {
    VarItem *vi =  findVarItem((string)i);
    if(vi) {
      sp = n;//strstr(cmd, i) + strlen(i);
      sout << " setupVar name ["<<i<< "] cmd ["<<sp<<"] "<< endl;
      st->send(sout);
      //var = new VarItem((char *)n ,(char *)t ,(char *)d);
      rc = vi->setupCmd(tdest, cA);
    }
  } else {
    sout << " sendVar [name] [desc] rc ("<<rc<<") "<< endl;
    st->send(sout);
  }
  return 0;
}



int varTypes(void *tdest, Array *cmd, void *data)
{
  stringstream sout;
  SockThread *st = (SockThread *)tdest;
  //int t;
  //int rc;
  VarType *vt = NULL;
  int idx = 0;
  if(g_vtArray) {
    while (g_vtArray->findNext((void **)&vt, idx, 0)) {
      sout << " name " << vt->vname
	   << " desc " << vt->vdesc
	   << " code " << vt->icode
	   << endl;
    }
    st->send(sout);
  }
  
  return 0;
}



int main(int argc, char *argv[])
{

  g_varArray=new Array(16,16, "Vars");
  g_varIdArray=new Array(16,16, "Var Ids");
  g_varGArray=new Array(16,16, "Var Groups");
  initTests();
  initVars();
  
  addTest("vars", "Show Vars", showVars, NULL, g_tdest);
  addTest("Var", "Add Var name Type desc key key", addVar, NULL, g_tdest);
  addTest("addVar", "Add Var", addVar, NULL, g_tdest);
  addTest("addVarItem", "Add VarItem", addVarI, NULL, g_tdest);
  addTest("varTypes", "ShowVar Types", varTypes, NULL, g_tdest);
  //addTest("read", "read (name) from file", finVars, NULL, g_tdest);
  addTest("write", "write (name) to file", foutVars, NULL, g_tdest);
  addTest("send", "send to (name) cmd", sendVarCmd, NULL, g_tdest);
  addTest("setup", "setup (name) cmd", setupVarCmd, NULL, g_tdest);
  addTest("threads", "show Threads ", showThreads, NULL, g_tdest);

  if (argc == 1) {
    runTest(g_tdest, (char *) "prompt");
  } else {

    for (int i=1 ; i < argc ; i++ ) {
      runTest(g_tdest, argv[i]);
    }
  }

  // close down
  
  VarItem *var = NULL;
  int idx = 0;
  while(g_varArray->findNext((void **) &var, idx, 0)) {
    delete var;
    var = NULL;
    
  }

  VarGroup *vg = NULL;
  idx = 0;
  while(g_varGArray->findNext((void **) &vg, idx, 0)) {
    delete vg;
    vg = NULL;
    
  }
  
  delete g_varArray; 
  delete g_varIdArray; 
  delete g_varGArray; 
  delTests();
  
  return 0;
}


#endif
