// crazy idea but...
// produce an list of angles for a chosen x/y location
// all we need are arm lengths
// prepare a table of x/y locations and add a list of possible
// angles to get to that location
// so this is how it works.
// we'll need a server
// given the arm configuration we'll request an x/y point
// one we get a request we'll set up a xyref for that point or a range of points close.
// if we already have one then we can find the closest point in that ref.
//  We'll probably need about 1024 points per ref but there we go.
//
// return the i,j,k for the selected point

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#define PI 3.14159265 

#define AL1  120
#define AL2  100
#define AL3  45
#define AL4  35
#define AL5  20

int max_ang = 90;
int min_ang = 0;

float pval = PI / 180.0;

// get angle array
int get_array( double *x, double *y)
{
  for (int i = 0; i < 360; i++) {
      x[i] = cos(i  * pval);
      y[i] = sin(i  * pval);
  }
  return 0;
}

struct xypoint {
  double x;
  double y;
  int i,j,k;
  struct xypoint *next;
};

struct xyref {
  double x;
  double y;
  double x_min;
  double y_min;
  double x_max;
  double y_max;
  int num_points;
  int num_points_req;
  struct xypoint *points;  
};

#define NUM_REF 40960
#define NUM_POINTS 40960
#define MAX_NUM_POINTS 1024

struct xyref g_xyr[NUM_REF];
int g_xyrf = 0;

struct xypoint g_xyp[NUM_POINTS];
int g_xyrp = 0;
double g_a1_x [360];
double g_a1_y [360];


// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080
struct xypoint;
struct xypoint *getAngles(double x, double y);
double findDist(struct xypoint *xyp, double x, double y);


int main_server(int argc, char *argv[])
{
  int server_fd, new_socket, valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};
  //char *hello = "Hello from server";
  char xcref[1024] = {0};
  char ycref[1024] = {0};

  double xref;
  double yref;
  
  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
      perror("socket failed");
      exit(EXIT_FAILURE);
    }

#ifndef SO_REUSEPORT
#define SO_REUSEPORT 0
#endif

  // Forcefully attaching socket to the port 8080
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
		 &opt, sizeof(opt)))
    {
      perror("setsockopt");
      exit(EXIT_FAILURE);
    }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( PORT );

  // Forcefully attaching socket to the port 8080
  if (bind(server_fd, (struct sockaddr *)&address,
	   sizeof(address))<0)
    {
      perror("bind failed");
      exit(EXIT_FAILURE);
    }
  if (listen(server_fd, 3) < 0)
    {
      perror("listen");
      exit(EXIT_FAILURE);
    }
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
			   (socklen_t*)&addrlen))<0)
    {
      perror("accept");
      exit(EXIT_FAILURE);
    }
  valread = 1;
  while (valread > 0)
    {
      valread = read(new_socket , buffer, 1024);
      printf("[%s] %d\n",buffer, valread );
      sscanf(buffer,"%s , %s", xcref, ycref);
      xref = atof(xcref);
      yref = atof(ycref);
      
      struct xypoint *
	xyp = getAngles(xref, yref);
      
      if(xyp) {
	snprintf(buffer, sizeof(buffer),
		 " >>>> Dist %g for loc x %g y %g i %d j %d k %d\n"
		 , findDist(xyp, xref, yref)
		 , xref
		 , yref
		 , xyp->i
		 , xyp->j
		 , xyp->k
		 );
	
      } else {
	//        printf(
	snprintf(buffer, sizeof(buffer),
		 
		 " NO loc for x %g y %g\n"
		 , xref
		 , yref
		 );
      }
      send(new_socket , buffer , strlen(buffer) , 0 );

      printf("xref [%s] %g , yref [%s] %g\n", xcref, xref, ycref, yref);
  
    }
      //send(new_socket , hello , strlen(hello) , 0 );

      //printf("Hello message sent\n");
  return 0;
}

int initRefs(void)
{
  struct xyref *xyrp;

  for (int i = 0 ; i < NUM_REF; i++) {
    xyrp = &g_xyr[i];
    xyrp->num_points = 0;
    xyrp->num_points_req = 0;
    xyrp->points = NULL;
  }
  return 0;
}

struct xyref *findReference (double x, double y)
{
  struct xyref *xyrp = NULL;
  for (int i = 0 ; i < g_xyrf; i++) {
    xyrp = &g_xyr[i];
    if(
       (xyrp->x_min <= x) && (xyrp->x_max > x) &&
       (xyrp->y_min <= y) && (xyrp->y_max > y)
       ) {
      return xyrp;
    }
  }
  return NULL;
}

