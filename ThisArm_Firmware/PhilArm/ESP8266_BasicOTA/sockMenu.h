#ifndef _sockMenu_H
#define _sockMenu_H

#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

struct wConn {
  WiFiClient sCs;
  uint8_t sbuf[256];
  int len;
  uint8_t swords[8][64];
  int slen[8];
  int nwords;
};

struct wCmd {
  String key;
  String desc;
  void * data;
  void (*fcn)(struct wConn & wc, void * data);
};

struct savedCmd {
  savedCmd(){
    nwords = 0;
    count = 0;
    rtimer = 0;
    stime=0;
    cmd[0] = 0;
    wc=NULL;
    };
    ~savedCmd(){};
  uint8_t cmd[256];
  //uint8_t swords[8][64];
  int nwords;
  int stime;
  int rtimer;
  struct wConn *wc;
  int count;
  int len;
  int enabled;
};

// max number of saved commands


void addWcmd(String key, String desc, void (*fcn)(struct wConn & wc, void *data), void *data);
void setupWserv(WiFiServer &ws);
void writeString(String sData, struct wConn &wc);
void acceptWserv(WiFiServer &ws);
void checkWserv(WiFiServer &ws);
void handleWserv(WiFiServer &ws);
char *splitWords(struct wConn &wc, uint8_t*sbuf,int len);
int runCmd(char *key, struct wConn &wc);

#endif
