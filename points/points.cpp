#include <iostream>
#include <math.h>

#include "points.h"

//c++ -o points points.cpp

/*
 * ON the surface a point is just that a point in space. with x y and z 
 * coordinates.
 * given this location we can derive distance , elevation and azimuth to a point * from zero or from another point.
 *  As an extra bonus , given a distance (c = x->y) and two arm lengths (a) and (b) 
 * longer that that distance we can plot the angles at each of the arms  
 *
 *                                    B
 *                                    *
 *                                  *    *
 *                                *        *
 *                               *           *C
 *                              *              
 *                           A *              D
 *
 * given distance A-C we want angles a1 (C-A-B) and b1 (A-B-C)
 * we also need a2 (D-A-C)
 *
 * if this was a crane arm then the elevation is angle a3 (D-A_B)
 * which is      a1 + a2 where D is at the baseline like A
 * Also for a crane arm the elbow angle is b1 (A-B-C)
 * So far so good.....
 * Now it gets interesting
 *
 * donsider the following point options
 *   point1 x,z,y  = absolute location in space
 *   point2 x,y,z  = distance moved in 1 sec for point1
 *   point3 x,y,z  = forces applied to the point1 from x,y,z cordinates
 *   point4 x,y,z  = destination path from point1 to its destination.
 *
 * So if we apply point2 to point1 we will get a movement of point1
 * If we give point1 a mass then we can derive changes in speed (point2) due to
 * forces from point3
 * the forces can come from motor or gravitational objects 
 * all this means that we can place an object point1 of mass m and then place 
 * some gravitational objects in the same space of mass m1 m2 m3 etc
 * the point will experience force from these objects and start to move.
 *
 * hmmmm
 * in the robot arm world one key feature is navigating the arm around 
 *  objects in or near its path.
 * we can attract ourselves to a destination by giving it a negative gravity
 * we can repell ourselves from objects by providing a non linear repelling 
 * force. get too close and we push away.
 * one nifty thing about points is teh fact that they can easily translate into 
 * vetcors with elevation, azimuth and distance
 * calculate that from the viewpoint of a remote object and we have a way to 
 * always point towards another object or to have another object pointing at us.
 * it gets even more interesting butthat is for later
 */


/*
 * moving in a circle or part of it
   we have a path x , y , z  and we are a distance d1 down that path.
   we hvae to avoid an object in a circular path based at a,b,c by a distance d2
 
 First we need a way to calculate our new location given the distance travelled.
 
 add dist to a point 
 Then calculate the distance from the circle center.
  get a circumference from a radius
calculate the arc length
derive the angle from arc length



 */
using namespace std;

int  test_distance(void)
{
  cout <<endl << __FUNCTION__ << endl;
  Point p1(1.0,1.0,1.0);
  Point p2(1.0,1.0,2.0);
  p1.showxyz("P1");
  p2.showxyz("P2");
  cout << "dist between points " << p1.distFrom(p2)<< endl;
  p1.showear("P1");
  return 0;
}

int test_distadd(void)
{
  cout <<endl << __FUNCTION__ << endl;
  Point p3(0.0,0.0,5.0);
  Point p4 = p3.addDist(2.5);
  p3.showxyz("P3 base");
  p4.showxyz("P3 + 2.5");

  p3.showear("P3 base");
  p4.showear("P3 + 2.5");
  return 0;
}

int test_vector(void)
{
    cout <<endl << __FUNCTION__ << endl;
    Point p1(1.0,1.0,1.0);
    Point p2(1.0,1.0,2.0);
    Point pv = p2 - p1;   // this is a vector to get from p1 to p2
    p1.showxyz("P1 base");
    p2.showxyz("P2 base");
    pv.showxyz("Pv vector");

    Point pd = pv / 10.0;
    pd.showxyz("Pd pv/10");


}

int test_elevation(void)
{
    cout <<endl << __FUNCTION__ << endl;
    Point p1(0.0,0.0,1.0);
    p1.showxyz("P1 base ");
    p1.showear("P1 ear  1 ");

    p1.elevate(45);
    p1.showxyz("P1 elevate ");
    p1.showear("P1 ear 2 ");

    cout << "p1  elevation " << p1.elevation() << endl;

}

int main(int argc, char * argv[])
{


  Point p1(1.0,1.0,1.0);
  Point p2(1.0,1.0,2.0);
  //Point p3(5.0,0.0,0.0);
  //Point p3(0.0,5.0,0.0);
  Point p3(0.0,0.0,5.0);
  Point p4 = p3.addDist(2.5);
  
  Point c1(0.0,0.0,5.0);


  
  Point pv = p2 - p1;   // this is a vector to get from p1 to p2
  cout << "p2 - p1  pv.x,y,z " << pv.x << " " << pv.y << " " << pv.z << endl;
  Point pd = pv / 10.0;
  cout << "pd / 10  pd.x,y,z " << (double) pd.x << " " << pd.y << " " << pd.z << endl;

  cout << "hello  dist between points " << p1.distFrom(p2)<< endl;
  cout << "p1.z " << p1.z << " p2.z " << p2.z<< endl;
  cout << "p1  elevation " << p1.elevation() << endl;
  cout << "p1->p2  elevation " << p1.elevation(p2) << endl;
  cout << "p1  azimuth " << p1.azimuth() << endl;
  cout << "p1->p2  azimuth " << p1.azimuth(p2) << endl;
  //cout << "p3  x,y,z " << p3.x << " "<<p3.y <<" " << p3.z << endl;
  //cout << "p4  x,y,z " << p4.x << " "<<p4.y <<" " << p4.z << endl;
  cout << " radius " << c1.radius() << " circum " << c1.circum() << endl;
  cout << " arc " << c1.arc(.05) << endl;
  Point c2;
  
  cout << "c1  x,y,z " << c1.x << " "<<c1.y <<" " << c1.z << endl;
  for (int i = 0 ; i < 180; i++){
    c2 = c1.arcP(.5 * i);
    cout << "c2 ["<< i <<" x,y,z " << c2.x << " "<<c2.y <<" " << c2.z << endl;
  }
  test_distance();
  test_distadd();
  test_vector();
  test_elevation();
  
  return 0;
}
