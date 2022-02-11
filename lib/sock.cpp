
// sock.cpp
// basic socket class
//
// g++ -o sock -D_MAIN sock.cpp -lpthread -lrt
// todo
// move work into sock.cpp from sock.h
// add command list
// add connect list
// The way it works is you simply add a command for a socket number 
// addCmd(5555, ,"show ",cmd_socks,NULL);
// addCmd(5556, ,"show ",cmd_socks,NULL);
// moving on to udp and broadcast soocks
// UBTX and UBRX

#include <malloc.h>
#include <iostream>
#include <pthread.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <cstdlib>
#include <unistd.h>

#include "sock.h"
#include "array.h"
#include "tests.h"

extern void * g_tdest;

using namespace std;

int testServer(void *tdest, Array *cmd, void *data);
int testClient(void *tdest, Array *cmd, void *data);

//static int g_local_debug = 0;
Array*g_srvArray= NULL;

SockCommand::SockCommand(string ckey, string chelp, int (*caction)(struct SockThread* st, char *msg, int len, void *data), void *cdata)
{
  key = ckey;
  help = chelp;
  action = caction;
  data = cdata;
  return;  
};

// this is a client connection
Sock::Sock(string xaddr, int xport)
{
  //int err;
  int i;

  type = SOCK_CLIENT;
  stArray=NULL;
  cmdArray=NULL;
  parseArray=NULL;
  cmd = 1;
  parser = 0;
  //char *ip="127.0.0.1";
  addr=xaddr;
  sockfd=socket(AF_INET,SOCK_STREAM,0);
  if(sockfd<0) {
    cout<<"Fail to create socket..."<<endl;
    return;
  }
  cout<<"Creating connection..."<<endl;
  serAdd.sin_family=AF_INET;
  serAdd.sin_addr.s_addr=inet_addr(addr.c_str());
  serAdd.sin_port=htons(port);
  if(connect(sockfd,(const sockaddr *)&serAdd,sizeof(serAdd))==0) {
    cout<<"Connected!"<<endl;
    cin>>i;      
  } else {
    cout<<"Connect failed..."<<endl;
    cin>>i;
  }
}  

// this is the main server
//we accept clients 
Sock::Sock(int xport)
{
  int enable = 1;
  
  type = SOCK_SERVER;
  stArray = NULL;
  cmdArray = NULL;
  parseArray = NULL;
  cmd = 1;
  parser = 0;
  //addr =  NULL;
  port = xport;
  sockfd=socket(AF_INET,SOCK_STREAM,0);
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    cout <<"setsockopt(SO_REUSEADDR) failed"<<endl;
  if(sockfd<0) {
    cout<<"Fail to create socket..."<<endl;
    return;
  }
  cout<<"Binding..."<<endl;
  serAdd.sin_family=AF_INET;
  //serAdd.sin_addr.s_addr=inet_addr("127.0.0.1");
  serAdd.sin_addr.s_addr=INADDR_ANY;
  serAdd.sin_port=htons(port);
  if(bind(sockfd,(const sockaddr *)&serAdd,sizeof(serAdd))==0) {
    cout<<"Binding succeded!..."<<endl;
  } else {
    cout<<"Binding failed..."<<endl;
    // TODO close socket
    close(sockfd);
    sockfd = -1;
    return ;
  }
  if(listen(sockfd,1)==0) {
    cout<<"Listening on port " << port<< "..."<<endl;
  } else {
    cout<<"Listening failed..."<<endl;
    close(sockfd);
    sockfd = -1;
  }
  addCmd("help","show commands",cmd_help,NULL);
  addCmd("quit","Close connection",cmd_quit,NULL);
  
  return;

}

int Sock::saccept(void){
  socklen_t size = sizeof(cliAdd);
  int asock = accept(sockfd, (sockaddr *)&cliAdd, &size);
  if (asock>=0) {
    cout<<"Connection created!"<<endl;
  } else {
    cout<<"accept failed..."<<endl;
    //return -1;
  }
  return asock;
}

int Sock::addCmd(string key, string help, int (*action)(struct SockThread* st, char *msg, int len, void *data) , void *data)
{
  if(cmdArray == NULL) {
    cmdArray = new Array(16,16,"Sock Cmds");
  }
  SockCommand *scmd = new SockCommand(key, help, action, data);
  cmdArray->addItem(scmd);
  return 0;
}

