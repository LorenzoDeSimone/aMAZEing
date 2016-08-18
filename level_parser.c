//Lorenzo De Simone N86/1008

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <curses.h>
#include "level_parser.h"

int amazeing_ok;//Variabile globale che segnala la presenza d'errori
//1-> Nessun errore
//0-> Errore

struct parsed_pos *init_parsed_pos(int y, int x)//Inizializza una struttura parsed_pos
{
  struct parsed_pos *new_p=(struct parsed_pos*) malloc(sizeof(struct parsed_pos));
  new_p->y=y;
  new_p->x=x;
  return new_p;
}

struct parsed_guard *init_parsed_guard(struct parsed_pos *pos, int guard_type)//Inizializza una struttura parsed_guard
{
  struct parsed_guard *p_g=(struct parsed_guard*) malloc(sizeof(struct parsed_guard));
  p_g->pos=init_parsed_pos(pos->y,pos->x);
  p_g->type=guard_type;
  return p_g;
}

struct parsed_level *init_parsed_level(int time, int n_guards, struct parsed_pos *start, struct parsed_pos *exit, struct list *guard_list)//Inizializza una struttura parsed_level
{
   struct parsed_level *p_lv=(struct parsed_level*) malloc(sizeof(struct parsed_level));
   p_lv->time=time;
   p_lv->n_guards=n_guards;
   p_lv->start=start;
   p_lv->exit=exit;
   p_lv->guard_list=guard_list;
   return p_lv;
}

void free_parsed_guard(struct parsed_guard *p_g)//Dealloca una struttura parsed_guard
{
  if(p_g)
  {
    free(p_g->pos);
    free(p_g);
  }
}

void free_guard_list(struct list *guard_list)//Dealloca una lista di guardie
{
  struct parsed_guard *p_g;
  struct list *L=guard_list;
  while(L)
  {
    p_g=get_head(L);
    L=delete_head(L);
    free(p_g);
  }
}

void free_parsed_level(struct parsed_level *p_lv)//Dealloca una struct parsed_level
{
  if(p_lv)
  {
    free_guard_list(p_lv->guard_list);
    free(p_lv->start);
    free(p_lv->exit);
    free(p_lv);
  }
}

/*--------------------------------------------------------------------------------------*/
//Funzioni di parsing

int amazeing_match(FILE *file, type et)//Ritorna 1 se il prossimo carattere nel file è del tipo et, 0 altrimenti
{	
  int c,res=0;
  type rp;                   
	
  while(((c=fgetc(file))=='\t')||(c=='\r')||(c=='\n')||(c==' '));//Salta tutti i caratteri ininfluenti
	
  switch(c)//Legge e assegna il tipo al carattere letto
  {    
     case '[': rp = S_LPAR  ;  break;
     case ']': rp = S_RPAR  ;  break;
     case '(': rp = R_LPAR  ;  break;
     case ')': rp = R_RPAR  ;  break;
     case ',': rp = COMMA   ;  break;
     case 'S': rp = START   ;  break;
     case 'E': rp = EXIT    ;  break;
     case 'A': rp = GUARD   ;  break;
     case 'B': rp = GUARD   ;  break;
     case 'C': rp = GUARD   ;  break;
     case 'D': rp = GUARD   ;  break;
     case EOF: rp = ENDF    ;  break;
     default : rp = -1      ;  break;  
  }
  
  ungetc(c,file);//Rimette nello stream il carattere letto

  if(rp==et)   
    res=1;
  
  return res;
}

void amazeing_syntax_error(FILE *file)//Segnala un errore di sintassi e imposta la variabile globale err a 0
{
  amazeing_ok=0;
  int c=fgetc(file);
  
  mvprintw(LINES/2-2,(COLS-30)/2,"Error while parsing level file");
  if(c!=EOF)
    mvprintw(LINES/2,(COLS-45)/2,"< Syntax error: '%c' found in position [%d] >",c,(int)ftell(file)-1);
  else
    mvprintw(LINES/2,(COLS-45)/2,"< End of file not expected in position [%d] >",(int)ftell(file));
}

