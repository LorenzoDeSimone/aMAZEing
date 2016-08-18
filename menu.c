//Lorenzo De Simone N86/1008

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <stdarg.h>
#include <string.h>
#include "menu.h"

char **init_choices(int n_choices, ...)//Crea un array di stringhe per il menu con le stringhe in input
{
  va_list arg;
  char **choices=(char**) calloc(n_choices,sizeof(char*));
  char *curr;
  int i=0;
  
  va_start(arg,n_choices);
  
  //Copio le stringhe nell'array choices
  for(i=0;i<n_choices;i++)
  {
    curr=va_arg(arg,char*);
    choices[i]=(char*) calloc(strlen(curr),sizeof(char)+1);//Alloco lo spazio per la stringa 
    strcpy(choices[i],curr);//Copio la stringa nella sua posizione dell'array
  }
  
  va_end(arg);

  return choices;
}

void free_choices(char **choices, int n_choices)//Dealloca l'array di char contenente le scelte possibili del menu
{
  int i;
  
  for(i=0;i<n_choices;i++)
    free(choices[i]);
  
  free(choices);
}

//Stampano un menu con con la possibilità di selezionare l'opzione desiderata

int print_menu(int height, int width, char **choices, int n_choices, int color_pair, FNLOGO title)
{
  WINDOW *menu_win;
  int highlight = 1;
  int choice = 0;
  int c,start_y,start_x;
  
  erase();
  start_y = ((LINES - height) / 1.5) ;
  start_x = (COLS - width) / 2;
  menu_win = newwin(height, width, start_y, start_x);
  keypad(menu_win,TRUE);
  attron(COLOR_PAIR(color_pair));
  wattron(menu_win,COLOR_PAIR(color_pair));
  title(start_y,start_x);//Stampa il logo

  print_choices(menu_win,choices,highlight,n_choices);
  
  do
  {	
    c=wgetch(menu_win);
    switch(c)	
    {				
      case KEY_UP:
	if(highlight==1)
	  highlight=n_choices;
	else
	  highlight--;
      break;
	
      case KEY_DOWN:
	if(highlight==n_choices)
	  highlight=1;
	else 
	  highlight++;
      break;
      
      case 10://Tasto enter
	choice=highlight;
      break;     
    }
    print_choices(menu_win,choices,highlight,n_choices);
  }while(choice==0);
  
  attroff(COLOR_PAIR(color_pair));
  wattroff(menu_win,COLOR_PAIR(color_pair));
  delwin(menu_win);
  erase();
  return choice;
}

void print_choices(WINDOW *menu_win, char **choices, int highlight, int n_choices)
{
  int x,y,i;	

  x=1;
  y=1;
  
  box(menu_win,0,0);
  
  for(i=0;i<n_choices;++i)
  {	
    if(highlight==i+1)//Illumina la scelta corrente
    {	
      wattron(menu_win, A_REVERSE); 
      mvwprintw(menu_win, y, x, "%s", choices[i]);
      wattroff(menu_win, A_REVERSE);
    }
    else
      mvwprintw(menu_win,y,x,"%s",choices[i]);
    y++;
  }
  wrefresh(menu_win);
}