int Sock::addParser(string key, string help, int (*action)(struct SockThread* st, char *msg, int len, void *data) , void *data)
{
  if(parseArray == NULL) {
    parseArray = new Array(16,16,"Sock Parsers");
  }
  SockCommand *scmd = new SockCommand(key, help, action, data);
  parseArray->addItem(scmd);
  parser = 1;
  
  return 0;
}
    

int Sock::runAccept(Array *srvArray)
{
  int fd = 0;
  skdone = 0;
  srvArray->addItem((void*)this);
  if(stArray == NULL) {
    stArray = new Array(16,16, "Sock Threads");
  }
  while (!skdone) {
    fd = saccept();
    if(fd > 0) {
      struct SockThread *st = new SockThread(this, fd, NULL);
      stArray->addItem((void*)st);
    }
  }
  return fd;
}


int Sock::runCmd(SockThread *st, char *msg, int len)
{
  Sock *sk = st->sk;
  int idx=0;
  //char sp[1024];
  SockCommand *scmd = NULL;
  int rc;
  char cmd[128];
  rc = sscanf(msg, "%127s", cmd);
  if(rc > 0) {
    while (sk->cmdArray->findNext((void **)&scmd, idx,0)) {

      if(strcmp(cmd, scmd->key.c_str()) == 0) {
	rc = scmd->action(st, msg, len, scmd->data);
	cout << " Ran cmd " << scmd->key << " ] rc [" << rc<<"]" << endl;
	return rc;
      }
    }
  }
  return 0;
}

int Sock::runParser(SockThread *st, char *msg, int len)
{
  Sock *sk = st->sk;
  int idx=0;
  char sp[1024];
  SockCommand *scmd = NULL;
  int rc=len;
  if(sk->parseArray) {
    if(rc > 0) {
      while (sk->parseArray->findNext((void **)&scmd, idx, 0)) {
	
	rc = scmd->action(st, msg, len, scmd->data);
	return rc;
      }
    }
  } else {
    snprintf(sp, sizeof(sp), " NO parsers on  port %d message # %d <%s>\n"
	     , sk->port
	     , st->msgnum
	     , msg
	     );
    
    rc = write(st->sfd, sp, strlen(sp));

  }
  return 0;
}

void *sockThread(void *data)
{
  int rc = 0;
  SockThread *st = (SockThread *)data;
  Sock *sk = st->sk;
  char sp[1024];
  
  do {
    cout << " SockThread running fd " << st->sfd << endl;
    rc = read(st->sfd, st->buf, st->buf_len-1);
    if (rc > 0 ) st->buf[rc]=0;  
    cout << " read ["<<rc<<"] bytes data [" <<st->buf<<"]" << endl;
    if (rc > 0 ) {
      snprintf(sp, sizeof(sp), " Hello from port %d message # %d ->"
	       , sk->port
	       , st->msgnum
	       );
      st->msgnum++;
      if(sk->parser == 0) {
	sk->runCmd(st, st->buf, rc);
	rc = write(st->sfd, sp, strlen(sp));
	rc = write(st->sfd, st->buf, rc);
      } else {
	sk->runParser(st, st->buf, rc);
      }
    }
  } while (rc >0);
  close(st->sfd);
  return NULL;
}


void *sockAcceptThread(void *data)
{

  Sock *sk = (Sock*)data;
  do {
    cout << " SockAcceptThread running fd " << sk->sockfd << " port ["<< sk->port << endl;
    sk->runAccept(g_srvArray);
  } while (sk->sockfd > 0);
  return NULL;
}



// addCmd(5555, ,"show ",cmd_socks,NULL);
// int cmd_help(struct SockThread*st, char *msg, void *data)

