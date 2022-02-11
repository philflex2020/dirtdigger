#include <Arduino.h>
#ifdef ESP32
#include "ioVars.h"

int g_numiovars = 0;
struct ioVar iovs[128];

int ioInit(void)
{
 g_numiovars = 0;
 return 0; 
}

int setupioVar (String iname, int itype, int ipin)
{
  struct ioVar *iv = &iovs[g_numiovars];
  iv->idx = g_numiovars;
  iv->initAdc(iname, itype, ipin);
  //iv->readCnt = 0;
  if(g_numiovars < 128) 
    g_numiovars ++;
  return iv->idx; 
}

int ioVar::initAdc(String iname, int itype, int ipin)
{
  val = 0;
  lval = 0;
  valdiff = 0;
  count = 0;
  iname = iname;
  itype = itype;
  pin = ipin;
  totVal = 0;

}

int ioVar::inputAdc()
{
    val = analogRead(pin);
    totVal += val;
    valdiff = val -lval;
    lval= val;
    count++;
    return val;
}

int ioAdc(int idx)
{
    struct ioVar *iv = &iovs[idx];
    return iv->inputAdc();
}
int getAdc(int idx)
{
    struct ioVar *iv = &iovs[idx];
    return iv->getVal();
}
int getAdcDiff(int idx)
{
    struct ioVar *iv = &iovs[idx];
    return iv->getDiff();
}


unsigned long cMillis = 0;   // NOTE: 1
unsigned long pMillis= 0;   // NOTE: 1
int adcInterval = 100;
//int adcnum = 0;
int readAdc() {
  int rc = 0;
  //cMillis = millis();  
  //if (cMillis - pMillis >= adcInterval) { 
    //rc = 1;
    //pMillis = cMillis;
  for (int i = 0; i < 4 ; i++) {
      ioAdc(i); 
  }
  
  return rc;
}
#endif
