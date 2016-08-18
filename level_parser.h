//Lorenzo De Simone N86/1008

#ifndef LIST_H
  #define	LIST_H
  #include "list.h"
#endif
#include <stdio.h>

//Grammatica BNF per leggere i livelli

//<level>      ::= <time> <source> <end> <n_guards> <guard_list>
//<time>       ::= '['int']'
//<start>      ::=  S <pos>
//<exit>       ::=  E <pos>
//<pos>        ::= '('int,int')'
//<n_guards>   ::= '('int')'
//<guard>      ::= [A,B,C,D] <pos>
//<guard_list> ::= {guard}

typedef enum {S_LPAR,S_RPAR,R_LPAR,R_RPAR,COMMA,START,EXIT,GUARD,ENDF} type;//Tipi terminali

//S_LPAR   parentesi quadra sinistra
//S_RPAR   parentesi quadra destra
//R_LPAR   parentesi tonda sinistra
//R_RPAR   parentesi tonda destra
//COMMA    virgola
//START    'S'
//EXIT     'E'
//GUARD    {'A','B','C','D'}
//ENDF      End of file

/*----------------------------------------------*/
//Struct per parsing sui livelli del gioco

struct parsed_pos//Contiene le coordinate di una posizione
{
  int y;
  int x;
};

struct parsed_level//Struttura contenente tutte le informazioni del livello letto da file
{
  long time;//Tempo limite in cui terminare il livello espresso in secondi
  int n_guards;//Numero di guardie (uguale a quello delle chiavi iniziali)
  struct parsed_pos *start;//Posizione dell'entrata
  struct parsed_pos *exit;//Posizione dell'uscita
  struct list *guard_list;//Lista contenente puntatori a strutture di tipo parsed_guard
};

struct parsed_guard//Struttura contenente posizione e tipo di una guardia letta da file
{
  struct parsed_pos *pos;//Posizione della guardia
  int type;//Tipo della guardia
};

//Si è scelto di porre nell'header solo le funzioni indispensabili per ragioni di sicurezza: in questo modo chiunque
//importi la libreria potrà solo usare queste funzionalità

void free_parsed_level(struct parsed_level *p_lv);//Dealloca una struct parsed_level
struct parsed_level *amazeing_parse_level(FILE *file);//Esegue il parsing di un file di testo e restitusice una struct parsed_level contenente tutte le informazioni su un livello del gioco aMAZEing

