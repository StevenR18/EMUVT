
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h> 
#include "..\Vt\includes\text_widget.h"

/* Funciones para manenejar el buffer de la consola*/

int vt_len_char_printable(const wchar_t *ptrChar)
{
  int len=0; 
  while((isprint(*ptrChar)||(*ptrChar >= 0x2500) && (*ptrChar <= 0x257F)))
    {
      len++;
      ptrChar++;
    }
  return len;
}

Line *vt_create_newLine(const wchar_t **str)
{
    Line *newLine = (Line *)malloc(sizeof(Line));
    if (newLine == NULL) return NULL;
    size_t sizeStr = (str[0] == NULL) ? 0 : (wcslen(str[0]) + 1); 
    size_t sizeEsc = (str[1] == NULL) ? 0 : (wcslen(str[1]) + 1);
    if (sizeStr > 0)
      {
	newLine->ptrStr = (unsigned char *)malloc(sizeof(wchar_t) * sizeStr);
	if (newLine->ptrStr == NULL) return NULL;
	wcscpy_s((wchar_t *)newLine->ptrStr, sizeStr, str[0]);
        newLine->len = (int)(sizeStr - 1);
      }
    else
      {
	newLine->ptrStr = NULL;
      }
    if (sizeEsc > 0)
      {
	newLine->ptrEscape = (unsigned char *)malloc(sizeof(wchar_t) * sizeEsc);
	if (newLine->ptrEscape == NULL) return NULL;
	wcscpy_s((wchar_t *)newLine->ptrEscape, sizeEsc, str[1]);
      }
    else
      {
	newLine->ptrEscape = NULL;
      }
    newLine->isEnableEsc = 0; // Inicializamos el flag para secuencias de escape
    newLine->next = NULL; // Inicializamos el puntero al siguiente nodo como NULL
    return newLine;
}



/*
 * add_line2  es una funcion para crear un array de listas enlazadas, a paratir de
 * cadenas ,las cadenas estan divididad en palabras, secuencias de escape: ejempl
 * Palabra: "hola", "Mundo" etc
 * Secuencias de escape: "\x1[A" , "\n" etc.
 * la lista se llena de esta forma :
 * Secuencias de escape = SE
 * Palabra = WORD
 * Entonces: si tenemos un array  de caracteres que tiene esto [SE|WORD|WORD|SE], este
 * array puede estar en cualquier, tonces la lista tiene que formarse en ese orde
 * [SE|ptrSigiente]--> [WORD|ptrSigiente]-->[WORD|ptrSigiente]-->[SE|ptrSigiente]-->null
 *--------------------------------------------------------------------------------------
 *Parametros de entrada (ListLine *, const char *)
 *ListLine: Miembros Line**, int countLine
 *           Miembors Line : const char * ptrStr (Almacena la cadena), int len (longitudo * de cadena)
 *--------------------------------------------------------------------------------------
 * Retorno : retorna un (int) que son el numero de Linias agegadas
 *-------------------------------------------------------------------------------------
 * Estructura de list line:
 * ListLine * -> [ListLine]-->[Line**]---->[*Line]-->[[WORD|ptrSig]]->[[WORD|ptrSig]]-
 *                                         [*Line+1]->[[WORD|ptrSig]]->[[WORD|ptrSig]]
 *                                         [*Line+2]
 *                                         [*Line+n] 
 *
 ****************************************************************************************
 */

int vt_len_escape( const wchar_t *sscape)
{
  int len = 1;
  while (!(iswcntrl(*++sscape)))
    {
      if(iswalpha(*sscape))
	{
	  const wchar_t *aux = sscape;
	  const wchar_t *next= ++sscape;
	 if (iswalpha(*next) || ((*next >= 0x2500) && (*next <= 0x257F)))
	    {
	      len++;
	      break;
            }
	  sscape = aux;
        }
      len++;
    }
  if (iswcntrl(*sscape) && (*sscape != L'\0'))
    {
      len += len_escape(sscape);
    }

    return len;
}

/// agrega una cadena al final de cada linia

int vt_add_end_nodo(Line **head,Line *line)
{
 
  static Line *aux=NULL;
  if(*head == NULL)
    {
      Line *lastNodo= line;
      *head= lastNodo;
       aux= *head;
      return 1;
    }
   Line *lastNodo=line;
   aux->next=lastNodo;
   aux = aux->next;
   return 0;  
}


