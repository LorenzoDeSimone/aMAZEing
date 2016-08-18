//Lorenzo De Simone N86/1008

#include <stdlib.h>
#include <stdio.h>
#include "loading.h"

/*----------------------------------------------------*/
/*                     LOADING                        */
/*----------------------------------------------------*/
struct level *create_level(struct parsed_level *p_lv, struct maze *m, struct player_data *data)//Crea una struttura level con le informazioni ricavate dal parser
{
  struct level *lv=(struct level*) malloc(sizeof(struct level));
  struct parsed_guard *p_g;
  struct guard *g;
  struct item *k;
  struct player *p;
  int i;
  
  init_amazeing(m);//Il labirinto generico della libreria maze.c viene riempito con le strutture specifiche di questo gioco. Restituisce il numero di corridoi
  lv->m=m;
  lv->status=PLAYING;
  lv->time_left=p_lv->time;
  lv->n_keys=p_lv->n_guards;
  lv->bonus_tot=NULL;
  lv->malus_tot=NULL;
  lv->guards_tot=NULL;  
  lv->info_win=NULL;
  lv->log_win=NULL;
  lv->maze_win=NULL;
  lv->p=NULL;
  
  //Verifica della posizione del giocatore
  if(get_square(lv,p_lv->start->y,p_lv->start->x))
  {
    lv->start_y=p_lv->start->y;
    lv->start_x=p_lv->start->x;
    p=create_player(p_lv->start->y,p_lv->start->x);  
    if(data)//Se data!=NULL, vengono modificati gli attributi del giocatore
    {
      p->points=data->points;
      p->lives=data->lives;
    }	 
    add_object(lv,p,player);//Aggiungo il giocatore a tutte le strutture del livello
  }
  else
  {
    mvprintw(LINES/2,(COLS-31)/2,"Invalid player position (%d,%d)",p_lv->start->y,p_lv->start->x);
    free_level(lv);
    return NULL;
  }           
  
  //Verifica della posizione dell'uscita          
  if(get_square(lv,p_lv->exit->y,p_lv->exit->x))
  {
    lv->exit_y=p_lv->exit->y;
    lv->exit_x=p_lv->exit->x;
  }
  else
  {
    mvprintw(LINES/2,(COLS-29)/2,"Invalid exit position (%d,%d)",p_lv->exit->y,p_lv->exit->x);
    free_level(lv);
    return NULL;
  }
  
  //Inserimento di tutte le guardie nel livello
  for(i=0;i<lv->n_keys;i++)
  {
    p_g=get_head(p_lv->guard_list);//Ottengo la guardia (di tipo parsed_guard) in testa alla lista
    p_lv->guard_list=delete_head(p_lv->guard_list);//Avanzo nella lista       
    
    //Verifica la coerenza fra i due file di testo
    if(get_square(lv,p_g->pos->y,p_g->pos->x))
    {
      g=create_guard(char_to_guard_type(p_g->type),p_g->pos->y,p_g->pos->x);//Creo una guardia con le info appena lette dal file
      add_object(lv,g,guard);//Aggiungo la guardia appena creata a tutte le strutture del livello
      k=create_item(p_g->pos->y,p_g->pos->x,key);//Creo una guardia con le info appena lette dal file
      add_object(lv,k,item);//Aggiungo la chiave appena creata a tutte le strutture del livello
    }
    else//Non appena si trova un oggetto in posizioni non valide del labirinto, si dealloca il livello finora costruito e si restituisce NULL
    {
      mvprintw(LINES/2,(COLS-29)/2,"Invalid guard position (%d,%d)",p_g->pos->y,p_g->pos->x);
      free_level(lv);
      return NULL;
    }
  }       
  return lv;
}

struct level *load_custom_level(struct player_data *data, char *level_path, char *maze_path)//Caricamento di un livello dai path in input
{
   FILE *file;
   struct parsed_level* p_lv;
   struct level *lv=NULL;
   struct maze *m;
   struct player *p;
   
