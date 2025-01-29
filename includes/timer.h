#ifndef TIMER_H
#define TIMER_H
#include <windows.h>
typedef struct
{
  LARGE_INTEGER frequency;
  LARGE_INTEGER startTime;
  LARGE_INTEGER endTime;
}Clok;

double inic_clock(Clok * clok);
double stop_clock(Clok *clok);



#endif
