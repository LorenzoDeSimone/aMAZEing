//Lorenzo De Simone N86/1008

#include <curses.h>
#include "maze.h"

#define SPACE      32
#define STANDARD   1
#define BLUE       2
#define GREEN      3
#define RED        4
#define YELLOW     5

//Elenco di tutte le classi implementate
typedef enum {printable, character, player, guard, item, trigger} Class;
/*----------------------------------------------------*/
/*                       LEVEL                        */
/*----------------------------------------------------*/
//Struttura principale del livello del gioco aMAZEing

typedef enum {LEVEL_CLEARED, PLAYING, GAME_OVER, INTERRUPT} game_status;

struct level
{
  struct maze *m;//Labirinto 
  game_status status;//Stato corrente del gioco: 1 Livello superato, 0 gioco in corso, -1 Game Over, -2 Esc
  WINDOW *maze_win;//Finestra contenente il labirinto
  WINDOW *info_win;//Finestra contenente le informazioni sul gioco
  WINDOW *log_win;//Finestra contenente il log delle azioni avvenute nel gioco
  int n_keys;//Numero di chiavi rimaste nel livello
  int lv_n;//Numero del livello
  long time_left;//Tempo, espresso in secondi, rimasto al giocatore per terminare il livello 
  struct player *p;//Puntatore al giocatore
  struct list *guards_tot;//Lista contenente tutte le guardie nel gioco 
  struct list *bonus_tot;//Lista contenente tutte le struct trigger di tipo bonus sul terreno di gioco
  struct list *malus_tot;//Lista contenente tutte le struct trigger di tipo malus sul terreno di gioco
  int start_y;//y dell'entrata
  int start_x;//x dell'entrata
  int exit_y;//y dell'uscita
  int exit_x;//x dell'uscita
};

void init_amazeing(struct maze *m);//Riempie tutti i campi void* info del labirinto generico in maze.h con la struttura appropriata level_square

//Questa struct riempie il campo void* info del labirinto generico in maze.h
struct level_square
{
  Class character_Class;//Contiene l'informazione per dereferenziare character 
  void *character;//Puó essere un character qualsiasi implementato (player,guard)
  Class collectable_Class;//Contiene l'informazione per dereferenziare collectable
  void *collectable;//Puó essere un qualsiasi oggetto collectable implementato (item,trigger)
};

struct level_square *create_level_square();//Inizializza una struttura di tipo level_square
void add_level_square(struct level *lv, int y, int x);//Aggiunge una casella al livello in posizione (y,x))
void check_square(struct level *lv, void *object, Class c);//Gestisce tutti gli eventi che accadono entrando nella nuova casella
int has_trap(struct level_square *square);//Ritorna 1 se la casella contiene una trappola, 0 altrimenti
//Le funzioni sono equivalenti: una ricava la casella dal livello, l'altra direttamente dalla matrice
struct level_square *get_square(struct level *lv, int y, int x);//Ritorna la casella del livello in posizione (y,x), NULL in caso d'errore
struct level_square *mget_square(struct maze *m, int y, int x);//Ritorna la casella della matrice del livello in posizione (y,x), NULL in caso d'errore
int check_range(struct level *lv, int start_y, int start_x, int end_y, int end_x, int range);//Ritorna 1 se end si trova nel quadrato di range a partire da start
struct pos *random_pos(struct level *lv);//Genera una posizione libera random del labirinto: qualora il labirinto sia pieno, ritorna NULL
/*----------------------------------------------------*/
/*                      CLASSI                        */
/*----------------------------------------------------*/
//Di seguito sono presenti tutte le classi implementate
//ed i loro metodi

/*----------------------------------------------------*/
/*                     PRINTABLE                      */
/*----------------------------------------------------*/

struct printable//Generico elemento del gioco(Classe astratta)
{
  int c;//Carattere da stampare a video
  int color;//Serve a stampare il colore in ncurses
  int y;//y dell'oggetto stampabile
  int x;//x dell'oggetto stampabile
};

struct printable *create_printable(int c, int color, int y, int x);//Crea una struttura di tipo printable

