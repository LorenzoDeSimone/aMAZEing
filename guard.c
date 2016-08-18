//Lorenzo De Simone N86/1008

#include <stdlib.h>
#include <stdio.h>
#include "guard.h"

/*----------------------------------------------------*/
/*                       GUARD                        */
/*----------------------------------------------------*/
guard_type char_to_guard_type(int c)//Converte il carattere in input nel guard_type corrispondente
{
  if(c=='A')
    return A;
  else if(c=='B')
    return B;
  else if(c=='C')
    return C;
  else if(c=='D')
    return D;
}

struct guard *create_custom_guard(int c, int y, int x, int range, int speed, int key_range, FNDIRECTION backup, FNDIRECTION chase, FNDIRECTION protect, FNDIRECTION retreat)//Crea una struttura di tipi guard con tutte le specifiche in input
{
  struct guard *new_guard=(struct guard*) malloc(sizeof(struct guard));
  new_guard->super=create_character(c,RED,y,x,range,speed);//Costruttore della superclasse
  //All'inizio del gioco le guardie si trovano nella stessa posizione della loro chiave
  new_guard->key_y=y;
  new_guard->key_x=x;
  new_guard->key_range=key_range;
  new_guard->status=PROTECT;
  new_guard->path_cache=NULL;
  //Assegnamento degli algoritmi di intelligenza artificiale delle guardie
  new_guard->backup=backup;  
  new_guard->chase=chase;
  new_guard->protect=protect;
  new_guard->retreat=retreat;

  return new_guard;
}

struct guard *create_guard(guard_type type, int y, int x)//Crea una struct di tipo guard usando dei set predefiniti di caratteristiche
{   
  //A seconda del tipo, vengono impostati gli algoritmi e le statistiche corrispondenti alla nuova guardia
  if(type==A)
    return create_custom_guard('A',y,x,3,10,5,panic,chase_weak,protect_random,retreat_weak);
  else if(type==B)
    return create_custom_guard('B',y,x,4,12,2,help_another_guard,chase_careful,protect_careful,retreat_careful);
  else if(type==C)
    return create_custom_guard('C',y,x,5,12,3,steal_bonus,intercept,protect_random,retreat_weak);
  else if(type==D)
    return create_custom_guard('D',y,x,6,14,2,help_another_guard,chase_a_star,protect_straight,retreat_a_star);
  else
    return NULL;
}

void free_guard_cache(struct level *lv, struct guard *g)//Dealloca la path_cache di una guardia
{
  if(g->path_cache)//Se la guardia ha ancora una cache piena, viene svuotata
  {
    free_pred(g->path_cache,lv->m->height,lv->m->width);
    g->path_cache=NULL;
  }
}

void free_guard(struct guard *g)//Dealloca una guardia
{
  free_character(g->super);
  free(g);
}

