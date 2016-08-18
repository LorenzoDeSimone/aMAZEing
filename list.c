//Lorenzo De Simone N86/1008

#include <stdlib.h>
#include <stdio.h>
#include "list.h"

struct list *create_node(void* new_info)//Alloca un nuovo nodo con il campo info uguale al valore in input
{
   struct list *new_node;

   new_node=(struct list*) malloc (sizeof(struct list));  
   new_node->info=new_info;
   new_node->pred=NULL;
   new_node->next=NULL;
   return new_node;
}

struct list *insert_head(struct list *sentinel, void *new_info)//Inserisce un nodo in testa alla lista in input
{
  struct list* new_head;
  
  if(!sentinel)
  {
    sentinel=create_node(NULL);//Creazione nodo sentinella
    new_head=create_node(new_info);
    
    sentinel->pred=new_head;
    sentinel->next=new_head;  
    
    new_head->next=sentinel; 
    new_head->pred=sentinel;
  }
  else
  {
    new_head=create_node(new_info);
    
    new_head->next=sentinel->next;
    new_head->pred=sentinel;
    
    sentinel->next=new_head;
    new_head->next->pred=new_head;
  }
  
  return sentinel;
}

struct list *delete_head(struct list *sentinel)//Cancella un nodo dalla testa della lista in input
{
  struct list* node;
  
  if(sentinel)//Verifica che si sia un nodo sentinella allocato
  {
    if(node=sentinel->next)//Verifica che la lista effettiva non sia vuota
    {
      if(node==sentinel->pred)//Lista contenente un solo nodo
      {
	free(sentinel);
	free(node);
	sentinel=NULL;
      }
      else
      {
        sentinel->next=node->next;
        node->next->pred=sentinel;
	free(node);
      }  
    }
  }
  return sentinel;
}

struct list *insert_rear(struct list *sentinel, void *new_info)//Inserisce un nodo in coda alla lista in input
{
  struct list* new_rear;
  
  if(!sentinel)
  {
    sentinel=create_node(NULL);//Creazione nodo sentinella
    new_rear=create_node(new_info);
    
    sentinel->pred=new_rear;
    sentinel->next=new_rear;  
    
    new_rear->next=sentinel; 
    new_rear->pred=sentinel;
  }
  else
  {
    new_rear=create_node(new_info);
    new_rear->pred=sentinel->pred;
    new_rear->next=sentinel;
    
    sentinel->pred->next=new_rear;
    sentinel->pred=new_rear;
  }
  
  return sentinel;
}

struct list *delete_rear(struct list *sentinel)//Cancella un nodo dalla coda della lista in input
{
  struct list* node;
  
  if(sentinel)//Verifica che si sia un nodo sentinella allocato
  {
    if(node=sentinel->pred)//Verifica che la lista effettiva non sia vuota
    {
      if(node==sentinel->next)//Lista contenente un solo nodo
      {
	free(sentinel);
	free(node);
	sentinel=NULL;
      }
      else
      {
	node->pred->next=sentinel;
	sentinel->pred=node->pred;
	free(node);
      }  
    }
  }
  
  return sentinel;
}

void *get_head(struct list *sentinel)//Ritorna il puntatore all'info dell'elemento in testa
{
  if((sentinel)&&(sentinel!=sentinel->next))//Verifica che si sia la sentinella e che ci sia almeno un elemento nella lista effettiva
    return sentinel->next->info;
  else
    return NULL;
}

void *get_rear(struct list *sentinel)//Ritorna il puntatore all'info dell'elemento in coda
{
  if((sentinel)&&(sentinel!=sentinel->pred))//Verifica che si sia la sentinella e che ci sia almeno un elemento nella lista effettiva
    return sentinel->pred->info;
  else
    return NULL;
}

int has_next (struct list* sentinel)//Ritorna 1 se il nodo ha un successore, 0 altrimenti
{
  if(sentinel && sentinel->next->info)
    return 1;
  else
    return 0;
}

struct list *next(struct list *sentinel)//Ritorna il link al prossimo elemento della lista
{
  if(sentinel && sentinel->next->info)
    return sentinel->next;
  else
    return NULL;//Violazione del contratto
}