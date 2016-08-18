//Lorenzo De Simone N86/1008

#include <stdlib.h>
#include <stdio.h>
#include "item.h"

/*----------------------------------------------------*/
/*                       ITEM                         */
/*----------------------------------------------------*/
#define n_item 5//Numero di item attualmente implementati nel gioco (esclusa la chiave e la porta)
FNITEM all_item[n_item];//Array globale utilizzati per la generazione random di item

struct item *create_item (int y, int x, FNITEM collect)//Crea una struttura di tipo item
{
  struct item *new_item=(struct item*) malloc(sizeof(struct item));
  int c;
  
  if(collect==key)
    c=ACS_STERLING;
  else if(collect==door)
    c=ACS_BLOCK;
  else
    c=ACS_DEGREE;
  
  new_item->super=create_printable(c,YELLOW,y,x);
  new_item->collect=collect;
  
  return new_item;  
}

void free_item(struct item *i)//Dealloca un oggetto
{
 free(i->super);
 free(i);
}

void init_all_item()//Inizializza l'array contenente puntatori a tutti gli oggetti implementati
{
  all_item[0]=life;
  all_item[1]=points;
  all_item[2]=few_points;
  all_item[3]=seconds;
  all_item[4]=golden_path;
}

FNITEM rand_item()//Ritorna un puntatore alla funzione di un oggetto casuale
{
  return all_item[rand()%n_item];
}
/*                  ITEM IMPLEMENTATI                 */

/*LIFE*/
char *life(struct level *lv)//Aumenta di uno le vite del giocatore
{
  lv->p->lives++;
  return (char*)__func__;
}

/*POINTS*/
char *points(struct level *lv)//Aumenta di 20 i punti del giocatore
{
  lv->p->points+=40;
  return (char*)__func__;
}

/*FEW_POINTS*/
char *few_points(struct level *lv)//Aumenta di 5 i punti del giocatore
{
  lv->p->points+=5;
  return (char*)__func__;
}

/*SECONDS*/
char *seconds(struct level *lv)//Aumenta di 30 secondi il tempo a disposizione del giocatore
{
  lv->time_left+=30;
  return (char*)__func__;  
}

void fill_golden_path(struct pos ***pred, struct level *lv, int start_y, int start_x, int end_y, int end_x)//Funzione ausiliaria a golden_path
{
  struct item *i;
  struct level_square *square;
  
  if((start_y==end_y) && (start_x==end_x))//Se si è tornati indietro fino alla partenza, bisogna terminare la ricorsione
   return;
  else if(pred[end_y][end_x])
  {
    square=get_square(lv,pred[end_y][end_x]->y,pred[end_y][end_x]->x);
    //Se sul percorso è presente un trigger, viene rimosso: gli oggetti vengono lasciati dove sono
    //Il controllo serve a preservare eventuali chiavi sul percorso e ad evitare deallocazioni inutili di oggetti già presenti
    if(!square->collectable || (square->collectable && square->collectable_Class==trigger))
    {
      if(square->collectable)
        remove_object(lv,square->collectable,square->collectable_Class);
      i=create_item(pred[end_y][end_x]->y,pred[end_y][end_x]->x,few_points);//Creo un item few_points  
      add_object(lv,i,item);//Lo aggiungo
      print_square(lv,i,item);//Lo stampo
    }
    fill_golden_path(pred,lv,start_y,start_x,pred[end_y][end_x]->y,pred[end_y][end_x]->x);//Chiamata ricorsiva
  }  
}

/*GOLDEN_PATH*/
char *golden_path(struct level *lv)//Se è presente la porta, lastrica il percorso verso la porta di items del tipo few_points. Se la porta non dovesse essere ancora comparsa, sceglie un bonus. Se non c'è alcun bonus, si comporta come l'item points
{
  struct level_square *dest=get_square(lv,lv->exit_y,lv->exit_x);
  int start_y=get_y(lv->p,player),start_x=get_x(lv->p,player),end_y,end_x;
  struct pos*** pred;  
  
  if(check_item(dest,door))
  {
    end_y=lv->exit_y;
    end_x=lv->exit_x;
  }
  else if(lv->bonus_tot)
  {
    end_y=get_y(get_head(lv->bonus_tot),trigger);
    end_x=get_x(get_head(lv->bonus_tot),trigger);
  }
  else
    return points(lv);

  pred=a_star(lv->m,start_y,start_x,end_y,end_x);
  
  if(pred)
  {
    fill_golden_path(pred,lv,start_y,start_x,end_y,end_x);
    free_pred(pred,lv->m->height,lv->m->width);
  }
  return (char*)__func__;
}