void change_status(struct level *lv, struct guard *g)//Modifica lo stato della guardia in base allo stato precedente e alla situazione di gioco
{
  int y=get_y(g,guard),x=get_x(g,guard);
  struct level_square *square;
 
  square=get_square(lv,g->key_y,g->key_x);//Ottiene la casella alle coordinate della chiave della guardia    
  
  //Si è preferito essere ridondanti nei controlli ma lasciare il flusso di controllo semplice e coerente con le funzioni di transizione di stato dell'automa
  //Ogni if esterno corrisponde ad uno stato e gli if interni verificano le eventuali transizioni
  if(g->status==BACKUP)
  {
    if(check_range(lv,y,x,get_y(lv->p,player),get_x(lv->p,player),get_range(g,guard)) && !lv->p->invincibility)//Verifica se il giocatore è nel range della guardia e non è invincibile
      g->status=CHASE; 
  }
  else if(g->status==CHASE)
  {
    if((!check_range(lv,y,x,get_y(lv->p,player),get_x(lv->p,player),get_range(g,guard)) || lv->p->invincibility) && check_item(square,key))//Se il giocatore non è in range o è invincibile e la guardia ha ancora la chiave, si entra in modalità ritirata
      g->status=RETREAT;
    else if((!check_range(lv,y,x,get_y(lv->p,player),get_x(lv->p,player),get_range(g,guard)) || lv->p->invincibility) && !check_item(square,key))//Se il giocatore non è in range o è invincibile e la guardia non ha più la chiave, si entra in modalità backup
      g->status=BACKUP;
  }
  else if(g->status==PROTECT)
  {
    if(check_range(lv,y,x,get_y(lv->p,player),get_x(lv->p,player),get_range(g,guard)) && !lv->p->invincibility)//Verifica se il giocatore è nel range della guardia e non è invincibile
      g->status=CHASE; 
    else if(!check_item(square,key))//Se la guardia non ha più la chiave, entra in modalità backup
      g->status=BACKUP;
  }
  else if(g->status==RETREAT)
  {
    if(check_range(lv,y,x,get_y(lv->p,player),get_x(lv->p,player),get_range(g,guard)) && !lv->p->invincibility)//Verifica se il giocatore è nel range della guardia e non è invincibile
      g->status=CHASE; 
    else if(check_item(square,key) && y==g->key_y && x==g->key_x)//Se la guardia ha ancora la chiave e ci è tornata sopra mentre era in modalità retreat, può iniziare il pattugliamento(La guardia inizia la partita in modalità protect mentre è sulla sua chiave)
      g->status=PROTECT;
    else if(!check_item(square,key))//Se la guardia non ha più la chiave entra in modalità backup
      g->status=BACKUP;
    if(g->status!=RETREAT)
      free_guard_cache(lv,g);//Ogni volta che la guardia esce dallo stato di ritirata, cancella la cache del percorso precedente
  }
}

void move_guard(struct level *lv, struct guard *g)//Muove una guardia nel livello
{
  int dir;
  int y=get_y(g,guard),x=get_x(g,guard);
  struct level_square *square;
  //La direzione generata dagli algoritmi é sempre possibile: tutte le verifiche di eventuali ostacoli
  //vengono valutate nelle strategie delle guardie. Qui si decide unicamente quale delle tre stragegie 
  //usare in relazione alla situazione di gioco

  square=get_square(lv,get_y(g,guard),get_x(g,guard));
  check_square(lv,g,guard);//Controlla la propria casella prima di muoversi
  remove_object(lv,g,guard);//Rimuove il riferimento della guardia dalla propria casella
  
  if(square->collectable)//Se c'era anche in item alla propria locazione, esso va stampato
    print_square(lv,square->collectable,square->collectable_Class);  
  else//Altrimenti basta cancellare il carattere della guardia
    clear_square(lv,g,guard);
  
  change_status(lv,g);//Cambia lo stato della guardia in funzione della situazione di gioco
    
  //Scelta dell'algoritmo in base allo stato
  if(g->status==CHASE)
    dir=g->chase(lv,g);
  else if(g->status==BACKUP)
    dir=g->backup(lv,g);
  else if(g->status==PROTECT)
    dir=g->protect(lv,g);
  else if(g->status==RETREAT)
    dir=g->retreat(lv,g);  

  if(dir==KEY_UP)
    set_y((get_y(g,guard)+lv->m->height-1)%lv->m->height,g,guard);  
  else if(dir==KEY_DOWN)
    set_y((get_y(g,guard)+lv->m->height+1)%lv->m->height,g,guard);  
  else if(dir==KEY_RIGHT)
    set_x((get_x(g,guard)+lv->m->width+1)%lv->m->width,g,guard);  
  else if(dir==KEY_LEFT)
    set_x((get_x(g,guard)+lv->m->width-1)%lv->m->width,g,guard);  
  
  set_direction(dir,g,guard);//Imposta la direzione della guardia
  check_square(lv,g,guard);//Gestisce tutti gli eventi che accadono entrando nella nuova casella
  move_object(lv,g,guard);//Muove la guardia nella nuova casella
  if(g->status==CHASE)
    wattron(lv->maze_win,A_REVERSE);
  print_square(lv,g,guard);//Stampa la guardia nella nuova casella
  wattroff(lv->maze_win,A_REVERSE);
}

