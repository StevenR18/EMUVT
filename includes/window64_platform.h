#ifndef WINDOW_64
#define WINDOW_64
#include <Windows.h>
#include <stdbool.h>
typedef struct
{
  BITMAPINFO Info;
  void *Pixels;
  int Width;         // ancho del bitmap  en pixeles
  int Height;        // alto del bitmpa   en pixeles
  int Pitch; // sancada
  HDC BackBuffer;
  HBITMAP HoldBitmap;
  HBITMAP Hbitmap;
}Win32ScreenBuffer;
typedef struct
{
  int Width;
  int Height;
}Win32WindowDimension;

#endif


