

#include "sockMenu.h"

#define MAX_SRV_CLIENTS 8
#define MAX_COMMANDS 32
#define MAX_SCMDS 16

struct wConn wCs[MAX_SRV_CLIENTS];
struct wCmd wCmds[MAX_COMMANDS];
struct savedCmd sCmds[MAX_SCMDS];


int g_numsCmds = -1;
int g_numCmds = -1;
float g_mtot = 0.0;
float g_mcnt = 0.0;
unsigned int g_mmax = 0;
unsigned int g_lmicros=0;

void checkMicros()
{
  unsigned int lmicros  = micros();
  if((g_lmicros > 0) && (lmicros >g_lmicros))  {
    unsigned int lint  = lmicros - g_lmicros;
    g_mtot += ((float)lint/1000000.0);
    g_mcnt += 1;
    if (lint > g_mmax) {
      g_mmax = lint;
    }
  }
  g_lmicros = lmicros;

}

void writeString(String sData, struct wConn &wc) { 
  for (int i = 0; i < sData.length(); i++) {
    wc.sCs.write(sData[i]);  
  }

}

void helpWcmd(struct wConn &wc, void *data) {
   char buf[128];
   snprintf(buf,  sizeof(buf)," avg_loop: (%2.3f)mS max (%2.3f) mS\n"
               , (g_mtot *1000.0) /g_mcnt
               , (float)g_mmax/ 1000.0
   );  
   g_mmax = 0;
   writeString(buf, wc);
   
  for (int i = 0; i < g_numCmds; i++) {
    writeString(wCmds[i].key, wc);
    //writeString(", wc);
    wc.sCs.write(" -->");
    writeString(wCmds[i].desc, wc);
    wc.sCs.write("\n");
  }
}
void quitWcmd(struct wConn &wc, void *data) {
    wc.sCs.write("Goodbye");
}

void testWcmd(struct wConn &wc, void *data) {
    wc.sCs.write("Testing");
}

void showSched(int i, struct savedCmd &sc ,struct wConn &wc, void *data)
{
    String msg = "Sched: id: " + String(i,DEC)  + " interval: " +String(sc.rtimer,DEC)+" en:" + String(sc.enabled, DEC);
    String msg2 = " count: " + String(sc.count, DEC) + " Command: "+ (char *) sc.cmd;
    
    wc.sCs.write(msg.c_str());
    wc.sCs.write(msg2.c_str());
    wc.sCs.write("\n");

}

void showScheds(struct wConn &wc, void *data)
{
  wc.sCs.write("Sched Commands:\n");
 
  for(int i = 0; i <g_numsCmds; i++) {
    showSched(i, sCmds[i], wc, data); 
  }
}

int runSchedWcs(struct wConn *wC, uint8_t *sbuf, int len)
{
  char *key = splitWords(*wC, sbuf, len);
  //wC.sCs.write(wC.sbuf, wC.len);
  //showWords(wC);
  int rc = runCmd(key, *wC);
  if (rc == 0) 
    wC->sCs.write("\nCmd Not found\n");
  return rc;
}

void runSched(int idx, int now, void *data)
{
  savedCmd *sc = &sCmds[idx];
  if ( now < (sc->stime+sc->rtimer) ) return;
  sc->stime = now;
  sc->count++;
  runSchedWcs(sc->wc, sc->cmd, sc->len);
}

void runScheds( void *data)
{
  //wc.sCs.write("Sched Commands:\n");
 int now = millis();
 
  for(int i = 0; i <g_numsCmds; i++) {
    runSched(i, now, data); 
  }
}


void schedCmd(struct wConn &wc, void *data) {
    if(wc.nwords < 3) {
      showScheds(wc, data);
    }
    else {
      if(g_numsCmds == -1) g_numsCmds = 0;
      struct savedCmd &sc = sCmds[g_numsCmds];
      sc.rtimer = atoi((char *)&wc.swords[1]);
      sc.wc = &wc;
      sc.len =  strlen((char *)&wc.swords[2])+1;
      sc.enabled = 1;
      memcpy(sc.cmd,(char *)&wc.swords[2], sc.len);
      
      sc.stime = millis();
      sc.count = 0;
      showSched(g_numsCmds, sc, wc, data);
      if(g_numsCmds < MAX_SCMDS) g_numsCmds++;     

    }
}

void addWcmd(String key, String desc, void (*fcn)(struct wConn &wc, void *data), void *data)
{
  if(g_numCmds == -1) {
    g_numCmds = 0;
    addWcmd("h", "Help stuff",helpWcmd, NULL);
    addWcmd("q", "Quit socket", quitWcmd, NULL);
    addWcmd("sched", "sched cmd interval \"cmd\"", schedCmd, NULL);
    
  }
  struct wCmd *wCmd = &wCmds[g_numCmds];
  wCmd->key = key;
  wCmd->desc = desc;
  wCmd->data = data;
  wCmd->fcn = fcn;
  if (g_numCmds < MAX_COMMANDS) g_numCmds++;
  
}

int runCmd(char *key, struct wConn &wc)
{
  for (int i = 0; i < g_numCmds; i++) {
    struct wCmd *wCmd = &wCmds[i];
 
    if (strcmp(key, wCmd->key.c_str()) == 0 ) {
      wCmd->fcn(wc, wCmd->data);
      return 1; 
    }
  }
  return 0;
}
//void schedCmd(struct wConn &wc, void *data) {

