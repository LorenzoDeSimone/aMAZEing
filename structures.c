//Lorenzo De Simone N86/1008

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "structures.h"

#define range_max 10//Range massimo ai fini della giocabilità
/*----------------------------------------------------*/
/*                       LEVEL                        */
/*----------------------------------------------------*/
void init_amazeing(struct maze *m)//Riempie tutti i campi void* info del labirinto generico in maze.h con la struttura appropriata level_square
{
  int i,j;
  
  for(i=0;i<m->height;i++)
    for(j=0;j<m->width;j++)
      if(m->matrix[i][j])
        m->matrix[i][j]->square_info=create_level_square();
}

struct level_square *create_level_square()//Inizializza una struttura di tipo level_square
{
  struct level_square *new_square=malloc(sizeof(struct level_square));
  new_square->character=NULL;
  new_square->collectable=NULL;
  return new_square;
}

void add_level_square(struct level *lv, int y, int x)//Aggiunge una casella al livello in posizione (y,x)
{
  if(!get_square(lv,y,x))//Se la casella è già presente non bisogna scavare
  {
    add_vertex(lv->m,y,x);
    lv->m->matrix[y][x]->square_info=create_level_square();
  } 
}

void check_square(struct level *lv, void *object, Class c)//Gestisce tutti gli eventi che accadono entrando nella nuova casella
{
  int y=get_y(object,c),x=get_x(object,c);//Coordinate dell'oggetto in input
  struct level_square *square=get_square(lv,y,x);  
  
  if(!lv->p->invincibility)//Prima di fare le verifiche del caso per togliere eventualmente una vita al giocatore si controlla lo status di invincibilità
  {
    if(c==player && square->character && square->character_Class==guard)
      death(lv);
    else if(c==guard && square->character && square->character_Class==player)
      death(lv);
  }
  if(square->collectable)
    collect(lv,object,c,square->collectable,square->collectable_Class);
}

int has_trap(struct level_square *square)//Ritorna 1 se la casella contiene una trappola, 0 altrimenti
{
  if((square->collectable && square->collectable_Class==trigger && !is_bonus(square->collectable)))
    return 1;
  else
    return 0;
}

struct level_square* get_square(struct level *lv, int y, int x)//Ritorna la casella del livello in posizione (y,x), NULL in caso d'errore
{
  if(y<lv->m->height && x<lv->m->width && lv->m->matrix[y][x])
    return (struct level_square*)lv->m->matrix[y][x]->square_info;
  else
    return NULL;
}

struct level_square *mget_square(struct maze *m, int y, int x)//Ritorna la casella della matrice del livello in posizione (y,x), NULL in caso d'errore
{
  if(y<m->height && x<m->width && m->matrix[y][x])
    return (struct level_square*)m->matrix[y][x]->square_info;
  else
    return NULL; 
}

int check_range(struct level *lv, int start_y, int start_x, int end_y, int end_x, int range)//Ritorna 1 se end si trova nel quadrato di range a partire da start
{
  int dist_y=min(abs(end_y-start_y),start_y+lv->m->height-end_y);
  int dist_x=min(abs(end_x-start_x),start_x+lv->m->width-end_x);
  
  if(range>range_max)//Per questioni di giocabilità e costruzione di livelli random si impone un massimo sul range effettivo
    range=range_max;
  
  if(dist_y<=range && dist_x<=range)
    return 1;
  else
    return 0;
}

struct pos *random_pos(struct level *lv)//Genera una posizione libera random del labirinto: qualora il labirinto sia pieno, torna NULL
{
  int i,j,y,x;
  struct level_square *dest;
  
  //Generazione di coordinate random
  i=y=rand()%lv->m->height;
  j=x=rand()%lv->m->width;
 
  do
  {
    dest=get_square(lv,i,j);
    if(dest && !check_item(dest,key) && !check_item(dest,door))//Controlla se la posizione è valida e non ci sono nè chiavi nè porte
      return init_pos(i,j);
    if(++j==lv->m->width)
    {
      j=0;
      if(++i==lv->m->height)
        i=0;
    }
  }while((i!=y||j!=x));
  
  return NULL;//Nel caso estremo in cui si sia scorsa tutta la matrice senza trovare un corridoio libero, si ritorna NULL
}

