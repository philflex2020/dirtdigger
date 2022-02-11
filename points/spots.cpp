#include <iostream>
#include <math.h>
#include <unistd.h>


#include "spots.h"

//c++ -o spots spots.cpp

/*
 * ON the surface a spot is just that a point in space. with x y and z 
 * coordinates.
 * given this location we can derive distance 
 *
 * donsider the following point options
 *   spot1 x,z,y  = absolute location in space
 *   spot2 x,y,z  = distance moved in 1 sec for point1
 *   spot3 x,y,z  = forces applied to the spot1 from x,y,z cordinates
 *   spot4 x,y,z  = destination path from spot1 to its destination.
 *
 * So if we apply spot2 to spot1 we will get a movement of spot1
 * If we give spot1 a mass then we can derive changes in speed (spot2) due to
 * forces from spot3
 * the forces can come from motor or gravitational objects 
 * all this means that we can place an object spot1 of mass m and then place 
 * some gravitational objects in the same space of mass m1 m2 m3 etc
 * the point will experience force from these objects and start to move.
 *  The example uses a fixed deacceleration.
 * what about circles
 * to make a spot move abount in a circle simply change the speed spot 
 * each cycle to move over the next arc
 * lets say we want to move the speed spot2 buy T degrees each step  
 * we will go in a -Z direction
 * Zstep  Z0*cos(n * Tr)  where n is the number of steps
 * Xstep  X0*sin(n * Tr)  where n is the number of steps
 *  Tr = Td *PI / 180
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

int test_accel(void)
{
  cout <<endl << __FUNCTION__ << endl;
  Spot s1(0.0,1.0,0.0);   // initial position
  Spot s2(0.5,0.5,0.0);   // initial speed
  Spot s3(0.0,-0.1,0.0);   // initial acceleration

  while (s1.y > 0.0) {
    s1 = s1 + s2;
    s2 = s2 + s3;
    s1.show ( "new pos ");
    sleep (1);
  }
  return 0;
}

int test_circle(void)
{
  cout <<endl << __FUNCTION__ << endl;
  Spot s1(5.0,1.0,0.0);   // initial position
  Spot s20(0.0,0.0,-1.0);   // initial speed
  double Tdeg = 5;
  double Tr = Tdeg *g_pi/180.0;
  int n = 1;
  for(n = 1 ; n < 45; n++)
    {
      double Tr = n *Tdeg *g_pi/180.0;

      double Zstep = s20.z*cos(n * Tr);  //where n is the number of steps
      double Xstep = s20.z*sin(n * Tr);  //where n is the number of steps
      double Ystep = 0.0;
      Spot step(Xstep, Ystep, Zstep);
      s1 = s1 + step;
      //cout << " Step num "<< n;
      //step.show ( " step ");
      s1.show ();
    }
  
  return 0;
}


int main(int argc, char * argv[])
{
  test_distance();
  test_distadd();
  test_accel();
  test_circle();
  return 0;
}
