// crazy idea but...
// produce an list of angles for a chosen x/y location
// all we need are arm lengths
// prepare a table of x/y locations and add a list of possible
// angles to get to that location
// so this is how it works.
// we'll need a server
// given the arm configuration we'll request an x/y point
// one we get a request we'll set up a xyzref for that point or a range of points close.
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


// k5 move to point blocks in space
// need to add the 'Z' component to rotate the unit
// i,j,k will be the joints and r will be the base
// i,j,k,r will be combined into a set object
// x,y,z will be a spoint object.
// we'll also reference points in x,y,z blocks
// each x will have a list of y's
// each y will have a list of 'z's'


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

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

struct kangs {
  int i,j,k,r;
};

struct krefs {
  double x;
  double y;
  double z;
};

struct xypoint {
  struct krefs kr;
  struct kangs ka;
  double x;
  double y;
  double z;
  int i,j,k,r;
  double pdist;
  struct xypoint *next;
};

struct xyzref {
  struct krefs kr;   // found
  struct krefs krr;  // requested
  struct krefs kmin;
  struct krefs kmax;
  double x;
  double y;
  double z;
  double xr;    // requested x
  double yr;    // requested y
  double zr;    // requested z
  double x_min;
  double y_min;
  double z_min;
  double x_max;
  double y_max;
  double z_max;
  
  double pdist;
  // number of points that fit within max and min
  int num_points;
  int num_points_req;
  // list of points
  struct xypoint *points;  
};



#define NUM_REF 40960
#define NUM_POINTS 40960
#define MAX_NUM_POINTS 1024

struct xyzref g_xyzr[NUM_REF];
int g_xyzrf = 0;

struct xypoint g_xyzp[NUM_POINTS];
int g_xyzrp = 0;

double g_a1_x [360];
double g_a1_y [360];
double g_a1_z [360];

double g_al1_x [360];
double g_al1_y [360];
double g_al1_z [360];

double g_al2_x [360];
double g_al2_y [360];
double g_al2_z [360];

double g_al3_x [360];
double g_al3_y [360];
double g_al3_z [360];


// Server side C/C++ program to demonstrate Socket programming



#define PORT 8080
struct xypoint;
struct xypoint *getAngles(struct krefs *kr);
struct xypoint *getLAngles(struct krefs *kr, struct kangs *ka);
double findDist(struct xypoint *xyp, struct krefs *kr);
double findLDist(struct xypoint *xyp, struct krefs *kr, struct kangs *ka);
int myport = PORT;