/*----------------------------------------------------*/
/*                      CLASSI                        */
/*----------------------------------------------------*/
//Di seguito sono presenti tutte le classi implementate
//ed i loro metodi

/*----------------------------------------------------*/
/*                     PRINTABLE                      */
/*----------------------------------------------------*/
struct printable *create_printable(int c, int color, int y, int x)//Crea una struttura di tipo printable
{
  struct printable *new_p=(struct printable*) malloc(sizeof(struct printable));
  new_p->c=c;
  new_p->color=color;
  new_p->y=y;
  new_p->x=x;
  
  return new_p;
}

//Funzioni utilizzabili per comoditá con qualsiasi 
//oggetto implementato

void set_y(int y, void *object, Class c)//Assegna la y all'oggetto in input
{
  if(c==printable)
    ((struct printable*)object)->y=y;
  else if(c==character)
    ((struct character*)object)->super->y=y;
  else if(c==player)
    ((struct player*)object)->super->super->y=y;
  else if(c==guard)
    ((struct guard*)object)->super->super->y=y;
  else if (c==item)
    ((struct item*)object)->super->y=y;
  else if(c==trigger)
    ((struct trigger*)object)->super->y=y;
}

void set_x(int x, void *object, Class c)//Assegna la x all'oggetto in input
{
  if(c==printable)
    ((struct printable*)object)->x=x;
  else if(c==character)
    ((struct character*)object)->super->x=x;
  else if(c==player)
    ((struct player*)object)->super->super->x=x;
  else if(c==guard)
    ((struct guard*)object)->super->super->x=x;
  else if (c==item)
    ((struct item*)object)->super->x=x;
  else if(c==trigger)
    ((struct trigger*)object)->super->x=x;
}

int get_y(void *object, Class c)//Restituisce la y dell'oggetto in input
{
  if(c==printable)
    return ((struct printable*)object)->y;
  else if(c==character)
    return ((struct character*)object)->super->y;
  else if(c==player)
    return ((struct player*)object)->super->super->y;
  else if(c==guard)
    return ((struct guard*)object)->super->super->y;
  else if (c==item)
    return ((struct item*)object)->super->y;
  else if(c==trigger)
    return ((struct trigger*)object)->super->y;
}

int get_x(void *object, Class c)//Restituisce la x dell'oggetto in input
{
  if(c==printable)
    return ((struct printable*)object)->x;
  else if(c==character)
    return ((struct character*)object)->super->x;
  else if(c==player)
    return ((struct player*)object)->super->super->x;
  else if(c==guard)
    return ((struct guard*)object)->super->super->x;
  else if (c==item)
    return ((struct item*)object)->super->x;
  else if(c==trigger)
    return ((struct trigger*)object)->super->x;
}

int get_color(void *object, Class c)//Restituisce il colore dell'oggetto in input
{
  if(c==printable)
    return ((struct printable*)object)->color;
  else if(c==character)
    return ((struct character*)object)->super->color;
  else if(c==player)
    return ((struct player*)object)->super->super->color;
  else if(c==guard)
    return ((struct guard*)object)->super->super->color;
  else if (c==item)
    return ((struct item*)object)->super->color;
  else if(c==trigger)
    return ((struct trigger*)object)->super->color;
}

int get_char(void *object, Class c)//Restituisce il carattere dell'oggetto in input
{
  if(c==printable)
    return ((struct printable*)object)->c;
  else if(c==character)
    return ((struct character*)object)->super->c;
  else if(c==player)
    return ((struct player*)object)->super->super->c;
  else if(c==guard)
    return ((struct guard*)object)->super->super->c;
  else if (c==item)
    return ((struct item*)object)->super->c;
  else if(c==trigger)
    return ((struct trigger*)object)->super->c;
}

//Funzione per l'aggiunta ed il movimento dei personaggi nel livello
//Per muovere un personaggio basterà passarlo in input a queste funzioni dopo avere impostato le loro nuove coordinate
//con i metodi di set
void move_object(struct level *lv, void *object, Class c)//Muove un oggetto qualsiasi nel labirinto sovrascrivendo ciò che eventualmente c'era prima
{
  int y=get_y(object,c),x=get_x(object,c);//Coordinate dell'oggetto in input
  struct level_square *square=get_square(lv,y,x);
  
  //A seconda della classe in input si aggiornano i campi corretti della casella individuata
  if(c==player || c==guard)
  {
    square->character=object;
    square->character_Class=c;         
  }
  else if(c==item || c==trigger)
  {
    square->collectable=object;
    square->collectable_Class=c;    
  }
}