int *amazeing_parse_int(FILE *file)//Se il prossimo non terminale è un intero positivo ritorna un puntatore al suo valore, NULL altrimenti
{
  int c;
  char *integer=(char*) calloc(8,sizeof(char));
  int i=0;
  int *ret=NULL;
  
  // max integer: [n][n][n][n][n][n][n][\0]

  while(isdigit(c=fgetc(file))&&(i<7))//Riempie l'array di char* con gli interi letti dal file
    integer[i++]=c;
  
  if(!(isdigit(c)))//Controlla che non ci siano altre cifre dopo la settima
  {  
    integer[i]='\0';
    ungetc(c,file);//Rimette nello stream il primo carattere successivo all'intero
    
    if(i>0)//Indica che almeno una cifra è stata trovata
    {
      ret=(int*) malloc(sizeof(int));
      *ret=atoi(integer);
    }
    else
      amazeing_syntax_error(file);
  }
  else
  {
    ungetc(c,file);
    amazeing_syntax_error(file);
  }
  
  free(integer);
  
  return ret;
}

long *amazeing_parse_long(FILE *file)//Se il prossimo non terminale è un intero positivo ritorna un puntatore al suo valore, NULL altrimenti
{
  int c;
  char *number=(char*) calloc(10,sizeof(char));
  int i=0;
  long *ret=NULL;
  
  // max number: [9][9][9][9][9][9][9][9][9][\0]
  
  while(isdigit(c=fgetc(file))&&(i<9))//Riempie l'array di char* con gli interi letti dal file
    number[i++]=c;
  
  if(!(isdigit(c)))//Controlla che non ci siano altre cifre dopo la settima
  {  
    number[i]='\0';
    ungetc(c,file);//Rimette nello stream il primo carattere successivo all'intero
    
    if(i>0)//Indica che almeno una cifra è stata trovata
    {
      ret=(long*) malloc(sizeof(long));
      *ret=atol(number);
    }
    else
      amazeing_syntax_error(file);
  }
  else
  {
    ungetc(c,file);
    amazeing_syntax_error(file);
  }
  
  free(number);
  
  return ret;
}

long *amazeing_parse_time(FILE *file)//Se il prossimo non terminale è <time> ne ritorna il valore, altrimenti ritorna NULL
{
  long *ret=NULL;

  if(amazeing_match(file,S_LPAR)==1)
  {
    fseek(file,1,SEEK_CUR);
    ret=amazeing_parse_long(file);
    if(amazeing_ok==1)
    {
      if(amazeing_match(file,S_RPAR)==1)
        fseek(file,1,SEEK_CUR);
      else
      {   
        free(ret);
        ret=NULL;
        amazeing_syntax_error(file);
      }
    }
  }
  else
    amazeing_syntax_error(file);
  
  return ret;
}

struct parsed_pos *amazeing_parse_pos(FILE *file)//Se il prossimo non terminale è <pos> ne ritorna il valore, altrimenti ritorna NULL
{
  int c,*y=NULL,*x=NULL;
  struct parsed_pos *pos=NULL;
  
  if(amazeing_match(file,R_LPAR)==1)
  {
    fseek(file,1,SEEK_CUR);
    y=amazeing_parse_int(file);
    if(amazeing_ok==1)
    {
      if(amazeing_match(file,COMMA)==1)
      {
        fseek(file,1,SEEK_CUR);
        x=amazeing_parse_int(file);
        if(amazeing_ok==1)
        {
          if(amazeing_match(file,R_RPAR)==1)
          {
            fseek(file,1,SEEK_CUR);
            pos=init_parsed_pos(*y,*x);
          }
          else
            amazeing_syntax_error(file);
        }
      }
      else
        amazeing_syntax_error(file);
    }
  }
  else
    amazeing_syntax_error(file);
  
  free(y);
  free(x);
  return pos;
}

struct parsed_pos *amazeing_parse_start(FILE *file)//Se il prossimo non terminale è <source> ne ritorna il valore in una struct parsed_pos, altrimenti ritorna NULL
{
  struct parsed_pos *pos=NULL;
  
  if(amazeing_match(file,START)==1)
  {
    fseek(file,1,SEEK_CUR);
    pos=amazeing_parse_pos(file);
  }
  else
    amazeing_syntax_error(file);
  
  return pos;
}

struct parsed_pos *amazeing_parse_exit(FILE *file)//Se il prossimo non terminale è <exit> ne ritorna il valore in una struct parsed_pos, altrimenti ritorna NULL
{
  struct parsed_pos *pos=NULL;
  
  if(amazeing_match(file,EXIT)==1)
  {
    fseek(file,1,SEEK_CUR);
    pos=amazeing_parse_pos(file);
  }
  else
    amazeing_syntax_error(file);
  
  return pos;
}

