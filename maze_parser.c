//Lorenzo De Simone N86/1008

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include "maze_parser.h"

int maze_ok;//Variabile globale che segnala la presenza d'errori
//1-> Nessun errore
//0-> Errore

void maze_syntax_error(FILE *file)//Segnala un errore di sintassi e imposta la variabile globale err a 0
{
  maze_ok=0;
  int c=fgetc(file);
  
  mvprintw(LINES/2-2,(COLS-29)/2,"Error while parsing maze file");
  if(c!=EOF)
    mvprintw(LINES/2,(COLS-45)/2,"< Syntax error: '%c' found in position [%d] >",c,(int)ftell(file)-1);
  else
    mvprintw(LINES/2,(COLS-45)/2,"< End of file not expected in position [%d] >",(int)ftell(file));
}

int maze_match(FILE *file, token et)//Ritorna 1 se il prossimo carattere nel file è del tipo et, -1 altrimenti
{	
  int c;
  token rt;                   
  int res=0;
	
  switch(c=getc(file))//Legge e assegna il tipo al carattere letto
  {    	      
    case ' '  : rt = blank;  break;
    case '#'  : rt = hash ;  break;
    case '\r' : rt = bkr  ;  break;
    case '\n' : rt = bkn  ;  break;
    case EOF  : rt = eof  ;  break;
    default   : rt = -1   ;  break; 
  }
  
  ungetc(c,file);//Rimette nello stream il carattere letto

  if(rt==et)   
    res=1;
  
  return res;
}

void maze_parse_newline(FILE *file)//Esegue il parsing di newline
{  

  //Viene accettato sia '\r\n' che '\n'
  if(maze_match(file,bkr)==1)
  {
    fseek(file,1,SEEK_CUR);
    if(maze_match(file,bkn)==1)
      fseek(file,1,SEEK_CUR);
    else
      maze_syntax_error(file);  
  }
  else if(maze_match(file,bkn)==1)
    fseek(file,1,SEEK_CUR);
  else
    maze_syntax_error(file);
}


struct parsed_maze *maze_parse_matrix(FILE *file)//Esegue il parsing della matrice del labirinto e restituisce una struct contenente tutte le caselle corridoio
{
  //Si codificano in questo modo le posizioni delle caselle corridoio in array
  //[y1][x1][y2][x2][y3][x3]...etc dove (y,x) è la posizione della casella corridoio all'y-esima riga e alla x-esima colonna
  maze_ok=1;
  int width=-1,pos=0,y=0,x=0,dim=10,*array;
  struct parsed_maze *mat=NULL;
  array=(int*) calloc (dim,sizeof(int));

  do
  { 
    while((maze_match(file,hash)==1)||((maze_match(file,blank)==1)))//Scorre tutta la riga
    {
      if(maze_match(file,blank)==1)//Memorizza in array tutte le posizioni delle caselle corridoio
      {
        if(pos==dim)
        {
	  dim=dim*2;
	  array=realloc(array,dim*sizeof(int));
        }
        array[pos++]=y;
        array[pos++]=x;
      }  
      fseek(file,1,SEEK_CUR);
      x++;
    }
    
    if(maze_match(file,eof)!=1)
        maze_parse_newline(file);
    
    if(maze_ok==1)
    {
      if(width==-1)//In width viene memorizzata l'ampiezza del labirinto: serve a controllare che tutte le righe siano larghe quanto la prima
        width=x;
      else if(width!=x)
	maze_syntax_error(file);
    }
    //Aggiorna le coordinate 
    y++;
    x=0;    
  }while((maze_match(file,eof)!=1) && (maze_ok==1));//Il parsing continua fino a che non finisce il file o non si trova un errore di sintassi
  
  if(y==0 || width==0)//Il labirinto deve avere almeno 1x1
    maze_syntax_error(file);
  
  if(maze_ok==1)
  {
    mat=(struct parsed_maze*) malloc(sizeof(struct parsed_maze));
    mat->array=array;
    mat->length=pos;//La posizione di array successiva all'ultima coordinata memorizzata 
    mat->height=y;//L'ultimo valore della i è l'altezza della matrice
    mat->width=width;
  }
  else
    free(array);
  
  return mat;
}