int vt_add_line2(Line **head, const wchar_t *str)
{
  const wchar_t *firstChar = str;
  int x = 0;
  while (firstChar[x] != L'\0') // Usa `L'\0'` para cadenas de `wchar_t`
    {
      int countElement = 0;
      Line *aux = NULL;
      wchar_t *ptrChars[2] = {NULL, NULL};
      while (countElement < 2 && firstChar[x] != L'\0')
	{
	  if (iswprint(firstChar[x])||((firstChar[x] >= 0x2500) &&
					 (firstChar[x] <= 0x257F)))
	    {
	      // Extracción de la cadena
	      int lenPrintable = len_char_printable(&firstChar[x]);
	      ptrChars[0] = (wchar_t *)malloc(sizeof(wchar_t) * (lenPrintable + 1));
	      if (ptrChars[0] == NULL) return -1;
	      wcsncpy(ptrChars[0], &firstChar[x], lenPrintable);
	      ptrChars[0][lenPrintable] = L'\0'; // Usa `L'\0'` para el terminador nulo
	      x += lenPrintable;
	    }
	  else
	    {
	      if (iswcntrl(firstChar[x]))
                {
		  const wchar_t *iterChar = &firstChar[x];
		  int lenNoPrintable = len_escape(iterChar);
		  ptrChars[1] = (wchar_t *)malloc(sizeof(wchar_t)*(lenNoPrintable + 1));
		  if (ptrChars[1] == NULL) return -1;
		  wcsncpy(ptrChars[1], &firstChar[x], lenNoPrintable); // Usa `wcsncpy`
		  ptrChars[1][lenNoPrintable] = L'\0';
		  x += lenNoPrintable;
                }
            }
            countElement++;
        }
      aux = create_newLine((const wchar_t *)ptrChars);
      aux->isEnableEsc = 1;
      add_end_nodo(head, aux); // Agregamos el nodo
      if (ptrChars[0] != NULL) free(ptrChars[0]);
      if (ptrChars[1] != NULL) free(ptrChars[1]);
      ptrChars[0] = NULL;
      ptrChars[1] = NULL;
    }
    return 1;
}

int vt_free_list(Line **head)
{
  if((*head == NULL)) return -1;
  while(*head!=NULL)
    {
       Line * temp=*head;
       *head = (*head)->next;
       free(temp);
    }
  *head=NULL;
  return 0;
}


/*fuciones para procesesar  el buffer de la consola */

Vterm *vt_inic_virtualt(int Width, int Height,unsigned char * NameFont)
{
  Vterm *consol = (Vterm *)malloc(sizeof(Vterm));
  consol->Cursor.x=0;
  consol->Cursor.y=0;
  consol->Width=Width;
  consol->Height=Height;
  consol->MaxCol = Width/8;
  consol->MaxRow = Height/16;
  consol->NameFont= NameFont;
  return consol;
}

void vt_free_console(Vterm * consol)
{

  free(consol);
}

/*
  render_line : Recibe un buffer de linias que en errealidad es un estrucutra
                 con un buffer de caracteres, [a,e,b,\n,ESC] debe ser capas
		 de renderizar los caracteres imprimibles y actuar de acuerdo
		 a las secuencias de escape moviendo el cursor, haciendo saltos de linea
		 retornos de caro, cambiar de color los caracrteres,limpiar la pantalla,

 */
 

int vt_update_atri(Line * line,LineATributes * atri)
{
  if(line == NULL) return -1;
  line->pxy=atri->position;
  return 1;
}

/*actualizar la funciones render_lines para procesar las secuencias de escape*/
int vt_write(Line **lines, HDC *memdc)
{
  // TODO ARREGLAR ESTA FUCNION
    if (lines == NULL || memdc == NULL) return -1; // Validar punteros
        Line *firstLine = *lines;
        LineATributes atri={0}; 
        while (firstLine != NULL)
        {
	  int lenLine = firstLine->len;
	  wchar_t *str = firstLine->ptrStr;
	  wchar_t *esc = firstLine->ptrEscape;
	  unsigned char flag = firstLine->isEnableEsc;
            if (str != NULL && lenLine > 0)
	      {
		if(flag)
		  {
		     atri=vt_process_escape_sequences(firstLine,memdc);
		     update_atri(firstLine,&atri);
		     firstLine->isEnableEsc=0;
		  }
		POINT pxy=firstLine->pxy;
		//TextOutA(*memdc,(pxy.x*7),(pxy.y*16),(LPCSTR)str, lenLine);
		draw_font(memdc, str, pxy);
		
	      }
	    
            firstLine = firstLine->next; // Avanzar al siguiente nodo
        }
    return 1; // Éxito
}

