#ifndef __POINTS_H
#define __POINTS_H

#include <cmath>
#include <math.h>
#define PI 3.14159265
using namespace std;

class Point {
 public:
  Point(){ x=0;y=0;z=0;};
  Point(double x1, double y1, double z1) {
    x = x1;
    y = y1;
    z = z1;
    e = elevation();
    a = azimuth();
    r = radius();
  };

  //get the distance to point p
  double distFrom(Point &p)
  {
    return sqrt((x-p.x)*(x-p.x) + (y-p.y)*(y-p.y) + (z-p.z)*(z-p.z));
  }

  void elevate(double deg)
  {
    e += deg;
    while (e >= 360.0) e -= 360.0;
    while (e <= -360.0) e += 360.0;
    double er = e * PI /180.0;
    double ar = (a+90.0) * PI /180.0;
    x = r * cos(er) * cos (ar);
    y = r * sin(er) * cos (ar);
    z = r * cos(er);
  }
  // return a new point after moving a certain distance 
  Point addDist(double dist)
  {

    //cout << " Elevation " << e << endl;
    //cout << " Azimuth " << a << endl;
    double er = e * PI /180.0;
    double ar = a * PI /180.0;
    
    double x1 = (r+dist) * cos(er) * cos (ar);
    double y1 = (r+dist) * sin(er) * cos (ar);
    double z1 = (r+dist) * cos(er);
    return Point(x1,y1,z1);
    
  }
  // get the elevation in degrees from zero
  double elevation(void)
  {
    return atan2(y,x) *180 /PI; 
  }
  
  // get the elevation to point p
  double elevation(Point &p)
  {
    return atan2(y-p.y,x-p.x) *180 /PI; 
  }
  double radius()
  {
     return sqrt((x)*(x) + (y)*(y) + (z)*(z));
  }

  double circum()
  {
    return radius() * 2.0 * PI;
  }
  double arc(double l)
  {
    return l * 2.0 * PI / circum(); 
  }
  
  Point arcP(double l)
  {
      Point p(0,0,0);
      double e = elevation();
      cout << " Elevation " << e << endl;
      double a = azimuth();
      cout << " Azimuth " << a << endl;
      a = a + arc(l);
      setVector(p, e, a, radius());
      return p;
  }
  
  // get the azimuth from zero
  double azimuth(void)
  {
    return (atan2(z,y) *180 /PI) - 90.0; 
  }
  
  // get the azimuth to point p
  double azimuth(Point &p)
  {
    return (atan2(z-p.z,y-p.y) *180 /PI) - 90.0; 
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
    double xs = l * (sin(e * PI/180));
    double xc = l * (cos(e * PI/180));
    p.x = xc * (cos(a * PI/180));
    p.z = xc * (sin(a * PI/180));
    p.y = l * (sin(e * PI/180));
	       
  }
  
  // add one point to another
  // the second point can be the speed vector 
  void add(Point &p)
  {
    x += p.x;
    y += p.y;
    z += p.z;
  }

  Point operator - (Point p)
  {
    double x1 = x - p.x;
    double y1 = y - p.y;
    double z1 = z - p.z;
    return (Point(x1,y1,z1));
  }

  Point operator + (Point p)
  {
    double x1 = x + p.x;
    double y1 = y + p.y;
    double z1 = z + p.z;
    return (Point(x1,y1,z1));
  }

  Point operator / (double d)
  {
    double x1 = x/d;
    double y1 = y/d;
    double z1 = z/d;
    return (Point(x1,y1,z1));
  }

  Point operator * (double d)
  {
    double x1 = x*d;
    double y1 = y*d;
    double z1 = z*d;
    return (Point(x1,y1,z1));
  }

  void showxyz(string who)
  {
    if (who.length() > 0)
        cout << who << " x,y,z " << x << " "<<y <<" " << z << endl;
    else
        cout << " Point  x,y,z " << x << " "<<y <<" " << z << endl;
  }
  
  void showear(string who)
  {
    if (who.length() > 0)
        cout << who << " e,a,r " << e << " "<<a <<" " << r << endl;
    else
        cout << " Point  e,a,r " << e << " "<<a <<" " << r << endl;
  }
  
  double e;    // elevation from zero plane
  double a;    // azimuth from zero plane
  double r;    // range
  double x;
  double y;
  double z;

};

  

#endif  // _POINTS_H
