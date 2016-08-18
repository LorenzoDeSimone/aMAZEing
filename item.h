//Lorenzo De Simone N86/1008

#ifndef STRUCTURES_H
  #define STRUCTURES_H
  #include "structures.h"
#endif

struct item *create_item (int y, int x, FNITEM collect);//Crea una struttura di tipo item
void free_item(struct item *i);//Dealloca un oggetto
void init_all_item();//Inizializza l'array contenente puntatori a tutti gli oggetti implementati
FNITEM rand_item();//Ritorna un puntatore alla funzione di un oggetto casuale

/*                  ITEM IMPLEMENTATI                 */
/*LIFE*/
char *life(struct level *lv);//Aumenta di uno le vite del giocatore
/*POINTS*/
char *points(struct level *lv);//Aumenta di 40 i punti del giocatore
/*FEW_POINTS*/
char *few_points(struct level *lv);//Aumenta di 5 i punti del giocatore
/*SECONDS*/
char *seconds(struct level *lv);//Aumenta di 30 secondi il tempo a disposizione del giocatore
/*GOLDEN_PATH*/
void fill_golden_path(struct pos ***pred, struct level *lv, int start_y, int start_x, int end_y, int end_x);//Funzione ausiliaria a golden_path
char *golden_path(struct level *lv);//Se è presente la porta, lastrica il percorso verso la porta di items del tipo few_points. Se la porta non dovesse essere ancora comparsa, sceglie un bonus. Se non c'è alcun bonus, si comporta come l'item points