#ifndef _sockMenu_H
#define _sockMenu_H

#include <WiFiClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

struct wConn {
  WiFiClient sCs;
  uint8_t sbuf[256];
  int len;
  uint8_t swords[8][32];
  int slen[8];
  int nwords;  
};

struct wCmd {
  String key;
  String desc;
  void * data;
  void (*fcn)(struct wConn & wc, void * data);
};

void addWcmd(String key, String desc, void (*fcn)(struct wConn & wc, void *data), void *data);
void setupWserv(WiFiServer &ws);
void writeString(String sData, struct wConn &wc);
void acceptWserv(WiFiServer &ws);
void checkWserv(WiFiServer &ws);
void handleWserv(WiFiServer &ws);

#endif
