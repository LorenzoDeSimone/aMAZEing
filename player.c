//Lorenzo De Simone N86/1008

#include <stdlib.h>
#include <stdio.h>
#include "player.h"

/*----------------------------------------------------*/
/*                      PLAYER                        */
/*----------------------------------------------------*/

#define ESC        27
#define SPACE      32
#define n_skills 4//Numero di skill attualmente implementate nel gioco

struct skill *create_skill_set()//Crea un array di struct weapon
{
  struct skill *new_weapons=(struct skill*) calloc(n_skills,sizeof(struct skill));
  
  new_weapons[0].use=time_shot;
  new_weapons[0].cost=40;

  new_weapons[1].use=set_trap;
  new_weapons[1].cost=40;
  
  new_weapons[2].use=drill;
  new_weapons[2].cost=60;
  
  new_weapons[3].use=escape;
  new_weapons[3].cost=80;
  
  return new_weapons;
}

/*                 SKILL IMPLEMENTATE                 */

/*TIME SHOT*/
char *time_shot(struct level *lv)//Assegna il malus slow a tutte le guardie nel range del giocatore
{
  struct list *L=lv->guards_tot;
  struct guard *g;
  while(has_next(L))
  {
    g=get_head(L);
    if(check_range(lv,get_y(lv->p,player),get_x(lv->p,player),get_y(g,guard),get_x(g,guard),get_range(lv->p,player)))
      slow(lv,g,guard,ON);
    L=next(L);
  }
  return (char*)__func__;
}

/*SET TRAP*/
char *set_trap(struct level *lv)
{
  int y=get_y(lv->p,player),x=get_x(lv->p,player);
  struct trigger *tr;
  if(get_direction(lv->p,player)!=SPACE)//Per lasciare una trappola dietro di se il giocatore deve essere in movimento
  {
    if(get_direction(lv->p,player)==KEY_UP)
      y=((get_y(lv->p,player)+lv->m->height)+1)%lv->m->height;
    else if(get_direction(lv->p,player)==KEY_DOWN)
      y=((get_y(lv->p,player)+lv->m->height)-1)%lv->m->height;
    else if(get_direction(lv->p,player)==KEY_RIGHT)
      x=((get_x(lv->p,player)+lv->m->width)-1)%lv->m->width;    
    else if(get_direction(lv->p,player)==KEY_LEFT)
      x=((get_x(lv->p,player)+lv->m->width)+1)%lv->m->width;
  
    if(get_square(lv,y,x))
    {
      tr=create_trigger(y,x,MALUS,slow);
      add_object(lv,tr,trigger);
      print_square(lv,tr,trigger);
    }
    return (char*)__func__;
  }
  else
    return NULL;
}

/*DRILL*/
char *drill(struct level *lv)//Rimuove un muro davanti ad un giocatore
{
  int y=get_y(lv->p,player),x=get_x(lv->p,player);
  
  if(get_direction(lv->p,player)==KEY_UP)
    y=((get_y(lv->p,player)+lv->m->height)-1)%lv->m->height;
  else if(get_direction(lv->p,player)==KEY_DOWN)
    y=((get_y(lv->p,player)+lv->m->height)+1)%lv->m->height;
  else if(get_direction(lv->p,player)==KEY_RIGHT)
    x=((get_x(lv->p,player)+lv->m->width)+1)%lv->m->width;
  else if(get_direction(lv->p,player)==KEY_LEFT)
    x=((get_x(lv->p,player)+lv->m->width)-1)%lv->m->width;
  
  if(get_square(lv,y,x))
    return NULL;
  else
  {
    add_level_square(lv,y,x);
    return (char*)__func__;   
  }
}

/*ESCAPE*/
char *escape(struct level *lv)//Teletrasporta il giocatore nella locazione iniziale
{
  if(get_y(lv->p,player)==lv->start_y && get_x(lv->p,player)==lv->start_x)
    return NULL;
  else
  {
    set_y(lv->start_y,lv->p,player); 
    set_x(lv->start_x,lv->p,player);
    set_direction(SPACE,lv->p,player);  
    move_object(lv,lv->p,player);//Il giocatore viene mosso
    print_square(lv,lv->p,player);//Il giocatore viene stampato 
    return (char*)__func__;
  }
}

struct player *create_player(int y, int x)//Crea un a struttura di tipo player con le statistiche iniziali
{
  struct player *new_player=(struct player*) malloc(sizeof(struct player));
  new_player->super=create_character('@',STANDARD,y,x,4,15);//Costruttore della superclasse   
  new_player->skill_set=create_skill_set();//Inizializzazione dell'array con le armi
  new_player->keys=0;//Il giocatore inizia con 0 chiavi
  new_player->lives=5;//Il giocatore inizia con 3 vite
  new_player->points=120;//Il giocatore inizia con 120 punti
  new_player->turn=-1;//Memorizza l'intenzione di svoltare appena possibile nella direzione turn
  new_player->invincibility=3;//All'inizio del gioco il giocatore è invincibile per tre secondi
  return new_player;
}