typedef struct
{
  void (*funcion)(const wchar_t * params, LineATributes * atri); // funcion
  wchar_t charFuncion; // caracter para determinar de que funcion se  trata
}FUNCIONVT100;

/// crear un arreglo de esta estructura que contenga las diferente funciones

// secuencias de escape para manipular el cursor

void cursor_up_inverse_newline(const wchar_t *params, LineATributes *atri)
{
    if (*params == L'M')  // Usamos L'M' para caracteres anchos
        cursor.y -= 1;    // Mueve el cursor una línea hacia arriba
    atri->position = cursor;
}

void save_cursor(const wchar_t *params, LineATributes *atri)
{
    if (*params == L'7')  // Usamos L'7' para caracteres anchos
        saveCursor = cursor;
    atri->position = cursor;
}

void restore_cursor(const wchar_t *params, LineATributes *atri)
{
    if (*params == L'7')  // Usamos L'7' para caracteres anchos
        cursor = saveCursor;
    atri->position = cursor;
}

// Funciones CSI
void csi_cursor_up(const wchar_t *params, LineATributes *atri)
{
    int len = (int) wcslen(params);  // Usamos wcslen para cadenas de wchar_t
    int n = 0;
    if (len > 0)
        n = (wcstol(params, NULL, 10) == 0) ? 1 : wcstol(params, NULL, 10);  // wcstol para convertir a entero
    else
        n = 1;  // Si no hay parámetros, se asume que se movió 1 línea
    cursor.y -= n;
    atri->position = cursor;
}

void csi_cursor_down(const wchar_t *params, LineATributes *atri)
{
    int len = (int) wcslen(params);  // Usamos wcslen para cadenas de wchar_t
    int n = 0;
    if (len > 0)
        n = (wcstol(params, NULL, 10) == 0) ? 1 : wcstol(params, NULL, 10);  // wcstol para convertir a entero
    else
        n = 1;  // Si no hay parámetros, se asume que se movió 1 línea
    cursor.y += n;
    atri->position = cursor;
}

void csi_cursor_C(const wchar_t *params, LineATributes *atri)
{
    // sin len en 0 quiere decir que se omitieron los parametros
    int len = (int) wcslen(params);  // Usamos wcslen para cadenas de wchar_t
    int n = 0;
    if (len > 0)
        n = (wcstol(params, NULL, 10) == 0) ? 1 : wcstol(params, NULL, 10);  // wcstol para convertir a entero
    else
        n = 1;  // Si no hay parámetros, se asume que se movió 1 columna
    cursor.x += n;
    atri->position = cursor;
}

void csi_cursor_D(const wchar_t *params, LineATributes *atri)
{
    // sin len en 0 quiere decir que se omitieron los parametros
    int len = (int) wcslen(params);  // Usamos wcslen para cadenas de wchar_t
    int n = 0;
    if (len > 0)
        n = (wcstol(params, NULL, 10) == 0) ? 1 : wcstol(params, NULL, 10);  // wcstol para convertir a entero
    else
        n = 1;  // Si no hay parámetros, se asume que se movió 1 columna
    cursor.x -= n;
    atri->position = cursor;
}

void csi_cursor_E(const wchar_t *params, LineATributes *atri)
{
    // sin len en 0 quiere decir que se omitieron los parametros
    int len = (int) wcslen(params);  // Usamos wcslen para cadenas de wchar_t
    int n = 0;
    if (len > 0)
        n = (wcstol(params, NULL, 10) == 0) ? 1 : wcstol(params, NULL, 10);  // wcstol para convertir a entero
    else
        n = 1;  // Si no hay parámetros, se asume que se movió 1 fila
    cursor.y += n;
    atri->position = cursor;
}

