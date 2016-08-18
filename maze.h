//Lorenzo De Simone N86/1008

#include "maze_parser.h"
#include "maze_queue.h"

#ifndef LIST_H
  #define	LIST_H
  #include "list.h"
#endif

typedef enum {WHITE, GRAY, BLACK} color;//Colori possibili per i vertici

//Rendono più leggibile il codice che lavora sulle adiacenze
#define UP 0 
#define RIGHT 1
#define DOWN 2
#define LEFT 3 

struct maze//Struttura di un labirinto
{
  struct square ***matrix;//Matrice effettiva contenente puntatori a struct square
  int height;//Altezza della matrice
  int width;//Larghezza della matrice
};

struct square//Struttura di una casella
{
  void *square_info;//Dato generico contenuto nella casella 
  //(Non viene MAI gestito in questa libreria: viene riempito a seconda delle esigenze nelle librerie che includono maze.h usando opportunamente la riflessione)  
  
  //Archi uscenti del vertice
  //Peso == 0 -> Arco non presente (C'è un muro adiacente)
  float u_weight;//Peso dell'arco in su
  float r_weight;//Peso dell'arco a destra
  float d_weight;//Peso dell'arco in giù
  float l_weight;//Peso dell'arco a sinistra
};

/*----------------------------------*/
//Funzioni di base sul labirinto

typedef struct pos **(*FNADJ) (struct maze *,int, int);//Funzioni che ricavano adiacenze

struct maze *init_maze(int height, int width);//Inizializza un labirinto
void free_maze(struct maze *m);//Dealloca un labirinto
void add_vertex(struct maze *m, int y, int x);//Inserisce un corridoio alla posizione (y,x)
void del_vertex(struct maze *m, int y, int x);//Inserisce un muro alla posizione (y,x) (N.B. l'info NON viene deallocato)
void set_weight(struct maze *m, int y, int x, float weight, int d);//Imposta il peso dell'arco uscente in input
struct pos** get_adj(struct maze *m, int y, int x);//Restituisce una struct adj contenente tutte le adiacenze della casella in posizione (y,x)
void free_adj(struct pos **adj);//Dealloca la struttura adj in input
struct maze *read_maze(char *path);//Legge un labirinto da file

/*----------------------------------*/
//Algoritmi ausiliari

color **paint_white(int height, int width);//Restituisce una matrice allocata di dimensione height*width con il colore di ogni locazione inizializzato a bianco
void free_color(color **col, int height);//Dealloca la matrice colore
struct pos ***init_pred(int height, int width);//Inizializza la matrice pred contenente puntatori a struct pos
void free_pred(struct pos ***pred, int height, int width);//Dealloca pred e tutte le struct pos allocate 
struct pos *find_first_vertex(struct maze *m);//Ritorna le coordinate del primo corridoio del labirinto, NULL se il labirinto contiene solo muri
int is_connected(struct maze *m);//Restituisce 1 se il labirinto è una componente connessa, 0 altrimenti
float h(struct maze *m, int start_y, int start_x, int end_y, int end_x);//Funzione di stima euristica per un labirinto toroidale

//Algoritmi di ricerca
//Oltre alla versione standard, viene fornitau na versiona customizzata, con la poossibilità di usare formule di adiacenza particolari (es. considerare anche i muri adiacenti)

struct pos ***BFS(struct maze *m, int y, int x);//Visita in ampiezza del labirinto che riempie una matrice di puntatori a struct pos con le posizioni dei nodi visitati
struct pos ***BFS_custom(struct maze *m, int y, int x, FNADJ my_adj);//BFS con funzione di adiacenza custom

//Ritornano una lista contenente coordinate di vertici
struct list *nBFS(struct maze *m, int y, int x, int n);//Restituisce tutti i nodi ad una distanza n da (y,x)
struct list *nBFS_custom(struct maze *m, int y, int x, int n, FNADJ my_adj);//nBFS con funzione di adiacenza custom
struct list *get_path_list(struct pos*** pred, int start_y, int start_x, int end_y, int end_x);//Crea una lista contenente i nodi di un percorso minimo
struct list *reach_path(struct maze *m, int y, int x);//Ritorna i nodi del percorso minimo che, se scavati, congiungerebbero il nodo ad una componente connessa già presente

struct pos ***dijkstra(struct maze *m, int y, int x);//Ricerca del percorso minimo che riempie una matrice di puntatori a struct pos con le posizioni dei nodi visitati
struct pos ***dijkstra_custom(struct maze *m, int y, int x, FNADJ my_adj);//dijkstra con funzione di adiacenza custom

struct pos ***a_star(struct maze *m, int start_y, int start_x, int end_y, int end_x);//Ricerca del percorso minimo mediante l'algoritmo A*
struct pos ***a_star_custom(struct maze *m, int start_y, int start_x, int end_y, int end_x, FNADJ my_adj);//a_star con funzione di adiacenza custom

/*----------------------------------*/
//Algoritmo per la generazione di labirinti random e ausiliari

typedef struct maze *(*FNRAND_MAZE) (int, int);//Funzioni che creano labirinti random delle dimensioni in input

struct maze *random_maze(int height, int width);//Genera un labirinto random di altezza e larghezza fissata
void random_maze_visit(struct maze *m, color **col, int y, int x);//Visita ricorsiva ausiliaria a random_maze
struct pos** get_wall_adj(struct maze *m, int y, int x);//Ottiene le adiacenze in un labirinto ignorando la presenza di corridoi o muri
int rand_index(struct pos **adj);//Restituisce un indice random da 0 a 3 che non è ancora stato riempito nell'array adj