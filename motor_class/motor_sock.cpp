
// socket handling for motor class demo

#include <semaphore.h>

#include "motor_sock.h"

#include "pthread.h"
#include <string>

using namespace std;


int run_server = 0;
int server_sock = -1;
pthread_t sthread;
void *server_thread(void *p);

Server::Server( int p)
{
  sem_init(&cli_sem,0, 1);
  port = p;
  sock = -1;
}

Server::~Server()
{
   sem_destroy(&cli_sem);
}


int Server::start()
{
  run_server = 1;
  sock = startup((u_short)port);
  printf("httpd running on port %d\n", port);
  // now run an accept thread
  if (pthread_create(&stthread , NULL, server_thread, (void *)this) != 0) {
    perror("pthread_create");
  }
  printf("httpd thread running\n");
  
  return 0;
}

int Server::stop()
{
  run_server = 0;
  close (sock); // this should kill it
  pthread_join(stthread, NULL);
  return 0;
}


#define MAX_METHOD 32
#define MAX_URL 1024

/**********************************************************************/
/* A request has caused a call to accept() on the server port to
 * return.  Process the request appropriately.
 * Parameters: the socket connected to the client */
/**********************************************************************/
void *accept_request(void *cli)
{
  char buf[1024];
  char msg[1024];
  int numchars;
  string method;
  string url;
  string path;
  string query;
  int client;
  Server *serv = (Server *)cli;
  
  sem_wait(&serv->cli_sem);
  client = serv->getClient();
  sem_post(&serv->cli_sem);
  
  size_t i, j;
  struct stat st;
  int cgi = 0;      /* becomes true if server decides this is a CGI
		     * program */
  
  numchars = getLine(client, buf, sizeof(buf));
  //printf(" Got LINE buf [%s]\n", buf);

  method = "";
  i = 0; j = 0;
  //read into method upto the first space
  while (!ISspace(buf[j]) && (i < MAX_METHOD - 1)) {
      method += buf[j];
      i++; j++;
  }
  
  //printf(" Got method [%s] buf [%s]\n", method.c_str(), buf);
  
  
  // POST provides CGI
  if (method == "POST")
    cgi = 1;
  

  i = 0;
  // skip spaces
  while (ISspace(buf[j]) && (j < sizeof(buf))) {
    j++;
  }
  url = "";
  while (!ISspace(buf[j]) && (i < MAX_URL - 1) && (j < sizeof(buf))) {
      url +=  buf[j];
      i++; j++;
    }
  
  // work out qyery string , everything after the ?
  if (method == "GET") {
      int qx = 0;
      int qx_start = 0;
      query = "";
      qx = url.find("?");
      if (qx != string::npos) {
	query = url;
	url.erase(url.begin()+qx, url.end());
	//query.erase(query.begin(), query.begin() + qx +1);
	query.erase(0, qx +1);
	cgi = 1;
      }
  }
  
  if(cgi) {
    
    snprintf(msg, sizeof(msg)," Got CGI \n buf [%s] \n m [%s]\n u [%s]\n q [%s]\n"
	     ,buf,  method.c_str(), url.c_str(), query.c_str());
  } else {
    snprintf(msg, sizeof(msg)," Got NON CGI \n buf [%s]\n m [%s]\n u [%s]\n"
	     , buf, method.c_str(), url.c_str());
  }
  printf("%s\n", msg);
  write (client, msg, strlen(msg));
  close(client);
  return NULL;
}

/**********************************************************************/
/* Inform the client that a request it has made has a problem.
 * Parameters: client socket */
/**********************************************************************/
void bad_request(int client)
{
 char buf[1024];

 sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
 send(client, buf, sizeof(buf), 0);
 sprintf(buf, "Content-type: text/html\r\n");
 send(client, buf, sizeof(buf), 0);
 sprintf(buf, "\r\n");
 send(client, buf, sizeof(buf), 0);
 sprintf(buf, "<P>Your browser sent a bad request, ");
 send(client, buf, sizeof(buf), 0);
 sprintf(buf, "such as a POST without a Content-Length.\r\n");
 send(client, buf, sizeof(buf), 0);
}

/**********************************************************************/
/* Put the entire contents of a file out on a socket.  This function
 * is named after the UNIX "cat" command, because it might have been
 * easier just to do something like pipe, fork, and exec("cat").
 * Parameters: the client socket descriptor
 *             FILE pointer for the file to cat */
/**********************************************************************/
void cat(int client, FILE *resource)
{
 char buf[1024];

 fgets(buf, sizeof(buf), resource);
 while (!feof(resource))
 {
  send(client, buf, strlen(buf), 0);
  fgets(buf, sizeof(buf), resource);
 }
}

/**********************************************************************/
/* Inform the client that a CGI script could not be executed.
 * Parameter: the client socket descriptor. */
