//Lorenzo De Simone N86/1008

#include "loading.h"
#include "menu.h"

typedef enum {ADVENTURE, SURVIVAL, SINGLE_GAME, CUSTOM} game_mode;//Modalità di gioco

/*----------------------------------------------------*/
/*                   GUI HANDLING                     */
/*----------------------------------------------------*/
void menu_amazeing();//Crea il menu di selezione dei livelli del gioco aMAZEing
void loading_screen(struct level* lv);//Schermata di caricamento del livello selezionato
void game_result(struct level *lv);//Stampa a video l'esito della partita
void print_amazeing_logo(int start_y, int start_x);//Funzione per la stampa del logo sopra al menu di selezione
void print_gameover_logo(int start_y, int start_x);//Stampa il logo di game over
void print_victory_logo(int start_y, int start_x);//Stampa il logo di vittoria
void create_maze_win(struct level *lv);//Inizializza la finestra di gioco disegnando il labirinto
void create_info_win(struct level *lv);//Inizializza la finestra delle info sul gioco
void create_log_win(struct level *lv);//Inizializza la finestra contenente il log delle azioni di gioco
void update_info_win(struct level *lv);//Esegue un refresh sulla finestra con le statistiche di gioco
void update_log_win(struct level *lv, char *action, char *name, int color);//Esegue un refresh sulla finestra con le azioni di gioco
/*----------------------------------------------------*/
/*                     GAMEPLAY                       */
/*----------------------------------------------------*/
void amazeing_gameplay(struct level *lv);//Fa partire il livello in input con i dati di un giocatore e la modalità di gioco scelta
void time_check(struct level *lv);//Se i secondi rimasti sono 0 e il giocatore ha ancora una vita, la rimuove e aggiunge 30 secondi. Altrimenti è gameover
int delta_time(double last, double now, void* object, Class c);//Calcola il nuovo delta del personaggio in input e restituisce 1 se il personaggio deve agire, 0 altrimenti
int has_vanished(struct trigger *t);//Diminuisce di uno il tempo rimasto al trigger sul terreno di gioco e ritorna 1 se deve scomparire, 0 altrimenti
struct list *vanish_handling(struct level *lv, struct list *L);//Gestisce la scadenza di tutti i trigger di una lista e restituisce una lista con tutti i trigger ancora presenti
void expire_handling(struct level *lv, void *object, Class c);//Gestisce gli effetti del personaggio e notifica lo svanire degli effetti nel log del gioco
void death(struct level *lv);//Rimuove una vita al giocatore e modifica coerentemente lo status del gioco
void add_random_collectable(struct level *lv);//Aggiunge un oggetto collectable casuale al terreno di gioco