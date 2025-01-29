typedef struct
{
  wchar_t *s;  /// cadena
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

Node *createNode(const wchar_t *s2, int len)
{
  #if debug
  Node * nodo=(Node *)mymalloc(sizeof(Node),__func__, __FILE__);
  #else
  Node * nodo=(Node *)malloc(sizeof(Node));
  #endif
  #if debug
  nodo->s = (wchar_t *)mymalloc(sizeof(wchar_t)*(len+1),__func__, __FILE__);
  #else
  nodo->s = (wchar_t *)malloc(sizeof(wchar_t)*(len+1));
  #endif
  if(nodo->s == NULL) return NULL;
  wcsncpy(nodo->s,s2,len);
  nodo->s[len]=L'\0';
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


void appendNode(const wchar_t *s, int len)
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
      wprintf(L"Nodo [%p]\n"
	     "Candena [%s]\n"
	     "Next [%p]\n",current,current->s,current->next);

      current =(Node *) current->next;
      
    }
  wprintf(L"Count %d\n",count);
}

int lenEscape(const wchar_t *s)
{
   int len=0;
   int x=0;
   while(s[x] != L'\0')
    {
      if(s[x] == 0x1b)
	{
	  len++;
	  x++; // contamos el caracter de incio de secuencia
	  if(s[x] == L'[')
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
	  while(!iswalpha(s[x]))
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
#define splitBuffer wchar_t **

splitBuffer spliteEcapeAndWord(const wchar_t *s, int *countOut)
{
  splitBuffer splBuffer = NULL;
  int splIndex = 0;
  int x = 0;
  int len = 0;
  while (s[x] != L'\0')
    {
      if (s[x] == 0x1b)
	{ // Inicio de una secuencia de escape
	  len = lenEscape(&s[x]);
	  #if debug
	  splBuffer = (splitBuffer)myrealloc(splBuffer, sizeof(wchar_t *) * (splIndex + 1),__func__, __FILE__);
	  splBuffer[splIndex] = (wchar_t *)mymalloc(sizeof(wchar_t) * (len + 1),__func__, __FILE__);
	  #else
	  splBuffer = (splitBuffer)realloc(splBuffer, sizeof(wchar_t *) * (splIndex + 1));
	  splBuffer[splIndex] = (wchar_t *)malloc(sizeof(wchar_t) * (len + 1));
	  #endif


	  wcsncpy(splBuffer[splIndex], &s[x], len);
	  splBuffer[splIndex][len] = L'\0';
          x += len; // Avanzar el índice en la cadena original
          splIndex++;
	}
      else
	{
	  
	  // Es una palabra o frase
	  len = 0;
	  int start = x; // Marca el inicio de la palabra
	  while (iswprint(s[x]) && s[x] != L'\0' && s[x] != 0x1b)
	    {
                len++;
                x++;
            }
	  if (len > 0)
	    {
	      #if debug
	      splBuffer = (splitBuffer)myrealloc(splBuffer, sizeof(wchar_t *) * (splIndex + 1),__func__, __FILE__);
	      splBuffer[splIndex] = (wchar_t *)mymalloc(sizeof(wchar_t) * (len + 1),__func__, __FILE__);
	      #else
	      splBuffer = (splitBuffer)realloc(splBuffer, sizeof(wchar_t *) * (splIndex + 1));
	      splBuffer[splIndex] = (wchar_t *)malloc(sizeof(wchar_t) * (len + 1));
	      #endif
	      wcsncpy(splBuffer[splIndex], &s[start], len);
	      splBuffer[splIndex][len] = L'\0';
	      splIndex++;
            }
	  else
	    {
	      len=0;
	      int start = x;
	      #if debug
	      splBuffer = (splitBuffer)myrealloc(splBuffer, sizeof(wchar_t *) * (splIndex + 1),__func__, __FILE__);
	      splBuffer[splIndex] = (wchar_t *)mymalloc(sizeof(wchar_t) * (len + 1),__func__, __FILE__);
	      #else
	      splBuffer = (splitBuffer)realloc(splBuffer, sizeof(wchar_t *) * (splIndex + 1));
	      splBuffer[splIndex] = (wchar_t *)malloc(sizeof(wchar_t) * (len + 1));
	      #endif
	      wcsncpy(splBuffer[splIndex], &s[start], (len+2));
	      splBuffer[splIndex][len+1] = L'\0';
	      splIndex++;
	      x++;

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
void appendBuffer(const wchar_t *s, int len)
{
  int c;
      splitBuffer splBuffer= spliteEcapeAndWord(s, &c);
  for(int x=0; x < c; x++) appendNode(splBuffer[x], wcslen(splBuffer[x]));

  #if debug
  freeSplBuffer(splBuffer, c);
  myfree(splBuffer);
  #else
  freeSplBuffer(splBuffer, c);
  free(splBuffer);
  #endif
  splBuffer=NULL;
}