int getXYZref(struct krefs *kr, struct kangs *ka);

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
  //char ycref[1024] = {0};
  //char xcref[1024] = {0};
  char zcref[1024] = {0};
  char rcref[1024] = {0};


  struct krefs kr;
  struct kangs klast;
  klast.i = -1;
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
      //int i,j,k,r;
      struct kangs ka;
      //struct krefs kr;
      
      valread = read(new_socket , buffer, 1024);
      printf("[%s] %d\n",buffer, valread );
      iscan = sscanf(buffer,"%s %s %s %s %s", xcmd, xcref, ycref, zcref, rcref);
      printf("%d [%s] \n",iscan, xcmd );

      if(iscan > 0) {
	if(strcmp(xcmd, "help")==0) {
	  snprintf(buffer, sizeof(buffer), "%s",
		   " Command Help: help \n"
		   " Command XYZ xval yval zval : send XYZ point get angles\n"
		   " Command ANG i j k r: send i,j,k,r angs get XYZ\n"
		   " Command XYXList xval yval zval: get closest (TODO) \n"
		   " Command ANGList i j k r:  get closest (TODO) \n"
		   );
	  send(new_socket , buffer , strlen(buffer) , 0 );
	}
	else if(strcmp(xcmd, "XYZ")==0) {

	  kr.x = atof(xcref);
	  kr.y = atof(ycref);
	  kr.z = atof(zcref);
	  xyp = getAngles(&kr);
	  xypl = NULL;
	  
	  if(xyp) {
	    if(klast.i == -1) {
	      klast.i = xyp->i;
	      klast.j = xyp->j;
	      klast.k = xyp->k;
	      klast.r = xyp->r;
	    } else {
	      xypl = getLAngles(&kr, &klast);
	      snprintf(buffer2, sizeof(buffer2),
		       " >>>> Corrected Dist %g for loc x/y/z %g/%g/%g i %d j %d k %d r %d\n"
		       , findDist(xyp, &kr)
		       , kr.x
		       , kr.y
		       , kr.z
		       , xypl->i
		       , xypl->j
		       , xypl->k
		       , xypl->r
		       );
	      
	    }
	    snprintf(buffer, sizeof(buffer),
		  " >>>> Dist %g for loc x/y/z %g/%g/%g i %d j %d k %d r %d\n"
		     , findDist(xyp, &kr)
		     , kr.x
		     , kr.y
		     , kr.z
		     , xyp->i
		     , xyp->j
		     , xyp->k
		     , xyp->r
		     );
	  } else {
	    //        printf(
	    snprintf(buffer, sizeof(buffer),
		     " NO loc for x/y/z %g/%g/%g\n"
		     , kr.x
		     , kr.y
		     , kr.z
		     );
	  }
	  send(new_socket , buffer , strlen(buffer) , 0 );
	  if (xypl) 
	    send(new_socket , buffer2 , strlen(buffer2) , 0 );
	}
	else if(strcmp(xcmd, "ANG")==0) {
	  ka.i = atoi(xcref);
	  ka.j = atoi(ycref);
	  ka.k = atoi(zcref);
	  ka.r = atoi(rcref);

	  getXYZref(&kr, &ka);
	  snprintf(buffer, sizeof(buffer),
		   "ANG  %g/%g/%g for i %d j %d k %d r %d\n"
		   , kr.x
		   , kr.y
		   , kr.z
		   , ka.i
		   , ka.j
		   , ka.k
		   , ka.r
		   );
	  send(new_socket , buffer , strlen(buffer) , 0 );
	}
      }
  }
  return 0;
}

int initRefs(void)
{
  struct xyzref *xyzrp;

  for (int i = 0 ; i < NUM_REF; i++) {
    xyzrp = &g_xyzr[i];
    xyzrp->num_points = 0;
    xyzrp->num_points_req = 0;
    xyzrp->points = NULL;
  }
  return 0;
}

// find an xyzref in range of xr and yr
struct xyzref *findReference (struct krefs *kr)
  {
  struct xyzref *xyzrp = NULL;
  for (int i = 0 ; i < g_xyzrf; i++) {
    xyzrp = &g_xyzr[i];
    if(
       (xyzrp->x_min <= kr->x) && (xyzrp->x_max > kr->x) &&
       (xyzrp->y_min <= kr->y) && (xyzrp->y_max > kr->y) &&
       (xyzrp->z_min <= kr->z) && (xyzrp->z_max > kr->z) 
       ) {
      return xyzrp;
    }
  }
  return NULL;
}

int showRef(struct xyzref *xyzrp, int i)
{
    printf(" XRef [%d] x %g y %g z %g xmin %g xmax %g ymin %g ymax %g zmin %g zmax %g num_points %d/%d\n"
	 ,i
	 ,xyzrp->x
	 ,xyzrp->y
	 ,xyzrp->z
	 ,xyzrp->x_min
	 ,xyzrp->x_max
	 ,xyzrp->y_min
	 ,xyzrp->y_max
	 ,xyzrp->z_min
	 ,xyzrp->z_max
	 ,xyzrp->num_points
	 ,xyzrp->num_points_req
	 );
  return 0;
}

int showRefPoint(struct xypoint *xyp, int ix, int i)
{
  printf("  [%d] ===> Xpoint [%d]  x %g y %g z %g i %d j %d k %d r %d\n"
	 ,ix
	 ,i
	 ,xyp->x
	 ,xyp->y
	 ,xyp->z
	 ,xyp->i
	 ,xyp->j
	 ,xyp->k
	 ,xyp->r
	 );
  return 0;
}

int showRefPoints(struct xyzref *xyzrf, int ix, int max)
{
  int i = 0;
  struct xypoint *xyp;
  xyp = xyzrf->points;
  while ((i < max) && (i< xyzrf->num_points)) {
    showRefPoint(xyp, ix, i);
    i++;
    xyp = xyp->next;
  }
  return 0;
}

