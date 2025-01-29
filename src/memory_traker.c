#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\..\Vt\includes\memory_traker.h"


Lista list={0}; // cabeza de la lista enlazada
int b= windows_include;

Nnodo * new(void * memLocation,int size,const char * file, const char *func)
{
  
  Nnodo * new= NULL;
  new= (Nnodo *)malloc(sizeof(Nnodo));
  
  if(!new)
    {
      // PRINTD("ERRO AL ASIGNAR LA MEMORIA \n");
	 return NULL;
      
    } 
  new->memoryLocation= memLocation;
  new->size= size;
  sprintf(new->file,"Archivo: %s",file);
  sprintf(new->func,"Funcion: %s",func);
  new->pNodoNext= NULL;
  return new;
}

void printList()
{
  if(list.nCapacity != 0)
    {
       Nnodo *last = list.pNodo;
       char buffer[256];
       int totSize=0;
       int i=0;
       while(last!= NULL)
	 {
	   totSize+=last->size;
	   snprintf(buffer,sizeof(buffer),
		   "---------------------------------------\n"
		   "* Nodo [%d]\n"
		   "* Memory Location [%p]\n"
		   "* Size [%d]\n"
		   "* File [%s]\n"
		   "* Func [%s]\n"
		   "* Next [%p]\n"
		   "---------------------------------------\n",
		   i,
		   last->memoryLocation,
		   last->size,
		   last->file,
		   last->func,
		   last->pNodoNext);
	   #if windows_include
	   OutputDebugStringA (buffer);
	   #else
	   printf("%s",buffer);
	   #endif
	   last = (Nnodo *)last->pNodoNext;
	    i++;
	 }
       char strMem[100];
       snprintf(strMem,sizeof(strMem),
		"Memory size: %d bytes \n",totSize);
           #if windows_include
           OutputDebugStringA (strMem);
	   #else
	   printf("Memory size: %d bytes\n",totSize);
	   #endif
	  
    }
  else
    {
      #if windows_include
       OutputDebugStringA ("La lista esta vacia");
      #else
      PRINTD("LA LISTA ESTA VACIA \n");
      #endif
    }
}

void * trackMalloc(int size, const char * nameFunc, const char * nameFile)
{
  void *ptr= NULL;

  ptr=malloc(size);
  if(!ptr)
    {
      // PRINTD("ERRO AL ASIGNAR LA MEMORIA \n");
    }
  Nnodo * newNodo = new(ptr,size,nameFile,nameFunc);
  if(list.nCapacity == 0)
    {
      list.pFirstNodo = list.pNodo = newNodo;
      list.pEndNodo=newNodo;
    }
  else
    {
      list.pEndNodo->pNodoNext =(struct Nnodo *) newNodo;
      list.pEndNodo = newNodo;
      
    }
  list.nCapacity++;
  return ptr;
}


void deleteNodo(Nnodo * prev, Nnodo * findNodo)
{
  if(prev == findNodo)
    {
      if((prev->pNodoNext != NULL)&&(findNodo->pNodoNext != NULL))
	{
	  // la lista tiene mas de un elemento
	  list.pNodo=list.pFirstNodo=(Nnodo *)list.pNodo->pNodoNext;
	  list.nCapacity--;
	  free(findNodo);
	  findNodo=NULL;
	}
      else
	{
	  // la lista solo tiene un elemento
	  free(findNodo);
	  list.pNodo=list.pFirstNodo=list.pEndNodo=NULL;
	  list.nCapacity--;
	  findNodo=NULL;
	}
    }
  else
    {
      if((prev->pNodoNext!= NULL) && (findNodo->pNodoNext != NULL))
	{
	  // prev esta en una posicion intermedia
	  prev->pNodoNext= findNodo->pNodoNext;
	  list.nCapacity--;
	  free(findNodo);
	  findNodo=NULL;
	}
      else
	{
	  if(findNodo->pNodoNext == NULL)
	    {
	      // findNodo es el ultimo de la lista
	      prev->pNodoNext= findNodo->pNodoNext;
	      list.pEndNodo=prev;
	      list.nCapacity--;
	      free(findNodo);
	      findNodo=NULL;
	    }
	}
    }
}


void * trackRealloc(void * prevMemory,int newSize,const char *func,const char *file)
{
  
  void *ptr= NULL;
  ptr = realloc(prevMemory,newSize);
  if(!ptr)
    {
      // PRINTD("ERRO AL ASIGNAR LA MEMORIA CON REALLOC() \n");
      
    }
  Nnodo *newNodo = new(ptr,newSize,file,func);
  
  /// eliminar el nodo prevei
  // Nnodo aux; // temporal guarda el nodo antes de eliminar
  if(prevMemory != ptr)
    {
      Nnodo * findNodo= list.pNodo;
      Nnodo * prev=findNodo;
     
      while(((findNodo) && (prevMemory != NULL)))
	{
	  if(findNodo->memoryLocation == prevMemory)
	    {
	      /// eliminar el nodo previo
	      // aux= *findNodo;
	      deleteNodo(prev, findNodo);
	      findNodo=NULL;
	    }
	  prev = findNodo;
	  if(findNodo!= NULL)findNodo=(Nnodo *)findNodo->pNodoNext;
	}
      /// agregar el nuevo nodo con los  nuevos datos
      if(list.nCapacity == 0)
	{
	  list.pNodo =list.pFirstNodo = list.pEndNodo =newNodo;
	}
      else
	{
	   list.pEndNodo->pNodoNext = (struct Nnodo *)newNodo;
	   list.pEndNodo = newNodo;
	}
      list.nCapacity++;
    }
 
  return ptr;
}


void trackFree(void * memLocation)
{
  Nnodo * findNodo= list.pNodo;
  Nnodo * prevNodo=NULL;
  prevNodo = findNodo;
  while(findNodo)
    {
      if(findNodo->memoryLocation == memLocation)
	{
	  
	  deleteNodo(prevNodo,findNodo);
	  free(memLocation); // elimina el puntero
	  break;
	}

       prevNodo= findNodo;
      findNodo= (Nnodo *)findNodo->pNodoNext;
    }
  if( list.nCapacity != 0)
    {
      #if windows_include
      OutputDebugStringA ("La lista aun no esta vacia");
      #else
      PRINTD("la lista aun no esta vacia \n");
      #endif
       //printList();
       Nnodo *last = list.pNodo;
       int mb=0;
       while(last)
	 {
	   mb+=last->size;
	   last=(Nnodo *) last->pNodoNext;
	   
	 }
       printf("Memoria por liberar %d\n",mb);
    }
  else
    {
       #if windows_include
      OutputDebugStringA ("La lista esta vacia");
      #else
      PRINTD("las lista esta vacia  \n");
      #endif
     
    }
  
}

void freeAll()
{
  Nnodo *current = list.pFirstNodo;
  Nnodo *next;
  while (current)
    {
      next = (Nnodo *)current->pNodoNext;
      free(current->memoryLocation);
      free(current);
      current = next;
    }
  list.pFirstNodo = list.pEndNodo = list.pNodo = NULL;
  list.nCapacity = 0;
}



/*
int main()
{
  
  char * memory = (char *)mymalloc(sizeof(char)*5,__func__,__FILE__);
  char * mem = (char *)mymalloc(sizeof(char),__func__,__FILE__);
  char * memm=(char *)mymalloc(sizeof(char)*10,__func__,__FILE__);
  char * newMem =(char *) myrealloc(memm,sizeof(char)*20,__func__,__FILE__);
  printList();
  printf("_________________________________________________________\n");
  freeAll();
  printList();

  return 0;
  
}
*/
