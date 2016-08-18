//Lorenzo De Simone N86/1008

//Non vengono deallocati gli info dei nodi
//Questa libreria serve solo come supporto
//         
// to Rear<--|        |<--|    |<-- ... <--|    |<--
//           |Sentinel|   |Head|           |Rear|
//           |        |-->|    |--> ... -->|    |--> to Sentinel
// 
// Uso stack: 
//            Insert_head
//            Delete_head
// Uso coda:
//            Insert_head
//            Delete_rear
//
// Uso lista linkata IN LETTURA:
//            has_next
//            next
//

/*------------------------------------------------------------*/
//Struttura del nodo
struct list
{
  void *info;
  struct list *pred;
  struct list *next;
};

struct list *create_node(void* new_info);//Alloca un nuovo nodo con il campo info uguale al valore in input

struct list *insert_head(struct list *sentinel, void *new_info);//Inserisce un nodo in testa alla lista in input
struct list *delete_head(struct list *sentinel);//Cancella un nodo dalla testa della lista in input

struct list *insert_rear(struct list *sentinel, void *new_info);//Inserisce un nodo in coda alla lista in input
struct list *delete_rear(struct list *sentinel);//Cancella un nodo dalla coda della lista in input

void *get_head(struct list *sentinel);//Ritorna il puntatore all'info dell'elemento in testa
void *get_rear(struct list *sentinel);//Ritorna il puntatore all'info dell'elemento in coda

int has_next (struct list* sentinel);//Ritorna 1 se il nodo ha un successore, 0 altrimenti
struct list *next(struct list *sentinel);//Ritorna il link al prossimo elemento della lista