#ifndef __VECS_H
#define __VECS_H


// A vector is something that can happen between two spots
// we basically define distance elevation and azimuth between two spots
// 0 deg azimuth heads off into the -z coordinage
//

#include <cmath>
#include <math.h>
#include "spots.h"

//#define PI 3.14159265
using namespace std;

class Vec {
 public:
  Vec(){ e=0.0;a=0.0;d=0.0;};
  Vec(double e1, double a1, double d1) {
    e = e1;
    a = a1;
    d = d1;
  };

  void getvec(Spot s1, Spot s2)
  {
    d = s1.distTo(s2);
    e = elevation(s1, s2);
    a = azimuth(s1, s2);
    
  }

  void toVec(Spot s)
  {
    Spot s0(0,0,0);
    getvec(s0, s);
    return;
  }

  double elevation(Spot s1, Spot s2)
  {
    Spot ds = s2 - s1;
    return elevation(ds);
  }
  
  double elevation(Spot ds)
  {
    double dp = sqrt((ds.x *ds.x) * (ds.z * ds.z));
    double ev = atan2(ds.y,dp) *180 /g_pi;
    if (ev < 0.00001) ev = 0.0;
    return ev;
  }
  
  double azimuth(Spot s1, Spot s2)
  {
    Spot ds = s2 - s1;
    return azimuth(ds);
  }
    
  double azimuth(Spot ds)
  {
    return atan2(ds.x,ds.z) *180 /g_pi;
  }
  // add one vec to another
  // the second point can be the speed vector 
  void add(Vec &s)
  {
    e += s.e;
    a += s.a;
    d += s.d;
  }

  Spot toSpot(void)
  {
    Spot s;
    double xd = d * cos(degtorad(e));
    s.x = xd *sin (degtorad(a));
    s.z = xd *cos (degtorad(a));
    s.y = d *sin (degtorad(e));
    return s;
  }

  Vec operator - (Vec s)
  {
    double e1 = e - s.e;
    double a1 = a - s.a;
    double d1 = d - s.d;
    return (Vec(e1,a1,d1));
  }

  Vec operator + (Vec s)
  {
    double e1 = e + s.e;
    double a1 = a + s.a;
    double d1 = d + s.d;
    return (Vec(e1,a1,d1));
  }

  Vec operator / (double dx)
  {
    double e1 = e/dx;
    double a1 = a/dx;
    double d1 = d/dx;
    return (Vec(e1,a1,d1));
  }

  Vec operator * (double dx)
  {
    double e1 = e*dx;
    double a1 = a*dx;
    double d1 = d*dx;
    return (Vec(e1,a1,d1));
  }

  void show(string who)
  {
    if (who.length() > 0)
        cout << who << " e,a,d " << e << " "<<a <<" " << d << endl;
    else
        cout << " Vec  e,a,d " << e << " "<<e <<" " << d << endl;
  }
  
  double e;
  double a;
  double d;
};

  

#endif  // _VECS_H