void remove_object(struct level *lv, void *object, Class c)//Rimuove un oggetto qualsiasi dal labirinto (Non lo dealloca)
{
  int y=get_y(object,c),x=get_x(object,c);//Coordinate dell'oggetto in input
  struct level_square *square=get_square(lv,y,x);
  
  //A seconda della classe in input si elimina il campo corretto della casella individuata
  if(c==player || c==guard)
    square->character=NULL;
  else if(c==item || c==trigger)
    square->collectable=NULL;
}

void add_object(struct level *lv, void *object, Class c)//Aggiunge un oggetto qualsiasi a tutte le strutture del gioco)
{    
  move_object(lv,object,c);//Inserisce l'oggetto nel labirinto
  //Inserisce l'oggetto nella struttura dati ausiliaria corretta discriminandoli in base al valore di c in input
  if(c==player)
    lv->p=object;
  else if(c==guard)
    lv->guards_tot=insert_head(lv->guards_tot,object);
  else if(c==trigger)
  {
    //Un'ulteriore distinzione è necessaria per bonus/malus
    if(is_bonus(object))
      lv->bonus_tot=insert_head(lv->bonus_tot,object);
    else
      lv->malus_tot=insert_head(lv->malus_tot,object);
  }
}

void free_object(void *object, Class c)//Dealloca un oggetto qualsiasi
{
  if(object)
  {
    if(c==printable)
      free(object);
    else if(c==character)
      free_character(object);
    else if(c==player)
      free_player(object);
    else if(c==guard)
      free_guard(object);
    else if (c==item)
      free_item(object);
    else if(c==trigger)
      free_trigger(object);
  }
}

//Funzioni per la gestione video degli oggetti
void print_square(struct level *lv, void *object, Class c)//Stampa un oggetto alla sua posizione nella finestra del labirinto
{
  wmove(lv->maze_win,get_y(object,c),get_x(object,c));
  wattron(lv->maze_win,COLOR_PAIR(get_color(object,c)));
  waddch(lv->maze_win,get_char(object,c));
  wattroff(lv->maze_win,COLOR_PAIR(get_color(object,c)));
}

void clear_square(struct level *lv, void *object, Class c)//Elimina il carattere di un oggetto dalla sua posizione corrente nella finestra del labirinto
{
  wmove(lv->maze_win,get_y(object,c),get_x(object,c));
  waddch(lv->maze_win,' ');
}

/*----------------------------------------------------*/
/*                     CHARACTER                      */
/*----------------------------------------------------*/
struct character *create_character(int c, int color, int y, int x, int range, int speed)//Crea una struttura di tipo character
{
  struct character *new_character=(struct character*) malloc(sizeof(struct character));
  
  new_character->super=create_printable(c,color,y,x);//Costruttore della superclasse
  new_character->bonus=NULL;//Lista dei bonus vuota
  new_character->malus=NULL;//Lista dei malus vuota
  new_character->delta=0;
  new_character->range=range;
  new_character->speed=speed;
  new_character->direction=SPACE;//Direzione di movimento del personaggio

  return new_character;
}

struct character *get_character(void *object, Class c)//Restituisce il puntatore alla superclasse di un giocatore/guardia
{
  if(c==character)
    return object;   
  if(c==player)
    return ((struct player*)object)->super;
  else if(c==guard)
    return ((struct guard*)object)->super;
  else
    return NULL;
}

double get_delta(void *object, Class c)//Ritorna il delta del personaggio in input
{
  return get_character(object,c)->delta;
}

int get_range(void *object, Class c)//Ritorna il range del personaggio in input
{
  return get_character(object,c)->range;
}

int get_speed(void *object, Class c)//Ritorna la velocità del personaggio in input
{
  return get_character(object,c)->speed; 
}

int get_direction(void *object, Class c)//Ritorna la direzione del personaggio in input
{
  return get_character(object,c)->direction;
}

struct list *get_bonus(void *object, Class c)//Ritorna la lista di bonus del personaggio in input
{
  return get_character(object,c)->bonus;    
}