int showRefNum(int i)
{
  struct xyzref *xyzrp = &g_xyzr[i];
  return showRef(xyzrp, i);
}

int showRefPointsNum(int i, int max)
{
  struct xyzref *xyzrp = &g_xyzr[i];
  return showRefPoints(xyzrp, i, max);
}

double getMinVal(double x, double r)
{
  double xi = 0.0;

  while(xi < x) xi+=5.0;
  if(xi > 0) xi -=5.0;

  if (x >=0 ) {
    while(xi < x) xi+=r;
    if(xi > 0) xi -=r;
    
    return xi;
  } else {
    while(xi > x) xi-=r;
    return xi;
  }

}

double getXMinVal(double x)
{
  double xi = getMinVal(x, 5.0);

>>>>>>> a90bf07e205ae2dd6704df036a31550c7c469f27
  return xi;
}

double getXMaxVal(double x)
{
  x = x + 5;
  return x;
}

double getYMinVal(double x)
{
  double xi = getMinVal(x, 2.0);

  return xi;
}

double getZMinVal(double x)
{
  double xi = getMinVal(x, 2.0);
  return xi;
}

double getYMaxVal(double x)
{
  x = x + 2.0;
  return x;
}

double getZMaxVal(double x)
{
  x = x + 2.0;
  return x;
}


struct xyzref *newXyzrf(struct krefs *kr)
{

  struct xyzref *xyzrf = NULL;
  int i = g_xyzrf;
  //= (struct xyzref *)malloc(sizeof(struct xyzref));
  xyzrf = &g_xyzr[i];
  xyzrf->x = kr->x;
  xyzrf->y = kr->y;
  xyzrf->z = kr->z;
  xyzrf->x_min = getXMinVal(kr->x);
  xyzrf->x_max = getXMaxVal(xyzrf->x_min);
  xyzrf->y_min = getYMinVal(kr->y);
  xyzrf->y_max = getYMaxVal(xyzrf->y_min);
  xyzrf->z_min = getXMinVal(kr->z);
  xyzrf->z_max = getXMaxVal(xyzrf->z_min);

