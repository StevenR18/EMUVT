#include <stdio.h>
#include <stdlib.h>     // Para malloc, free, realloc
#include <string.h>     // Para strncpy, strlen
#include <ctype.h>
#include "..\..\Vt\includes\memory_traker.h"
#define debug 1
#define debugInfo __func__ ,__FILE__
typedef struct
{
  char *s;  /// cadena
  struct Node *next;
}Node;
typedef struct
{
  int countNode;
  Node* head; /// cabeza de la lista
  Node* tail; // cola de la lista
}Buffer;
Buffer buffer={0,NULL,NULL};
#define head  buffer.head
#define tail  buffer.tail
#define count buffer.countNode



Node *createNode(const char *s2, int len)
{
  #if debug
  Node * nodo=(Node *)mymalloc(sizeof(Node),__func__, __FILE__);
  #else
  Node * nodo=(Node *)malloc(sizeof(Node));
  #endif
  #if debug
  nodo->s = (char *)mymalloc(sizeof(char)*(len+1),__func__, __FILE__);
  #else
  nodo->s = (char *)malloc(sizeof(char)*(len+1));
  #endif
  if(nodo->s == NULL) return NULL;
  strncpy(nodo->s,s2,len);
  nodo->s[len]='\0';
  nodo->next=NULL;
  return nodo;
}

void freeBuffer()
{
  Node *current = head;
  while (current)
    {
      Node *next =(Node *) current->next; // Guarda el siguiente nodo antes de liberar el actual
      #if debug
      myfree(current->s);          // Libera la cadena asociada al nod
      myfree(current);             // Libera el nodo
      #else
      free(current->s);          // Libera la cadena asociada al nodo
      free(current);             // Libera el nodo
      #endif
      current = next;            // Pasa al siguiente nodo
    }
    // Reinicia las variables del buffer
    head = NULL;
    tail = NULL;
    count = 0;
}


void appendNode(const char *s, int len)
{
  Node * newNode= createNode(s,len);
  if(head == NULL)
    {
      // entra aqui si la lista esta vacia
      head =  tail = newNode;
    }
  else
    {
      // entra aqui si hay  por lo menos un nodo
      tail->next =(struct Node *) newNode;
      tail= newNode;
    }
  count ++;
}

void printBuffer()
{
  Node * current= head;
  while(current)
    {
      printf("Nodo [%p]\n"
	     "Candena [%s]\n"
	     "Next [%p]\n",current,current->s,current->next);

      current =(Node *) current->next;
      
    }
  printf("Count %d\n",count);
}

int lenEscape(const char *s)
{
   int len=0;
   int x=0;
   while(s[x] != '\0')
    {
      if(s[x] == 0x1b)
	{
	  len++;
	  x++; // contamos el caracter de incio de secuencia
	  if(s[x] == '[')
	    {
	      len++; // contamos el corchete
	      x++;   // abanzamos en la secuencia
	    }
	  else
	    {
	      /// una secuencia sin parametros
	      x++; // abanzamos al siguiente caracter
	      continue;
	    }
	  while(!isalpha(s[x]))
	    {
	      len++; // contamos el caracter
	      x++; // abanzamos al sigiente caracter
	    }
	  len++; // contamos el caracter de final de secuencia
	  x++; // abanazamos al siguiente caracter
	}
      else
	{
	   break;
	}
     
    }

   return len;
}
#define splitBuffer char **