/*               ALGORITMI AUSILIARI                  */
int get_dir(struct level *lv, struct pos*** pred, int start_y, int start_x, int end_y, int end_x)//Esaminando la matrice dei predecessori creata da un algoritmo di ricerca restituisce la direzione di movimento
{
  int res=0;
  
  if((start_y==end_y) && (start_x==end_x))//Se la posizione d'arrivo è uguale a quella di partenza, bisogna rimanere fermi
    return SPACE;
  else
  {
    if(pred[end_y][end_x])
    {
      res=get_dir(lv,pred,start_y,start_x,pred[end_y][end_x]->y,pred[end_y][end_x]->x);//Chiamata ricorsiva sul predecessore di end
      //Si verifica se il valore locale di end sia un adiacente di start: in tal caso, a seconda della sua posizione,
      //viene restituita la direzione nella quale effettuare un passo sul percorso minimo ricavato da pred
      if(end_y == ((start_y+lv->m->height-1)%lv->m->height) && end_x==start_x)
        return KEY_UP;
      else if(end_y == ((start_y+lv->m->height+1)%lv->m->height) && end_x==start_x)
        return KEY_DOWN;
      else if(end_y == start_y && end_x==(start_x+lv->m->width+1)%lv->m->width)
        return KEY_RIGHT;
      else if(end_y == start_y && end_x==(start_x+lv->m->width-1)%lv->m->width)
        return KEY_LEFT;
      else//Altrimenti non si tratta di un'adiacenza della sorgente e quindi si ritorna il valore ritornato dalla chiamata ricorsiva
        return res;
    }
    else//Se un predecessore è NULL, non esistono percorsi da start a end
      return ERR;
  } 
}

int random_dir()//Ritorna una direzione random
{
  //Viene generato un intero nell'intervallo dei seguenti valori:
  //KEY_DOWN  258
  //KEY_UP    259
  //KEY_LEFT  260
  //KEY_RIGHT 261
  return rand()%(KEY_RIGHT-KEY_DOWN+1)+KEY_DOWN;
}

int random_valid_dir(struct level *lv, int start_y, int start_x)//Ritorna una direzione random valida entro il range in input
{
  int dir,next_dir=random_dir();
  int y,x;
  
  do//Prova ciclicamente tutte le adiacenze, partendo da una casuale
  {
    dir=next_dir;
    y=start_y;
    x=start_x;
    if(dir==KEY_DOWN)
    {
      next_dir=KEY_UP;
      y=(y+lv->m->height+1)%lv->m->height;
    }
    else if(dir==KEY_UP)
    {
      next_dir=KEY_LEFT;
      y=(y+lv->m->height-1)%lv->m->height;
    }
    else if(dir==KEY_LEFT)
    {
      next_dir=KEY_RIGHT;
      x=(x+lv->m->width-1)%lv->m->width;
    }
    else if(dir==KEY_RIGHT)
    {
      next_dir=KEY_DOWN;
      x=(x+lv->m->width+1)%lv->m->width;
    }
  }while(!get_square(lv,y,x));//Essendo il labirinto una componente connessa, si avrà sempre un'adiacenza
  
  return dir;
}

struct pos **get_careful_adj(struct maze *m, int y, int x)//Funzione di adiacenza che non considera adiacenti le caselle contenenti trappole
{
  int new_y,new_x;
  struct pos **adj=NULL;
  struct level_square *square;
  if((y<m->height) && (x<m->width))
  {
     
    if(m->matrix[y][x])//Verifica che non ci sia un muro in input
    {
      adj=(struct pos**) calloc(4,sizeof(struct pos*));
      
      //UP
      new_y=(y+m->height-1)%m->height;
      square=mget_square(m,new_y,x);  
      if(square && !has_trap(square))//Verifica se c'è l'adiacenza ed essa non contiene una trappola
        adj[UP]=init_pos(new_y,x);
      else
	adj[UP]=NULL;
      
      //Procedimento speculare per le altre adiacenze
      //RIGHT
      new_x=(x+m->width+1)%m->width;
      square=mget_square(m,y,new_x);
      if(square && !has_trap(square))
        adj[RIGHT]=init_pos(y,new_x);
      else
        adj[RIGHT]=NULL;	
	
      //DOWN
      new_y=(y+m->height+1)%m->height;
      square=mget_square(m,new_y,x);  
      if(square && !has_trap(square))
        adj[DOWN]=init_pos(new_y,x);
      else
        adj[DOWN]=NULL;	
	
      //LEFT
      new_x=(x+m->width-1)%m->width;
      square=mget_square(m,y,new_x);
      if(square && !has_trap(square))
        adj[LEFT]=init_pos(y,new_x);
      else
        adj[LEFT]=NULL;	
    }
  }
  return adj;
}