int *amazeing_parse_n_guards(FILE *file)//Se il prossimo non terminale è <n_guards> ne ritorna il valore, altrimenti ritorna NULL
{
  int *ret=NULL;

  if(amazeing_match(file,R_LPAR)==1)
  {
    fseek(file,1,SEEK_CUR);
    ret=amazeing_parse_int(file);
    if(amazeing_ok==1)
    {
      if(*ret>0 && amazeing_match(file,R_RPAR)==1)
        fseek(file,1,SEEK_CUR);
      else
      {   
        free(ret);
        ret=NULL;
        amazeing_syntax_error(file);
      }
    }
  }
  else
    amazeing_syntax_error(file);
  
  return ret;
}

struct parsed_guard *amazeing_parse_guard(FILE *file)//Se il prossimo non terminale è <guard> ne ritorna il valore, altrimenti ritorna NULL
{
  struct parsed_guard *p_g=NULL;
  struct parsed_pos *pos=NULL;
  int guard_type;
  
  if(amazeing_match(file,GUARD)==1)
  {
    guard_type=fgetc(file);
    pos=amazeing_parse_pos(file);
    if(amazeing_ok==1)
      p_g=init_parsed_guard(pos,guard_type);
    else
    {
      free(pos);
      fseek(file,-1,SEEK_CUR);
      amazeing_syntax_error(file);
    }
  }
  else
    amazeing_syntax_error(file);
  
  return p_g;
}

struct parsed_level *amazeing_parse_level(FILE *file)//Esegue il parsing di un file di testo e restitusice una struct parsed_level contenente tutte le informazioni su un livello del gioco aMAZEing
{
  int i=0,*n_guards=NULL;
  long *time=NULL;
  struct parsed_pos *start=NULL,*exit=NULL;
  struct parsed_level *p_lv=NULL;
  struct list *guard_list=NULL;  
  struct list *L=NULL;
  struct parsed_guard *curr_guard;
  
  amazeing_ok=1;//La variabile d'errore viene resettata in caso di precedenti chiamate alla funzione

  time=amazeing_parse_time(file);
  if(amazeing_ok==1)
  {
    start=amazeing_parse_start(file);

    if(amazeing_ok==1)
    {
      exit=amazeing_parse_exit(file);
      if(amazeing_ok==1)
      {
        n_guards=amazeing_parse_n_guards(file);
        while(amazeing_ok==1 && i<*n_guards)//Parsing dell'intera lista di guardie
        {
          curr_guard=amazeing_parse_guard(file);
          //Nessuna guardia può iniziare il gioco trovandosi sull'uscita o sull'entrata
          //e due guardie non possono partire nella stessa locazione
          
          if(curr_guard &&(   (curr_guard->pos->y==start->y && curr_guard->pos->x==start->x)
                            ||(curr_guard->pos->y==exit->y  && curr_guard->pos->x==exit->x)
                            ||(check_same_pos(curr_guard,guard_list))) )          
            amazeing_syntax_error(file);
          else if(curr_guard)
          {
            guard_list=insert_rear(guard_list,curr_guard);
            i++;
          }
        }    
        
        if(i==0)//Ci deve essere almeno una guardia nel livello
        {
          fseek(file,-1,SEEK_CUR);
          amazeing_syntax_error(file);
        }
        if(amazeing_ok==1)//Verifica che si sia usciti dal while senza errori
        {
          if(amazeing_match(file,ENDF)==1)//Se il file di testo finisce qui, si crea la struct parsed_level da ritornare
             p_lv=init_parsed_level(*time,*n_guards,start,exit,guard_list);
          else
            amazeing_syntax_error(file);
        }
        else
        {
          free(n_guards);
          free(start);
          free(exit);
          free_guard_list(guard_list);
        }
      }
      else
        free(start);
    }
  }
  //Vanno sempre deallocati perchè vengono passati per valore a init_parsed_level()
  free(time);
  free(n_guards);
  return p_lv;
}

int check_same_pos(struct parsed_guard *curr_guard, struct list *guard_list)//Verifica se la guardia in input ha coordinate uguali a una delle guardie già esaminate
{
  struct parsed_guard *p_g;
  while(has_next(guard_list))
  {
    p_g=get_head(guard_list);
    if(curr_guard->pos->y==p_g->pos->y && curr_guard->pos->x==p_g->pos->x)
      return 1;
    else
      guard_list=next(guard_list);
  }
  return 0;
}
