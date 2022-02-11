#ifndef __MOVE_H
#define __MOVE_H

// move controller
// we have a list of movers
// each one is given one or more moves
struct Move {
  Move();
  ~Move();

  Move(float mnow, float mend, float mtime);
  float Step(float mtime);
  int Done();

  int action;
  float start;
  float end;
  float time;
  float inc;
  float current;
  // if set up and the action is wait we'll wait for this key/value
  int *key;
  int value;
  
};

#define MAX_MOVES 16
#define MAX_MOVERS 16


struct Mover {
  Mover(int mid);
  ~Mover();
  int addMove(Move * mv);
  float Step(float mtime);
  
  int id;
  int cnum;
  int num;
  struct Move *moves[MAX_MOVES];

};

struct Movers {
  Movers();
  ~Movers();
  int num;
  float Step(float mtime);
  int addMover(Mover *mv);
  struct Mover *movers[MAX_MOVERS];
};

#endif
  