   if(file=fopen(level_path,"rb"))
   {
     if(p_lv=amazeing_parse_level(file))
     {
       if(m=read_maze(maze_path))
       {
         if(m->height<=max_height && m->width<=max_width)
         {
           if(m->height>=min_height && m->width>=min_width)
           {
             if(is_connected(m))
             {
               lv=create_level(p_lv,m,data);
               free_parsed_level(p_lv);
             }
             else
             {
               free_maze(m);
               mvprintw(LINES/2,(COLS-33)/2,"Maze is not a connected component");
             }
           }
           else
           {
             free_maze(m);
             mvprintw(LINES/2,(COLS-17)/2,"Maze is too small");
           }
         }
         else
         {
           free_maze(m);
           mvprintw(LINES/2,(COLS-15)/2,"Maze is too big");
         }
       }
     }
     fclose(file);
   }
   else
     mvprintw(LINES/2,(COLS-20)/2,"Level file not found");
   
 return lv;  
}

struct level *load_level(struct player_data *data, int lv_n)//Funzione di caricamento del livello indicato in input
{
  char *maze_path=NULL, *level_path=NULL;
  struct level *lv=NULL;
  
  if(lv_n<1000)//999 é il livello massimo (per evitare errori di buffering del path)
  {
    level_path=calloc(strlen("_level_999.txt"),sizeof(char));//Path delle info del livello
    sprintf(level_path,"_level_%d.txt",lv_n);
    maze_path=calloc(strlen("_maze_999.txt"),sizeof(char));//Path del labirinto
    sprintf(maze_path,"_maze_%d.txt",lv_n);
    lv=load_custom_level(data,level_path,maze_path);
    if(lv)
      lv->lv_n=lv_n;
  }
  else
    mvprintw(LINES/2,(COLS-21)/2,"Level number too high");
 
 free(maze_path);
 free(level_path);
 
 return lv;  
}

void free_level(struct level *lv)//Dealloca tutto ciò che è contenuto in un livello
{
  int i,j;
  void *curr_object;
  struct guard *g;
  if(lv)
  {
    //Deallocazione delle finestre grafiche
    if(lv->info_win)
      delwin(lv->info_win);
    if(lv->log_win)
      delwin(lv->log_win);
    if(lv->maze_win)
      delwin(lv->maze_win);
    
    free_object(lv->p,player);//Deallocazione del giocatore      
  
    while(lv->guards_tot)//Deallocazione delle guardie
    {
      g=get_head(lv->guards_tot);
      lv->guards_tot=delete_head(lv->guards_tot);
      free_guard_cache(lv,g);
      free_object(g,guard);      
    }
  
    while(lv->bonus_tot)//Deallocazione dei bonus
    {
      curr_object=get_head(lv->bonus_tot);
      lv->bonus_tot=delete_head(lv->bonus_tot);
      free_object(curr_object,trigger);      
    }
  
    while(lv->malus_tot)//Deallocazione dei malus
    {
      curr_object=get_head(lv->malus_tot);
      lv->malus_tot=delete_head(lv->malus_tot);
      free_object(curr_object,trigger);      
    }
  
    //Tutti gli oggetti puntati dai puntatori nelle caselle sono stati deallocati:
    //ora vanno deallocate solo tutte le caselle del gioco e la struttura generica
    //del labirinto ereditata dalla libreria generica "maze.h"
    for(i=0;i<lv->m->height;i++)
    {
      for(j=0;j<lv->m->width;j++)
      {
        free(get_square(lv,i,j));//Deallocazione della struct level_square del gioco aMAZEing
        free(lv->m->matrix[i][j]);//Deallocazione della locazione del labirinto generico contenitore
      }
      free(lv->m->matrix[i]);//Deallocazione della riga
    }
    free(lv->m);//Deallocazione della struct maze
    free(lv);//Deallocazione della struct level
  }
}

struct player_data *save_player_data(struct level *lv)//Crea una struct contenente le informazioni da passare al prossimo livello
{
  struct player_data *data=(struct player_data*) malloc(sizeof(struct player_data));
  data->lives=lv->p->lives;
  data->points=lv->p->points;
  data->lv_n=lv->lv_n;
  
  return data;  
}