void free_player(struct player *p)//Dealloca un giocatore
{
  free_character(p->super);
  free(p->skill_set);
  free(p);
}

void move_player(struct level *lv)//Muove il giocatore nel livello
{
  int input;

  remove_object(lv,lv->p,player);//Rimuove il riferimento al giocatore nella casella precedente al movimento
  clear_square(lv,lv->p,player);//Cancella dallo schermo il carattere del giocatore
  
  input=wgetch(lv->maze_win);//Lettura da input del comando utente
  flushinp();//Pulisce il buffer input
    
  check_square(lv,lv->p,player);//Controlla la propria casella prima di muoversi

  if(input==ESC)//Se il giocatore preme ESC, il gioco viene interrotto 
    lv->status=INTERRUPT;
  use_skill(lv,input); 
  
  //Modifica delle coordinate del personaggio
  if(!(step(lv,input)))//Tentativo di movimento con l'input
  { 
    if(!(step(lv,lv->p->turn)))//Tentativo di curva
      step(lv,get_direction(lv->p,player));//Tentativo di movimento con la direzione precedente
    else
      lv->p->turn=-1;//Se si riesce a curvare, la variabile va resettata a -1
  } 
  check_square(lv,lv->p,player);//Gestisce tutti gli eventi che accadono entrando nella nuova casella
  move_object(lv,lv->p,player);//Muove il giocatore nella nuova casella
  if(lv->p->invincibility)
    wattron(lv->maze_win,A_REVERSE);
  print_square(lv,lv->p,player);//Stampa il giocatore nella nuova casella
  wattroff(lv->maze_win,A_REVERSE);
}

void use_skill(struct level *lv, int input)//Prova ad usare un'abilità corrispondente al tasto in input
{
  int index=-1;
  char *skill_name;
 
  //Converte il tasto premuto in un indice dell'array delle abilità
  if(input=='1')//time_shot
    index=0;
  else if(input=='2')//set_trap
    index=1;
  else if(input=='3')//drill
    index=2;
  else if(input=='4')//escape
    index=3;
  
  if(index!=-1)
  {
    if(lv->p->points-lv->p->skill_set[index].cost>=0)
    {
      skill_name=lv->p->skill_set[index].use(lv);
      if(skill_name)
      {
        lv->p->points-=lv->p->skill_set[index].cost;
        update_log_win(lv,"The player used the skill",skill_name,STANDARD);
      }
      else
        update_log_win(lv,"The player's skill","had no effect",RED);
    }
    else
      update_log_win(lv,"Not enough points to use","the skill",RED);
  }
}

int step(struct level *lv, int input)//1 passo effettuato, 0 passo non effettuato
{  
  int n,ret=0;
  
  switch(input)
  {
    case SPACE :
      set_direction(input,lv->p,player);
      lv->p->turn=ERR;
      ret++;
    break;
    case KEY_UP :
      n=((get_y(lv->p,player)+lv->m->height)-1)%lv->m->height;
      if(get_square(lv,n,get_x(lv->p,player)))
      {
	set_y(n,lv->p,player);
        set_direction(input,lv->p,player);
	ret++;
      }
      else if(input!=get_direction(lv->p,player))
	lv->p->turn=input;
    break;
    case KEY_DOWN :
      n=((get_y(lv->p,player)+lv->m->height)+1)%lv->m->height;
      if(get_square(lv,n,get_x(lv->p,player)))
      {
	set_y(n,lv->p,player);
        set_direction(input,lv->p,player);
        ret++;
      }
      else if(input!=get_direction(lv->p,player))
	lv->p->turn=input;
    break;
    case KEY_RIGHT :
      n=((get_x(lv->p,player)+lv->m->width)+1)%lv->m->width;
      if(get_square(lv,get_y(lv->p,player),n))
      {
        set_x(n,lv->p,player);
        set_direction(input,lv->p,player);
	ret++;
      }
      else if(input!=get_direction(lv->p,player))
	lv->p->turn=input;
    break;
    case KEY_LEFT : 
      n=((get_x(lv->p,player)+lv->m->width)-1)%lv->m->width;
      if(get_square(lv,get_y(lv->p,player),n))
      {
        set_x(n,lv->p,player);
        set_direction(input,lv->p,player);
        ret++;
      }
      else if(input!=get_direction(lv->p,player))
	lv->p->turn=input;
    break;
  } 
  return ret;
}
