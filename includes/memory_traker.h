#ifndef MEMORY_TRAKER_H
#define MEMORY_TRAKER_H
#define windows_include 1


typedef struct
{
  void * memoryLocation;
  int size;
  char file[100];
  char func[100];
  struct Nnodo *pNodoNext;
}Nnodo;

typedef struct
{
  Nnodo * pNodo;  // puntero nodo
  int nCapacity; // numero de nodos que tiene la lista
  Nnodo * pFirstNodo; // apunta al primer nodo
  Nnodo * pEndNodo;  // apunta la ultimo nodo
}Lista;

Nnodo * new(void * memLocation,int size,const char * file, const char *func);
void printList();
void * trackMalloc(int size, const char * nameFunc, const char * nameFile);
void deleteNodo(Nnodo * prev, Nnodo * findNodo);
void * trackRealloc(void * prevMemory,int newSize,const char *func,const char *file);
void trackFree(void * memLocation);
void freeAll();


#define PRINTD(str)({\
      char buffer[100];\
      snprintf(buffer,sizeof((str)),"%s",(str));	\
      if(windows_include)				\
      OutputDebugStringA(buffer);\
      else\
	printf("%s",buffer);			\
    })

#define mymalloc(size,func,file)(trackMalloc((size),(func),(file)))
#define myrealloc(prevmem,newsize,func,file)(trackRealloc((prevmem),(newsize),(func),(file)))
#define myfree(mem)(trackFree((mem)))


#endif