void set_y(int y, void *object, Class c);//Assegna la y all'oggetto in input
void set_x(int x, void *object, Class c);//Assegna la x all'oggetto in input
int get_y(void *object, Class c);//Restituisce la y dell'oggetto in input
int get_x(void *object, Class c);//Restituisce la x dell'oggetto in input
int get_color(void *object, Class c);//Restituisce il colore dell'oggetto in input
int get_char(void *object, Class c);//Restituisce il carattere dell'oggetto in input

//Funzione per l'aggiunta ed il movimento dei personaggi nel livello
//Per muovere un personaggio basterà passarlo in input a queste funzioni dopo avere impostato le loro nuove coordinate
//con i metodi di set
void move_object(struct level *lv, void *object, Class c);//Muove un oggetto qualsiasi nel labirinto sovrascrivendo ciò che eventualmente c'era prima
void remove_object(struct level *lv, void *object, Class c);//Rimuove un oggetto qualsiasi dal labirinto (Non lo dealloca)
void add_object(struct level *lv, void *object, Class c);//Aggiunge un oggetto qualsiasi a tutte le strutture del gioco)
void free_object(void *object, Class c);//Dealloca un oggetto qualsiasi

//Funzioni per la gestione video degli oggetti
void print_square(struct level *lv, void *object, Class c);//Stampa un oggetto alla sua posizione nella finestra del labirinto
void clear_square(struct level *lv, void *object, Class c);//Elimina il carattere di un oggetto dalla sua posizione corrente nella finestra del labirinto
/*----------------------------------------------------*/
/*                     CHARACTER                      */
/*----------------------------------------------------*/

struct character//Generico personaggio del gioco(Classe astratta)
{ 
  struct printable *super;//puntatore alla superclasse element
  struct list *bonus;//Lista contenente puntatori a struct di tipo mod contenente i bonus attivi
  struct list *malus;//Lista contenente puntatori a struct di tipo mod contenente i malus attivi
  double delta;//Tempo passato dall'ultima azione del personaggio
  int range;//Raggio d'azione del personaggio(es. area d'effetto dei colpi di ritardo o altre armi per il giocatore, area di sorveglianza per una guardia)
  int speed;//Velocitá del personaggio
  int direction;//Direzione di movimento del personaggio
};

struct character *create_character(int c, int color, int y, int x, int range, int speed);//Crea una struttura di tipo character
struct character *get_character(void *object, Class c);//Restituisce il puntatore alla superclasse di un giocatore/guardia
//Il contratto di questi metodi prevede che c sia {character,player,guard}
double get_delta(void *object, Class c);//Ritorna il delta del personaggio in input
int get_range(void *object, Class c);//Ritorna il range del personaggio in input
int get_speed(void *object, Class c);//Ritorna la velocità del personaggio in input
int get_direction(void *object, Class c);//Ritorna la direzione del personaggio in input
struct list *get_bonus(void *object, Class c);//Ritorna la lista di bonus del personaggio in input
struct list *get_malus(void *object, Class c);//Ritorna la lista di malus del personaggio in input
void set_delta(double delta, void *object, Class c);//Assegna il delta al personaggio in input
void set_range(int range, void *object, Class c);//Assegna il range al personaggio in input
void set_speed(int speed, void *object, Class c);//Assegna la velocità del personaggio in input
void set_direction(int direction, void *object, Class c);//Assegna la direzione del personaggio in input
void set_bonus(struct list *L,void *object, Class c);//Imposta lista di bonus del personaggio in input
void set_malus(struct list *L,void *object, Class c);//Imposta la lista di malus del personaggio in input
void collect(struct level *lv, void *character, Class character_Class, void *collectable, Class collectable_Class);//Fa collezionare collectable al personaggio c
void free_character(struct character *c);//Dealloca un personaggio implementato qualsiasi

/*----------------------------------------------------*/
/*                      PLAYER                        */
/*----------------------------------------------------*/

typedef char *(*FNSKILL) (struct level*);//Funzioni che propagano effetti sui personaggi (usando funzioni di tipo FNEFFECT) a partire dalla posizione del giocatore nell'area del suo range

struct skill//Nodo dell'array delle abilità
{
  int cost;//Numero di punti da spendere per usare l'abilità
  FNSKILL use;//Effetti dell'abilità
};

