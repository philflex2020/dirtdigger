// a path is a collection of points or spots that we want to follow.
//
// for now we'll read them from a file
// but soon we'll listen of a socket.
// a pathitem will have a spot (x,y,z) and a speed .
// This is the time taken to get to that point

#include <iostream>
#include "spots.h"
#include "path.h"


#define MAX_ITEMS 65

int g_num_items = 0;
PItem *pitems[MAX_ITEMS];
PItem *curpi=NULL;

int addPItem( Spot s, double t)
{
  PItem *ppi;
  if (g_num_items < MAX_ITEMS) {
    ppi = new PItem(s,t);
    pitems[g_num_items] = ppi;
    if(g_num_items>0)
      pitems[g_num_items -1]->setDest(ppi);
    g_num_items++;
  }
  return 0;
}

int showPItems(void)
{
  int i;
  PItem * pi;
  for (i=0 ; i<g_num_items; i++) {
   pi = pitems[i];
    cout << " index " <<  i
	 << " spot [ "
	 << pi->s.x << " , "
	 << pi->s.y << " , "
	 << pi->s.z << " ] time [ "
	 << pi->time << "]"
      << " dest [ "
	 << pi->sp.x << " , "
	 << pi->sp.y << " , "
	 << pi->sp.z << " ] "
      	 << endl;
  }
  return 0;
}


int main()
{
  addPItem(Spot(0,1,2), 5);
  addPItem(Spot(5,1,2), 5);
  addPItem(Spot(10,1,2), 5);
  addPItem(Spot(10,5,2), 10);
  showPItems();
  return 0;
}

