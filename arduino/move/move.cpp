// move management

//#include <iostream>
//#include <stdio.h>
//#include <stddef.h>
#include "move.h"

using namespace std;

Move::Move(void)
{
  action = 0;
  start = 0.0;
  end = 1.0;
  time = 5.0; // seconds
  inc = (end - start) / time;;
  current=start;
  // if set up and the action is wait we'll wait for this key/value
  key = (int *)0;
  value = 0;
}


Move::Move(float mnow, float mend, float mtime)
{
  current = mnow;
  start = current;
  end = mend;
  time = mtime;
  inc = (end - start) / time;;
  current=start;
}

// step mu mtime seconds
float Move::Step(float mtime)
{
  float save = current;
  current += inc*mtime;
  if(end > start) {
      if (current >= end) current = end;
  } else {
    if (current <= end) current = end;
  }
  return current - save;
}

Move::~Move(void)
{
}

int Move::Done(void)
{
  if(end > start) {
    if(current >= end) return 1;
  } else {
    if(current <= end) return 1;
  }
  return 0;
}


Mover::Mover(int mid)
{
  id = mid;
  cnum = -1;
  num = 0;
  for (int i = 0 ; i < MAX_MOVES; i++) {
    moves[i] = (Move *)0;
  }
}

Mover::~Mover()
{
  for (int i = 0 ; i < num; i++) {
    delete moves[i];
  }
}

float Mover::Step(float mtime)
{
  if (cnum >= 0) {
    moves[cnum]->Step(mtime);
    if(moves[cnum]->Done()) {
      //cout<< " Next move "<<cnum << " Num " << num;
      //printf(" moves[cnum] %p \n", moves[cnum]);
      if (cnum < num-1) cnum++;
    }
  }
  
  return 0.0;
};

int Mover::addMove(Move *mv)
{
  int rc = 0;
  if (num < MAX_MOVES) {
    moves[num] = mv;
    num++;
    rc++;
    if(cnum < 0) cnum = 0;
  }
  
  return rc;
};


Movers::Movers()
{
  num = 0;
  for (int i = 0 ; i < MAX_MOVERS; i++) {
    movers[i] = (Mover *)0;
  }
}

Movers::~Movers()
{
  for (int i = 0 ; i < num; i++) {
    delete movers[i];
  }
};

float Movers::Step(float mtime)
{
  for (int i = 0 ; i < num; i++) {
    movers[i]->Step(mtime);
  }
  return 0.0;
};

int Movers::addMover(Mover *mv)
{
  int rc = 0;
  if (num < MAX_MOVERS) {
    movers[num] = mv;
    num++;
    rc++;
  }
  return rc;
};

int main_test(int argc, char * argv[])
{

  Movers *mvrs = new Movers();
  Mover *mvr1 = new Mover(101);
  Mover *mvr2 = new Mover(202);
  
  Move *m1 = new Move(1.0, 5.0, 2.5);
  Move *m2 = new Move(1.0, 10.0, 2.0);
  Move *m3 = new Move(10.0, 3.0, 0.5);
  mvr1->addMove(m1);
  mvr2->addMove(m2);
  mvr2->addMove(m3);
  mvrs->addMover(mvr1);
  mvrs->addMover(mvr2);
  
  //cout << " Sizes move  " <<  sizeof (Move);
  //cout << " mover  " <<  sizeof (Mover);
  //cout << " movers  " <<  sizeof (Movers) << endl;
  //cout << " Hello Move curr " << m1->current<< endl;
  //cout << " Hello Move end " << m1->end<< endl;
  //cout << " Hello Move inc " << m1->inc<< endl;
  while (!m1->Done()) {
    mvrs->Step(0.1);
    //mvr2->Step(0.1);
    //cout<< " Move pos 1 >>" << m1->current
    //	<< " 2 >>" << m2->current
    //	<< " 3 >>" << m3->current
    //	<< endl;
  }
  
  delete mvrs;
  //delete mvr2;
  

  return 0;
}
