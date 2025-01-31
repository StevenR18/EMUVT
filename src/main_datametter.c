#include <windows.h>
#include <stdio.h>
#include "..\..\Vt\includes\window64_platform.h"
#include "..\..\Vt\includes\main_datametter.h"
#include "..\..\Vt\includes\font.h"
#include "..\..\Vt\includes\newwvt.h"
#include "..\..\Vt\includes\liked_list.h"
#include "..\..\Vt\includes\memory_traker.h"
#define CTRL_KEY(ascii) ((ascii) & 0x1F)
#define ARROW_UP(s0)    (((s0) & 0x7700)) // Representa 'W'
#define ARROW_DOWN(s0)  (((s0) & 0x7300)) // Representa 'S'
#define ARROW_LEFT(s0)  (((s0) & 0x6100)) // Representa 'A'
#define ARROW_RIGHT(s0) (((s0) & 0x6400)) // Representa 'D'
#define OFSET_X 7
#define ROW_MAIN 1
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
#define ATCITIVE_WASD 0
#define ROW_MAIN 1
#define ifYexceedTOrowcap(y) (( (y) +1 ) > (caprow - 1))
extern Win32ScreenBuffer GBuffer;
extern int Runnig;
extern unsigned char enableBlinkCursor; // se utiliza para el parapadeo del cursor
extern Termv Emu; /// estructura de newwvt.c
enum
  {
    EDIT,
    TERMINAL
  };

typedef struct
{
  wchar_t *s;
  int len;
  int rowType;
}Row;

typedef struct
{
  POINT vCursor;
  Nodo *buffercomand;
  int  width;
  int  height;
  Row **row; // array del buffe principal
  int capRow;
}View;

#define WIDTH_TERMV  40
#define HEIGHT_TERMV 30

View vEmu;

#define comand vEmu.buffercomand
#define cursor_x vEmu.vCursor.x
#define cursor_y vEmu.vCursor.y
#define Cursor  vEmu.vCursor
#define width  vEmu.width
#define height vEmu.height
#define row vEmu.row
#define caprow vEmu.capRow


void appendRow(const wchar_t * s, int len)
{
  if(s == NULL)return;
  row = (Row **) realloc(row,sizeof(Row* )*(caprow+1));
  row[caprow]= (Row *) malloc(sizeof(Row));
  row[caprow]->s = (wchar_t *)malloc(sizeof(wchar_t)*(len+1));
  wcsncpy(row[caprow]->s, s,len); 
  row[caprow]->len=len;
  row[caprow]->s[len]=L'\0';
  caprow++;
}

void freeRow()
{
  if(row == NULL)return;
  for(int r =0; r <caprow; r++)
    {
      free(row[r]->s);
      free(row[r]);
    }
   row=NULL;
   caprow=0;
}

