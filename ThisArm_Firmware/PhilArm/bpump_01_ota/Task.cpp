
#include <Arduino.h>
#include "Task.h"

Task *setupTask(int count ,void (*myfcn) (void *, void *))
{
  Task  * t = new Task;
  t->count = count;
  t->fcn = myfcn;
  return t;
}