struct list *get_malus(void *object, Class c)//Ritorna la lista di malus del personaggio in input
{
  return get_character(object,c)->malus;    
}

void set_delta(double delta, void *object, Class c)//Assegna il delta al personaggio in input
{
  get_character(object,c)->delta=delta;
}

void set_range(int range, void *object, Class c)//Assegna il range al personaggio in input
{
  get_character(object,c)->range=range;
}

void set_speed(int speed, void *object, Class c)//Assegna la velocità del personaggio in input
{
  get_character(object,c)->speed=speed; 
}

void set_direction(int direction, void *object, Class c)//Assegna la direzione del personaggio in input
{
  get_character(object,c)->direction=direction;    
}

void set_bonus(struct list *L,void *object, Class c)//Imposta lista di bonus del personaggio in input
{
  get_character(object,c)->bonus=L; 
}

void set_malus(struct list *L,void *object, Class c)//Imposta la lista di malus del personaggio in input
{
  get_character(object,c)->malus=L; 
}

void collect(struct level *lv, void *character, Class character_Class, void *collectable, Class collectable_Class)//Fa collezionare collectable al personaggio c
{  
  char *item_name;
    
  if(character_Class==player && collectable_Class==item)//Verifica se é il giocatore a prendere l'oggetto e l'oggetto è un item
  { 
    item_name=((struct item*)collectable)->collect(lv);//Si richiama l'apposita funzione callback per eseguire l'effetto dell'item
    update_log_win(lv,"The player collected",item_name,get_color(collectable,collectable_Class));
    remove_object(lv,collectable,collectable_Class);//L'item viene rimosso dal labirinto
    free_object(collectable,collectable_Class);
  }
  else if(collectable_Class==trigger)
  {
    item_name=((struct trigger*)collectable)->effect(lv,character,character_Class,ON);//Si richiama l'apposita funzione callback per eseguire l'effetto del trigger
    if(character_Class==player)
      update_log_win(lv,"The player triggered",item_name,get_color(collectable,collectable_Class));
    else
      update_log_win(lv,"One guard triggered",item_name,get_color(collectable,collectable_Class));
    remove_object(lv,collectable,collectable_Class);//L'item viene rimosso dal labirinto
  }  
}

void free_character(struct character *c)//Dealloca un personaggio implementato qualsiasi
{  
  free(c->super);
  while(c->bonus)//Deallocazione della lista di bonus
  {
    free(get_head(c->bonus));
    c->bonus=delete_head(c->bonus);
  }
  while(c->malus)//Deallocazione della lista di malus
  {
    free(get_head(c->malus));
    c->malus=delete_head(c->malus);
  }
  free(c);
}

int check_item(struct level_square *square, FNITEM i)//Verifica se nella casella in input c'è un oggetto di un certo tipo
{
  if(square->collectable && square->collectable_Class==item && ((struct item*)square->collectable)->collect==i)
    return 1;
  else
    return 0;
}

/*KEY*/
char *key(struct level *lv)//Aumenta di uno il numero di chiavi del giocatore e di 60 i suoi punti
{    
  struct item *d;
  struct level_square *square;
  
  lv->p->keys++;  
  lv->p->points+=60;
  if(lv->p->keys==lv->n_keys)//Verifica se il giocatore ha acquisito tutte le chiavi
  {
    d=create_item(lv->exit_y,lv->exit_x,door);//Viene creata l'uscita del livello
    square=get_square(lv,lv->exit_y,lv->exit_x);
    if(square->collectable)//Qualora ci fosse già un oggetto, esso va rimosso e deallocato
    {
      remove_object(lv,square->collectable,square->collectable_Class);
      if(square->collectable_Class==item)
        free_object(square->collectable,square->collectable_Class);
    }
    add_object(lv,d,item);//Viene aggiunta l'uscita del livello 
    print_square(lv,d,item);//Viene stampata l'uscita del livello
    lv->p->points+=lv->time_left;//I secondi rimanenti vengono tramutati in punti
  }
  return (char*)__func__;
}

/*DOOR*/
char *door(struct level *lv)//Fa superare il livello al giocatore
{
  lv->status=LEVEL_CLEARED;
  return (char*)__func__;
}