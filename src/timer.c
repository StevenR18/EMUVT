#include <windows.h>
#include <stdio.h>
#include "..\..\Vt\includes\timer.h"
double inic_clock(Clok * clok)
{
  LARGE_INTEGER *hz = &clok->frequency;
  LARGE_INTEGER *startT = &clok->startTime;
  if( hz->QuadPart == 0)
    {
    QueryPerformanceFrequency(hz); // obtines la frecuecia del contador
    }
  
    QueryPerformanceCounter(startT); 

    double deltaT=(double)(startT->QuadPart)/(double)(hz->QuadPart); // devuelve el tiempo en segundo
    return deltaT;
}


double stop_clock(Clok *clok)
{
  QueryPerformanceCounter(&clok->endTime); // la frecuencias del contador constante
  LONGLONG  endT=clok->endTime.QuadPart;
  LONGLONG  startT=clok->startTime.QuadPart;
  LONGLONG  hz=clok->frequency.QuadPart;
  double deltaT= (double)(endT - startT)/(double)(hz); // devuelve el tiempo en segundos
  return deltaT;
}