int min(int a, int b)//Minimo fra due interi
{
  if(a<=b)
    return a;
  else 
    return b;
}

/*         ALGORITMI DI PROTEZIONE DELLA CHIAVE       */
int protect_random(struct level *lv, struct guard *g)//Esegue un passo random rimanendo nel range di guardia
{ 
  int y,x,dir;
  
  if(rand()%5)//La guardia ha 1/5 di possibilità di provare a muoversi: viene fatto per evitare un movimento frenetico della stessa
    return SPACE;
  else
  {
    y=get_y(g,guard);
    x=get_x(g,guard);
    dir=random_valid_dir(lv,y,x);
    //Aggiorna il valore di una delle due coordinate di partenza
    if(dir==KEY_UP)
      y=(y+lv->m->height-1)%lv->m->height;
    else if(dir==KEY_DOWN)
      y=(y+lv->m->height+1)%lv->m->height;  
    else if(dir==KEY_RIGHT)
      x=(x+lv->m->width+1)%lv->m->width;  
    else if(dir==KEY_LEFT)
      x=(x+lv->m->width-1)%lv->m->width;  
    if(check_range(lv,y,x,g->key_y,g->key_x,g->key_range))//Se nella nuova posizione c'è una casella nel range di guardia ritorna la direzione
      return dir;
    else//Altrimenti ritorna SPACE, indicando di non muoversi
      return SPACE;
  } 
}

int protect_straight(struct level *lv, struct guard *g)//Sceglie una direzione random e la segue finchè è in key_range
{
  int y,x;
  y=get_y(g,guard);
  x=get_x(g,guard);
  int dir=get_direction(g,guard);

  switch(dir)
  {   
    case SPACE:
      return protect_random(lv,g);
    break;
    case KEY_UP:
      y=(y+lv->m->height-1)%lv->m->height;
    break;
    case KEY_DOWN:
      y=(y+lv->m->height+1)%lv->m->height; 
    break;
    case KEY_RIGHT:
      x=(x+lv->m->width+1)%lv->m->width;
    break;
    case KEY_LEFT:
      x=(x+lv->m->width-1)%lv->m->width;  
    break;
  }
  if(!check_range(lv,y,x,g->key_y,g->key_x,g->key_range) || !get_square(lv,y,x))
     return protect_random(lv,g);
  return dir;
}

int protect_careful(struct level *lv, struct guard *g)//Sceglie una direzione random e la segue finchè è in key_range, evitando però trappole
{
  int ret=protect_straight(lv,g);
  struct level_square *square;
  int y,x;
  y=get_y(g,guard);
  x=get_x(g,guard);
  
  //Aggiorna il valore di una delle due coordinate di partenza
  if(ret==KEY_UP)
    y=(y+lv->m->height-1)%lv->m->height;  
  else if(ret==KEY_DOWN)
    y=(y+lv->m->height+1)%lv->m->height;  
  else if(ret==KEY_RIGHT)
    x=(x+lv->m->width+1)%lv->m->width;  
  else if(ret==KEY_LEFT)
    x=(x+lv->m->width-1)%lv->m->width; 
  
  square=get_square(lv,y,x);//Si ottiene la casella verso cui si sta per muovere un passo
  if(has_trap(square))//Se sulla nuova casella c'è una trappola, la guardia rimane ferma
    ret=SPACE;
  
  return ret;
}

