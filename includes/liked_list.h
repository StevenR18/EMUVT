#ifndef LIKED_LIST_H
#define LIKED_LIST_H
typedef struct Nodo {
  void *data;
  struct Nodo *next;
} Nodo;

int insert_nodo_end(Nodo **head, void *data, size_t data_size);
int free_lista(Nodo **head);



#endif
