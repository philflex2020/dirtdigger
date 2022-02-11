#include "udpbcast.h"

int bcastMillis = 0;
int bcastDel = 3000;
extern int wifiServ;
extern char *project;
char blocalIP[32];



WiFiUDP Udp;
unsigned int localUdpPort = 4211;  // local port to listen on
//char incomingPacket[255];  // buffer for incoming packets
IPAddress IP_Bcast(192, 168, 1, 255);
int port_Bcast = 4511;
int bcnt = 21;

void udpbcastIP(char *ip)
{
  snprintf(blocalIP, 32, "%s", ip);
//  snprintf(blocalIP, 32, "%s", "no Idea");

}
void handleBcast()
{
  int nowM = millis();
  if (bcastMillis == 0) {
    bcastMillis = nowM;
    return;
  }

  if ((nowM - bcastMillis)  > bcastDel) {
    char buf_Bcast[128];
    snprintf(buf_Bcast, 128, "{\"proj\":\"%s\",\"id\":%d,\"ip\":\"%s\",\"port\":%d}\n", project, bcnt++, blocalIP, wifiServ);
    bcastMillis = nowM;
    Udp.beginPacket(IP_Bcast, port_Bcast); // subnet Broadcast IP and port
    Udp.write((const uint8_t*)buf_Bcast, strlen(buf_Bcast));
    Udp.endPacket();


  }
}
 
