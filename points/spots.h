#ifndef __SPOTS_H
#define __SPOTS_H

#include <cmath>
#include <math.h>
#include <iostream>
#include <string>

#define PIc 3.14159265
using namespace std;
double g_pi = PIc;
//x/180 = r/PI;
static inline double degtorad( double deg)
{
  return deg *g_pi / 180;
}

static inline double radtodeg(double rad)
{
  return rad *180 / g_pi;
}

class Spot {
 public:
  Spot(){ x=0.0;y=0.0;z=0.0;};
  Spot(double x1, double y1, double z1) {
    x = x1;
    y = y1;
    z = z1;
  };

  //get the distance to Spot s
  double distTo(Spot &s)
  {
    return sqrt((s.x-x)*(s.x-x) + (s.y-y)*(s.y-y) + (s.z-z)*(s.z-z));
  }
  
  // add one point to another
  // the second point can be the speed vector 
  void add(Spot &s)
  {
    x += s.x;
    y += s.y;
    z += s.z;
  }

  Spot operator - (Spot s)
  {
    double x1 = x - s.x;
    double y1 = y - s.y;
    double z1 = z - s.z;
    return (Spot(x1,y1,z1));
  }

  Spot operator + (Spot s)
  {
    double x1 = x + s.x;
    double y1 = y + s.y;
    double z1 = z + s.z;
    return (Spot(x1,y1,z1));
  }

  Spot operator / (double d)
  {
    double x1 = x/d;
    double y1 = y/d;
    double z1 = z/d;
    return (Spot(x1,y1,z1));
  }

  Spot operator * (double d)
  {
    double x1 = x*d;
    double y1 = y*d;
    double z1 = z*d;
    return (Spot(x1,y1,z1));
  }

  void show(string who)
  {
    if (who.length() > 0)
        cout << who << " x,y,z " << x << " "<<y <<" " << z << endl;
    else
        cout << " Point  x,y,z " << x << " "<<y <<" " << z << endl;
  }
  void show(void)
  {
    cout << x << " "<<y <<" " << z << endl;
  }
  
  double x;
  double y;
  double z;
};

  

#endif  // _POINTS_H
