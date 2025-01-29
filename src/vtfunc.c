
void cursor_up_inverse_newline(const wchar_t *params,HDC *memdc)
{
    if (*params == L'M')  // Usamos L'M' para caracteres anchos
        cursor.y -= 1;    // Mueve el cursor una línea hacia arriba
}
void save_cursor(const wchar_t *params, HDC *memdc)
{
    if (*params == L'7')  // Usamos L'7' para caracteres anchos
        saveCursor = cursor;
}

void restore_cursor(const wchar_t *params, HDC *memdc)
{
    if (*params == L'7')  // Usamos L'7' para caracteres anchos
        cursor = saveCursor;
}

// Funciones CSI
void csi_cursor_up(const wchar_t *params, HDC *memdc)
{
    int len = (int) wcslen(params);  // Usamos wcslen para cadenas de wchar_t
    int n = 0;
    if (len > 0)
        n = (wcstol(params, NULL, 10) == 0) ? 1 : wcstol(params, NULL, 10);  // wcstol para convertir a entero
    else
        n = 1;  // Si no hay parámetros, se asume que se movió 1 línea
    cursor.y -= n;
}

void csi_cursor_down(const wchar_t *params, HDC *memdc)
{
    int len = (int) wcslen(params);  // Usamos wcslen para cadenas de wchar_t
    int n = 0;
    if (len > 0)
        n = (wcstol(params, NULL, 10) == 0) ? 1 : wcstol(params, NULL, 10);  // wcstol para convertir a entero
    else
        n = 1;  // Si no hay parámetros, se asume que se movió 1 línea
    cursor.y += n;
}

void csi_cursor_C(const wchar_t *params, HDC *memdc)
{
    // sin len en 0 quiere decir que se omitieron los parametros
    int len = (int) wcslen(params);  // Usamos wcslen para cadenas de wchar_t
    int n = 0;
    if (len > 0)
        n = (wcstol(params, NULL, 10) == 0) ? 1 : wcstol(params, NULL, 10);  // wcstol para convertir a entero
    else
        n = 1;  // Si no hay parámetros, se asume que se movió 1 columna
    cursor.x += n;
}

void csi_cursor_D(const wchar_t *params, HDC *memdc)
{
    // sin len en 0 quiere decir que se omitieron los parametros
    int len = (int) wcslen(params);  // Usamos wcslen para cadenas de wchar_t
    int n = 0;
    if (len > 0)
        n = (wcstol(params, NULL, 10) == 0) ? 1 : wcstol(params, NULL, 10);  // wcstol para convertir a entero
    else
        n = 1;  // Si no hay parámetros, se asume que se movió 1 columna
    cursor.x -= n;
}

void csi_cursor_E(const wchar_t *params, HDC *memdc)
{
    // sin len en 0 quiere decir que se omitieron los parametros
    int len = (int) wcslen(params);  // Usamos wcslen para cadenas de wchar_t
    int n = 0;
    if (len > 0)
        n = (wcstol(params, NULL, 10) == 0) ? 1 : wcstol(params, NULL, 10);  // wcstol para convertir a entero
    else
        n = 1;  // Si no hay parámetros, se asume que se movió 1 fila
    cursor.y += n;
}

void csi_cursor_F(const wchar_t *params, HDC *memdc)
{
    // sin len en 0 quiere decir que se omitieron los parametros
    int len = (int) wcslen(params);  // Usamos wcslen para cadenas de wchar_t
    int n = 0;
    if (len > 0)
        n = (wcstol(params, NULL, 10) == 0) ? 1 : wcstol(params, NULL, 10);  // wcstol para convertir a entero
    else
        n = 1;  // Si no hay parámetros, se asume que se movió 1 fila
    cursor.y -= n;
}


/*secuencias de escape  para manipular el aspecto del cursor*/


/// se mueve  de manera orizontal a la posicion que le indique de manera absoluta

void csi_cursor_G(const wchar_t *params, HDC *memdc)
{
  int len = (int) wcslen(params);  // Usamos wcslen para cadenas de wchar_t
  int n = 0;
  if (len > 0)
     n = (wcstol(params, NULL, 10) == 0) ? 1 : wcstol(params, NULL, 10);  // wcstol para convertir a entero
  else
    n = (wcstol(params, NULL, 10) + 1);  // wcstol en lugar de atoi
  cursor.x = n;
}

void csi_cursor_d(const wchar_t *params, HDC *memdc)
{
  int len = (int) wcslen(params);  // Usamos wcslen para cadenas de wchar_t
  int n = 0;
  if (len > 0)
     n = (wcstol(params, NULL, 10) == 0) ? 1 : wcstol(params, NULL, 10);  // wcstol para convertir a entero
  else
    n = (wcstol(params, NULL, 10) + 1);  // wcstol en lugar de atoi
  cursor.y = n;
}

void csi_cursor_H(const wchar_t *params, HDC *memdc)
{
  /// todo
  //hay que controlar  tambien cunado no tiene parametros
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
  
  for (int y = 0; (y < 2 && x != 0); y++)
  {
    if (valueParams[y] > 0)
      n = (valueParams[y] == 0) ? 0 : valueParams[y];
    else
      n = (valueParams[y]);
    if (y == 0) cursor.y = n;
    else cursor.x = n;
  }
  if(x == 0)
    {
      cursor.x=0;
      cursor.y=0;
    }
  
}

void csi_cursor_f(const wchar_t *params, HDC *memdc)
{
  csi_cursor_H(params,memdc);
}

void csi_cursor_s(const wchar_t *params, HDC *memdc)
{
  if (*params == L's') saveCursor = cursor;  // Cambié 's' por L's' para usar wchar_t
}