/*      ALGORITMI DI INSEGUIMENTO DEL PERSONAGGIO     */
int chase_weak(struct level *lv, struct guard *g)//Esegue un passo seguendo il percorso minimo verso il giocatore con qualche passo random
{
  struct pos ***pred;
  int end_y=get_y(lv->p,player),end_x=get_x(lv->p,player);
  int y=get_y(g,guard),x=get_x(g,guard);
  int ret;

  //Ad ogni turno ci sono 2/3 di probabilità che la guardia segua il percorso minimo e 1/3 che esegua un passo casuale
  if(rand()%3)
  {
    pred=a_star(lv->m,get_y(g,guard),get_x(g,guard),end_y,end_x);
    if(pred)
    {
      ret=get_dir(lv,pred,get_y(g,guard),get_x(g,guard),end_y,end_x);
      free_pred(pred,lv->m->height,lv->m->width);
    }
  }
  else
  {
    ret=random_dir(lv,g);
    //Aggiorna il valore di una delle due coordinate di partenza
    if(ret==KEY_UP)
      y=(y+lv->m->height-1)%lv->m->height;  
    else if(ret==KEY_DOWN)
      y=(y+lv->m->height+1)%lv->m->height;  
    else if(ret==KEY_RIGHT)
      x=(x+lv->m->width+1)%lv->m->width;  
    else if(ret==KEY_LEFT)
      x=(x+lv->m->width-1)%lv->m->width;  
      
    if(!get_square(lv,y,x))//Se nella nuova posizione non c'è una casella rimane fermo
      ret=SPACE;
  }  
      
  return ret;
}

int chase_a_star(struct level *lv, struct guard *g)//Esegue un passo seguendo il percorso minimo verso il giocatore
{
  struct pos ***pred;
  int ret=SPACE,end_y=get_y(lv->p,player),end_x=get_x(lv->p,player);

  pred=a_star(lv->m,get_y(g,guard),get_x(g,guard),end_y,end_x);
   
  if(pred)
  {
    ret=get_dir(lv,pred,get_y(g,guard),get_x(g,guard),end_y,end_x);
    free_pred(pred,lv->m->height,lv->m->width);
  }
  
  return ret;
}

int chase_careful(struct level *lv, struct guard *g)
{
  struct pos ***pred;
  int ret=SPACE,end_y=get_y(lv->p,player),end_x=get_x(lv->p,player);

  //Si simula la "cautela" della guardia utilizzando una funzione apposita di adiacenza che non considera le caselle con sopra delle trappole
  pred=a_star_custom(lv->m,get_y(g,guard),get_x(g,guard),end_y,end_x,get_careful_adj);
 
  if(pred)//La guardia rimane ferma nel caso in cui tutti i percorsi verso il giocatore contengono trappole
  {
    ret=get_dir(lv,pred,get_y(g,guard),get_x(g,guard),end_y,end_x);
    free_pred(pred,lv->m->height,lv->m->width);
  }
  
  return ret;
}

int intercept(struct level *lv, struct guard *g)////Esegue un passo seguendo il percorso minimo verso una previsione della posizione futura del giocatore
{
  struct pos ***pred;
  int ret=SPACE,y,next_y,x,next_x;
  y=next_y=get_y(lv->p,player);
  x=next_x=get_x(lv->p,player);

  switch(get_direction(lv->p,player))
  {
    case SPACE:
      return chase_a_star(lv,g);
    break;
    case KEY_UP:
      while(get_square(lv,next_y,next_x))
      {
        y=next_y;
        next_y=(y+lv->m->height-1)%lv->m->height;
        if(next_y==get_y(lv->p,player) && next_x==get_x(lv->p,player))
          break;
      }
    break;
    case KEY_DOWN:
      while(get_square(lv,next_y,next_x))
      {
        y=next_y;
        next_y=(y+lv->m->height+1)%lv->m->height; 
        if(next_y==get_y(lv->p,player) && next_x==get_x(lv->p,player))
          break;
      }
    break;
    case KEY_RIGHT:
      while(get_square(lv,next_y,next_x))
      {
        x=next_x;
        next_x=(x+lv->m->width+1)%lv->m->width;  
        if(next_y==get_y(lv->p,player) && next_x==get_x(lv->p,player))
          break;
      }
    break;
    case KEY_LEFT:
      while(get_square(lv,next_y,next_x))
      {
        x=next_x;
        next_x=(x+lv->m->width-1)%lv->m->width;  
        if(next_y==get_y(lv->p,player) && next_x==get_x(lv->p,player))
          break;
      }
    break;
  }
  //Quando si esce dallo switch, (next_y,next_x) è la posizione di un muro, (y,x) quella immediatamente precedente
  pred=a_star(lv->m,get_y(g,guard),get_x(g,guard),y,x);
  if(pred)
  {
    ret=get_dir(lv,pred,get_y(g,guard),get_x(g,guard),y,x);
    free_pred(pred,lv->m->height,lv->m->width);
  }
  return ret;
}

