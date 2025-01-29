#ifndef TEXT_WIDGET
#define TEXT_WIDGET
#include <Windows.h>
typedef struct Line
{
  wchar_t * ptrStr;          // puntero a una cadena
  wchar_t * ptrEscape;      //  puntero a una secuencias de escape
  POINT pxy;                     //   posicion de la cadena
  unsigned int len;             //    longitud de la cadena
  unsigned char isEnableEsc;   //     (1=Active ; 0= desacativado)
  struct Line * next;         //      siguiente nodo
}Line;
typedef struct
{
  Line ** arrLine; // array de linias
  unsigned int countLine;
  
}ListLine;
int add_end_nodo(Line **head,Line *line);
void add_line(Line * line, const char * str, unsigned int len);
int add_line2(Line ** head, const wchar_t *str);
int add_charToline(ListLine * list,unsigned char c);
int add_char_index_Toline(ListLine * list, unsigned char c, int row);
int free_list(ListLine * list);
#endif
