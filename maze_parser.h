//Lorenzo De Simone N86/1008

#include <stdio.h>

typedef enum {bkr, bkn, blank, hash, eof} token;//Definisce i token accettati
//bkr   -> '\r'
//bkn   -> '\n'
//blank -> ' '
//hash  -> '#'

struct parsed_maze//Struttura di ritorno del parser di labirinti
{
  int *array;//Array contenente la posizione di tutte le caselle corridoio
  int length;//Lunghezza dell'array
  int height;//Altezza della matrice
  int width;//Larghezza della matrice
};

void maze_syntax_error(FILE *file);//Segnala un errore di sintassi e imposta la variabile globale err a 0
int maze_match(FILE *file, token et);//Ritorna 1 se il prossimo carattere nel file è del tipo et, -1 altrimenti
void maze_parse_newline(FILE *file);//Esegue il parsing di newline
struct parsed_maze *maze_parse_matrix(FILE *file);//Esegue il parsing della matrice del labirinto e restituisce una struct contenente tutte le caselle corridoio