/// funcion  agrea un caracter la final  de cada row
wchar_t * appendCharsRow(const wchar_t * c,int x, int len)
{
  if(c == NULL) return NULL;
  int prevLen= row[cursor_y]->len;
  
  /// ay que saber si desde la posicion en la que se ba a incertar los caracteres
  // no se pasa de el ancho de la ventana4
  int ofsetx= (row[cursor_y]->rowType == ROW_MAIN) ? 7 : 0;
  x+=ofsetx;
  // memoria reservada
  row[cursor_y]->s = (wchar_t*) realloc(row[cursor_y]->s,sizeof(wchar_t) * (prevLen + len +1));
  row[cursor_y]->s[prevLen+len]=L'\0';
  //cuando se van a mover los datos y cuando no se va o mover los datos
 
  if((x != prevLen)&&(x < width))
    {
      // width= 10
      // se hace movimiento de memoria
      //len 10
      // prevLen=10
      // x=2
      // 
      //aux=[ h | o | l | a | m | u | n | d | o | a ]
      //             x=2     
      //s=  [ h | o | l | a | l | a | m | u | n | d | o ]
      //                     x+2
      // tenemos que llenar los huecos
      //s=  [ h | o | l | a | l | a | m | u | n | d | o ]
      //                     x+2
      if (prevLen == width)prevLen= width;
      wchar_t * aux = (wchar_t *) malloc(sizeof(wchar_t) * prevLen+1);
      wcsncpy(aux,row[cursor_y]->s,prevLen);
      aux[prevLen]=L'\0';
      wmemmove(&row[cursor_y]->s[x+len], &aux[x], (prevLen - x ));
      row[cursor_y]->s[prevLen+len]='\0';
      wcsncpy(&row[cursor_y]->s[x],c,len);
      free(aux);
      aux=NULL; 
    }
  else
    {
       // no se hace movimeinto de memoria
      wcsncpy(&row[cursor_y]->s[x], c,len); 
    }
  wchar_t * resto=NULL;

  if((prevLen + len) > width )
    {
      int lenResto = wcslen(&row[cursor_y]->s[prevLen]);
      resto = (wchar_t *) malloc(sizeof(wchar_t)* (lenResto+1));
      wcsncpy(resto,&row[cursor_y]->s[prevLen],lenResto);
      resto[lenResto]=L'\0';
      row[cursor_y]->len = width;
      row[cursor_y]->s[width]=L'\0';
    }
  else
    {
       row[cursor_y]->len = prevLen + len;
       row[cursor_y]->s[prevLen+len]=L'\0';
      
    }
  return resto;
  
}
wchar_t * addCharsTORowLIFO(const wchar_t * c,int len, int y)
{
  if(c == NULL)return NULL;
  wchar_t * aux= NULL;
  wchar_t * resto=NULL;
  int prevLen=0;
  if(row[y]->len == width)
    {
      prevLen=width;
      
    }
  else
    {
      prevLen= row[y]->len;
    }
 
  aux= (wchar_t *)malloc(sizeof(wchar_t)*prevLen+1);
  aux[prevLen]= L'\0';
  wcsncpy(aux, &row[y]->s[0],prevLen); // haces una copia el contenido anterior
  
  row[y]->s=(wchar_t *)realloc(row[y]->s,sizeof(wchar_t)*(prevLen+len+1));
  
  wcsncpy(&row[y]->s[0], c,len); // agregamos el caracter
  wcsncpy(&row[y]->s[len], aux,prevLen+1);
  int lenDefinitiv =0;
  if (prevLen + len > width)
    {
      lenDefinitiv = width;
      resto= (wchar_t *) malloc(sizeof(wchar_t) *(len+1));
      wcsncpy(resto,&row[y]->s[prevLen -1],(len+1));
    }
  else
    {
      lenDefinitiv = prevLen + len;
    }
  
  row[y]->s[lenDefinitiv]= L'\0';
  row[y]->len=lenDefinitiv;
  free(aux);
  aux=NULL;
  return resto;
}

void addAllCharsToRow(const wchar_t * c, int len, int y)
{
  if(c == NULL) return;

  ///definir el caso base cuando la funcion  saldra de la recursion
  // caprow=3
  // cursor_x=x;
  // cursor_y = 0
  // caprow=1 [       0             ROW 1                       ]
  // caprow=2 [       0             ROW 2                       ]
  // caprow=3 [       0             ROW 3                       ]
  if(!ifYexceedTOrowcap(y))
    {
      wchar_t * resto=NULL;
      resto=addCharsTORowLIFO(c,len,(y+1));
      addAllCharsToRow(resto,len, (y + 1));
      if(resto != NULL)
	{
	  free(resto);
	  resto=NULL;
	}
      return;

    }
  // si y excede  a caprow entonces se crear una nueva  row
  appendRow(c, len);
}

