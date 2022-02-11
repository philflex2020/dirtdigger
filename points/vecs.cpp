#include <iostream>
#include <math.h>

#include "spots.h"
#include "vecs.h"

//c++ -o vecs vecs.cpp

/*
 * a Vec is the representation of the distance, azimuth and elevation 
 *   between two spots 
 *
 */

using namespace std;

int  test_distance(void)
{
  cout <<endl << __FUNCTION__ << endl;
  Spot s1(1.0,1.0,1.0);
  Spot s2(1.0,1.0,2.0);
  s1.show("S1");
  s2.show("S2");
  cout << "dist from s1 to s2 points " << s1.distTo(s2)<< endl;
  return 0;
}

int test_distadd(void)
{
  cout <<endl << __FUNCTION__ << endl;
  Spot s1(0.0,0.0,5.0);
  Spot s2(0.5,0.5,0.0);
  Spot s3 = s1 + s2;
  s1.show("S1 ");
  s2.show("S2 ");
  s3.show("S1 + S2 ");
  s3 = s1 + (s2*2);
  s3.show("S1 + (S2*2) ");

  return 0;
}

int test_azi_ele(void)
{
  cout <<endl << __FUNCTION__ << endl;
  Spot s1(0.0,0.0,0.0);
  Spot s2(1.0,1.0,-1.0);
  Spot sd =  s2 - s1;
  Vec v;
  double az = v.azimuth(sd);
  double ez = v.elevation(sd);
  
  sd.show("Sd  ");
  cout << "dist from s1 to s2 " << s1.distTo(s2)<< endl;
  cout << "elevation of s2 to s1 " << ez << endl;
  cout << "azimuth of s2 to s1 " << az << endl;

  return 0;
  
}

int test_toSpot(void)
{
  Vec v(30.0,45.0,2.0);
  Spot s = v.toSpot();
  cout << " Elevation deg "<< v.e << endl;
  cout << " Elevation rad "<< degtorad(v.e) << endl;
  cout << " Elevation sin "<< sin(degtorad(v.e)) << endl;
  cout << " Elevation cos "<< cos(degtorad(v.e)) << endl;
  double y = v.d * sin(degtorad(v.e));
  double xd = v.d * cos(degtorad(v.e));
  double x = xd * sin(degtorad(v.a));
  double z = -xd * cos(degtorad(v.a));
  cout << " Spot y "<< y << endl;
  cout << " Spot xd "<< xd << endl;
  cout << " Vec  d^2 "<< v.d * v.d << endl;
  cout << " Spot xd^2 "<< xd * xd << endl;
  cout << " Spot y^2 "<< y * y << endl;
  cout << " Spot y^2 + xd^2 " << (y * y) + (xd * xd) << endl;

  v.show(" vec ");
  s.show(" vec as spot");
  Vec v2;
  v2.toVec(s);
  v2.show(" vec2 ");

}

int main(int argc, char * argv[])
{
  test_distance();
  test_distadd();
  test_azi_ele();
  test_toSpot();
  return 0;
}
