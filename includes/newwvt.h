#ifndef NEWWVT_T
#define NEWWVT_T
#include <wchar.h>
#include "..\..\Vt\includes\liked_list.h"

typedef struct
{
  POINT saveCursor;
  POINT cursor;
  wchar_t *glifo;
  char flag;
  Nodo *inputBuffer;
}Termv; // Terminal virtual

#define  saveCursor Emu.saveCursor
#define  cursor     Emu.cursor
#define  glifo      Emu.glifo
#define  flag       Emu.flag
#define  input      Emu.inputBuffer

Nodo * getInput();
wchar_t * getGlifo();
Nodo ** getRootInput();
void appendBuffer(const wchar_t *s, int len);
void freeBuffer();
void printBuffer();

#endif
