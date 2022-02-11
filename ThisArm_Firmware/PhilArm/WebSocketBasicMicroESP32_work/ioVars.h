#ifndef _IOVARS_H
#define _IOVARS_H
struct ioVar {

  int idx;
  int pin;
  int itype;
  String iname;
  int val;
  int lval;
  int valdiff;
  double totVal;
  unsigned int count;
  int initAdc(String iname, int itype, int ipin);

  int getDiff() { return valdiff;};
  int getVal() { return val;};
  int inputAdc();

};
#define AN_IN 0

int ioInit(void);
int ioAdc(int idx);
int setupioVar (String name, int itype, int pin);
int getAdc(int idx);
int getAdcDiff(int idx);
int readAdc(void);

#endif
