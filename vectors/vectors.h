#ifndef __VECTORS_H
#define __VECTORS_H

#include <cmath>
#include <math.h>
#define PI 3.14159265

class Point {
 public:
  Point(){ x=0;y=0;z=0;};
  Point(double x1, double y1, double z1){ x=x1; y=y1; z=z1;};

  //get the distance to point p
  double distFrom(Point &p)
  {
    return sqrt((x-p.x)*(x-p.x) + (y-p.y)*(y-p.y) + (z-p.z)*(z-p.z));
  }
  
  // get the elevation from zero
  double elevation(void)
  {
    return atan2(x,y) *180 /PI; 
  }
  
  // get the elevation to point p
  double elevation(Point &p)
  {
    return atan2(x-p.x,y-p.y) *180 /PI; 
  }

  // get the azimuth from zero
  double azimuth(void)
  {
    return atan2(z,y) *180 /PI; 
  }
  
  // get the azimuth to point p
  double azimuth(Point &p)
  {
    
    return atan2(z-p.z,y-p.y) *180 /PI; 
  }
  // given two arms A and B and a remote point p calculate base Angle
  double baseAngle(double a, double b, Point &p)
  {
    double e;
    double c = distFrom(p);
    double d;
    // cos(E) = (a*a + l*l -b * b)/ 2 * a * l
    d = (a*a + c*c - b*b)/ (2 * a * c);
    e = acos(d) * 180 / PI;
    return e;
  }

  // given two arms A and B and a remote point p calculate joint Angle
  double jointAngle(double a, double b, Point &p)
  {
    double e;
    double c = distFrom(p);
    double d;
    // cos(E) = (a*a + l*l -b * b)/ 2 * a * l
    d = (a*a + b*b -c* c)/ (2 * a * b);
    e = acos(d) * 180 / PI;
    return e;
  }
  
  // create a vector p from elevation , azimuth and length
  // with a unit length this can become teh speed of a point
  void setVector(Point &p, double e, double a, double l)
  {
    p.x = l * (sin(a * PI/180));
    p.y = l * (cos(a * PI/180));
    p.z = l * (sin(e * PI/180));
	       
  }
  // add one point to another
  // the second point can be the speed vector 
  void add(Point &p)
  {
    x += p.x;
    y += p.y;
    z += p.z;
  }
  
  double x;
  double y;
  double z;

};

  

#endif
