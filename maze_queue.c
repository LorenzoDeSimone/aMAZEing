//Lorenzo De Simone N86/1008

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "maze_queue.h" 

struct pr_queue *init_pr_queue(int height, int width)
{
  struct pr_queue *queue;
  int i,j;
  
  queue=(struct pr_queue*) malloc(sizeof(struct pr_queue));
  queue->root=NULL;
  queue->heapsize=0;
  
  queue->map=(struct heap***) calloc(height,sizeof(struct heap**));
  
  for(i=0;i<height;i++)
    queue->map[i]=(struct heap**) calloc(width,sizeof(struct heap*));
  
  queue->mapheight=height;
  queue->mapwidth=width;
  
  return queue;
}

void free_pr_queue(struct pr_queue *queue)
{
  int i;
  
  for(i=0;i<queue->mapheight;i++)
    free(queue->map[i]);
  
  free(queue->map);
  
  free_heap(queue->root);
  free(queue);  
}

struct heap *init_heap_node(struct pos *p)
{
  struct heap *new_node=(struct heap*) malloc(sizeof(struct heap));
  
  new_node->info=p;
  new_node->priority=FLT_MAX;
  new_node->left=NULL;
  new_node->right=NULL;
  new_node->parent=NULL;
  
  return new_node;
}

void free_heap(struct heap *root)
{
  if(root)
  {
    free_heap(root->left);
    free_heap(root->right);
    free(root);
  }    
}

struct pos* init_pos(int y, int x)//Inizializza una struct pos con le coordinate in input
{
  struct pos *new_pos=(struct pos*) malloc(sizeof(struct pos));
  
  new_pos->y=y;
  new_pos->x=x;
  
  return new_pos;
}

void heapify(struct heap ***map, struct heap *root)//Esegue heapify sugli elementi di root
{
  struct heap *min, *temp;
  
  if(root)
  {
    if(root->left && root->left->priority<root->priority)
      min=root->left;
    else
      min=root;
    
    if(root->right && root->right->priority<min->priority)
      min=root->right;
    
    if(min!=root)
    {
      swap_heap(map,min,root);
      heapify(map,min);
    }
  }
}

void swap_heap(struct heap ***map, struct heap *node1, struct heap *node2)//Esegue lo swap dei dati fra due nodi
{
  struct heap *temp=(struct heap*) malloc(sizeof(struct heap)); 
  
  map[node1->info->y][node1->info->x]=node2;//Metto nello slot di node1 il puntatore a node2
  map[node2->info->y][node2->info->x]=node1;//Metto nello slot di node2 il puntatore a node1
  
  temp->info=node1->info;
  temp->priority=node1->priority;
  
  node1->info=node2->info;
  node1->priority=node2->priority;
  
  node2->info=temp->info;
  node2->priority=temp->priority;
  
  free(temp);
}

struct pos *get_min(struct pr_queue *queue)//Ritorna l'elemento con priorità minima
{
  if(queue && queue->root)
    return queue->root->info;
  else
    return NULL;
}

void increase_key_heap(struct heap ***map, struct heap *node, float new_priority)//Aumenta la priorità del dato contenuto nel nodo in input
{  
  if(new_priority>node->priority)
  {
    node->priority=new_priority;
    heapify(map,node);
  }
}

void increase_key(struct pr_queue *queue, struct pos *p, float new_priority)//Aumenta la priorità del dato
{ 
  if(queue->map[p->y][p->x])
    increase_key_heap(queue->map,queue->map[p->y][p->x],new_priority);  
}

void decrease_key(struct pr_queue *queue, struct pos *p, float new_priority)//Diminuisce la priorità del dato
{
  if(queue->map[p->y][p->x])
    decrease_key_heap(queue->map,queue->map[p->y][p->x],new_priority);  
}

struct pos *find_node(struct pr_queue *queue, struct pos *p)//Ritorna un puntatore al nodo nella coda a prioritá se esso é presente, NULL altrimenti
{
  if(queue->map[p->y][p->x])
    return queue->map[p->y][p->x]->info;
  else
    return NULL;
}

void decrease_key_heap(struct heap ***map, struct heap *node, float new_priority)//Diminuisce la priorità del dato contenuto nel nodo in input
{ 
  if(new_priority < node->priority)
  {
    node->priority=new_priority;
    while(node->parent && node->priority < node->parent->priority)
    {
      swap_heap(map,node,node->parent);
      node=node->parent;
    }
  } 
}

float get_priority(struct pr_queue *queue, struct pos *p)//Restituisce la priorità attuale del dato
{ 
  if(queue->map[p->y][p->x])
    return queue->map[p->y][p->x]->priority;
  else
    return -1;
}

void insert_key(struct pr_queue *queue, struct pos *p, float new_priority)
{
  struct heap *pred,*new_node;
  
  if(!queue->map[p->y][p->x])//Se è già presente un nodo con quelle coordinate, non va inserito
  {
    new_node=init_heap_node(p);
    queue->map[p->y][p->x]=new_node;//Inserisco nella mappa il puntatore al nuovo nodo
    queue->heapsize++;
   
    if(queue->root)
    {
      pred=find_node_bin(queue,queue->heapsize/2);//pred è il padre dell'ultima foglia (cioè il nodo che sto inserendo)
    
      if(pred)
      {
        if(!pred->left)
          pred->left=new_node;
        else if(!pred->right)
          pred->right=new_node;
        else
        {
          pred=queue->root;
	  while(pred->left) 
	    pred=pred->left;
	  pred->left=new_node;
        }
      }
      new_node->parent=pred;
      decrease_key_heap(queue->map,new_node,new_priority);
    }
    else
    {
      new_node->priority=new_priority;
      queue->root=new_node;  
    }
  }
}

void extract_min(struct pr_queue *queue)
{
  struct heap *pred,*last_leaf;
  
  if(queue->heapsize>1)
  {
    last_leaf=find_node_bin(queue,queue->heapsize);   
    
    if(last_leaf==last_leaf->parent->left)
      last_leaf->parent->left=NULL;
    else
      last_leaf->parent->right=NULL;
    
    if(last_leaf!=queue->root)
    {
       swap_heap(queue->map,queue->root,last_leaf);
       heapify(queue->map,queue->root);
    }
    queue->map[last_leaf->info->y][last_leaf->info->x]=NULL;
    free(last_leaf);
    
    queue->heapsize--;
  }
  else if(queue->heapsize==1)//Se è presente solo un nodo va semplicemente cancellata la radice
  {
    queue->map[queue->root->info->y][queue->root->info->x]=NULL;
    free(queue->root);
    queue->root=NULL;
    queue->heapsize--;
  } 
}

struct heap *find_node_bin(struct pr_queue *queue, int n)
{
  int k;
  struct heap *node=queue->root;
  k=sizeof(int)*8-1;//k è il numero di bit necessari a rappresentare un intero
  
  if(queue->heapsize!=0)
  {
    while((n&(1<<k--))==0);//Scarto tutti gli 0 iniziali
    
      while(k>=0) 
      {	  
        if((n&(1<<k--))==0)
          node=node->left;
        else
          node=node->right;
      }
  }  
  return node;
}