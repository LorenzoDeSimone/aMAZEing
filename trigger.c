//Lorenzo De Simone N86/1008

#include <stdlib.h>
#include <stdio.h>
#include "trigger.h"

/*----------------------------------------------------*/
/*                      TRIGGER                       */
/*----------------------------------------------------*/
//Tutte le struct di seguito sono trigger ed 
//appartengono alla categoria collectable: essi possono
//essere bonus o malus. 
//Per creare uno o l'altro basterá passare, oltre
//all'effetto desiderato, il trigger_type corretto

#define n_bonus 3//Numero di bonus attualmente implementate nel gioco
#define n_malus 4//Numero di malus attualmente implementate nel gioco
#define vanish_time 15//Tempo impiegato da un trigger per svanire

//Array globali utilizzati per la generazione random di trigger
FNEFFECT all_bonus[n_bonus];
FNEFFECT all_malus[n_malus];

struct trigger *create_trigger(int y, int x, trigger_type type, FNEFFECT effect)//Crea una struttura di tipo trigger: contiene una bonus o un malus a seconda di type
{
  struct trigger *new_trigger=(struct trigger*) malloc(sizeof(struct trigger));
  int c=ACS_BOARD;
  
  new_trigger->type=type;
  
  if(type==BONUS)
    new_trigger->super=create_printable(c,GREEN,y,x);
  else
    new_trigger->super=create_printable(c,RED,y,x);
  
  new_trigger->time_left=vanish_time;
  new_trigger->effect=effect;
  
  return new_trigger;
}

int is_bonus(struct trigger *t)//Ritorna 1 se il trigger è un bonus, 0 se è un malus
{
  if(t && t->type==BONUS)
    return 1;
  else
    return 0;
}

void set_time_left(long time, void *object, Class c)//Assegna il valore time al tempo rimanente del trigger in input
{
  if(c==trigger && time>=0)
    ((struct trigger*)object)->time_left=time;
}

long get_time_left(void *object, Class c)//Ritorna il valore time_left del trigger in input
{
  if(c==trigger)
    return ((struct trigger*)object)->time_left;
}

void free_trigger(struct trigger *t)//Dealloca un trigger
{
  free(t->super);
  free(t);
}

struct mod *create_mod(long time, FNEFFECT effect)//Crea una struttura di tipo mod
{
  struct mod *new_mod=(struct mod*) malloc(sizeof(struct mod)); 
  new_mod->expire_time=time;  
  new_mod->effect=effect;
}

int has_expired(struct mod *m)//Diminuisce di uno il tempo rimasto all'effetto e ritorna 1 se deve svanire, 0 altrimenti
{
  if(--m->expire_time)//Se ha un valore diverso da 0, l'effetto non deve svanire
    return 0;
  else
    return 1;    
}

void add_bonus(struct mod *new_mod, void *object, Class c)//Aggiunge un bonus al personaggio in input
{
  struct character *curr_character=get_character(object,c);
  if(curr_character)  
    curr_character->bonus=insert_head(curr_character->bonus,new_mod);
}

void init_all_bonus()//Inizializza l'array contenente puntatori a tutti i bonus implementati
{
  all_bonus[0]=haste;
  all_bonus[1]=range_bonus;
  all_bonus[2]=cure;
}

FNEFFECT rand_bonus()//Ritorna un puntatore alla funzione di un bonus casuale
{
  return all_bonus[rand()%n_bonus];    
}

void add_malus(struct mod *new_mod, void *object, Class c)//Aggiunge un malus al personaggio in input
{
  struct character *curr_character=get_character(object,c);
  if(curr_character)  
    curr_character->malus=insert_head(curr_character->malus,new_mod);
}

void init_all_malus()//Inizializza l'array contenente puntatori a tutti i malus implementati
{
  all_malus[0]=slow;
  all_malus[1]=range_malus;
  all_malus[2]=curse;
  all_malus[3]=teleport;
}

FNEFFECT rand_malus()//Ritorna un puntatore alla funzione di un malus casuale
{
  return all_malus[rand()%n_malus];    
}

/*                 BONUS IMPLEMENTATI                 */

/*HASTE*/
char *haste(struct level *lv, void *object, Class c, trigger_mode mode)//Aumenta la velocitá di un personaggio
{
  struct mod *new_mod;
  
  if(mode==ON)
  {
    new_mod=create_mod(15,haste);//Crea il nuovo modificatore
    add_bonus(new_mod,object,c);//Lo aggiunge alla lista dei bonus del personaggio
    set_speed(get_speed(object,c)+5,object,c);//Aumenta la velocità del personaggio
  }
  else if(mode==OFF)
    set_speed(get_speed(object,c)-5,object,c);//Rimuove il bonus di velocità
  
  return (char*)__func__;
}

