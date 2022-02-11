#ifndef __SERVER_H_
#define __SERVER_H_

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <errno.h>
#include <error.h>
#include <semaphore.h>

using namespace std;


#define ISspace(x) isspace((int)(x))
#define SERVER_STRING "Server: pswhttpd/0.1.0\r\n"


//void     (*sa_sigaction)(int, siginfo_t *, void *);
//void     (*sa_sigaction)(int, siginfo_t *, void *);
//void (*runFoo)(int client, const char *file, int cgi)

class Server {
 public:
  
  Server(int p);
  ~Server();
  int acceptSock(void);
  int getPort(void){ return port;};
  int getSock(void){ return sock;};
  int setClient(int cli){ cli_sock = cli;};
  int getClient(void){ return cli_sock;};
  int start(void);
  int stop(void);

  int port;

  int sock;
  pthread_t stthread;
  sem_t cli_sem;
  int cli_sock;
 private:
  

  
};


void *accept_request(void *);
void bad_request(int);
void cat(int, FILE *);
void cannot_execute(int);
void error_die(const char *);
void execute_cgi(int, const char *, const char *, const char *);
void headers(int, const char *);
void not_found(int, const char *);
void serve_file(int, const char *);
void serve_motor(int client, const char * file, int cli);
void serve_ajax(int client, const char * file, int cli);
int startup(u_short);
void unimplemented(int, const char *);
int getLine(int sock, char *buf, int size);

#endif
