//Lorenzo De Simone N86/1008

#ifndef STRUCTURES_H
  #define STRUCTURES_H
  #include "structures.h"
#endif

/*----------------------------------------------------*/
/*                      TRIGGER                       */
/*----------------------------------------------------*/
struct trigger *create_trigger(int y, int x, trigger_type type, FNEFFECT effect);//Crea una struttura di tipo trigger: contiene una bonus o un malus a seconda di type
int is_bonus(struct trigger *t);//Ritorna 1 se il trigger è un bonus, 0 se è un malus
void set_time_left(long time, void *object, Class c);//Assegna il valore time al tempo rimanente del trigger in input
long get_time_left(void *object, Class c);//Ritorna il valore time_left del trigger in input
void free_trigger(struct trigger *t);//Dealloca un trigger

struct mod//Modificatore da aggiungere alla lista di effetti attivi del giocatore (andrà nella liste bonus e malus della struct character)
{
  long expire_time;//Indica il tempo rimasto, espresso in secondi, prima che l'effetto svanisca
  FNEFFECT effect;//Funzione che modifica lo stato del personaggio che raccoglie il trigger (viene richiamata nel gioco con il trigger mode OFF)) 
};

struct mod *create_mod(long time, FNEFFECT effect);//Crea una struttura di tipo mod
int has_expired(struct mod *m);//Diminuisce di uno il tempo rimasto all'effetto e ritorna 1 se deve svanire, 0 altrimenti

void add_bonus(struct mod *m, void *object, Class c);//Aggiunge un bonus al personaggio in input
void init_all_bonus();//Inizializza l'array contenente puntatori a tutti i bonus implementati
FNEFFECT rand_bonus();//Ritorna un puntatore alla funzione di un bonus casuale

void add_malus(struct mod *m, void *object, Class c);//Aggiunge un malus al personaggio in input
void init_all_malus();//Inizializza l'array contenente puntatori a tutti i malus implementati
FNEFFECT rand_malus();//Ritorna un puntatore alla funzione di un malus casuale

/*                 BONUS IMPLEMENTATI                 */

/*HASTE*/
char *haste(struct level *lv, void *object, Class c, trigger_mode mode);//Aumenta la velocitá di un personaggio
/*RANGE BONUS*/
char *range_bonus(struct level *lv, void *object, Class c, trigger_mode mode);//Aumenta il range di un personaggio
/*CURE*/
char *cure(struct level *lv, void *object, Class c, trigger_mode mode);//Rimuove tutti i malus da un personaggio

/*                 MALUS IMPLEMENTATI                 */

/*SLOW*/
char *slow(struct level *lv, void *object, Class c, trigger_mode mode);//Diminuisce la velocitá di un personaggio
/*RANGE MALUS*/
char *range_malus(struct level *lv, void *object, Class c, trigger_mode mode);//Aumenta il range di un personaggio
/*CURSE*/
char *curse(struct level *lv, void *object, Class c, trigger_mode mode);//Rimuove tutti i bonus da un personaggio
/*TELEPORT*/
char *teleport(struct level *lv, void *object, Class c, trigger_mode mode);//Muove il personaggio in una posizione random del labirinto