void csi_cursor_F(const wchar_t *params, LineATributes *atri)
{
    // sin len en 0 quiere decir que se omitieron los parametros
    int len = (int) wcslen(params);  // Usamos wcslen para cadenas de wchar_t
    int n = 0;
    if (len > 0)
        n = (wcstol(params, NULL, 10) == 0) ? 1 : wcstol(params, NULL, 10);  // wcstol para convertir a entero
    else
        n = 1;  // Si no hay parámetros, se asume que se movió 1 fila
    cursor.y -= n;
    atri->position = cursor;
}


/*secuencias de escape  para manipular el aspecto del cursor*/


/// se mueve  de manera orizontal a la posicion que le indique de manera absoluta

void csi_cursor_G(const wchar_t *params, LineATributes *atri)
{
  int len = (int) wcslen(params);  // Usamos wcslen para cadenas de wchar_t
  int n = 0;
  if (len > 0)
     n = (wcstol(params, NULL, 10) == 0) ? 1 : wcstol(params, NULL, 10);  // wcstol para convertir a entero
  else
    n = (wcstol(params, NULL, 10) + 1);  // wcstol en lugar de atoi
  cursor.x = n;
  atri->position = cursor;
}

void csi_cursor_d(const wchar_t *params, LineATributes *atri)
{
  int len = (int) wcslen(params);  // Usamos wcslen para cadenas de wchar_t
  int n = 0;
  if (len > 0)
     n = (wcstol(params, NULL, 10) == 0) ? 1 : wcstol(params, NULL, 10);  // wcstol para convertir a entero
  else
    n = (wcstol(params, NULL, 10) + 1);  // wcstol en lugar de atoi
  cursor.y = n;
  atri->position = cursor;
}

void csi_cursor_H(const wchar_t *params, LineATributes *atri)
{
  wchar_t* buffer;
  wchar_t * token = wcstok((wchar_t *)params, L";",&buffer);
  int x = 0;
  int n = 0;
  int valueParams[2];
  while (token != NULL)
  {
    valueParams[x] = (int) wcstol(token, NULL, 10);  // wcstol para convertir a entero
    token = wcstok(NULL, L";",&buffer);
    x++;
  }
  for (int y = 0; y < 2; y++)
  {
    if (valueParams[y] > 0)
      n = (valueParams[y] == 0) ? 1 : valueParams[y];
    else
      n = (valueParams[y] + 1);
    if (y == 0) cursor.y = n;
    else cursor.x = n;
  }
  atri->position = cursor;
}

void csi_cursor_f(const wchar_t *params, LineATributes *atri)
{
  csi_cursor_H(params, atri);
}

void csi_cursor_s(const wchar_t *params, LineATributes *atri)
{
  if (*params == L's') saveCursor = cursor;  // Cambié 's' por L's' para usar wchar_t
  atri->position = cursor;
}

void csi_cursor_u(const wchar_t *params, LineATributes *atri)
{
  if (*params == L's') cursor = saveCursor;  // Cambié 's' por L's' para usar wchar_t
  atri->position = cursor;
}

/* Secuencias de escape simples */
void process_simple_escape_sequence(const wchar_t *params, LineATributes *atri)
{
  wchar_t es = *params;  // Cambié char por wchar_t
  switch (es)
  {
    case L'\n':  // Cambié '\n' por L'\n' para usar wchar_t
    {
      cursor.y += 1;
      atri->position = cursor;
    } break;
    case L'\r':  // Cambié '\r' por L'\r' para usar wchar_t
    {
      cursor.x = 0;
      atri->position = cursor;
    } break;
    default:
      break;
  }
}


#define NUMBER_FUNCION 16
void vt_process_funcion(wchar_t funcion, wchar_t* params, FUNCIONVT100 * arrf, LineATributes *atri, HDC *memdc)
{
    for (int x = 0; x < NUMBER_FUNCION; x++)
    {
        if (arrf[x].charFuncion == funcion)
        {
            // Ahora la función en el array también debe recibir un wchar_t* en lugar de un char*
            void (*func)(const wchar_t *paramas, LineATributes *atri) = arrf[x].funcion;
            
            // Ejecutamos la función pasándole el parámetro `params`, que es ahora un `wchar_t*`
            func(params, atri);
        }
    }
}


