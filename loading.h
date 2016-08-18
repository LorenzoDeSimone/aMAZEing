//Lorenzo De Simone N86/1008

#include "level_parser.h"
#include "guard.h"
#include "player.h"

//Dimensioni necessarie per evitare errori di GUI e/o giocabilità
#define max_height 20
#define max_width  98
#define min_height 10
#define min_width  45

/*----------------------------------------------------*/
/*                     LOADING                        */
/*----------------------------------------------------*/

struct player_data//Statistiche del personaggio da portare da un livello ad un altro
{
  int points;//Punti del giocatore
  int lives;//Vite del giocatore
  int lv_n;//Livello al quale è arrivato il giocatore
};

struct level *create_level(struct parsed_level *p_lv, struct maze *m, struct player_data *data);//Crea una struttura level con le informazioni ricavate dal parser
struct level *load_custom_level(struct player_data *data, char *level_path, char *maze_path);//Caricamento di un livello dai path in input
struct level *load_level(struct player_data *data, int lv_n);//Funzione di caricamento del livello indicato in input
void free_level(struct level *lv);//Dealloca tutto ciò che è contenuto in un livello
struct player_data *save_player_data(struct level *lv);//Crea una struct contenente le informazioni da passare al prossimo livello
/*LOADING RANDOM*/
//Il contratto di load_random_level prevede che la funzione rand_maze in input generi labirinti che sono componenti connesse: altrimenti il funzionamento non è garantito
//Nel gioco viene sempre usata la stessa funzione random_maze importata da maze.h, ma il funzionamento è garantito con qualsiasi funzione di generazione che rispetta il contratto
struct level *load_random_level(struct player_data *data, int height, int width, int lv_n, FNRAND_MAZE rand_maze);//Carica un livello random usando la funzione di generazione in input
int check_guards(struct list *guard_list, int y, int x);//Verifica se le coordinate in input sono uguali a quelle di una guardia già generata
void add_reachable_square(struct level *lv, int y, int x);//Inserisce nel labirinto una nuova casella e la rende raggiungibile
void drill_walls(struct level *lv, struct list *L);//Scava tutti i nodi della lista in input
void add_hall(struct level *lv, int y, int x, int range);//Scava una "stanza" dalla posizione (y,x) di range in input