/**********************************************************************/
void cannot_execute(int client)
{
 char buf[1024];

 sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "Content-type: text/html\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
 send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Print out an error message with perror() (for system errors; based
 * on value of errno, which indicates system call errors) and exit the
 * program indicating an error. */
/**********************************************************************/
void error_die(const char *sc)
{
 perror(sc);
 exit(1);
}

/**********************************************************************/
/* Execute a CGI script.  Will need to set environment variables as
 * appropriate.
 * Parameters: client socket descriptor
 *             path to the CGI script */
/**********************************************************************/
void execute_cgi(int client, const char *path,
                 const char *method, const char *query_string)
{
 char buf[1024];
 int cgi_output[2];
 int cgi_input[2];
 pid_t pid;
 int status;
 int i;
 char c;
 int numchars = 1;
 int content_length = -1;

 buf[0] = 'A'; buf[1] = '\0';
 if (strcasecmp(method, "GET") == 0)
  while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
   numchars = getLine(client, buf, sizeof(buf));
 else    /* POST */
 {
  numchars = getLine(client, buf, sizeof(buf));
  while ((numchars > 0) && strcmp("\n", buf))
  {
   buf[15] = '\0';
   if (strcasecmp(buf, "Content-Length:") == 0)
    content_length = atoi(&(buf[16]));
   numchars = getLine(client, buf, sizeof(buf));
  }
  if (content_length == -1) {
   bad_request(client);
   return;
  }
 }

 sprintf(buf, "HTTP/1.0 200 OK\r\n");
 send(client, buf, strlen(buf), 0);

 if (pipe(cgi_output) < 0) {
  cannot_execute(client);
  return;
 }
 if (pipe(cgi_input) < 0) {
  cannot_execute(client);
  return;
 }

 if ( (pid = fork()) < 0 ) {
  cannot_execute(client);
  return;
 }
 if (pid == 0)  /* child: CGI script */
 {
  char meth_env[255];
  char query_env[255];
  char length_env[255];

  dup2(cgi_output[1], 1);
  dup2(cgi_input[0], 0);
  close(cgi_output[0]);
  close(cgi_input[1]);
  sprintf(meth_env, "REQUEST_METHOD=%s", method);
  putenv(meth_env);
  if (strcasecmp(method, "GET") == 0) {
   sprintf(query_env, "QUERY_STRING=%s", query_string);
   putenv(query_env);
  }
  else {   /* POST */
   sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
   putenv(length_env);
  }
  execl(path, path, NULL);
  exit(0);
 } else {    /* parent */
  close(cgi_output[1]);
  close(cgi_input[0]);
  if (strcasecmp(method, "POST") == 0)
   for (i = 0; i < content_length; i++) {
    recv(client, &c, 1, 0);
    write(cgi_input[1], &c, 1);
   }
  while (read(cgi_output[0], &c, 1) > 0)
   send(client, &c, 1, 0);

  close(cgi_output[0]);
  close(cgi_input[1]);
  waitpid(pid, &status, 0);
 }
}

/**********************************************************************/
/* Get a line from a socket, whether the line ends in a newline,
 * carriage return, or a CRLF combination.  Terminates the string read
 * with a null character.  If no newline indicator is found before the
 * end of the buffer, the string is terminated with a null.  If any of
 * the above three line terminators is read, the last character of the
 * string will be a linefeed and the string will be terminated with a
 * null character.
 * Parameters: the socket descriptor
 *             the buffer to save the data in
 *             the size of the buffer
 * Returns: the number of bytes stored (excluding null) */
/**********************************************************************/
int getLine(int sock, char *buf, int size)
{
 int i = 0;
 char c = '\0';
 int n;

 while ((i < size - 1) && (c != '\n'))
 {
  n = recv(sock, &c, 1, 0);
  /* DEBUG printf("%02X\n", c); */
  if (n > 0)
  {
   if (c == '\r')
   {
    n = recv(sock, &c, 1, MSG_PEEK);
    /* DEBUG printf("%02X\n", c); */
    if ((n > 0) && (c == '\n'))
     recv(sock, &c, 1, 0);
    else
     c = '\n';
   }
   buf[i] = c;
   i++;
  }
  else
   c = '\n';
 }
 buf[i] = '\0';
 
 return(i);
}

/**********************************************************************/
/* Return the informational HTTP headers about a file. */
/* Parameters: the socket to print the headers on
 *             the name of the file */
/**********************************************************************/
void headers(int client, const char *filename)
{

 (void)filename;  /* could use filename to determine file type */

 string res ="HTTP/1.0 200 OK\r\n";
 res+=SERVER_STRING;
 res += "Content-Type: text/html\r\n";
 res += "\r\n";

 send(client, res.c_str(), res.length(), 0);
}


/**********************************************************************/
/* Give a client a 404 not found status message. */
/**********************************************************************/
void not_found(int client, const char *file)
{

  string res ="HTTP/1.0 404 NOT FOUND\r\n";
  res += SERVER_STRING;
  res += "Content-Type: text/html\r\n";
  res += "\r\n";
  res += "<HTML><TITLE>Not Found</TITLE>\r\n";
  res += "<BODY><P>The server could not fulfill\r\n";
  res += "your request because the resource [";
  res+= file;
  res += "] \r\n";
  res += "is unavailable or nonexistent.\r\n";
  res +="</BODY></HTML>\r\n";
  send(client, res.c_str(), res.length(), 0);
}

/**********************************************************************/
/* Send a regular file to the client.  Use headers, and report
 * errors to client if they occur.
 * Parameters: a pointer to a file structure produced from the socket
 *              file descriptor
 *             the name of the file to serve */
/**********************************************************************/
void serve_file(int client, const char *filename)
{
 FILE *resource = NULL;
 int numchars = 1;
 char buf[1024];
 
 // trick to pre load buffer
 buf[0] = 'A'; buf[1] = '\0';

 /* read & discard headers */
 while ((numchars > 0) && strcmp("\n", buf)) {
  numchars = getLine(client, buf, sizeof(buf));
 }
 // try to open a file
 resource = fopen(filename, "r");
 if (resource == NULL)
   not_found(client ,(const char *)buf);
 else
 {
  headers(client, filename);
  cat(client, resource);
 }
 fclose(resource);
}

/**********************************************************************/
/* This function starts the process of listening for web connections
 * on a specified port.  If the port is 0, then dynamically allocate a
 * port and modify the original port variable to reflect the actual
 * port.
 * Parameters: pointer to variable containing the port to connect on
 * Returns: the socket */
/**********************************************************************/
int startup(u_short port)
{
  int sock = 0;
  struct sockaddr_in name;

  sock = socket(PF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    error_die("socket");
  }
  memset(&name, 0, sizeof(name));
  name.sin_family = AF_INET;
  name.sin_port = htons(port);
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
    error_die("bind");
  }

  int enable = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    error_die("setsockopt(SO_REUSEADDR) failed");
  }
  if (listen(sock, 5) < 0) {
    error_die("listen");
  }
  return(sock);
}