int showRef(struct xyref *xyrp, int i)
{
  
  printf(" XRef [%d] x %g y %g min x %g max x %g min y %g max y %g num_points %d/%d\n"
	 ,i
	 ,xyrp->x
	 ,xyrp->y
	 ,xyrp->x_min
	 ,xyrp->x_max
	 ,xyrp->y_min
	 ,xyrp->y_max
	 ,xyrp->num_points
	 ,xyrp->num_points_req
	 );
  return 0;
	 
}

int showRefPoint(struct xypoint *xyp, int ix, int i)
{
  printf(" Xref [%d] Xpoint %d  x %g y %g i %d j %d k %d\n"
	 ,ix
	 ,i
	 ,xyp->x
	 ,xyp->y
	 ,xyp->i
	 ,xyp->j
	 ,xyp->k
	 );
  return 0;
	 
}

int showRefPoints(struct xyref *xyrf, int ix, int max)
{
  int i = 0;
  struct xypoint *xyp;
  xyp = xyrf->points;
  while ((i < max) && (i< xyrf->num_points)) {
    showRefPoint(xyp, ix, i);
    i++;
    xyp = xyp->next;
  }
  return 0;
}
  


int showRefNum(int i)
{
  struct xyref *xyrp = &g_xyr[i];
  return showRef(xyrp, i);
}

int showRefPointsNum(int i, int max)
{
  struct xyref *xyrp = &g_xyr[i];
  return showRefPoints(xyrp, i, max);
}

double getXMinVal(double x)
{
  double xi = 0.0;
  while(xi < x) xi+=5.0;
  if(xi > 0) xi -=5.0;
  
  return xi;
}

double getXMaxVal(double x)
{

  x = x + 5;
  return x;

}

double getYMinVal(double x)
{
  double xi = 0.0;
  while(xi < x) xi+=2.0;
  if(xi > 0) xi -=2.0;
  return xi;
}

double getYMaxVal(double x)
{
  x = x + 2.0;
  return x;
}


struct xyref *newXyrf(double x , double y)
{

  struct xyref *xyrf = NULL;
  int i = g_xyrf;
  //= (struct xyref *)malloc(sizeof(struct xyref));
  xyrf = &g_xyr[i];
  xyrf->x = x;
  xyrf->y = y;
  xyrf->x_min = getXMinVal(x);
  xyrf->x_max = getXMaxVal(xyrf->x_min);
  xyrf->y_min = getYMinVal(y);
  xyrf->y_max = getYMaxVal(xyrf->y_min);
  if(g_xyrf < NUM_REF ) g_xyrf++;
  return xyrf;
}

// limit points
int addPoint (struct xyref *xyrf, double x, double y, int i , int j, int k)
{
  struct xypoint *xyp = NULL;
  struct xypoint *xypp = xyrf->points;

  int ip = g_xyrp;
  xyrf->num_points_req++;
  if(xyrf->num_points > MAX_NUM_POINTS) {
    return 0;
  }
  if (ip >= NUM_POINTS) {
    return 0;
  }
  //= (struct xyref *)malloc(sizeof(struct xyref));
  xyp = &g_xyp[ip];
  g_xyrp++;
  
  xyp->x = x;
  xyp->y = y;
  xyp->i = i;
  xyp->j = j;
  xyp->k = k;
  xyp->next = NULL;
  i = xyrf->num_points;
  if (i == 0) {
    xyrf->points = xyp;
    xyrf->num_points++;
    return 0;
  }
  while (--i > 0) {
    xypp = xypp->next;
  }
  xypp->next = xyp;
  xyrf->num_points++;
  return 0;
}


int addReference (double x, double y, int i , int j, int k)
{
  struct xyref *xyrf;
  xyrf = findReference (x, y);
  if(!xyrf) {
    xyrf = newXyrf(x,y);
  }
  addPoint(xyrf, x, y, i, j, k);
  return 0;
}

double findDist(struct xypoint *xyp, double x, double y)
{
  double xd = (x - xyp->x);
  double yd = (y - xyp->y);
  
  double dist = sqrt( (xd * xd) + (yd * yd));
  return dist;
}
int calcKinetics(double xreq, double yreq);

struct xypoint *findPoint(struct xyref *xyrf, double x, double y)
{
  int i = 0;
  double dist;
  double min_dist = -1;
  struct xypoint *xypf = NULL;
  struct xypoint *xypp = xyrf->points;
  while (i < xyrf->num_points) {
    dist =  findDist(xypp, x, y);
    if((i == 0) || ( dist < min_dist)) {
      
      printf( " min dist now %g  was %g index %d\n"
	      , dist
	      , min_dist
	      , i
	      );
      
      min_dist = dist;
      xypf = xypp;

    }
    xypp = xypp->next;
    i++;
    
  }
  return xypf;
  
}

