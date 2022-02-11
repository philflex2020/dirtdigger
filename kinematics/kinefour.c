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
//
// kinetwo better sort on i,j,k
// kinethree better use proper arm angle
// kinefour
//  better object management 
// only keep the 50 closest to the target

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
int print_reqs(int numr, int nump);

// get angle array
int get_array( double *x, double *y, double z)
{
  for (int i = 0; i < 360; i++) {
      x[i] = z * cos(i  * pval);
      y[i] = z * sin(i  * pval);
  }
  return 0;
}

struct xypoint {
  double x;
  double y;
  int i,j,k;
  double pdist;
  struct xypoint *next;
};

struct xyref {
  double x;
  double y;
  double xr;    // requested x
  double yr;    // requested y
  double x_min;
  double y_min;
  double x_max;
  double y_max;
  double pdist;
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

double g_al1_x [360];
double g_al1_y [360];

double g_al2_x [360];
double g_al2_y [360];

double g_al3_x [360];
double g_al3_y [360];


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
struct xypoint *getLAngles(double x, double y, int li, int lj, int lk);
double findDist(struct xypoint *xyp, double x, double y);
double findLDist(struct xypoint *xyp, double x, double y, int li, int lj, int lk);
int myport = PORT;

int getXYref(double *xreq, double *yreg, int i, int j , int k);

// open a port 8080

int main_server(int argc, char *argv[])
{
  int server_fd, new_socket, valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};
  char buffer2[1024] = {0};
  //char *hello = "Hello from server";
  char xcmd[1024] = {0};
  char xcref[1024] = {0};
  char ycref[1024] = {0};
  //char xcref[1024] = {0};
  char zcref[1024] = {0};

  double xref;
  double yref;

  int lasti, lastj, lastk;
  lasti = -1;
  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
      perror("socket failed");
      exit(EXIT_FAILURE);
  }