// agrega caracteres si hay  (Remainder o Resto)
void addCharactersIfRemainder(wchar_t **remainder,int *x,int *y)
{
  if(*remainder !=  NULL)
    {
      // la cadena supero el ancho de la ventana y se genero un resto
      int lenResto = wcslen(*remainder);
      // cursor_y=(cursor_x >= row[cursor_y]->len) ? cursor_y++ : cursor_y;
      int  ofset = (row[*y]->rowType == ROW_MAIN) ? OFSET_X : 0;
      int ofr=0;
      int yofset=0;
#define ifCursorNotXExceedrowLenAndWidth(x,y)(\
					      (((x))< row[(y)]->len -1)&& \
					      (((x)) <(width-1)))
      if(!ifCursorNotXExceedrowLenAndWidth((*x+ofset),*y))
	{
	  (*y)++;
	  *x=0;
	  yofset=1;
	}
      if((*x  == (width -1)))
	{
	  if(((*x % 2) != 0)&& (lenResto > 1) )
	    {
	      lenResto--;
	      ofr=lenResto;
	    }
	}
      addAllCharsToRow(&((*remainder)[ofr]), lenResto, (*y - yofset));
    }
}


void delEndCharofRow()
{
  int endChar= (row[cursor_y]->len - 1);
  if(row[cursor_y]->rowType ==  ROW_MAIN)
    {
      if((cursor_x + OFSET_X) != OFSET_X)
	{
	   row[cursor_y]->s[endChar]=L'\0';
	   row[cursor_y]->len--;
	   cursor_x--;
	  
	}
    }
}
 





void exit_app()
{
  Runnig=0; // se cierra la aplicacion
}
void term_readkey()
{
  /// devuelve el caracter leido  desde el teclado
  Nodo *inputbuffer= getInput();
  Nodo **buffercomand= &comand;
  if(inputbuffer == NULL) return; // la lista esta vacia
  while(inputbuffer)
    {
      wchar_t * str = (wchar_t *)inputbuffer->data;
      int len=sizeof(wchar_t)*2;
	      wchar_t up[2];
	      wchar_t down[2];
	      wchar_t right[2];
	      wchar_t left[2];
	      wchar_t back[5]={L'\x1b',L'[',L'3',L'~',L'\0'};
	      up[1]=L'\0';
	      down[1]=L'\0';
	      right[1]=L'\0';
	      left[1]=L'\0';
	      *up=ARROW_UP(0x5E00);
	      *down=ARROW_DOWN(0x5E00);
	      *right=ARROW_RIGHT(0x5E00);
	      *left=ARROW_LEFT(0x5E00);
      if(str[0] == L'\x1b')
	{
	  if(str[1]== L'[')
	    {
	      switch(str[2])
		{
		case L'A':insert_nodo_end(buffercomand,&up,len);break;
		case L'B':insert_nodo_end(buffercomand,&down,len); break;
		case L'C':insert_nodo_end(buffercomand,&right,len); break;
		case L'D':insert_nodo_end(buffercomand,&left,len); break;
		case L'3':insert_nodo_end(buffercomand,&back,4); break;
		}
	    }
	}
      else
	{
	  if((str[0] >= L'\a') && (str[0] <= L'\r'))
	    {
	      insert_nodo_end(buffercomand,&str[0],4);
	    }
	  else
	    {
	      int len= (wcslen(&str[0])+1)* sizeof(wchar_t);
	      switch(str[0])
		{
		case CTRL_KEY('i'):return;break;
		case CTRL_KEY('a'):return;break;
		case CTRL_KEY('f'):return;break;
		case CTRL_KEY('j'):return;break;
		  //case L'':return;break; ñ
		case CTRL_KEY('c'):return;break;
		case CTRL_KEY('v'):return;break;
		case CTRL_KEY('h'):return;break;
		case CTRL_KEY('q'):
		case CTRL_KEY('w'):
		case CTRL_KEY('e'):
		case CTRL_KEY('r'):
		case CTRL_KEY('t'):
		case CTRL_KEY('y'):
		case CTRL_KEY('u'):
		case CTRL_KEY('o'):
		case CTRL_KEY('p'):
		case CTRL_KEY('s'):
		case CTRL_KEY('d'):
		case CTRL_KEY('g'):
		case CTRL_KEY('k'):
		case CTRL_KEY('l'):
		case CTRL_KEY('z'):
		case CTRL_KEY('x'):
		case CTRL_KEY('b'):
		case CTRL_KEY('n'):
		  {
		    // comando correspondiente ctrl+ el alfabeto
		    // tienen que ser mappeados
		    insert_nodo_end(buffercomand,&str[0],len);
		    insert_nodo_end(buffercomand,&right,2*sizeof(wchar_t));
		    insert_nodo_end(buffercomand,&right,2*sizeof(wchar_t));
		  }break;
		default:
		  {
		    /// todo: aqui ingresan los caracteres al buffer de comandos
		    insert_nodo_end(buffercomand,&str[0],len);
		    // se agrega una secuencia para mover el cursor despues de cada
		    // ingreso de tecla
		    insert_nodo_end(buffercomand,&right,2*sizeof(wchar_t));
		  }break;
		}
	    }
	}
      
      inputbuffer= inputbuffer->next;
    }
}