/**********************************************************************/
/* Inform the client that the requested web method has not been
 * implemented.
 * Parameter: the client socket */
/**********************************************************************/
void unimplemented(int client, const char *method)
{
 char buf[1024];

 sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, SERVER_STRING);
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "Content-Type: text/html\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "<html><hrad><title>Method Not Implemented\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "</title></head>\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "<body><P>HTTP request method [%s] not supported.\r\n", method);
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "</body></html>\r\n");
 send(client, buf, strlen(buf), 0);
}

/**********************************************************************/


#if 0
int start_server(int port)
{
  
  if(run_server == 0) {
    run_server = 1;
    g_server = new Server(port);
    if (pthread_create(&sthread , NULL, server_thread, (void *)g_server) != 0)
      perror("pthread_create");
  }
  return 0;
}

#endif

int stop_server()
{
  
  if(run_server == 1) {
    run_server = 0;
    close(server_sock); // this should wake it up
    //pthread_join(sthread);
  }
  return 0;
}

void *server_thread(void *p)
{

  Server *serv= (Server *)p;
  u_short port = (u_short)serv->getPort();
  printf("httpd server_thread running port %d run_server %d\n"
	 , serv->getPort()
	 , run_server);

  int client_sock = -1;
  struct sockaddr_in client_name;
  unsigned int client_name_len = sizeof(client_name);
  pthread_t newthread;
  
  while (run_server) {
    client_sock = accept(serv->getSock(),
			 (struct sockaddr *)&client_name,
			 &client_name_len);
    printf("httpd server_thread accept  client %d \n", client_sock);
    if (client_sock == -1) {
      error_die("accept");
    }
    sem_wait(&serv->cli_sem);
    serv->setClient(client_sock);
    /* accept_request(client_sock); */
    
    if (pthread_create(&newthread , NULL, accept_request, (void *)p) != 0) {
      perror("pthread_create");
    }
    sem_post(&serv->cli_sem);

  }
  
  close(server_sock);
  printf("httpd server_thread quitting port %d \n", serv->getPort());
  
  pthread_exit(NULL);
}

//g_server = new Server(8080);
//g_server->start();  


#if 0
// run this when we get a hit on the server socket
int Server::setUpSock(void)
{

  sock = startup((u_short)port);
  
  printf("httpd running on port %d\n", port);
  // now run an accept thread
  if (pthread_create(&sthread , NULL, server_thread, (void *)g_server) != 0) {
    perror("pthread_create");
  }

  return sock;
}
#endif


int Server::acceptSock(void)
{

  int client_sock = -1;
  struct sockaddr_in client_name;
  unsigned int client_name_len = sizeof(client_name);
  client_sock = accept(sock,
		       (struct sockaddr *)&client_name,
		       &client_name_len);
  if (client_sock == -1) {
    printf("accept failed");
  }
  printf("httpd accepted sock %d\n", client_sock);
  
  return client_sock;
  
}