#ifndef SO_REUSEPORT
#define SO_REUSEPORT 0
#endif

  // Forcefully attaching socket to the port 8080
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
		 &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( myport );

  // Forcefully attaching socket to the port 8080
  if (bind(server_fd, (struct sockaddr *)&address,
	   sizeof(address))<0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
			   (socklen_t*)&addrlen))<0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  valread = 1;
  while (valread > 0) {
      struct xypoint *xyp = NULL;
      struct xypoint *xypl = NULL;
      int iscan;
      int i,j,k;
      valread = read(new_socket , buffer, 1024);
      printf("[%s] %d\n",buffer, valread );
      iscan = sscanf(buffer,"%s %s %s %s", xcmd, xcref, ycref, zcref);
      printf("%d [%s] \n",iscan, xcmd );

      if(iscan > 0) {
	if(strcmp(xcmd, "help")==0) {
	  snprintf(buffer, sizeof(buffer), "%s",
		   " Command Help: help \n"
		   " Command XY xval yval: send XY point get angles\n"
		   " Command ANG i j k: send i,j,k angs get XY\n"
		   " Command XYList xval yval: get closest (TODO) \n"
		   " Command ANGList i j k:  get closest (TODO) \n"
		   );
	  send(new_socket , buffer , strlen(buffer) , 0 );
	}
	else if(strcmp(xcmd, "XY")==0) {

	  xref = atof(xcref);
	  yref = atof(ycref);
	  xyp = getAngles(xref, yref);
	  xypl = NULL;
	  
	  if(xyp) {
	    if(lasti == -1) {
	      lasti = xyp->i;
	      lastj = xyp->j;
	      lastk = xyp->k;
	    } else {
	      xypl = getLAngles(xref, yref, lasti, lastj, lastk);
	      snprintf(buffer2, sizeof(buffer2),
		       " >>>> Corrected Dist %g for loc x %g y %g i %d j %d k %d\n"
		       , findDist(xyp, xref, yref)
		       , xref
		       , yref
		       , xypl->i
		       , xypl->j
		       , xypl->k
		       );
	      
	    }
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
	  if (xypl) 
	    send(new_socket , buffer2 , strlen(buffer2) , 0 );
	}
	else if(strcmp(xcmd, "ANG")==0) {
	  i = atoi(xcref);
	  j = atoi(ycref);
	  k = atoi(zcref);

	  getXYref(&xref, &yref, i, j , k);
	  snprintf(buffer, sizeof(buffer),
		   "ANG  %g  %g for i %d j %d k %d\n"
		   , xref
		   , yref
		   , i
		   , j
		   , k
		   );
	  send(new_socket , buffer , strlen(buffer) , 0 );
	}
      }
  }
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

// find an xyref in range of xr and yr
struct xyref *findReference (double xr, double yr)
{
  struct xyref *xyrp = NULL;
  for (int i = 0 ; i < g_xyrf; i++) {
    xyrp = &g_xyr[i];
    if(
       (xyrp->x_min <= xr) && (xyrp->x_max > xr) &&
       (xyrp->y_min <= yr) && (xyrp->y_max > yr)
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
  printf(" Xref [%d] Xpoint [%d]  x %g y %g i %d j %d k %d\n"
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

int appendPoint (int i, struct xypoint *xyp, struct xypoint *xypp)
{
  while (--i > 0) {
    xypp = xypp->next;
  }
  xypp->next = xyp;
  return i;
}

// put lower pdist first
int insertPoint (int i, struct xypoint *xyp, struct xypoint *xypp)
{
  double pdist=xyp->pdist;
  xypoint *xprev = NULL;
  while (--i > 0) {
    xprev = xypp;
    if (pdist > xypp->pdist) {
      xypp = xypp->next;
    }
  }
  if (i == 0) {
      // just dunk it on the end
      xypp->next = xyp;
  } else {
    // insert it
    xyp->next = xypp;
    xprev->next = xyp;
    
  }
  return i;
}

// limit points
// store points in decreasing pdist
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
  // this appends point
  appendPoint (i, xyp, xypp);
  
  xyrf->num_points++;
  return 0;
}


// given a requested xr, yr and a calculated x,y
// add an i,j,k point to the selected xyref
// x,y calculated from the i,j,k angles
//    will probably be a small distance away from xr,xy
//we will want to find the closest later
// 
int addReference (double xr, double yr, double x, double y, int i , int j, int k)
{
  struct xyref *xyrf;
  xyrf = findReference (x, y);
  if(!xyrf) {
    xyrf = newXyrf(x,y);
    xyrf->xr = xr;
    xyrf->yr = yr;
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

double findLDist(struct xypoint *xyp, double x, double y, int li, int lj, int lk)
{
  double id = (li - xyp->i);
  double jd = (lj - xyp->j);
  double kd = (lk - xyp->k);
  double dist = sqrt( (id * id) + (jd * jd) + (kd * kd));
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

struct xypoint *findLPoint(struct xyref *xyrf, double x, double y
			   , int li, int lj, int lk)
{
  int i = 0;
  double ldist;
  double min_dist = -1;
  struct xypoint *xypf = NULL;
  struct xypoint *xypp = xyrf->points;
  while (i < xyrf->num_points) {
    ldist =  findLDist(xypp, x, y, li, lj ,lk);
    if((i == 0) || ( ldist < min_dist)) {
      
      printf( " min Ldist now %g  was %g index %d dist %g i %d j %d k %d\n"
	      , ldist
	      , min_dist
	      , i
	      , findDist(xypp, x, y)
	      , xypp->i
	      , xypp->j
	      , xypp->k
	      );
      
      min_dist = ldist;
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

struct xypoint *getLAngles(double x, double y, int li, int lj, int lk)
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
    xyp = findLPoint(xyrf, x, y, li, lj ,lk);
    if(xyp) {
      return xyp;
    }
  } else {
    
    printf(" no ref for %g/%g\n", x, y);
  }
  return NULL;
  
}
//int print_reqs(int numr, int nump)

int getXYref_psw(double *xreq, double *yreq, int i, int j , int k)
{
  double end_x;
  double end_y;

  int ia, ja , ka;
  ia = i;
  ja = (j + ia - 179); if (ja < 0) ja += 360;
  ka = (k + ja - 179); if (ka < 0) ka += 360;

  printf (" %s \n", __FUNCTION__);
  printf ("     i  %d j  %d k  %d\n", i ,  j ,  k);
  printf ("     ia %d ja %d ka %d\n", ia , ja , ka);
  printf ("  g_al1_x/y\t%g\t%g \n", g_al1_x[ia], g_al1_y[ia]);
  printf ("  g_al2_x/y\t%g\t%g \n", g_al2_x[ja], g_al2_y[ja]);
  printf ("  g_al3_x/y\t%g\t%g \n", g_al3_x[ka], g_al3_y[ka]);

  //end_x = g_a1_x[i]*AL1 + g_a1_x[j]*AL2 + g_a1_x[k]*AL3;
  //end_y = g_a1_y[i]*AL1 + g_a1_y[j]*AL2 + g_a1_y[k]*AL3;
  end_x = g_al1_x[ia] + g_al2_x[ja] + g_al3_x[ka];
  end_y = g_al1_y[ia] + g_al2_y[ja] + g_al3_y[ka];
  printf ("  end_x/end_y\t%g\t%g \n", end_x, end_y);



  if(xreq) { *xreq = end_x;}
  if(yreq) { *yreq = end_y;}
  return 0;
}

int getXYref(double *xreq, double *yreq, int i, int j , int k)
{
  double end_x;
  double end_y;

  int ia, ja , ka;
  ia = i;
  ja = (j + ia - 175); if (ja < 0) ja += 360;
  ka = (k + ja - 175); if (ka < 0) ka += 360;
 

  //end_x = g_a1_x[i]*AL1 + g_a1_x[j]*AL2 + g_a1_x[k]*AL3;
  //end_y = g_a1_y[i]*AL1 + g_a1_y[j]*AL2 + g_a1_y[k]*AL3;
  end_x = g_al1_x[ia] + g_al2_x[ja] + g_al3_x[ka];
  end_y = g_al1_y[ia] + g_al2_y[ja] + g_al3_y[ka];

  if(xreq) { *xreq = end_x;}
  if(yreq) { *yreq = end_y;}
  return 0;
}


int test_reqs(int imin, int imax,int jmin, int jmax, int kmin, int kmax)
{
  double xreq;
  double yreq;
  int i,j,k;
  for (i = imin; i < imax; i++) {
    for (j = jmin; j < jmax; j++) {
      for (k = kmin; k < kmax; k++) {
	getXYref(&xreq, &yreq, i, j , k);
	addReference (xreq, yreq, xreq, yreq, i , j, k);	
      }
    }
  }
  return 0;
}

int showRef(struct xyref *xyrp, int i);
int showRefPoints(struct xyref *xyrf, int ix, int max);
int showRefNum(int i);
int showRefPointsNum(int i, int max);

int print_reqs(int numr, int nump)
{
  printf( " Num refs %d\n", g_xyrf);
  for( int i = 0; i < numr; i++)
    {
      if (i < g_xyrf) {
	showRefNum(i);
	showRefPointsNum(i,nump);
      }
    }
  return 0;
}

int calcKinetics(double xreq, double yreq)
{
  int i,j,k;
  int ia,ja,ka;
  double end_x;
  double end_y;
  double min_end_x;
  double min_end_y;
  double max_end_x;
  double max_end_y;
  //double pdist;
  int num_found = 0;
  double x_min = getXMinVal(xreq);
  double x_max = getXMaxVal(xreq);
  double y_min = getYMinVal(yreq);
  double y_max = getYMaxVal(yreq);

  // arm angle
  // if i = x deg j = (j + i - 175) % 360 k = (k + j - 175) % 360 
  
  for (i = 0 ; i < 179; i++) {
    ia = i;
    for (j = 0 ; j < 179; j++) {
      ja = (j + ia - 175); if (ja < 0) ja += 360;
      for (k = 0 ; k < 179; k++) {
	ka = (k + ja - 175); if (ka < 0) ka += 360;

	//end_x = g_a1_x[i]*AL1 + g_a1_x[j]*AL2 + g_a1_x[k]*AL3;
	//end_y = g_a1_y[i]*AL1 + g_a1_y[j]*AL2 + g_a1_y[k]*AL3;
	//end_x = g_a1_x[ia]*AL1 + g_a1_x[ja]*AL2 + g_a1_x[ka]*AL3;
	//end_y = g_a1_y[ia]*AL1 + g_a1_y[ja]*AL2 + g_a1_y[ka]*AL3;
	end_x = g_al1_x[ia] + g_al2_x[ja] + g_al3_x[ka];
	end_y = g_al1_y[ia] + g_al2_y[ja] + g_al3_y[ka];
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
	    // xreq and yreq are the requests
	    //pdist = sqrt((xreq-end_x)*(xreq-end_x) +(yreq-end_y)*(yreq-end_y)); 
	    addReference (xreq, yreq, end_x, end_y, i , j, k);
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


int getArgVal(char * aval)
{
  int iret = atoi(aval);
  if (iret < 0) iret = 10;
  if (iret > 179) iret = 179;
  return iret;
}

int main( int argc, char * argv[])
{
  initRefs();



  //get_array( g_a1_x, g_a1_y);
  get_array( g_a1_x, g_a1_y, 1.0);
  get_array( g_al1_x, g_al1_y, AL1);
  get_array( g_al2_x, g_al2_y, AL2);
  get_array( g_al3_x, g_al3_y, AL3);

  double xreq=183.1;
  double yreq=37.0;
  if(argc == 4) {
    int imin = 20;
    int jmin = 20;
    int kmin = 20;
    if(argc >= 2) imin = getArgVal(argv[1]);
    if(argc >= 3) jmin = getArgVal(argv[2]);
    if(argc >= 4) kmin = getArgVal(argv[3]);
    getXYref_psw(&xreq, &yreq, imin, jmin , kmin);
    addReference (xreq, yreq, xreq, yreq, imin, jmin, kmin);	
    imin +=1;
    getXYref_psw(&xreq, &yreq, imin, jmin , kmin);
    addReference (xreq, yreq, xreq, yreq, imin, jmin, kmin);	
    jmin +=1;
    getXYref_psw(&xreq, &yreq, imin, jmin , kmin);
    addReference (xreq, yreq, xreq, yreq, imin, jmin, kmin);	
    kmin +=1;
    getXYref_psw(&xreq, &yreq, imin, jmin , kmin);
    addReference (xreq, yreq, xreq, yreq, imin, jmin, kmin);	

    
    getAngles(xreq-5, yreq);
    print_reqs(10, 20);
    return 0;
  }
  if(argc > 1) {
    int numr = 20;
    int nump = 20;
    int imin = 20;
    int imax = 40;
    int jmin = 20;
    int jmax = 40;
    int kmin = 20;
    int kmax = 40;
    if(argc >= 2) imin = getArgVal(argv[1]);
    if(argc >= 3) imax = getArgVal(argv[2]);
    if(argc >= 4) jmin = getArgVal(argv[3]);
    if(argc >= 5) jmax = getArgVal(argv[4]);
    if(argc >= 6) kmin = getArgVal(argv[5]);
    if(argc >= 7) kmax = getArgVal(argv[6]);



    printf(" running test_reqs\n");
    test_reqs(imin, imax, jmin, jmax, kmin, kmax);
      //test_reqs();
    print_reqs(numr, nump);

  }
  
  printf(" Point server running on port %d "
	 "(telnet 127.0.0.1 %d )  after telnet login"
	 " type help for help\n",
	 myport, myport);
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
