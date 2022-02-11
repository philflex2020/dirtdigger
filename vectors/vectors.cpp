#include <iostream>
#include <math.h>

#include "vectors.h"

using namespace std;

int main(int argc, char * argv[])
{
  Point p1(1.0,1.0,1.0);
  Point p2(1.0,1.0,2.0);
  
  cout << "hello  dist between points " << p1.distFrom(p2)<< endl;
  cout << "p1.z " << p1.z << " p2.z " << p2.z<< endl;
  cout << "p1  elevation " << p1.elevation() << endl;
  cout << "p1->p2  elevation " << p1.elevation(p2) << endl;
  cout << "p1  azimuth " << p1.azimuth() << endl;
  cout << "p1->p2  azimuth " << p1.azimuth(p2) << endl;
  return 0;
}