struct player//Giocatore
{
  struct character *super;//puntatore alla superclasse character
  struct skill *skill_set;//Array di abilità del personaggio
  int keys;//Numero di chiavi recuperate nel livello corrente
  int lives;//Numero di vite rimaste al giocatore
  int points;//Punti collezionati dal giocatore, servono ad usare abilità
  int turn;//Tentativo di cambio di direzione del personaggio
  int invincibility;//Secondi di invincibilità rimasti
};

/*----------------------------------------------------*/
/*                       GUARD                        */
/*----------------------------------------------------*/

typedef enum {BACKUP, CHASE, PROTECT, RETREAT} guard_status;

struct guard//Nemico a guardia di una chiave
{
  struct character *super;//puntatore alla superclasse character
  //Quando la chiave sorvegliata viene presa dal giocatore, questi due valori divengono -1
  //e, a seconda dell'intelligenza della guardia, vengono eseguite differenti azioni
  int key_y; //Posizione y della chiave sorvegliata
  int key_x; //Posizione x della chiave sorvegliata
  int key_range;//Grandezza del quadrato di guardia attorno alla chiave
  struct pos*** path_cache;//Matrice contenente un percorso statico utile alla guardia: evita di fare numerosi algoritmi di ricerca per raggiungere lo stesso obiettivo statico
  guard_status status;//Stato della guardia
  int (*backup)(struct level *, struct guard*);//Algoritmo per il movimento dopo che la chiave è stata presa dal giocatore
  int (*chase)(struct level *, struct guard*);//Algoritmo per l'inseguimento del giocatore che è lontano in linea d'area range caselle
  int (*protect)(struct level *, struct guard*);//Algoritmo per la protezione della chiave sorvegliata
  int (*retreat)(struct level *, struct guard*);//Algoritmo per ritornare alla propria chiave
};

typedef int (*FNDIRECTION) (struct level*, struct guard*);//Funzioni che dicono in che direzione deve muoversi una guardia

/*----------------------------------------------------*/
/*                    COLLECTABLE                     */
/*----------------------------------------------------*/

//Tutte le struct di seguito appartengono alla categoria
//collectable: in diverse modalitá sono tutti oggetti da 
//prendere che hanno determinati effetti su personaggi
//del gioco

/*----------------------------------------------------*/
/*                       ITEM                         */
/*----------------------------------------------------*/

typedef char* (*FNITEM) (struct level*);//Funzioni che modificano lo stato del giocatore e del livello. Ritornano il nome della funzione dell'oggetto

struct item//Oggetto
{
  struct printable *super;//Puntatore alla superclasse printable
  FNITEM collect;//Funzione che modifica lo stato del giocatore che raccoglie quel'oggetto e del gioco
};

int check_item(struct level_square *square, FNITEM i);//Verifica se nella casella in input c'è un oggetto di un certo tipo

//Gli unici oggetti definiti in questa libreria sono quelli fondamentali allo svolgimento del gioco: tutti gli altri sono in item.h
/*KEY*/
char *key(struct level *lv);//Aumenta di uno il numero di chiavi del giocatore e di 60 i suoi punti
/*DOOR*/
char *door(struct level *lv);//Fa superare il livello al giocatore
/*----------------------------------------------------*/
/*                      TRIGGER                       */
/*----------------------------------------------------*/
//Tutte le struct di seguito sono trigger ed 
//appartengono alla categoria collectable: essi possono
//essere bonus o malus. 
//Per creare uno o l'altro basterá passare, oltre
//all'effetto desiderato, il trigger_type corretto

typedef enum {BONUS,MALUS} trigger_type;//Flag indicante se il trigger é un bonus o un malus
typedef enum {ON,OFF} trigger_mode;//Flag indicante se l'effetto va applicato o rimosso

typedef char *(*FNEFFECT) (struct level*, void *, Class, trigger_mode);//Funzioni che modificano lo stato di un personaggio e/o del livello 
//Se il parametro effect_mode é ON l'effetto viene applicato, altrimenti si rimuove ripristinando lo stato precedente del personaggio

struct trigger//Bonus/Malus
{
  trigger_type type;//Indica se il trigger è un bonus o un malus
  struct printable *super;//Puntatore alla superclasse element
  long time_left;//Indica il tempo rimasto, espresso in secondi, prima di scomparire dal labirinto
  FNEFFECT effect;//Funzione che modifica lo stato del personaggio che raccoglie il trigger (viene richiamata nel gioco con il trigger mode ON))
};