int isRowMain()
{
  return (row[caprow-1]->rowType == ROW_MAIN);
  
}
int getLastRowLen()
{
  return row[caprow-1]->len;
}
int isCommandValid()
{
  return 0;
}

/*permite mover el cursor*/

void term_move_cursor(wchar_t key)
{
  
  switch (key) {
  case ARROW_LEFT(0x5E00):
      int  ofx = (row[cursor_y]->rowType == ROW_MAIN) ? OFSET_X : 0;
      if(cursor_x+ ofx > ofx)cursor_x--;
      break;
  case ARROW_RIGHT(0x5E00):
      #if 0
    if(cursor_x < width)cursor_x++; // modoel<
      #else
      //comand>as
      int  ofsetx = (row[cursor_y]->rowType == ROW_MAIN) ? OFSET_X : 0;
      if(((cursor_x + ofsetx) < row[cursor_y]->len)&&
	 ((cursor_x + ofsetx)< width-1))
	{
	  cursor_x++;
	}
      else
	{
	  if(ifYexceedTOrowcap(cursor_y))appendRow(L"", 0);
	    cursor_y++;
	    cursor_x=0;
	}
      #endif
      break;
  case ARROW_UP(0x5E00):
      if(cursor_y != 0) cursor_y--;
      break;
    case ARROW_DOWN(0x5E00):
      if(cursor_y < height-1)cursor_y++;
      break;
  }
}

void term_process_keypress()
{
  /// procesa la tecla pulsada
  term_readkey(); // retorna la tecla  presionada
  Nodo *bufferComand = comand;
  wchar_t * resto=NULL;
  while(bufferComand!=NULL)
    {
      wchar_t * str =(wchar_t *) bufferComand->data;
    
      switch(str[0])
	{
	case L'\x1b':
	  {
	    if(str[1] == L'[')
	      {
		
		    delEndCharofRow();
		
	      }
	  }break;
	case CTRL_KEY('q'):
	  {
	    exit_app();
	  }break;
	case CTRL_KEY('w'):resto = appendCharsRow(L"^W",cursor_x ,2);break; 
	case CTRL_KEY('e'):resto = appendCharsRow(L"^E",cursor_x ,2);break; 
        case CTRL_KEY('r'):resto = appendCharsRow(L"^R",cursor_x ,2);break; 
        case CTRL_KEY('t'):resto = appendCharsRow(L"^T",cursor_x ,2);break; 
        case CTRL_KEY('y'):resto = appendCharsRow(L"^Y",cursor_x ,2);break;
        case CTRL_KEY('u'):resto = appendCharsRow(L"^U",cursor_x ,2);break; 
        case CTRL_KEY('o'):resto = appendCharsRow(L"^O",cursor_x ,2);break; 
        case CTRL_KEY('p'):resto = appendCharsRow(L"^P",cursor_x ,2);break; 
        case CTRL_KEY('s'):resto = appendCharsRow(L"^S",cursor_x ,2);break; 
        case CTRL_KEY('d'):resto = appendCharsRow(L"^D",cursor_x ,2);break; 
        case CTRL_KEY('g'):resto = appendCharsRow(L"^G",cursor_x ,2);break; 
        case CTRL_KEY('k'):resto = appendCharsRow(L"^K",cursor_x ,2);break; 
        case CTRL_KEY('l'):resto = appendCharsRow(L"^L",cursor_x ,2);break; 
        case CTRL_KEY('z'):resto = appendCharsRow(L"^Z",cursor_x ,2);break; 
        case CTRL_KEY('x'):resto = appendCharsRow(L"^X",cursor_x ,2);break; 
        case CTRL_KEY('b'):resto = appendCharsRow(L"^B",cursor_x ,2);break; 
        case CTRL_KEY('n'):resto = appendCharsRow(L"^N",cursor_x ,2);break; 
	case ARROW_UP(0x5E00):
	case ARROW_DOWN(0x5E00):
	case ARROW_LEFT(0x5E00):
	case ARROW_RIGHT(0x5E00):
	  {
	    term_move_cursor(str[0]);
	  }break;
	case L'\n':
	  {
	    /// aumentar capacity row +1
#define onlyReturnifisCommandRow (isRowMain() && (getLastRowLen() == OFSET_X))
	    
	    if(onlyReturnifisCommandRow)
	      {
		wchar_t * msg= L"comand>";
		appendRow(msg,wcslen(msg));
		cursor_y++;
		row[cursor_y]->rowType = ROW_MAIN;

		//// crear una funcion para procesar los comandos de la terminal
		
	      }
	       
	  }break;
	default:
	  // AQUI TENDRÍA QUE PROCESAR LAS TECLAS QUE SE SE VAN A MOSTRAR
	  // EN LA PANTALLA
          // arreglar para que funcionen las teclas espaciadoras
	  // y tambien para que funcionen las teclas wasd
	  resto =  appendCharsRow(&str[0],cursor_x,1);
	  break;
	}
      addCharactersIfRemainder(&resto,&cursor_x,&cursor_y);
      if(resto != NULL){ free(resto); resto=NULL;}
      bufferComand=(Nodo*) bufferComand->next;
    }
}


