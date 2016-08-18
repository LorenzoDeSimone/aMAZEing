//Lorenzo De Simone N86/1008

#include "trigger.h"

/*----------------------------------------------------*/
/*                      PLAYER                        */
/*----------------------------------------------------*/

struct skill *create_skill_set();//Crea un array di struct skill

/*                 SKILL IMPLEMENTATE                 */
/*TIME SHOT*/
char *time_shot(struct level *lv);//Assegna il malus slow a tutte le guardie nel range del giocatore
/*SET TRAP*/
char *set_trap(struct level *lv);//Lascia dietro di se un malus con effetto slow
/*DRILL*/
char *drill(struct level *lv);//Rimuove un muro davanti ad un giocatore
/*ESCAPE*/
char *escape(struct level *lv);//Teletrasporta il giocatore nella locazione iniziale

struct player *create_player(int y, int x);//Crea un a struttura di tipo player con le statistiche iniziali
void free_player(struct player *p);//Dealloca un giocatore

void move_player(struct level *lv);//Muove il giocatore nel livello
void use_skill(struct level *lv, int input);//Prova ad usare un'abilità corrispondente al tasto in input
int step(struct level *lv, int input);//Prova a spostarsi nella direzione in input: 1 passo effettuato, 0 passo non effettuato