/*                 ALGORITMI DI BACKUP                */
int panic(struct level *lv, struct guard *g)//La guardia fa mosse casuali, simulando una sorta di panico
{
  return random_valid_dir(lv,get_y(g,guard),get_x(g,guard));
}

int steal_bonus(struct level *lv, struct guard *g)//Esegue un passo verso un bonus seguendo il percorso minimo verso di esso
{
  struct trigger *tr;
  struct level_square *square;
  int ret=SPACE;
    
  if(lv->bonus_tot)
  {
    square=get_square(lv,g->key_y,g->key_x);
    if(g->path_cache && is_bonus(square->collectable))//Verifica se c'è ancora il bonus dove è diretta la guardia e che la cache sia ancora valida
      ret=get_dir(lv,g->path_cache,get_y(g,guard),get_x(g,guard),g->key_y,g->key_x);
    tr=get_head(lv->bonus_tot);
    //Poichè è un algoritmo di backup e la chiave della guardia è stata presa, vengono sfruttate le coordinate della chiave per memorizzare la posizione del bonus da raggiungere
    //La coerenza con la funzione move_guard() è garantita dal fatto che nesusn bonus può apparire sulla posizione di una chiave e se la guardia usa questo algoritmo di
    //backup non avrà mai più bisogno della posizione della propria chiave
    g->key_y=get_y(tr,trigger);
    g->key_x=get_x(tr,trigger);
    free_guard_cache(lv,g);
    g->path_cache=a_star(lv->m,get_y(g,guard),get_x(g,guard),get_y(tr,trigger),get_x(tr,trigger));//Segue il percorso minimo verso il bonus in testa alla lista
    if(g->path_cache)
      ret=get_dir(lv,g->path_cache,get_y(g,guard),get_x(g,guard),g->key_y,g->key_x);
  }
  return ret;
}

int help_another_guard(struct level *lv, struct guard *g)//Modifica il valore della chiave della guardia in input con quella di un'altra. Ritorna sempre SPACE
{  
  struct level_square *square=get_square(lv,get_y(g,guard),get_x(g,guard));;
  struct list *L;
  struct guard *another_g;
  int new_key=0;

  if(lv->p->keys<lv->n_keys)//Se il giocatore ha acquisito tutte le chiavi, è inutile scorrere la lista e cercare di aiutare un'altra guardia
  {
    //Se la guardia non ha più la propria chiave, allora cerca un altra guardia con una chiave: se la trova, la imposta come propria e dal turno successivo userà i suoi
    //algoritmi di guardia e, qualora si trovi lontano, la procedura di come_back()
    if(!check_item(square,key))
    {
      L=lv->guards_tot;
      while(has_next(L) && !new_key)
      {
        another_g=get_head(L);
        square=get_square(lv,another_g->key_y,another_g->key_x); 
        if(check_item(square,key)) 
        {
          g->key_y=get_y(square->collectable,square->collectable_Class);
          g->key_x=get_x(square->collectable,square->collectable_Class);
          g->status=RETREAT;//La guardia va messa nello stato di ritirata per farle raggiungere la nuova chiave
          new_key=1;
        }
        L=next(L);
      }      
    }
  }    
  return panic(lv,g);
}

