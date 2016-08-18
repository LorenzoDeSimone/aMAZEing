//Lorenzo De Simone N86/100

struct pr_queue
{
    struct heap *root;//Puntatore all'albero heap con le priorità
    int heapsize;//Numero di nodi memorizzati
    struct heap ***map;//Puntatore alla matrice dei puntatori ai nodi (permette di rendere costante la ricerca del nodo avendo in input (x,y))
    int mapheight;//Altezza della map
    int mapwidth;///Ampiezza della map
};

struct pos//Struttura per memorizzare una generica posizione
{
  int y;
  int x;
};

struct heap
{
  struct pos* info;
  float priority;
  struct heap *parent;
  struct heap *right;
  struct heap *left;
}; 

/*-----------------------------------------*/
//Funzioni di Inizializzazione e deallocazione
struct pr_queue *init_pr_queue(int height, int width);//Inizializza la coda con n_nodi=0 e albero vuoto
void free_pr_queue(struct pr_queue *queue);//Dealloca l'intera struttura coda a priorità
struct heap *init_heap_node(struct pos *p);//Alloca un nuovo nodo con il dato in input e priorità di default a FLT_MAX
void free_heap(struct heap *root);//Dealloca l'intero heap
struct pos* init_pos(int y, int x);//Inizializza una struct pos con le coordinate in input

/*-----------------------------------------*/
//Funzioni sulla coda a priorità

void heapify(struct heap ***map, struct heap *root);//Esegue heapify sugli elementi di root
void swap_heap(struct heap ***map, struct heap *node1, struct heap *node2);//Esegue lo swap dei dati fra due nodi
struct pos *get_min(struct pr_queue *queue);//Ritorna l'elemento con priorità minima

void increase_key_heap(struct heap ***map, struct heap *node, float new_priority);//Aumenta la priorità del dato contenuto nel nodo in input
void decrease_key_heap(struct heap ***map, struct heap *node, float new_priority);//Diminuisce la priorità del dato contenuto nel nodo in input

//Funzioni da usare esternamente alla libreria: prendendo in input direttamente la posizione, modificano la priorità del dato
void increase_key(struct pr_queue *queue, struct pos *p, float new_priority);//Aumenta la priorità del dato
void decrease_key(struct pr_queue *queue, struct pos *p, float new_priority);//Diminuisce la priorità del dato

struct pos *find_node(struct pr_queue *queue, struct pos *p);//Ritorna un puntatore al nodo nella coda a prioritá se esso é presente, NULL altrimenti

float get_priority(struct pr_queue *queue, struct pos *p);//Restituisce la priorità attuale del dato
void insert_key(struct pr_queue *queue, struct pos *p, float new_priority);//Inserisce un nuovo nodo nell'albero con la priorità ed il dato in input
void extract_min(struct pr_queue *queue);//Cancella la radice e modifica l'albero puntato dalla struct coda

struct heap *find_node_bin(struct pr_queue *queue, int n);//Restituisce il puntatore all'elemento n-esimo nell'ordinamento della coda, utilizzando il metodo del percorso codificato in 0 ed 1