int addCmd(int port, string key, string desc, int (*cmd)(struct SockThread*st, char *msg, int len, void *data), void * data)
{
  int found = 0;
  if(g_srvArray == NULL)
    g_srvArray = new Array(16,16,"Srv Array");
  int idx = 0;
  Sock *sock=NULL;
  while(g_srvArray->findNext((void **)&sock, idx, 0)) {
    if(sock->port == port) {
      found = 1;
      break;
    }
  }
  //sock(5555)
  if(!found) {
    sock = new Sock(port);
    pthread_create(&sock->st, NULL, sockAcceptThread, (void *)sock);
    //pthread
    //sock->runAccept(g_srvArray);pthread
  }
  sock->addCmd(key.c_str(), desc.c_str(), cmd, data);
  // note that this must be in a thread
  return 0;
}
int addParser(int port, string key, string desc, int (*cmd)(struct SockThread*st, char *msg, int len, void *data), void * data)
{
  int found = 0;
  if(g_srvArray == NULL)
    g_srvArray = new Array(16,16,"Srv Array");
  int idx = 0;
  Sock *sock=NULL;
  while(g_srvArray->findNext((void **)&sock, idx, 0)) {
    if(sock->port == port) {
      found = 1;
      break;
    }
  }
  //sock(5555)
  if(!found) {
    sock = new Sock(port);
    pthread_create(&sock->st, NULL, sockAcceptThread, (void *)sock);
    //pthread
    //sock->runAccept(g_srvArray);pthread
  }
  sock->addParser(key.c_str(), desc.c_str(), cmd, data);
  sock->parser = 1;
  // note that this must be in a thread
  return 0;
}

int cmd_help(struct SockThread*st, char *msg, int len, void *data)
{
  Sock *sk = st->sk;
  int idx=0;
  char sp[1024];
  SockCommand *scmd = NULL;
  int rc = 0;
  while (sk->cmdArray->findNext((void **)&scmd,idx,0)) {
    snprintf(sp, sizeof(sp), " Key [%s] Desc [%s]\n"
	    , scmd->key.c_str()
	    , scmd->help.c_str()
	    );
    rc = write(st->sfd, sp, strlen(sp));
  }
  return rc;
}

//g_srvArray);
int cmd_socks(struct SockThread*st, char *msg, int len, void *data)
{
  //Sock *sk = st->sk;
  int idx=0;
  char sp[1024];
  Sock *sock = NULL;
  int rc = 0;
  while (g_srvArray->findNext((void **)&sock,idx,0)) {
    snprintf(sp, sizeof(sp), " Sock fd [%d] port [%d]\n"
	    , sock->sockfd
	    , sock->port
	    );
    rc = write(st->sfd, sp, strlen(sp));
  }
  return rc;
}

//g_srvArray);
int cmd_parser(struct SockThread*st, char *msg, int len, void *data)
{
  Sock *sk = st->sk;
  sk->cmd = 0;
  sk->parser = 1;
  char sp[1024];
  //Sock *sock = NULL;
  int rc = 0;
  snprintf(sp, sizeof(sp), " %s parser test\n"
	   , __FUNCTION__
	   );
  rc = write(st->sfd, sp, strlen(sp));
  snprintf(sp, sizeof(sp), " message [%s] len %d \n"
	   , msg
	   , len
	   );
  rc = write(st->sfd, sp, strlen(sp));
  
  return rc;
}

int cmd_quit(struct SockThread*st, char *msg, int len, void *data)
{
  int rc = 0;
  char *sp = (char *)" Goodbye\n";
  rc = write(st->sfd, sp, strlen(sp));
  close(st->sfd);
  return rc;
}

#include "sock.h"

int send_bcast_msg(int bsock, int bport, unsigned char *msg, int mlen)
{
  int rc;
  struct sockaddr_in s;
  if(bsock < 0)
    return 0;

  if (mlen < 0) mlen = (int)strlen((char *)msg); 
  memset(&s, '\0', sizeof(struct sockaddr_in));
  s.sin_family = AF_INET;
  s.sin_port = (in_port_t)htons(bport ? bport : 3310);
  s.sin_addr.s_addr = htonl(INADDR_BROADCAST);

  printf("broadcast %s len %d to %d\n", msg, mlen, bsock);

  rc = sendto(bsock, msg, mlen, 0
	      , (struct sockaddr *)&s, sizeof(struct sockaddr_in));
  if (rc< 0)
    perror("sendto");
  
  return rc;
}

int testBMSGTX(void *tdest, Array *cmd, void *data)
{
  int rc = 0;
  int bport = 6500;
  int bsock =socket(AF_INET, SOCK_DGRAM, 0);

  unsigned char msg[1024];

  int mlen = -1;
  int bcen = 1;
  snprintf((char *)msg, sizeof(msg),"%s"," Hello Bcast");

  rc = setsockopt(bsock, SOL_SOCKET, SO_BROADCAST, &bcen, sizeof(bcen));
  printf(" setup bcen %d\n", rc);
  
  rc = send_bcast_msg(bsock, bport, msg, mlen);
  printf(" send bcast %d\n", rc);
  return rc;
}


