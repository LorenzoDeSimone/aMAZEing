//Lorenzo De Simone N86/1008

#ifndef STRUCTURES_H
  #define STRUCTURES_H
  #include "structures.h"
#endif

#define n_guard_types 4

/*----------------------------------------------------*/
/*                       GUARD                        */
/*----------------------------------------------------*/
typedef enum {A, B, C, D} guard_type;//Tipi di guardie

guard_type char_to_guard_type(int c);//Converte il char in input nel guard_type corrispondente
struct guard *create_custom_guard(int c, int y, int x, int range, int speed, int key_range, FNDIRECTION backup, FNDIRECTION chase, FNDIRECTION protect, FNDIRECTION retreat);//Crea una struttura di tipi guard con tutte le specifiche in input
struct guard *create_guard(guard_type type, int y, int x);//Crea una struct di tipo guard usando dei set predefiniti di caratteristiche
void free_guard_cache(struct level *lv, struct guard *g);//Dealloca la path_cache di una guardia
void free_guard(struct guard *g);//Dealloca una guardia
void change_status(struct level *lv, struct guard *g);//Modifica lo stato della guardia in base allo stato precedente e alla situazione di gioco
void move_guard(struct level *lv, struct guard *g);//Muove una guardia nel livello

/*               ALGORITMI AUSILIARI                  */
int get_dir(struct level *lv, struct pos*** pred, int start_y, int start_x, int end_y, int end_x);//Esaminando la matrice dei predecessori creata da un algoritmo di ricerca r
int random_dir();//Ritorna una direzione random
int random_valid_dir(struct level *lv, int start_y, int start_x);//Ritorna una direzione random valida
struct pos **get_careful_adj(struct maze *m, int y, int x);//Funzione di adiacenza che non considera adiacenti le caselle contenenti trappole
/*         ALGORITMI DI PROTEZIONE DELLA CHIAVE       */
int protect_random(struct level  *lv, struct guard *g);//Esegue un passo random rimanendo nel range di guardia
int protect_straight(struct level *lv, struct guard *g);//Sceglie una direzione random e la segue finchè è in key_range
int protect_careful(struct level *lv, struct guard *g);//Sceglie una direzione random e la segue finchè è in key_range, evitando però trappole
/*      ALGORITMI DI INSEGUIMENTO DEL PERSONAGGIO     */
int chase_weak(struct level *lv, struct guard *g);//Esegue un passo seguendo il percorso minimo verso il giocatore con qualche passo random
int chase_a_star(struct level *lv, struct guard *g);//Esegue un passo seguendo il percorso minimo verso il giocatore
int chase_careful(struct level *lv, struct guard *g);//Esegue un passo seguendo il percorso minimo verso il giocatore ma evitando le trappole
int intercept(struct level *lv, struct guard *g);////Esegue un passo seguendo il percorso minimo verso una previsione della posizione futura del giocatore
/*                 ALGORITMI DI BACKUP                */
int panic(struct level *lv, struct guard *g);//La guardia fa mosse casuali, simulando una sorta di panico
int steal_bonus(struct level *lv, struct guard *g);//Esegue un passo verso un bonus seguendo il percorso minimo verso di esso
int help_another_guard(struct level *lv, struct guard *g);//Modifica il valore della chiave della guardia in input con quella di un'altra. Ritorna sempre SPACE
/*               ALGORITMI DI RITIRATA                  */
int retreat_weak(struct level *lv, struct guard *g);//Algoritmo che fa intraprendere ad una guardia il percorso minimo verso la propria chiave con qualche passo random
int retreat_a_star(struct level *lv, struct guard *g);//Algoritmo che fa intraprendere ad una guardia il percorso minimo verso la propria chiave
int retreat_careful(struct level *lv, struct guard *g);//Algoritmo fa intraprendere ad una guardia il percorso minimo verso la propria chiave evitando le trappole