void setupWserv(WiFiServer &ws) {
  ws.begin();
  ws.setNoDelay(true);
  addWcmd("t", "Test stuff", testWcmd, NULL);
//  addWcmd("mv", "Main Volts", motorVolt, NULL);
}

void acceptWserv(WiFiServer &ws) {
   uint8_t i;
   if (ws.hasClient()){
      //Serial.println("Got connection");

     for(i = 0; i < MAX_SRV_CLIENTS; i++){
        //find free/disconnected spot
        if (!wCs[i].sCs || !wCs[i].sCs.connected()){
          if(wCs[i].sCs) wCs[i].sCs.stop();
          wCs[i].sCs = ws.available();
          break;
        }
      }
      if (i >= MAX_SRV_CLIENTS) {
        //no free/disconnected spot so reject
        ws.available().stop();
        //Serial.println("REFUSED CONNECTION");
      }
   }
}

char *splitWords(struct wConn &wc, uint8_t*sbuf,int len) {
  int ix = 0;
  int ixp = 0;
  char p;
  char ep=0;
  const char *esp = "\"'";
  
  for (int i = 0; i< len; i++) {
    p = sbuf[i];
    if(strchr(esp,p)) {
      if(ep == 0) {
        ep = p; 
        continue  ;
      } 
      else {
        if (ep == p) {
          wc.swords[ix][ixp] = 0;  // terminate
          //ixp = 0;
          wc.nwords = ix;
          if(ixp > 0)wc.nwords++;
          ep = 0;
          ixp = 0;
          if(ix < 7) ix++;
          continue;
        }
      }
    }
    if(ep != 0) {
      if (p != 0) {
    
        wc.swords[ix][ixp] = p;
        if (ixp < 31) ixp++;
        wc.slen[ix] = ixp;
        continue;
      }
      else {
        wc.swords[ix][ixp] = 0;  // terminate
        //ixp = 0;
        wc.nwords = ix;
        if(ixp > 0)wc.nwords++;
        ep = 0;
        continue;
        
      }
    }
    
    if (p == ' ') {
      if (ixp == 0) continue; // skip leading spaces
      wc.swords[ix][ixp] = 0;  // terminate
      if (ix < 7) ix++;
      ixp = 0;
    }
    else if (p == '\r') {
      continue;
    }
    else if (p == '\n') {
      //if (ixp == 0) continue; // skip leading spaces
      wc.swords[ix][ixp] = 0;  // terminate
      //ixp = 0;
      wc.nwords = ix;
      if(ixp > 0)wc.nwords++;

      return (char *)wc.swords[0];

    }

    else if(p != ' ') {
      wc.swords[ix][ixp] = p;
      if (ixp < 31) ixp++;
      wc.slen[ix] = ixp;
    }
    else {
      continue;
    }
  }
  wc.swords[ix][ixp] = 0;  // terminate
  wc.nwords = ix;
  if(ixp > 0)wc.nwords++;
  return (char *)wc.swords[0];
}

void showWords(struct wConn &wc) {
  for (int i = 0; i < wc.nwords; i++) {
    wc.sCs.write(':');
    wc.sCs.write(wc.swords[i], wc.slen[i]);
    wc.sCs.write(":\n");
  }
}

int runWcs(struct wConn &wC)
{
  char *key = splitWords(wC, wC.sbuf, wC.len);
  wC.sCs.write(wC.sbuf, wC.len);
  showWords(wC);
  int rc = runCmd(key, wC);
  if (rc == 0) 
    wC.sCs.write("\nCmd Not found\n");
  return rc;
}

void runWs(int i) {
  if (wCs[i].sCs && wCs[i].sCs.connected()){
     if(wCs[i].sCs.available()){
        //get data from the telnet client and push it to the UART
        wCs[i].sCs.write("HI ");
        //wCs[i].sCs.write(wCs[i].sCs.remoteIP());
        wCs[i].sCs.write(">");

        wCs[i].len = wCs[i].sCs.available();
        if (wCs[i].len > 0) {
          //uint8_t sbuf[len];
          wCs[i].sCs.readBytes(wCs[i].sbuf, wCs[i].len);
          runWcs(wCs[i]);
          //char *key = splitWords(wCs[i]);
          //wCs[i].sCs.write(wCs[i].sbuf, wCs[i].len);
          //showWords(wCs[i]);
          //int rc = runCmd(key, wCs[i]);
          //wCs[i].sCs.write(" key:");
          //wCs[i].sCs.write(key);
          //wCs[i].sCs.write(" rc:");
          //if (rc == 0) 
            //wCs[i].sCs.write("\nCmd Not found\n");
          //else 
          //  wCs[i].sCs.write("Cmd found");
          //wCs[i].sCs.write(": \n");
          //sCs[i].write(sCs[i].read());
              
        }
     }
  }
}

//check clients for data
void checkWserv(WiFiServer &ws) {
   int i;
   for(i = 0; i < MAX_SRV_CLIENTS; i++){
     runWs(i);
   }
}

void handleWserv(WiFiServer &ws)
{
  checkMicros();
  acceptWserv(ws);
  checkWserv(ws);
  runScheds(NULL);
}
