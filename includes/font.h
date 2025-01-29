#ifndef FONT_H
#define FONT_H
#include <windows.h>
HFONT create_font(HDC *hdc, int fontHeight);
void draw_font(HDC *hdc, const wchar_t * text, int x, int y);







#endif