/*               ALGORITMI DI RITIRATA                  */
int retreat_weak(struct level *lv, struct guard *g)//Algoritmo fa intraprendere ad una guardia il percorso minimo verso la propria chiave con qualche passo random
{
  int ret=SPACE;
  int y=get_y(g,guard),x=get_x(g,guard);

  if(rand()%3)
  {
    if(!g->path_cache)
      g->path_cache=a_star(lv->m,get_y(g,guard),get_x(g,guard),g->key_y,g->key_x);
    if(g->path_cache)
      ret=get_dir(lv,g->path_cache,get_y(g,guard),get_x(g,guard),g->key_y,g->key_x);
  }
  else
  {
    free_guard_cache(lv,g);//Cancella la cache del percorso della guardia
    ret=random_dir(lv,g);
    //Aggiorna il valore di una delle due coordinate di partenza
    if(ret==KEY_UP)
      y=(y+lv->m->height-1)%lv->m->height;  
    else if(ret==KEY_DOWN)
      y=(y+lv->m->height+1)%lv->m->height;  
    else if(ret==KEY_RIGHT)
      x=(x+lv->m->width+1)%lv->m->width;  
    else if(ret==KEY_LEFT)
      x=(x+lv->m->width-1)%lv->m->width;  
      
    if(!get_square(lv,y,x))//Se nella nuova posizione non c'è una casella rimane fermo
      ret=SPACE;
  }  
  
  return ret;
}

int retreat_a_star(struct level *lv, struct guard *g)//Algoritmo fa intraprendere ad una guardia il percorso minimo verso la propria chiave
{
  int ret=SPACE;

  if(!g->path_cache)//Se non si ha una cache del percorso valida, viene ricalcolata
    g->path_cache=a_star(lv->m,get_y(g,guard),get_x(g,guard),g->key_y,g->key_x);
  
  if(g->path_cache)
    ret=get_dir(lv,g->path_cache,get_y(g,guard),get_x(g,guard),g->key_y,g->key_x);//Viene ottenuta la direzione dall'array pred memorizzato nella guardia

  return ret;
}

int retreat_careful(struct level *lv, struct guard *g)//Algoritmo fa intraprendere ad una guardia il percorso minimo verso la propria chiave evitando le trappole
{
  int ret=SPACE;
  int y=get_y(g,guard),x=get_x(g,guard);
  struct level_square *square;

  if(!g->path_cache)//Se non si ha una cache del percorso valida, viene ricalcolata
    g->path_cache=a_star_custom(lv->m,get_y(g,guard),get_x(g,guard),g->key_y,g->key_x,get_careful_adj);
  
  if(g->path_cache)
  {
    ret=get_dir(lv,g->path_cache,get_y(g,guard),get_x(g,guard),g->key_y,g->key_x);//Viene ottenuta la direzione dall'array pred memorizzato nella guardia
    //Aggiorna il valore di una delle due coordinate di partenza
    if(ret==KEY_UP)
      y=(y+lv->m->height-1)%lv->m->height;  
    else if(ret==KEY_DOWN)
      y=(y+lv->m->height+1)%lv->m->height;  
    else if(ret==KEY_RIGHT)
      x=(x+lv->m->width+1)%lv->m->width;  
    else if(ret==KEY_LEFT)
      x=(x+lv->m->width-1)%lv->m->width; 
    square=get_square(lv,y,x);//Si ottiene la casella verso cui si sta per muovere un passo
    if(has_trap(square))//Se sul percorso calcolato precedentemente è spuntata una nuova trappola, si ricalcola il percorso
    {
      free_guard_cache(lv,g);
      g->path_cache=a_star_custom(lv->m,get_y(g,guard),get_x(g,guard),g->key_y,g->key_x,get_careful_adj);
      if(g->path_cache)
        ret=get_dir(lv,g->path_cache,get_y(g,guard),get_x(g,guard),g->key_y,g->key_x);//Viene ottenuta la direzione dall'array pred memorizzato nella guardia
      else
        ret=SPACE;//Se non ci sono percorsi senza trappole, la guardia rimane ferma
    }
  }
  return ret;
}