/*LOADING RANDOM*/
struct level *load_random_level(struct player_data *data, int height, int width, int lv_n, FNRAND_MAZE rand_maze)//Carica un livello random usando la funzione di generazione in input
{ 
  struct maze *m=rand_maze(height,width);
  struct level *lv=(struct level*) malloc(sizeof(struct level));
  int y,x,i,n_guards;
  struct player *p;
  struct guard *g;
  struct item *k;
  guard_type type;
  //Il numero di guardie viene calcolato in base alla dimensione del labirinto. 
  //Si divide per (min_height*min_width), in modo tale da popolare il labirinto minimo con una guardia
  //e far sì che al crescere del labirinto, aumenti il numero di guardie
  n_guards=(height*width)/(min_height*min_width);
  
  init_amazeing(m);//Si inizializzano le caselle con le strutture del gioco aMAZEing
  
  //Vengono assegnati i vari campi della struct level
  lv->m=m;
  lv->status=PLAYING;
  lv->time_left=n_guards*60;//Viene dato al giocatore un minuto per guardia
  lv->n_keys=n_guards;
  lv->bonus_tot=NULL;
  lv->guards_tot=NULL;
  lv->malus_tot=NULL;
  lv->info_win=NULL;
  lv->log_win=NULL;
  lv->maze_win=NULL;
  lv->lv_n=lv_n;
  
  //Creazione dell'uscita
  y=rand()%m->height;
  x=rand()%m->width;
  if(!get_square(lv,y,x))
    add_reachable_square(lv,y,x);

  lv->exit_y=y;
  lv->exit_x=x;
  
  //Aggiunta del giocatore
  y=rand()%m->height;
  x=rand()%m->width;
  p=create_player(y,x);
  if(data)//Se data!=NULL, vengono modificati gli attributi del giocatore
  {
    p->points=data->points;
    p->lives=data->lives;
  }
  if(!get_square(lv,y,x))
    add_reachable_square(lv,y,x);   
  add_object(lv,p,player);  
  
  lv->start_y=y;
  lv->start_x=x;
  
  //Aggiunta delle guardie
  for(i=0;i<n_guards;i++)
  {    
    g=create_guard(rand()%n_guard_types,-1,-1);
    do
    {
      y=rand()%m->height;
      x=rand()%m->width;
      set_y(y,g,guard);
      set_x(x,g,guard);
      g->key_y=y;
      g->key_x=x;
      
      if(y==lv->exit_y && x==lv->exit_x)//Le coordinate devono essere diverse da quelle dell'uscita
        continue;
      if(check_range(lv,y,x,get_y(lv->p,player),get_x(lv->p,player),get_range(g,guard)))//Il giocatore deve essere fuori da ogni riquadro diinseguimento all'inizio del gioco  
        continue;
      if(check_guards(lv->guards_tot,y,x))//Le coordinate devono essere diverse da quelle delle altre guardie
        continue;
      break;
    }while(1);
    
    if(!get_square(lv,y,x))
      add_reachable_square(lv,y,x);
    add_hall(lv,y,x,(rand()%(3))+2);//Genera una "stanza" attorno ad una guardia di grandezza variabile da 2 a 4
    //Aggiunta di una guardia
    add_object(lv,g,guard);
    //Creazione ed aggiunta della sua chiave
    k=create_item(y,x,key);
    add_object(lv,k,item);    
  }
  return lv;
}

int check_guards(struct list *guard_list, int y, int x)//Verifica se le coordinate in input sono uguali a quelle di una guardia già generata
{
  struct guard *g;
  while(has_next(guard_list))
  {
    g=get_head(guard_list);
    if(y==g->key_y && x==g->key_x)
      return 1;
    else
      guard_list=next(guard_list);
  }
  return 0;
}

void add_reachable_square(struct level *lv, int y, int x)//Inserisce nel labirinto una nuova casella e la rende raggiungibile
{
  struct list *L=reach_path(lv->m,y,x);
  drill_walls(lv,L);
}

void drill_walls(struct level *lv, struct list *L)//Scava tutti i nodi della lista in input
{   
  struct pos *p;
  while(L)
  {
    p=get_head(L);
    add_level_square(lv,p->y,p->x);
    free(p);
    L=delete_head(L);    
  }
}

void add_hall(struct level *lv, int y, int x, int range)//Scava una "stanza" dalla posizione (y,x) di range in input
{
  struct list *L=nBFS_custom(lv->m,y,x,range,get_wall_adj);
  drill_walls(lv,L);
}

