//Lorenzo De Simone N86/1008

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <time.h>
#include "gameplay.h"

int main()
{    
  initscr();//Inizializzo il terminale per renderlo interattivo
  if(!has_colors())
  {	
    endwin();
    printf("Your terminal does not support colors\n");
    exit(-1);
  }
  
  curs_set(0);//Rendo il cursore invisibile
  keypad(stdscr, TRUE);//Abilito l'input dei tasti control
  start_color();//Abilito i colori
  
  //Elimino l'input buffering
  noecho();
  cbreak();
  
  //Nel caso in cui siano implementati più giochi, il main può fungere da selezionatore di giochi:
  //essendocene solo uno, viene fatto partire immediatamente
  menu_amazeing();//Menu del gioco aMAZEing	    
  erase();
  endwin();
   
  return 0;
}