/**
 * file sock.h
 * Super Sock Class.
 *
 * g++ -o sock -D_MAIN sock.cpp -lpthread -lrt
 */
#ifndef ___SSOCK_H
#define ___SSOCK_H

#include <malloc.h>
#include <pthread.h>
#include <syslog.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "array.h"

 
using namespace std;


struct sock_command;
typedef struct sock_command sock_command_t;
struct SockThread;
typedef struct SockThread SockThread_t; 

#define DEF_MAX_ID 65535
#define SOCK_SERVER 1
#define SOCK_CLIENT 2


struct SockCommand {
  SockCommand(string ckey, string chelp, int (*caction)(struct SockThread* st, char *msg, int len, void *data), void *data);

  ~SockCommand(){};
  string key;
  string help;
  void * data;
  int (*action)(struct SockThread* st, char *msg, int len, void *data);
};

  
struct Sock {
  // default
  Sock(){sockfd=-1;port=0;};

  Sock(string xaddr, int xport);
  
  Sock(int xport);
  
  ~Sock() {
    //if(addr) free(addr);
    if(sockfd > 0) close(sockfd);
  };

  int saccept(void);
  
public:
  int runCmd(SockThread *st, char *msg, int len);
  int runParser(SockThread *st, char *msg, int len);
  
  int addCmd( string key, string help, int (*action)(struct SockThread* st, char *msg, int len, void *data), void *data);
  int addParser( string key, string help, int (*action)(struct SockThread* st, char *msg, int len, void *data), void *data);
  int runAccept(Array *srvArray);

  sockaddr_in serAdd;
  sockaddr_in cliAdd;
  int type;
  int port;
  int cmd;  // set to run a command
  int parser; // set to run a parser
  string addr;
  int sockfd;
  Array *stArray;
  Array *cmdArray;
  Array *parseArray;
  int skdone;
  pthread_t st;
  // struct addrinfo ainfo;
  //struct addrinfo *ares; = NULL;
  //struct addrinfo *asave; = NULL;
};


void *sockThread(void *data);

struct SockThread {
  SockThread(Sock *s, int fd, void *data) {
    sk = s;
    sfd = fd;
    sdata = data;
    buf_len = sizeof(buf);
    msgnum = 0;
    pthread_create(&pt, NULL, sockThread, (void *) this);
  };
  SockThread(int fd, void *data) {
    sk = NULL;
    sdata = data;
    buf_max = sizeof(buf);
    buf_len = sizeof(buf);
    outbuf_max = sizeof(buf);
    outbuf_len = 0;
    msgnum = 0;
    sfd = fd;
    //pthread_create(&pt, NULL, sockThread, (void *) this);
  };
  ~SockThread(){};
  char *getOutbuf (int &len) {
    len = outbuf_max-outbuf_len;
    return &outbuf[outbuf_len];
  };

  int send(stringstream &ss) {
    string sout = ss.str();
    int rc =  write (sfd, sout.c_str(), sout.size());
    ss.str(string());
    return rc;
  }
  void setOutbuf(int &len) {
    if (len > 0)
      outbuf_len += len;
    else
      outbuf_len = 0;
    
  };
  
  int sendOutbuf(void) {
    int rc=-1;
    if (sk == NULL) {
      rc = write(sfd, outbuf, outbuf_len);
      if (rc == outbuf_len) {
	outbuf_len = 0;
      }
    }
    return rc;
  }
  int msgnum;
  char buf[1024];
  int buf_len;
  int buf_max;
  char outbuf[1024];
  int outbuf_len;
  int outbuf_max;
  pthread_t pt;
  void *sdata;
  int sfd;
  Sock *sk;


};



struct sock_command
{
  char *key;
  char *help;
  int (*action)(SockThread_t *sthread, char *msg, int len, void *data);
  //  int (*old_action)(netc_thread_t *net_th, char *msg);
  int port;
  int active;
  void *data;

  //struct list_head clist;

  sock_command_t *next;

};


int cmd_help(struct SockThread*st, char *msg, int len, void *data);
int cmd_quit(struct SockThread*st, char *msg, int len, void *data);
int cmd_socks(struct SockThread*st, char *msg, int len, void *data);

#endif