FUNCIONVT100 arrFuncionVT100[]=
  {
    {.funcion=cursor_up_inverse_newline,.charFuncion= L'M'}, // realiza la funcion inversa de \n
    {.funcion=save_cursor,.charFuncion=L'7'}, // permite guardar la posicion actual del cursor
    {.funcion=restore_cursor,.charFuncion=L'8'}, // restaura la posicion del cursor guardada en me
    {.funcion=csi_cursor_up,.charFuncion=L'A'},  // cusor hacia delante
    {.funcion=csi_cursor_down,.charFuncion=L'B'}, //cusosr hacia abajo
    {.funcion=csi_cursor_C,.charFuncion=L'C'},  ///.......
    {.funcion=csi_cursor_D,.charFuncion=L'D'}, //...
    {.funcion=csi_cursor_E,.charFuncion=L'E'}, ///... etc.
    {.funcion=csi_cursor_F,.charFuncion=L'F'},
    {.funcion=csi_cursor_G,.charFuncion=L'G'},
    {.funcion=csi_cursor_d,.charFuncion=L'd'},
    {.funcion=csi_cursor_H,.charFuncion=L'H'},
    {.funcion=csi_cursor_f,.charFuncion=L'f'},
    {.funcion=csi_cursor_s,.charFuncion=L's'},
    {.funcion=csi_cursor_u,.charFuncion=L'u'},
    {.funcion=process_simple_escape_sequence,.charFuncion=L'z'}
    
  };


LineATributes vt_process_escape_sequences(Line *line, HDC *hdc)
{
    LineATributes lineAtri = {0};
    
    if ((line != NULL) && (line->ptrStr != NULL))
    {
        wchar_t *cBuffer = (wchar_t *)line->ptrEscape;  // Usamos wchar_t en lugar de char
        int iterBuffer = 0;
        
        while (cBuffer[iterBuffer] != L'\0')
        {
            if ((cBuffer[iterBuffer] == 0x1b) && cBuffer[iterBuffer + 1] == L'[')
            {
                // Secuencias de escape CSI
                iterBuffer += 2;  // Avanzamos más allá de '\x1b' y '['
                
                wchar_t params[100];
                int lenParams = 0;
                
                while ((cBuffer[iterBuffer + lenParams] != L'\0' &&
                        cBuffer[iterBuffer + lenParams] != 0x1b) &&
                       (!iswalpha(cBuffer[iterBuffer + lenParams])))
                {
                    lenParams++;
                }
                
                wcsncpy(params, &cBuffer[iterBuffer], lenParams);  // Usamos wcsncpy para copiar la cadena
                params[lenParams] = L'\0';  // Aseguramos que la cadena esté terminada
                
                wchar_t funcion = cBuffer[iterBuffer + lenParams];  // Función asociada con la secuencia de escape
                
                // Llamada a la función que procesa la secuencia
                process_funcion(funcion, (wchar_t *)&params, arrFuncionVT100, &lineAtri, hdc);
                
                iterBuffer += (lenParams > 0) ? (lenParams + 1) : 1;  // Avanzamos en el buffer
            }
            else
            {
                if (cBuffer[iterBuffer] == 0x1b)
                {
                    if (iswprint(cBuffer[iterBuffer + 1]))
                    {
                        iterBuffer++;
                        wchar_t funcion = cBuffer[iterBuffer];
                        
                        // Procesamos la función con el caracter correspondiente
                        process_funcion(funcion, &funcion, arrFuncionVT100, &lineAtri, hdc);
                        
                        iterBuffer++;  // Avanzamos a la siguiente secuencia
                    }
                }
                else
                {
                    // Secuencias de escape simples
                    wchar_t funcion = L'z';  // Asumimos un valor por defecto para el ejemplo
                    wchar_t params = cBuffer[iterBuffer];  // Guardamos el carácter de la secuencia
                    process_funcion(funcion, &params, arrFuncionVT100, &lineAtri, hdc);
                    iterBuffer++;  // Avanzamos a la siguiente secuencia
                }
            }
        }
    }
    else
    {
        // No hay línea que procesar
    }
    
    return lineAtri;
}



void vt_reset_cursor()
{
  cursor=(POINT){.x=0,.y=0};
  oldCursor=(POINT){.x=0,.y=0};
}