  if(g_xyzrf < NUM_REF ) g_xyzrf++;
  return xyzrf;
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
int addPoint (struct xyzref *xyzrf, struct krefs * kr, struct kangs *ka)
{
  struct xypoint *xyp = NULL;
  struct xypoint *xypp = xyzrf->points;
  int i;
  int ip = g_xyzrp;
  xyzrf->num_points_req++;
  if(xyzrf->num_points > MAX_NUM_POINTS) {
    return 0;
  }
  if (ip >= NUM_POINTS) {
    return 0;
  }
  //= (struct xyzref *)malloc(sizeof(struct xyzref));
  xyp = &g_xyzp[ip];
  g_xyzrp++;
  
  xyp->x = kr->x;
  xyp->y = kr->y;
  xyp->z = kr->z;
  xyp->i = ka->i;
  xyp->j = ka->j;
  xyp->k = ka->k;
  xyp->r = ka->r;
  xyp->next = NULL;

  i = xyzrf->num_points;
  if (i == 0) {
    xyzrf->points = xyp;
    xyzrf->num_points++;
    return 0;
  }
  // this appends point
  appendPoint (i, xyp, xypp);
  
  xyzrf->num_points++;
  return 0;
}


// given a requested xr, yr and a calculated x,y
// add an i,j,k point to the selected xyzref
// x,y calculated from the i,j,k angles
//    will probably be a small distance away from xr,xy
//we will want to find the closest later
// double xr, double yr, double zr)

struct xyzref *addReference(struct krefs *krr, struct krefs *kr, struct kangs *ka)
{
  struct xyzref *xyzrf;
  xyzrf = findReference (kr);
  if(!xyzrf) {
    xyzrf = newXyzrf(kr);
    xyzrf->xr = krr->x;
    xyzrf->yr = krr->y;
    xyzrf->zr = krr->z;
  }
  if(ka)addPoint(xyzrf, kr, ka);
  return xyzrf;
}

double findDist(struct xypoint *xyp, struct krefs *kr)
{
  double xd = (kr->x - xyp->x);
  double yd = (kr->y - xyp->y);
  double zd = (kr->z - xyp->z);
  
  double dist = sqrt( (xd * xd) + (yd * yd) + (zd * zd));
  return dist;
}

double findLDist(struct xypoint *xyp, struct krefs *kr, struct kangs *ka)
{
  double id = (ka->i - xyp->i);
  double jd = (ka->j - xyp->j);
  double kd = (ka->k - xyp->k);
  double rd = (ka->r - xyp->r);
  double dist = sqrt( (id * id) + (jd * jd) + (kd * kd) + (rd * rd));
  return dist;
}

int calcKinetics(struct krefs *kr);

struct xypoint *findPoint(struct xyzref *xyzrf, struct krefs *kr)
{
  int i = 0;
  double dist;
  double min_dist = -1;
  struct xypoint *xypf = NULL;
  struct xypoint *xypp = xyzrf->points;
  while (i < xyzrf->num_points) {
    dist =  findDist(xypp, kr);
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

struct xypoint *findLPoint(struct xyzref *xyzrf, struct krefs *kr
			   , struct kangs *ka)
{
  int i = 0;
  double ldist;
  double min_dist = -1;
  struct xypoint *xypf = NULL;
  struct xypoint *xypp = xyzrf->points;
  while (i < xyzrf->num_points) {
    ldist =  findLDist(xypp, kr, ka);
    if((i == 0) || ( ldist < min_dist)) {
      
      printf( " min Ldist now %g  was %g index %d dist %g i %d j %d k %d r %d\n"
	      , ldist
	      , min_dist
	      , i
	      , findDist(xypp, kr)
	      , xypp->i
	      , xypp->j
	      , xypp->k
	      , xypp->r
	      );
      
      min_dist = ldist;
      xypf = xypp;

    }
    xypp = xypp->next;
    i++;
    
  }
  return xypf;
  
}

struct xypoint *getAngles(struct krefs *kr)
{
  //int num_found;
  int i;
  struct xyzref *xyzrf;
  struct xypoint *xyp = NULL;
  xyzrf = findReference (kr);
  if(!xyzrf) {
    
    calcKinetics(kr);
    
    for (i = 0 ; i < 10; i++) {
      showRefNum(i);
      showRefPointsNum(i, 48);
    }
    xyzrf = findReference (kr);
  }
  
  
  if(xyzrf) {
    printf(" got ref for %g/%g/%g\n", kr->x, kr->y, kr->z);
    xyp = findPoint(xyzrf, kr);
    if(xyp) {
      return xyp;
    }
  } else {
    printf(" no ref for %g/%g/%g\n", kr->x, kr->y, kr->z);
  }
  return NULL;
  
}

struct xypoint *getLAngles(struct krefs *kr, struct kangs *ka)
{
  //int num_found;
  int i;
  struct xyzref *xyzrf;
  struct xypoint *xyp = NULL;
  xyzrf = findReference(kr);
  if(!xyzrf) {
    
    calcKinetics(kr);
    
    for (i = 0 ; i < 10; i++) {
      showRefNum(i);
      showRefPointsNum(i, 48);
    }
    xyzrf = findReference(kr);
  }
  
  if(xyzrf) {
    printf(" got ref for %g/%g/%g\n", kr->x, kr->y, kr->z);
    xyp = findLPoint(xyzrf, kr, ka);
		     
    if(xyp) {
      return xyp;
    }
  } else {
    
    printf(" no ref for %g/%g/%g\n", kr->x, kr->y, kr->z);
  }
  return NULL;
  
}
//int print_reqs(int numr, int nump)

int getXYZref_psw(struct krefs *kr, struct kangs *kan)
{
  double end_x;
  double end_y;
  double end_z;

  int ia, ja , ka;
  ia = kan->i;
  ja = (kan->j + ia - 179); if (ja < 0) ja += 360;
  ka = (kan->k + ja - 179); if (ka < 0) ka += 360;

  printf (" %s \n", __FUNCTION__);
  printf ("     i  %d j  %d k  %d r %d\n", kan->i ,  kan->j ,  kan->k, kan->r);
  printf ("     ia %d ja %d ka %d\n", ia , ja , ka);
  printf ("  g_al1_x/y\t%g\t%g\n", g_al1_x[ia], g_al1_y[ia]);
  printf ("  g_al2_x/y\t%g\t%g\n", g_al2_x[ja], g_al2_y[ja]);
  printf ("  g_al3_x/y\t%g\t%g\n", g_al3_x[ka], g_al3_y[ka]);

  //end_x = g_a1_x[i]*AL1 + g_a1_x[j]*AL2 + g_a1_x[k]*AL3;
  //end_y = g_a1_y[i]*AL1 + g_a1_y[j]*AL2 + g_a1_y[k]*AL3;
  end_x = g_al1_x[ia] + g_al2_x[ja] + g_al3_x[ka];
  end_y = g_al1_y[ia] + g_al2_y[ja] + g_al3_y[ka];
  end_z = end_x * sin(kan->r * pval);
  end_x = end_x * cos(kan->r * pval);
  
  printf ("  end_x/y/z\t%g\t%g\t%g \n", end_x, end_y, end_z);
  if(kr) {
    kr->x = end_x;
    kr->y = end_y;
    kr->z = end_z;
  }

  return 0;
}

int getXYZref(struct krefs *kr, struct kangs *ka)
{
  struct krefs krend;
  struct kangs kai;

  //int ia, ja , ka;
  kai.i = ka->i;
  kai.j = (ka->j + kai.i - 175); if (kai.j < 0) kai.j += 360;
  kai.k = (ka->k + kai.j - 175); if (kai.k < 0) kai.k += 360;
 
  krend.x = g_al1_x[kai.i] + g_al2_x[kai.j] + g_al3_x[kai.k];
  krend.y = g_al1_y[kai.i] + g_al2_y[kai.j] + g_al3_y[kai.k];
  krend.z = krend.x * sin(ka->r * pval);
  krend.x = krend.x * cos(ka->r * pval);

  if(kr) {
    kr->x = krend.x;
    kr->y = krend.y;
    kr->z = krend.z;
  }
  
  return 0;
}


int test_reqs(int imin, int imax,int jmin, int jmax, int kmin, int kmax, int rmin, int rmax)
{
  struct krefs kr;
  struct kangs ka;
  //int i,j,k,r;
  ka.r = 0;
  for (ka.i = imin; ka.i < imax; ka.i++) {
    for (ka.j = jmin; ka.j < jmax; ka.j++) {
      for (ka.k = kmin; ka.k < kmax; ka.k++) {
	getXYZref(&kr, &ka);
	addReference (&kr, &kr, &ka);	
      }
    }
  }
  return 0;
}

int showRef(struct xyzref *xyzrp, int i);
int showRefPoints(struct xyzref *xyzrf, int ix, int max);
int showRefNum(int i);
int showRefPointsNum(int i, int max);

int print_reqs(int numr, int nump)
{
  printf( " Num refs %d\n", g_xyzrf);
  for( int i = 0; i < numr; i++)
    {
      if (i < g_xyzrf) {
	showRefNum(i);
	showRefPointsNum(i,nump);
      }
    }
  return 0;
}

int getRangeVals(struct krefs *krmin,struct krefs *krmax,struct krefs *kr)
{
  
  krmin->x = getXMinVal(kr->x);  
  krmax->x = getXMaxVal(kr->x);
  krmin->y = getYMinVal(kr->y);
  krmax->y = getYMaxVal(kr->y);
  krmin->z = getZMinVal(kr->z);
  krmax->z = getZMaxVal(kr->z);
  return 0;
}

// handle z
int calcKinetics(struct krefs *kr)
{
  //int i,j,k,r;
  //int ia,ja,ka;
  
  struct kangs ka;
  struct kangs kai;
  struct krefs krmax;
  struct krefs krmin;
  struct krefs krmin_end;
  struct krefs krmax_end;
  struct krefs krend;
  //struct krefs krr;
  
  //double pdist;
  int num_found = 0;
  getRangeVals(&krmin, &krmax, kr);
  int rs = 0;
  int rmax = 5;
  // arm angle
  // if i = x deg j = (j + i - 175) % 360 k = (k + j - 175) % 360 
  
  for (ka.i = 0 ; ka.i < 179; ka.i++) {
    kai.i = ka.i;
    for (ka.j = 0 ; ka.j < 179; ka.j++) {
      kai.j = (ka.j + kai.i - 175); if (kai.j < 0) kai.j += 360;
      for (ka.k = 0 ; ka.k < 179; ka.k++) {
	kai.k = (ka.k + kai.j - 175); if (kai.k < 0) kai.k += 360;
	for (ka.r = rs ; ka.r < rmax; ka.r++) {
	  
	  //end_x = g_a1_x[i]*AL1 + g_a1_x[j]*AL2 + g_a1_x[k]*AL3;
	  //end_y = g_a1_y[i]*AL1 + g_a1_y[j]*AL2 + g_a1_y[k]*AL3;
	  //end_x = g_a1_x[ia]*AL1 + g_a1_x[ja]*AL2 + g_a1_x[ka]*AL3;
	  //end_y = g_a1_y[ia]*AL1 + g_a1_y[ja]*AL2 + g_a1_y[ka]*AL3;
	  krend.x = g_al1_x[kai.i] + g_al2_x[kai.j] + g_al3_x[kai.k];
	  krend.y = g_al1_y[kai.i] + g_al2_y[kai.j] + g_al3_y[kai.k];
	//addReference (end_x, end_y, i , j, k);
	  krend.z = krend.x * sin(ka.r * pval);
	  krend.x = krend.x * cos(ka.r * pval);
	  if((ka.i == 90) && (ka.j == 90) && (ka.i == 90)){
	    printf(" >>i %d j %d k %d r %d x /y /z %g/%g/%g \n"
		   , ka.i, ka.j, ka.k, ka.r
		   , krend.x, krend.y, krend.z
		   );
	    printf(" >>>>> MIN x /y /z %g/%g/%g \n"
		   , krmin.x, krmin.y, krmin.z
		   );
	    printf(" >>>>> MAX x /y /z %g/%g/%g \n"
		   , krmax.x, krmax.y, krmax.z
		   );
	  }
	  if (
	      (krend.x >krmin.x ) && (krend.x < krmax.x ) &&
	      (krend.y >krmin.y) && (krend.y < krmax.y ) &&
	      (krend.z >krmin.z) && (krend.z < krmax.z ) 
	      )
	    
	    {
	      if (num_found == 0) {
		krmin_end.x = krend.x;
		krmax_end.x = krend.x;
		krmin_end.y = krend.y;
		krmax_end.y = krend.y;
		krmin_end.z = krend.z;
		krmax_end.z = krend.z;
	    }
	    if(krend.x > krmax_end.x) krmax_end.x = krend.x;
	    if(krend.x < krmin_end.x) krmin_end.x = krend.x;	
	    
	    if(krend.y > krmax_end.y) krmax_end.y = krend.y;
	    if(krend.y < krmin_end.y) krmin_end.y = krend.y;

	    if(krend.z > krmax_end.z) krmax_end.z = krend.z;
	    if(krend.z < krmin_end.z) krmin_end.z = krend.z;

	    printf(" match at %d %d %d %d xval %g yval %g zval %g\n"
		   , ka.i, ka.j, ka.k, ka.r
		   , krend.x
		   , krend.y
		   , krend.z
		   );
	    // xreq and yreq are the requests
	    //pdist = sqrt((xreq-end_x)*(xreq-end_x) +(yreq-end_y)*(yreq-end_y)); 
	    addReference (kr, &krend, &ka);
	    num_found++;
	  }
	//end_y[i + (j * 90) + (k * 90 * 90)] = a1_y[i] + a2_y[j] + a3_y[k];
	
	}
      }
    }
  }
  printf(" at end num_found %d xmin/max %g/%g ymin/max %g/%g zmin/max %g/%g\n"
	 "refs %d points %d\n"
	 , num_found
	 , krmin_end.x
	 , krmax_end.x
	 , krmin_end.y
	 , krmax_end.y
	 , krmin_end.z
	 , krmax_end.z
	 , g_xyzrf
	 , g_xyzrp
	 
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

double getArgRVal(char * aval)
{
  double  iret = atof(aval);
  return iret;
}

int main( int argc, char * argv[])
{
  char *xcmd;
  char buffer[1024];
  struct xyzref *xyzrf;

  //AFOO afoo;
  //afoo.a1 = 1;
  //printf(" AFOO a1 %d\n", afoo.a1);
  //return 0;
  initRefs();
  //get_array( g_a1_x, g_a1_y);
  get_array( g_a1_x, g_a1_y, 1.0);
  get_array( g_al1_x, g_al1_y, AL1);
  get_array( g_al2_x, g_al2_y, AL2);
  get_array( g_al3_x, g_al3_y, AL3);

  struct krefs kr;
  kr.x=183.1;
  kr.y=37.0;
  kr.z=7.0;
  if (argc> 1) {
    xcmd = argv[1];
    if(strcmp(xcmd, "help")==0) {
      snprintf(buffer, sizeof(buffer), "%s",
	       " Command Help: help \n"
	       " Command XYZ xval yval zval : set XYZ point \n"
	       " Command XYZa xval yval zval : send XYZ point get angles\n"
	       " Command ANG i j k r: send i,j,k,r angs get XYZ\n"
	       " Command XYXList xval yval zval: get closest (TODO) \n"
	       " Command ANGList i j k r:  get closest (TODO) \n"
	       );
      printf("%s", buffer); return 0;
    } else if(strcmp(xcmd, "XYZ")==0) {
      snprintf(buffer, sizeof(buffer), "%s",
	       " get angles from XYZ point, NOt done yet\n"
	       );
      kr.x = 55.0;
      kr.y = 15.0;
      kr.z = 1.0;
      if(argc >= 2) kr.x = getArgRVal(argv[1]);
      if(argc >= 3) kr.y = getArgRVal(argv[2]);
      if(argc >= 4) kr.z = getArgRVal(argv[3]);
      xyzrf = addReference (&kr, &kr, NULL);
      kr.y += 15.0;
      xyzrf = addReference (&kr, &kr, NULL);
      kr.x += 15.0;
      xyzrf = addReference (&kr, &kr, NULL);
      print_reqs(10, 20);
      //printf("%s", buffer); return 0;
    } else if(strcmp(xcmd, "XYZa")==0) {

      snprintf(buffer, sizeof(buffer), "%s",
	       " get angles from XYZ point, NOt done yet\n"
	       );
      kr.x = 155.0;
      kr.y = 15.0;
      kr.z = 1.0;
      if(argc >= 2) kr.x = getArgRVal(argv[2]);
      if(argc >= 3) kr.y = getArgRVal(argv[3]);
      if(argc >= 4) kr.z = getArgRVal(argv[4]);
      calcKinetics(&kr);

      //xyzrf = addReference (&kr, &kr, NULL);
      //kr.y += 15.0;
      //xyzrf = addReference (&kr, &kr, NULL);
      //kr.x += 15.0;
      //xyzrf = addReference (&kr, &kr, NULL);
      print_reqs(10, 20);
      printf(" >>> input vals x/y/z %g/%g/%g\n"
	     , kr.x  , kr.y  , kr.z);
  
      //printf("%s", buffer); return 0;
      
    } else if(strcmp(xcmd, "ANG")==0) {
      snprintf(buffer, sizeof(buffer), "%s",
	       " get  XYZ point from angles, NOt done yet\n"
	       );
      struct kangs ka;
      ka.i = 20;
      ka.j = 20;
      ka.k = 20;
      ka.r = 10;
      if(argc >= 2) ka.i = getArgVal(argv[1]);
      if(argc >= 3) ka.j = getArgVal(argv[2]);
      if(argc >= 4) ka.k = getArgVal(argv[3]);
      if(argc >= 5) ka.r = getArgVal(argv[4]);
      getXYZref_psw(&kr, &ka);
      xyzrf = addReference (&kr, &kr, &ka);
      xyzrf->x_min = getXMinVal(kr.x);
      xyzrf->x_max = getXMaxVal(xyzrf->x_min);
      xyzrf->y_min = getYMinVal(kr.y);
      xyzrf->y_max = getYMaxVal(xyzrf->y_min);
      xyzrf->z_min = getXMinVal(kr.z);
      xyzrf->z_max = getXMaxVal(xyzrf->z_min);



      ka.i +=1;
      getXYZref_psw(&kr, &ka);
      addReference (&kr, &kr, &ka);	
      ka.j +=1;
      getXYZref_psw(&kr, &ka);
      addReference (&kr, &kr, &ka);
      
      ka.k +=1;
      getXYZref_psw(&kr, &ka);
      addReference (&kr, &kr, &ka);
      
      //kr.x =- 5;
      //getAngles(&kr);
      print_reqs(10, 20);
      
      //printf("%s", buffer);
      return 0;
      
    } else if(strcmp(xcmd, "XYZList")==0) {
      snprintf(buffer, sizeof(buffer), "%s",
	       " get List of XYZ points from Angles , NOt done yet\n"
	       );
      printf("%s", buffer); return 0;

    } else if(strcmp(xcmd, "ANGList")==0) {
      snprintf(buffer, sizeof(buffer), "%s",
	       " get ANG List , NOt done yet\n"
	       );
      printf("%s", buffer); return 0;
      
    }
  }
  return 0;
  
  if(argc == 5) {
    struct kangs ka;
    ka.i = 20;
    ka.j = 20;
    ka.k = 20;
    ka.r = 0;
    if(argc >= 2) ka.i = getArgVal(argv[1]);
    if(argc >= 3) ka.j = getArgVal(argv[2]);
    if(argc >= 4) ka.k = getArgVal(argv[3]);
    if(argc >= 5) ka.r = getArgVal(argv[4]);
    getXYZref_psw(&kr, &ka);
    addReference (&kr, &kr, &ka);	
    ka.i +=1;
    getXYZref_psw(&kr, &ka);
    addReference (&kr, &kr, &ka);	
    ka.j +=1;
    getXYZref_psw(&kr, &ka);
    addReference (&kr, &kr, &ka);

    ka.k +=1;
    getXYZref_psw(&kr, &ka);
    addReference (&kr, &kr, &ka);

    kr.x =- 5;
    getAngles(&kr);
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
    int rmin = 20;
    int rmax = 40;
    if(argc >= 2) imin = getArgVal(argv[1]);
    if(argc >= 3) imax = getArgVal(argv[2]);
    if(argc >= 4) jmin = getArgVal(argv[3]);
    if(argc >= 5) jmax = getArgVal(argv[4]);
    if(argc >= 6) kmin = getArgVal(argv[5]);
    if(argc >= 7) kmax = getArgVal(argv[6]);
    if(argc >= 8) rmin = getArgVal(argv[7]);
    if(argc >= 9) rmax = getArgVal(argv[8]);

    printf(" running test_reqs\n");
    test_reqs(imin, imax, jmin, jmax, kmin, kmax, rmin, rmax);
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

  struct xypoint *xyp = getAngles(&kr);
  
  if(xyp) {
    printf(" >>>>>Dist %g for loc x/y/z %g/%g/%g i %d j %d k %d r %d\n"
	   , findDist(xyp, &kr)
	   , kr.x
	   , kr.y
	   , kr.z
	   , xyp->i
	   , xyp->j
	   , xyp->k
	   , xyp->r
	   );
    
  } else {
        printf(" NO loc for x/t/z %g/%g/%g\n"
	   , kr.x
	   , kr.y
	   , kr.z
	   );
  }    
  //struct krefs kr;
  kr.x=173.1;
  kr.y=39.0;
  kr.z=5.0;

  
  xyp = getAngles(&kr);
  
  if(xyp) {
    printf(" >>>> Dist %g for loc x/y/z %g/%g/%g i %d j %d k %d r %d\n"
	   , findDist(xyp, &kr)
	   , kr.x
	   , kr.y
	   , kr.z
	   , xyp->i
	   , xyp->j
	   , xyp->k
	   , xyp->r
	   );
    
  } else {
        printf(" NO loc for x/y/z %g/%g/%g\n"
	   , kr.x
	   , kr.y
	   , kr.z
	   );
  }
  
  return 0;
}