int get_bcast_msg(int bsock, int bport, unsigned char *msg, int mlen)
{
  int rc;
  struct sockaddr_in s;
  socklen_t slen = 0;
  if(bsock < 0)
    return -1;

  //if (mlen < 0) mlen = strlen(mes); 
  memset(&s, '\0', sizeof(struct sockaddr_in));
  s.sin_family = AF_INET;
  s.sin_port = (in_port_t)htons(bport ? bport : 3310);
  s.sin_addr.s_addr = htonl(INADDR_ANY);

  printf("listen len %d to %d\n", mlen, bsock);
  rc =  bind (bsock,(struct sockaddr *)&s, sizeof(s)); 
  while (1) {
    memset(msg, 0x0, mlen);
    slen = sizeof(s);
    rc = recvfrom(bsock, msg, mlen, 0
		  , (struct sockaddr *)&s, &slen);
    if (rc< 0)
      perror("recvfrom");
    else
      printf(" got data len rc %d msg [%s]\n", rc, msg );
  }
  return rc;
}

int testBMSGRX(void *tdest, Array *cmd, void *data)
{
  int rc = 0;
  int bport = 6500;
  int bsock =socket(AF_INET, SOCK_DGRAM, 0);
  unsigned char msg[1024];
  int mlen = sizeof(msg);
  int bcen = 1;

  rc = setsockopt(bsock, SOL_SOCKET, SO_BROADCAST, &bcen, sizeof(bcen));
  printf(" setup bcen %d \n", rc);
  
  rc = get_bcast_msg(bsock, bport, msg, mlen);
  printf(" get bcast %d\n", rc);

  return rc;
}


#ifdef _MAIN




int testAddCmd(void *tdest, Array *cmd, void *data)
{
  //int fd;
  //int found = 0;
  int port = 5556;
  addCmd( port, "socks", "show server socks", cmd_socks, NULL);
  
  return 0;
}

int testAddParser(void *tdest, Array *cmd, void *data)
{
  //int fd;
  //int found = 0;
  int port = 5557;
  addParser( port, "parser", "demo parser", cmd_parser, NULL);
  
  return 0;
}

//int test_client(void)

int testClient(void *tdest, Array *cmd, void *data)
{
  Sock sk("127.0.0.1", 5555);
  return 0;
}

int testServer(void *tdest, Array *cmd, void *data)
{
  //int fd;
  int found = 0;
  int port = 5555;
  if(g_srvArray == NULL)
    g_srvArray = new Array(16,16,"Serv Array");

  int idx = 0;
  Sock *sock=NULL;
  while(g_srvArray->findNext((void **)&sock, idx, 0)) {
    if(sock->port == port) {
      found = 1;
      break;
    }
  }
  //sock(5555)
  if(!found) {
    sock = new Sock(port);
    pthread_create(&sock->st, NULL, sockAcceptThread, (void *) sock);
    //pthread
    //sock->runAccept(g_srvArray);pthread
    cout<< " New server created for port "<< port  << endl;
  } else {
    cout<< " Server already created for port "<< port  << endl;
  }

  //Sock *sk = new Sock(5555);
  //pthread_create(&sk->st, NULL, sockAcceptThread, (void *) sk);
  //  sk->runAccept(g_srvArray);
  //g_srvArray->addItem(
  // TODO add to global list of sockets
  //fd = sk->saccept();
  //SockThread *st = new SockThread(sk, fd, NULL);
  //if(fd>0)close(fd);
  
  return 0;
}

int main(int argc, char *argv[])
{
  initTests();

  addTest("server", "test Server", testServer, NULL, g_tdest);
  addTest("BTX", "test BMSG", testBMSGTX, NULL, g_tdest);
  addTest("BRX", "test BMSG", testBMSGRX, NULL, g_tdest);
  addTest("client", "test client", testClient, NULL, g_tdest);
  addTest("addCmd", "test addCmd", testAddCmd, NULL, g_tdest);
  addTest("addParser", "test addParser", testAddParser, NULL, g_tdest);

  for (int i=1 ; i < argc ; i++ ) {
    runTest(g_tdest, argv[i]);
  }
  delTests();
  return 0;
}


#endif