void csi_cursor_u(const wchar_t *params, HDC *memdc)
{
  if (*params == L's') cursor = saveCursor;  // Cambié 's' por L's' para usar wchar_t
}

/* Secuencias de escape simples */
void process_simple_escape_sequence(const wchar_t *params, HDC *memdc)
{
  wchar_t es = *params;  // Cambié char por wchar_t
  switch (es)
  {
    case L'\n':  // Cambié '\n' por L'\n' para usar wchar_t
    {
      cursor.y += 1;
    } break;
    case L'\r':  // Cambié '\r' por L'\r' para usar wchar_t
    {
      cursor.x = 0;
    } break;
    default:
      break;
  }
}

/*secuencias de escape para borrar la pantalla*/
void csi_cls_screen_EL_K(const wchar_t *params,  HDC *memdc)
{
    /* si <n> se omite el valor predeterminado e cero
       0: borra contenido desde la posición actual del cursor (inclusive) hasta el final de la línea o pantalla.
       1: borra contenido desde el principio de la línea o la pantalla hasta la posición actual del cursor (inclusive).
       2: borra la línea o pantalla completa.
     */

    wchar_t *buffer;
    wchar_t *token = wcstok((wchar_t *)params, L";", &buffer);
    int n = 0;
    if (token && !iswalpha(*token))
      {
        n = (int)wcstol(token, NULL, 10);
      }
    POINT currentCursor = cursor;
    int x0 = 0, xf = (80-1), dx = 0;
    wchar_t *line = NULL;

    switch (n)
      {
        case 0: // Desde el cursor hasta el final
            x0 = currentCursor.x;
            dx = (xf - x0) + 1; // Incluye terminador nulo
            break;
        case 1: // Desde el inicio hasta el cursor
            x0 = 0;
            xf = currentCursor.x;
            dx = (xf - x0) + 1; // Incluye terminador nulo
            break;
        case 2: // Línea completa
        default:
            x0 = 0;
            break;
      }

    line = (wchar_t *)mymalloc(sizeof(wchar_t) * dx,__func__,__FILE__);
    if (!line) {
        wprintf(L"Error: no se pudo asignar memoria.\n");
        return;
    }

    wmemset(line, L' ', dx - 1);
    line[dx - 1] = L'\0';

    draw_font(memdc, (const wchar_t *)line, currentCursor.x, currentCursor.y);

    // Liberar memoria
    myfree(line);
}

void csi_cls_screen_ED_J(const wchar_t *params,  HDC *memdc)
{
  /// BORRA EN PANTALLA

  /* si <n> se omite el valor predeterminado e cero
       0: borra contenido desde la posición actual del cursor (inclusive) hasta el final de la línea o pantalla.
       1: borra contenido desde el principio de la línea o la pantalla hasta la posición actual del cursor (inclusive).
       2: borra la línea o pantalla completa.
     */

    wchar_t *buffer;
    wchar_t *token = wcstok((wchar_t *)params, L";", &buffer);
    int n = 0;
    wchar_t bufferP[20];
    if (token && !iswalpha(*token))
      {
        n = (int)wcstol(token, NULL, 10);
      }
    POINT currentCursor = cursor;
    int x0 = 0, y =(60-1), dx = 0;
    wchar_t *line = NULL;
    switch (n)
      {
      case 0:// Desde el cursor hasta el final
	x0 = currentCursor.x;
	break;
      case 1: // Desde el inicio hasta el cursor
	x0 = 0;
	break;
      case 2: // Línea completa
      default:
	x0 = 0;
	break;
      }

    for (int i=0; i < y; i++)
      {
	swprintf(buffer, 20 ,L"%d;%d", x0, (currentCursor.y+i));
	csi_cls_screen_EL_K(bufferP,memdc);
      }
    
}

#define ID_TIMER_BLINK_CURSOR 1

/* esta funcion controla las propiedades del cursro*/

void csi_cursor_ctrl(const wchar_t *params, HDC *memdc)
{
  /// HWND hWindow= WindowFromDC(GHdc); 
  wchar_t command[3]={params[1],params[2],L'\0'};
  wchar_t estate= params[3]; // estado=activo/desactivate
  if((wcscmp(command, L"12") == 0))
      {
	 static int timerCreated = 0;
	if (estate == L'h' )
	  {
	    if (timerCreated == 0)
	      {
		flag|=0x02;
		int result = SetTimer(Wwindow, ID_TIMER_BLINK_CURSOR, 500, NULL);
		if (result == 0)
		  {
		    printf("No se pudo crear el temporizador\n");
		  }
		else
		  {
		    timerCreated = 1; // Marcamos que el temporizador fue creado correctamente
		  }
	      }
	    else
	      {
		//if(cur.flag & 0x02)cur.flag&=~0x02;
		//else cur.flag|=0x02;
		
	      }
	  }
	else
	  {
	    if(timerCreated == 1)
	      {
		flag&=~0x02;
	      }
	  }
      }
  else
    {
      if((wcscmp(command, L"25") == 0))
	{
	  static int showCursor=0;
	  if (estate == L'h')
	  {
	    // implementar
	     // muestra el cursor
	    if(showCursor == 0)
	      {
		 flag|=0x01;
		 showCursor=1;
	      } 
	  }
	else
	  {
	    // implemetar
	    // oculta el cursor
	    if(showCursor == 1)
	      {
		 flag&=~0x01;
		 showCursor=0;
	      }
	  }
	  
	}
    }
  // printf("flag cursor %d\n",cur.flag);
}