struct xypoint *getAngles(double x, double y)
{
  //int num_found;
  int i;
  struct xyref *xyrf;
  struct xypoint *xyp = NULL;
  xyrf = findReference (x, y);
  if(!xyrf) {
    
    calcKinetics(x, y);
    
    for (i = 0 ; i < 10; i++) {
      showRefNum(i);
      showRefPointsNum(i, 48);
    }
    xyrf = findReference (x, y);
  }
  
  
  if(xyrf) {
    printf(" got ref for %g/%g\n", x, y);
    xyp = findPoint(xyrf, x, y);
    if(xyp) {
      return xyp;
    }
  } else {
    
    printf(" no ref for %g/%g\n", x, y);
  }
  return NULL;
  
}


int calcKinetics(double xreq, double yreq)
{
  int i,j,k;
  double end_x;
  double end_y;
  double min_end_x;
  double min_end_y;
  double max_end_x;
  double max_end_y;
  int num_found = 0;
  double x_min = getXMinVal(xreq);
  double x_max = getXMaxVal(xreq);
  double y_min = getYMinVal(yreq);
  double y_max = getYMaxVal(yreq);

  for (i = 0 ; i < 179; i++) {
    for (j = 0 ; j < 179; j++) {
      for (k = 0 ; k < 179; k++) {
	end_x = g_a1_x[i]*AL1 + g_a1_x[j]*AL2 + g_a1_x[k]*AL3;
	end_y = g_a1_y[i]*AL1 + g_a1_y[j]*AL2 + g_a1_y[k]*AL3;
	//addReference (end_x, end_y, i , j, k);
	
	if (
	    (end_x >x_min ) && (end_x < x_max ) &&
	    (end_y >y_min) && (end_y < y_max ))
	  
	  {
	    if (num_found == 0) {
	      min_end_x = end_x;
	      max_end_x = end_x;
	      min_end_y = end_y;
	      max_end_y = end_y;
	    }
	    if(end_x > max_end_x) max_end_x = end_x;
	    if(end_x < min_end_x) min_end_x = end_x;	
	    
	    if(end_y > max_end_y) max_end_y = end_y;
	    if(end_y < min_end_y) min_end_y = end_y;
	    
	    printf(" match at %d %d %d xval %g yval %g\n"
		   , i, j ,k
		   , end_x
		   , end_y
		   );
	    addReference (end_x, end_y, i , j, k);
	    num_found++;
	  }
	//end_y[i + (j * 90) + (k * 90 * 90)] = a1_y[i] + a2_y[j] + a3_y[k];
	
      }
    }
  }
  printf(" at end num_found %d min/max_x %g/%g min/max_y %g/%g\n"
	 "refs %d points %d\n"
	 , num_found
	 , min_end_x
	 , max_end_x
	 , min_end_y
	 , max_end_y
	 , g_xyrf
	 , g_xyrp
	 
	 );
  
  return num_found;
}


int main( int argc, char * argv[])
{
  //int i;
  //int idx;


  //double end_x;
  //double end_y;
  //double min_end_x;
  //double min_end_y;
  //double max_end_x;
  //double max_end_y;
  //int num_found = 0;
  initRefs();

  
  printf(" running a1 \n");
  get_array( g_a1_x, g_a1_y);

  double xreq=183.1;
  double yreq=37.0;

  main_server(argc, argv);
  //return 0;
  //num_found = calcKinetics(xreq, yreq);
  
  //for (i = 0 ; i < 10; i++) {
  //showRefNum(i);
  //showRefPointsNum(i, 48);
  //}

  struct xypoint *xyp = getAngles(xreq, yreq);
  
  if(xyp) {
    printf(" >>>>>Dist %g for loc x %g y %g i %d j %d k %d\n"
	   , findDist(xyp, xreq, yreq)
	   , xreq
	   , yreq
	   , xyp->i
	   , xyp->j
	   , xyp->k
	   );
    
  } else {
        printf(" NO loc for x %g y %g\n"
	   , xreq
	   , yreq
	   );
  }    

  xreq=173.1;
  yreq=39.0;

  xyp = getAngles(xreq, yreq);
  
  if(xyp) {
    printf(" >>>> Dist %g for loc x %g y %g i %d j %d k %d\n"
	   , findDist(xyp, xreq, yreq)
	   , xreq
	   , yreq
	   , xyp->i
	   , xyp->j
	   , xyp->k
	   );
    
  } else {
        printf(" NO loc for x %g y %g\n"
	   , xreq
	   , yreq
	   );
  }
  
  return 0;

 
  
}