splitBuffer spliteEcapeAndWord(const char *s, int *countOut)
{
  splitBuffer splBuffer = NULL;
  int splIndex = 0;
  int x = 0;
  int len = 0;
  while (s[x] != '\0')
    {
      if (s[x] == 0x1b)
	{ // Inicio de una secuencia de escape
	  len = lenEscape(&s[x]);
	  #if debug
	  splBuffer = (splitBuffer)myrealloc(splBuffer, sizeof(char *) * (splIndex + 1),__func__, __FILE__);
	  splBuffer[splIndex] = (char *)mymalloc(sizeof(char) * (len + 1),__func__, __FILE__);
	  #else
	  splBuffer = (splitBuffer)realloc(splBuffer, sizeof(char *) * (splIndex + 1));
	  splBuffer[splIndex] = (char *)malloc(sizeof(char) * (len + 1));
	  #endif


	  strncpy(splBuffer[splIndex], &s[x], len);
	  splBuffer[splIndex][len] = '\0';
          x += len; // Avanzar el índice en la cadena original
          splIndex++;
	}
      else
	{ // Es una palabra o frase
	  len = 0;
	  int start = x; // Marca el inicio de la palabra
	  while (isprint(s[x]) && s[x] != '\0' && s[x] != 0x1b)
	    {
                len++;
                x++;
            }
	  if (len > 0)
	    {
	      #if debug
	      splBuffer = (splitBuffer)myrealloc(splBuffer, sizeof(char *) * (splIndex + 1),__func__, __FILE__);
	      splBuffer[splIndex] = (char *)mymalloc(sizeof(char) * (len + 1),__func__, __FILE__);
	      #else
	      splBuffer = (splitBuffer)realloc(splBuffer, sizeof(char *) * (splIndex + 1));
	      splBuffer[splIndex] = (char *)malloc(sizeof(char) * (len + 1));
	      #endif
	      strncpy(splBuffer[splIndex], &s[start], len);
	      splBuffer[splIndex][len] = '\0';
	      splIndex++;
            }
        }
    }

    *countOut = splIndex;
    return splBuffer;
}



// el len que se le pasa a esta funcion no devería tomar en cuenta las secuencias de escape

void freeSplBuffer( splitBuffer splBuffer, int c)
{
  for(int x=0; x < c; x++)
    myfree(splBuffer[x]);
}
void appendBuffer(const char *s, int len)
{
  int c;
      splitBuffer splBuffer= spliteEcapeAndWord(s, &c);
  for(int x=0; x < c; x++) appendNode(splBuffer[x], strlen(splBuffer[x]));

  #if debug
  freeSplBuffer(splBuffer, c);
  myfree(splBuffer);
  #else
  freeSplBuffer(splBuffer, c);
  free(splBuffer);
  #endif
  splBuffer=NULL;
}


void processEscape(Node * node,HDC *hdc)
{
  int x=0;
  int lenParams=0;
  char bufferParams[100]; // buffer para guardar los parametros
  char endEscape;        // caracter de final de secuencia          
  while(node->s[x] != '\0')
    {
      if(node->s[x] == 0x1b)
	{
	  x++;
	  if(node->s[x] == '[')
	    { /// secuencia de escape con parametros
	      /// extraer los parametros
	      int ofset = x++;
	      
	      while(!isalpha(node->[x]))
		{
		  lenParams++;
		  x++;
		}
	      endEscape = node->s[x];
	      memcpy(bufferParams, node->s[ofset],lenParams);
	      processFuncion(endEscape,params, arrFuncionVT100,hdc);
	    }
	  else
	    {
	      if(node->s[x] == 0x1b)
		{
		    /// secuencia sin parametros
		  endEscape = node->s[x+1];
		  processFuncion(endEscape,NULL, arrFuncionVT100,hdc);
		}
	      else
		{
		  // secuencias de control como \n \r \t
		  processFuncion(node->s[x],NULL, arrFuncionVT100,hdc);
		  
		}
	    
	    }
	  
	}
      
    }
  
}


#define isEsc(node) ((node)->s[0] == 0x1b)
void write(HDC memdc)
{
  Node * current= head;

  while(current)
    {
      if(isEsc(current)) // pregunta si es una secuencia de escape
	{
	  processEscape(current,&memdc);
	}
      else
	{
	  /// caso contrario es una palabra
	  drawFont(&memdc,current->s,cursor);
	}
      current=(Node *) current->next;
      
    }
  
}


int main()
{
  const char *testStr = "\x1b[31mHello\x1b[0m World\x1b[1m!\x1b[0m This is a \x1b[4mtest\x1b[0m string.";

  while(1)
    {
  appendBuffer(testStr,strlen(testStr));
  freeBuffer();// libera el buffer principal
  printList(); // imprimee la lista de seguimiento  de memoria para saber si esta vacia
  freeAll(); // libera la lista de seguimeinto de memoria
    }
  return 0;
}