/*RANGE BONUS*/
char *range_bonus(struct level *lv, void *object, Class c, trigger_mode mode)//Aumenta il range di un personaggio
{
  struct mod *new_mod;
  
  if(mode==ON)
  {
    new_mod=create_mod(10,range_bonus);//Crea il nuovo modificatore
    add_bonus(new_mod,object,c);//Lo aggiunge alla lista dei bonus del personaggio
    set_range(get_range(object,c)+3,object,c);//Aumenta il range del personaggio
  }
  else if(mode==OFF)
    set_range(get_range(object,c)-3,object,c);//Aumenta il range del personaggio
  
  return (char*)__func__;
}

/*CURE*/
char *cure(struct level *lv, void *object, Class c, trigger_mode mode)//Rimuove tutti i malus da un personaggio
{
  struct mod *m;
  struct list *L;
  
  if(mode==ON)
  {
    L=get_malus(object,c);
    while(L)
    {
      m=(struct mod*)get_head(L);
      L=delete_head(L);
      m->effect(lv,object,player,OFF);
      free(m);
    }
    set_malus(NULL,object,c);
  }
  return (char*)__func__;
}

/*                 MALUS IMPLEMENTATI                 */

/*SLOW*/
char *slow(struct level *lv, void *object, Class c, trigger_mode mode)//Diminuisce la velocitá di un personaggio
{
  struct mod *new_mod;
    
  if(mode==ON)
  {
    new_mod=create_mod(15,slow);//Crea il nuovo modificatore
    add_malus(new_mod,object,c);//Lo aggiunge alla lista dei malus del personaggio
    set_speed(get_speed(object,c)-5,object,c);//Diminuisce la velocità del personaggio
  }
  else if(mode==OFF)
    set_speed(get_speed(object,c)+5,object,c);//Rimuove il malus

  return (char*)__func__;
}

/*RANGE MALUS*/
char *range_malus(struct level *lv, void *object, Class c, trigger_mode mode)//Diminuisce il range di un personaggio
{
  struct mod *new_mod;
  
  if(mode==ON)
  {
    new_mod=create_mod(10,range_malus);//Crea il nuovo modificatore
    add_malus(new_mod,object,c);//Lo aggiunge alla lista dei malus del personaggio
    set_range(get_range(object,c)-3,object,c);//Diminuisce il range del personaggio
  }
  else if(mode==OFF)
    set_range(get_range(object,c)+3,object,c);//Rimuove il malus
  
  return (char*)__func__;
}

/*CURSE*/
char *curse(struct level *lv, void *object, Class c, trigger_mode mode)//Rimuove tutti i bonus da un personaggio
{
  struct mod *m;
  struct list *L;
  
  if(mode==ON)
  {
    L=get_bonus(object,c);
  
    while(L)
    {
      m=(struct mod*)get_head(L);
      L=delete_head(L);
      m->effect(lv,object,player,OFF);
      free(m);
    }
    set_bonus(NULL,object,c);
  }
  
  return (char*)__func__;
}

/*TELEPORT*/
char *teleport(struct level *lv, void *object, Class c, trigger_mode mode)//Muove il personaggio in una posizione random del labirinto
{
  struct pos *r_pos;
  struct level_square *square; 
  if(mode==ON)
  {
    r_pos=random_pos(lv);
    if(r_pos)
    {
      clear_square(lv,object,c);//Cancella il carattere nella posizione precedente
      square=get_square(lv,r_pos->y,r_pos->x);//Ottiene il puntatore alla casella d'arrivo
      //Imposta le nuove coordinate del personaggio e ne annulla l'eventuale direzione
      set_y(r_pos->y,object,c); 
      set_x(r_pos->x,object,c);
      set_direction(SPACE,object,c);
      
      move_object(lv,object,c);//Il personaggio viene mosso
      print_square(lv,object,c);//Il personaggio viene stampato 
      free(r_pos);
      if(c==guard)
      {
        free_guard_cache(lv,object);//Se una guardia attiva un trigger, se aveva una cache per un percorso essa non è più valida e va deallocata
        ((struct guard*)object)->status=RETREAT;//Impone una transizione per rendere coerente il comportamento dell'automa
        //Se ha la chiave, rimane nello status retreat e ci ritorna, se non ce l'ha, cambierà immediatamente in backup seguendo il normale diagramma di comportamento:
        //Se vede il giocatore, lo inseguirà normalmente
      }
    }
  }
  return (char*)__func__;
}
