#ifndef __PATH_H
#define __PATH_H
#include "spots.h"


class PItem {

 public:
  PItem(Spot s1, double t1) {
    s=s1;
    time = t1;
  };
  void setDest(PItem *d) {
    sp = d->s - s;
  }
  Spot s;
  Spot sp;
  double time;
};

#endif
