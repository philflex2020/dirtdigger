#ifndef __WEB_SOCK_H
#define __WEB_SOCK_H

#include <ArduinoJson.h>
#include <WebSocketsServer.h>

extern String wsdata; 

void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length);
void webSockSetup();

int webSockSend(char *sp);
void webSockLoop();
#endif