void term_get_window_size()
{
  // obtienes el tamaño de la ventan
  //width =80;
  //height =20;
}
void term_drawrow()
{

  
  for(int r=0; r < caprow; r++)
    {
       appendBuffer(row[r]->s,row[r]->len);
       appendBuffer(L"\n",1);
      
    }
 
}

void term_draw_cursor(HDC * memdc)
{
  
  wchar_t * arrGlifo= getGlifo();
  wchar_t g= arrGlifo[0];
  #if 1
  int ofsetx=0;
  if(row[cursor_y]->rowType == ROW_MAIN)
    ofsetx=7;
  else
    ofsetx=0;
  draw_font(memdc,(const wchar_t *)&g,cursor_x+ofsetx ,cursor_y);
  #else
  draw_font(memdc,(const wchar_t *)&g,cursor_x,cursor_y);
  #endif
  
}

void  inicCursor00()
{
  wchar_t buffer[32];
  swprintf(buffer, 32 ,L"\x1b[%d;%dH",0,0);
  appendBuffer(buffer,wcslen(L"\x1b[%d;%dH")); 
  
}

void term_refreshScreen(Win32ScreenBuffer *Buffer)
{
 
  term_drawrow();
  inicCursor00(); // si no reinicias el cursor los cursores aumentaras indefinidamente
  write(&Buffer->BackBuffer);
  term_draw_cursor(&Buffer->BackBuffer);
  
}

void freeall()
{
  freeBuffer(); // libera el buffer principal
  free_lista(getRootInput());//liberar el inputbuffer
  free_lista(&comand);//libera el buffer de coandos
  //freeRow();// libera el burffer que almazena el texto principal
  freeAll();// libera todo por si acaso
 
}

void main_app(Win32ScreenBuffer *Buffer,KeyboardEvent *k)
{
 
  translate_keyevent_tovt_sequence(*k); //traduce los enventos del teclado a secuencia
  term_refreshScreen(Buffer);// dibuja el primer frame
  term_process_keypress();//captura los eventos del teclado
  freeall();
  
}

void inic_app()
{
  vEmu.vCursor=(POINT){0,0};
  comand=NULL;
  width=WIDTH_TERMV;
  height=HEIGHT_TERMV;
  wchar_t * msg= L"comand>";
  appendRow(msg,wcslen(msg));
  
  row[0]->rowType = ROW_MAIN;
}
void end_app()
{

  
}

