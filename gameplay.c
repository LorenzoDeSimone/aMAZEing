//Lorenzo De Simone N86/1008

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "gameplay.h"
   
#define n_choices  7//Numero di scelte possibili nel menu
#define n_levels   3//Numero di livelli nel gioco

#define WIDTH      18
#define HEIGHT     n_choices+2

game_mode curr_mode;//Modalità di gioco corrente

/*----------------------------------------------------*/
/*                   GUI HANDLING                     */
/*----------------------------------------------------*/
void menu_amazeing()//Crea il menu di selezione dei livelli del gioco aMAZEing
{
  int i,chosen;
  struct level *lv;
  char **choices;
  struct player_data *data;
  char level_path[100],maze_path[100];
  game_status result;
  
  resize_term(35, 100);//Resize del terminale

  //Inizializzazione dei colori
  init_pair(STANDARD, COLOR_WHITE , COLOR_BLACK);
  init_pair(BLUE    , COLOR_BLUE  , COLOR_BLACK); 
  init_pair(GREEN   , COLOR_GREEN , COLOR_BLACK);
  init_pair(RED     , COLOR_RED   , COLOR_BLACK);
  init_pair(YELLOW  , COLOR_YELLOW, COLOR_BLACK);    

  //Inizializzazione delle strutture per la generazione di oggetti random
  init_all_item();
  init_all_bonus();
  init_all_malus();
  
  srand(time(NULL));
  
  //Per mantenere coerente la selezione inserendo nuovi livelli, basta inserire il nuovo livello (Es "Level 4" fra "Level 3" e "Adventure")
  choices=init_choices(n_choices,"Level 1","Level 2","Level 3","Adventure","Random Survival","Custom Level","Exit");  
  do
  {
    curr_mode=SINGLE_GAME;
    chosen=print_menu(HEIGHT,WIDTH,choices,n_choices,GREEN,print_amazeing_logo);
    attron(COLOR_PAIR(GREEN));
    
    if(chosen<=n_levels)//Livello singolo
    {
      lv=load_level(NULL,chosen);
      loading_screen(lv);
      game_result(lv);
      free_level(lv);  
    }
    else if(chosen==n_levels+1)//Avventura
    {
      curr_mode=ADVENTURE;
      data=NULL;
      i=1;
      do//Il ciclo continua finchè ci sono livelli, finchè non si verifica gameover e le operazioni di parsing non danno luogo a errori
      {  
        lv=load_level(data,i);
        free(data);
        loading_screen(lv);
        if(lv)//Se c'è stato qualche errore nella lettura lv è NULL
        {
          //Se lv!=NULL significa che si è appena usciti da un livello
          data=save_player_data(lv);//Salva le statistiche del giocatore
          result=lv->status;//Si salva l'esito del livello
          game_result(lv);//Si stampa a video l'esito del livello
          free_level(lv);//Si dealloca il livello
        }
        i++;//Aumento dell'indice di livello
      }
      while(i<=n_levels && result==LEVEL_CLEARED);
    }
    else if(chosen==n_levels+2)//Generazione di livelli random in serie
    {
      curr_mode=SURVIVAL;
      data=NULL;
      i=1;
      do
      {  
        lv=load_random_level(data,(rand() % (max_height+1-min_height))+min_height,
                                  (rand() % (max_width +1-min_width ))+min_width ,i,random_maze);
        free(data);
        loading_screen(lv);
        if(lv)//Se c'è stato qualche errore nella lettura lv è NULL
        {
          //Se lv!=NULL significa che si è appena usciti da un livello
          data=save_player_data(lv);//Salva le statistiche del giocatore
          result=lv->status;//Si salva l'esito del livello
          game_result(lv);//Si stampa a video l'esito del livello
          free_level(lv);//Si dealloca il livello
        }
        i++;//Aumento dell'indice di livello
      }while(lv && result==LEVEL_CLEARED);
    }
    else if(chosen==n_levels+3)//Livello personalizzato
    {
      curr_mode=CUSTOM;
      data=NULL;
      echo();
        
      mvprintw(LINES/2,(COLS-18)/2,"Insert level path\n");
      move(LINES/2+1,(COLS-18)/2);
      refresh();
      getnstr(level_path,99);
      erase();
        
      mvprintw(LINES/2,(COLS-16)/2,"Insert maze path\n");
      move(LINES/2+1,(COLS-16)/2);
      refresh();
      getnstr(maze_path,99);
      erase();
        
      noecho();
      lv=load_custom_level(data,level_path,maze_path);
      loading_screen(lv);
      game_result(lv);
      free_level(lv);
    }
  }while(chosen!=n_choices);
}

void loading_screen(struct level* lv)//Schermata di caricamento del livello selezionato
{  
  if(lv)
  {
    if(curr_mode==CUSTOM)
      mvprintw(LINES/2,(COLS-43)/2,"Custom Level Loaded, press any key to start");
    else
      mvprintw(LINES/2,(COLS-38)/2,"Level %d Loaded, press any key to start",lv->lv_n);
    refresh();
    getch();//Attende da input il tasto di conferma
    erase();
    refresh();
    amazeing_gameplay(lv);
  }
  else
  {
    //C'è stato un errore nel parsing del labirinto o delle info del livello
    mvprintw(LINES/2+2,(COLS-25)/2,"Press any key to continue");
    refresh();
    getch();//Attende da input il tasto di conferma  
  }
  erase();
}

void game_result(struct level *lv)//Stampa a video l'esito della partita
{
  if(lv)
  {
    if(lv->status==LEVEL_CLEARED)
    {
      if(curr_mode==CUSTOM)
        mvprintw(LINES/2,(COLS-12)/2,"Custom Level");
      else
        mvprintw(LINES/2,(COLS-7)/2,"Level %d",lv->lv_n);
      mvprintw(LINES/2+1,(COLS-15)/2,"Player Score: %d",lv->p->points);
      if(curr_mode==ADVENTURE)
      {
        if(lv->lv_n==n_levels && curr_mode==ADVENTURE)
          mvprintw(LINES/2+2,(COLS-40)/2,"Congratulations! You cleared all levels!");
        else
          mvprintw(LINES/2+2,(COLS-50)/2,"Level %d cleared! Press any key to load next level",lv->lv_n);
      }
      print_victory_logo(LINES/2,(COLS-56)/2);
      getch();
    }
    else if(lv->status==GAME_OVER)
    {
      attron(COLOR_PAIR(RED));    
      if(curr_mode==CUSTOM)
        mvprintw(LINES/2,(COLS-12)/2,"Custom Level");
      else
        mvprintw(LINES/2,(COLS-7)/2,"Level %d",lv->lv_n);
      mvprintw(LINES/2+1,(COLS-15)/2,"Player Score: %d",lv->p->points);
      if(curr_mode==SURVIVAL)
        mvprintw(LINES/2+2,(COLS-20)/2,"You cleared %d levels",lv->lv_n);
      print_gameover_logo(LINES/2,(COLS-73)/2);
      attroff(COLOR_PAIR(RED));
      getch();
    }
  }
  erase();
}

void print_amazeing_logo(int start_y, int start_x)//Funzione per la stampa del logo sopra al menu di selezione
{
  mvprintw(start_y-10,start_x-24,"                 ______  ________   ____");
  mvprintw(start_y-9 ,start_x-24,"         /'\\_/`\\/\\  _  \\/\\_____  \\ /\\  _`\\   __");
  mvprintw(start_y-8 ,start_x-24,"   __   /\\      \\ \\ \\L\\ \\/____//'/'\\ \\ \\L\\_\\/\\_\\    ___      __");
  mvprintw(start_y-7 ,start_x-24," /'__`\\ \\ \\ \\__\\ \\ \\  __ \\   //'/'  \\ \\  _\\L\\/\\ \\ /' _ `\\  /'_ `\\");
  mvprintw(start_y-6 ,start_x-24,"/\\ \\L\\.\\_\\ \\ \\_/\\ \\ \\ \\/\\ \\ //'/'___ \\ \\ \\L\\ \\ \\ \\/\\ \\/\\ \\/\\ \\L\\ \\");      
  mvprintw(start_y-5 ,start_x-24,"\\ \\__/.\\_\\\\ \\_\\\\ \\_\\ \\_\\ \\_\\/\\_______\\\\ \\____/\\ \\_\\ \\_\\ \\_\\ \\____ \\");
  mvprintw(start_y-4 ,start_x-24," \\/__/\\/_/ \\/_/ \\/_/\\/_/\\/_/\\/_______/ \\/___/  \\/_/\\/_/\\/_/\\/___L\\ \\");
  mvprintw(start_y-3 ,start_x-24,"                                                             /\\____/");
  mvprintw(start_y-2 ,start_x-24,"                                                             \\_/__/ ");
  refresh();
}

void print_gameover_logo(int start_y, int start_x)//Stampa il logo di game over
{
  mvprintw(start_y-8,start_x , " ____                                     _____");
  mvprintw(start_y-7,start_x , "/\\  _`\\                                  /\\  __`\\");
  mvprintw(start_y-6,start_x , "\\ \\ \\L\\_\\     __      ___ ___      __    \\ \\ \\/\\ \\  __  __     __   _ __");
  mvprintw(start_y-5,start_x , " \\ \\ \\L_L   /'__`\\  /' __` __`\\  /'__`\\   \\ \\ \\ \\ \\/\\ \\/\\ \\  /'__`\\/\\`'__\\");
  mvprintw(start_y-4,start_x , "  \\ \\ \\/, \\/\\ \\L\\.\\_/\\ \\/\\ \\/\\ \\/\\  __/    \\ \\ \\_\\ \\ \\ \\_/ |/\\  __/\\ \\ \\/");      
  mvprintw(start_y-3,start_x , "   \\ \\____/\\ \\__/.\\_\\ \\_\\ \\_\\ \\_\\ \\____\\    \\ \\_____\\ \\___/ \\ \\____\\\\ \\_\\");
  mvprintw(start_y-2,start_x , "    \\/___/  \\/__/\\/_/\\/_/\\/_/\\/_/\\/____/     \\/_____/\\/__/   \\/____/ \\/_/");
  refresh(); 
}

void print_victory_logo(int start_y, int start_x)//Stampa il logo di vittoria
{
  mvprintw(start_y-10,start_x, " __  __              __                           __");
  mvprintw(start_y-9 ,start_x, "/\\ \\/\\ \\  __        /\\ \\__                       /\\ \\");
  mvprintw(start_y-8 ,start_x, "\\ \\ \\ \\ \\/\\_\\    ___\\ \\ ,_\\   ___   _ __   __  __\\ \\ \\");
  mvprintw(start_y-7 ,start_x, " \\ \\ \\ \\ \\/\\ \\  /'___\\ \\ \\/  / __`\\/\\`'__\\/\\ \\/\\ \\\\ \\ \\");
  mvprintw(start_y-6 ,start_x, "  \\ \\ \\_/ \\ \\ \\/\\ \\__/\\ \\ \\_/\\ \\L\\ \\ \\ \\/ \\ \\ \\_\\ \\\\ \\_\\");      
  mvprintw(start_y-5 ,start_x, "   \\ `\\___/\\ \\_\\ \\____\\\\ \\__\\ \\____/\\ \\_\\  \\/`____ \\\\/\\_\\");
  mvprintw(start_y-4 ,start_x, "    `\\/__/  \\/_/\\/____/ \\/__/\\/___/  \\/_/   `/___/> \\\\/_/");
  mvprintw(start_y-3 ,start_x, "                                               /\\___/");      
  mvprintw(start_y-2 ,start_x, "                                               \\/__/");
  refresh(); 
}

void create_maze_win(struct level *lv)//Inizializza la finestra di gioco disegnando il labirinto
{
  int i,j;
  lv->maze_win=newwin(lv->m->height,lv->m->width,2,(COLS-lv->m->width)/2);
  wattron(lv->maze_win,COLOR_PAIR(BLUE));
  
  for(i=0;i<lv->m->height;i++)
    for(j=0;j<lv->m->width;j++)
      if(!lv->m->matrix[i][j])
        mvwaddch(lv->maze_win,i,j,ACS_BLOCK);
    
  wattroff(lv->maze_win,COLOR_PAIR(BLUE));
  
  keypad(lv->maze_win, TRUE);//Abilito l'input dei tasti control
  nodelay(lv->maze_win,TRUE);//Input non bloccante da tastiera
}

void create_info_win(struct level *lv)//Inizializza la finestra delle info sul gioco
{
  lv->info_win=newwin(12,lv->m->width/3,lv->m->height+3,(COLS-lv->m->width)/2);
  box(lv->info_win,0,0);
}

void create_log_win(struct level *lv)//Inizializza la finestra contenente il log delle azioni di gioco
{     
  WINDOW *frame=newwin(12,(lv->m->width*2)/3,lv->m->height+3,(COLS-lv->m->width)/2+lv->m->width/3+2);  
  box(frame,0,0);
  wrefresh(frame);
  delwin(frame);
  
  lv->log_win=newwin(10,(lv->m->width*2)/3-2,lv->m->height+4,(COLS-lv->m->width)/2+lv->m->width/3+3);  
  scrollok(lv->log_win, TRUE);
  idlok(lv->log_win,TRUE);
}

void update_info_win(struct level *lv)//Esegue un refresh sulla finestra con le statistiche di gioco
{
  int i=1;
  werase(lv->info_win);

  wattron(lv->info_win,COLOR_PAIR(STANDARD));  
  box(lv->info_win,0,0);
  if(lv->p->keys==lv->n_keys)
    wattron(lv->info_win,COLOR_PAIR(GREEN)); 
  mvwprintw(lv->info_win,i++,1,"Keys  :%d/%d",lv->p->keys,lv->n_keys);
  
  wattron(lv->info_win,COLOR_PAIR(STANDARD));
  if(lv->time_left<=30) 
    wattron(lv->info_win,COLOR_PAIR(RED));
  else if(lv->time_left<=60)
    wattron(lv->info_win,COLOR_PAIR(YELLOW));
  mvwprintw(lv->info_win,i++,1,"Time  :%d",lv->time_left);
  
  wattron(lv->info_win,COLOR_PAIR(STANDARD));
  if(lv->p->lives==1)
    wattron(lv->info_win,COLOR_PAIR(RED));
  mvwprintw(lv->info_win,i++,1,"Lives :%d",lv->p->lives);
  
  wattron(lv->info_win,COLOR_PAIR(STANDARD));
  if(get_range(lv->p,player)<=0)
    wattron(lv->info_win,COLOR_PAIR(RED));
  mvwprintw(lv->info_win,i++,1,"Range :%d",get_range(lv->p,player));
  
  wattron(lv->info_win,COLOR_PAIR(STANDARD));
  mvwprintw(lv->info_win,i++,1,"Points:%d",lv->p->points);
  mvwprintw(lv->info_win,i++,1,"SKILL COSTS",lv->p->points);
  mvwprintw(lv->info_win,i++,1,"1)TimeShot:%d",lv->p->skill_set[0].cost);
  mvwprintw(lv->info_win,i++,1,"2)SetTrap :%d",lv->p->skill_set[1].cost);
  mvwprintw(lv->info_win,i++,1,"3)Drill   :%d",lv->p->skill_set[2].cost);
  mvwprintw(lv->info_win,i++,1,"4)Escape  :%d",lv->p->skill_set[3].cost);
}

void update_log_win(struct level *lv, char *action, char *name, int color)//Esegue un refresh sulla finestra con le azioni di gioco
{
  wattron(lv->log_win,COLOR_PAIR(color));
  wprintw(lv->log_win,">%s %s\n",action,name);
  wattroff(lv->log_win,COLOR_PAIR(color));
}

/*----------------------------------------------------*/
/*                     GAMEPLAY                       */
/*----------------------------------------------------*/
void amazeing_gameplay(struct level *lv)//Fa partire il livello in input con i dati di un giocatore e la modalità di gioco scelta
{
  double now,last;//Tempi con risoluzione di millisecondi
  long sec_now,sec_last;//Tempi con risoluzione di secondi
  struct list *L;
  struct timeval t;
  struct guard* g;
  
  gettimeofday(&t,NULL);  
  last=(t.tv_sec+((double)t.tv_usec)/1000000);//Secondi,millisecondi totali
  sec_last=t.tv_sec;

  //Creazione della GUI del livello
  create_maze_win(lv); 
  create_info_win(lv);
  create_log_win(lv);
   
  //Loop principale del gioco
  do
  {  
    update_info_win(lv);//Update della finestra con le statistiche del gioco
    //Refresh grafico delle finestre
    wrefresh(lv->maze_win);
    wrefresh(lv->info_win);
    wrefresh(lv->log_win);

    gettimeofday(&t,NULL);  
    now=(t.tv_sec+((double)t.tv_usec)/1000000);//Secondi,millisecondi totali
    sec_now=t.tv_sec;

    if(sec_now-sec_last)//Verifica se è passato almeno un secondo dal ciclo di polling precedente
    {
      if(lv->p->keys<lv->n_keys)//Quando il giocatore ha collezionato tutte le chiavi, non vengono generati nuovi bonus  
        add_random_collectable(lv);//Aggiunge un oggetto collectable casuale al terreno di gioco
      //Aggiornamento del tempo rimasto per finire il livello
      lv->time_left--;
      //Scadenza dei bonus
      lv->bonus_tot=vanish_handling(lv,lv->bonus_tot);
      //Scadenza dei malus
      lv->malus_tot=vanish_handling(lv,lv->malus_tot);
      expire_handling(lv,lv->p,player);//Scadenza dei bonus/malus del giocatore
      if(lv->p->invincibility)//Se il giocatore è invincibile, si scala un secondo dal periodo di invincibilità
        lv->p->invincibility--;
    }
    
    //Movimento del giocatore
    if(delta_time(last,now,lv->p,player)>0)
      move_player(lv);
    
    //Movimento delle guardie
    L=lv->guards_tot;
    while((lv->status==PLAYING)&&(has_next(L)))
    {
      g=get_head(L);
      if(delta_time(last,now,g,guard)>0)
        move_guard(lv,g);
      if(sec_now-sec_last)//Se è passato un secondo gestisce tutti i bonus/malus attivi delle guardie
        expire_handling(lv,g,guard);
      L=next(L);
    }
    
    last=now;
    sec_last=sec_now;
    time_check(lv);
    
  }while(lv->status==PLAYING); 
}

void time_check(struct level *lv)//Se i secondi rimasti sono 0 e il giocatore ha ancora una vita, la rimuove e aggiunge 30 secondi. Altrimenti è gameover
{
  if(lv->time_left==0)
  {
    if(--lv->p->lives)
    {
      lv->time_left=30;
      update_log_win(lv,"One life lost:","You have 30 seconds more to escape!",RED);
    }
    else
      lv->status=GAME_OVER; 
  }
}

int delta_time(double last, double now, void* object, Class c)//Calcola il nuovo delta del personaggio in input e restituisce 1 se il personaggio deve agire, 0 altrimenti 
{
  set_delta(get_delta(object,c)+(now-last),object,c);//Tempo trascorso dall'ultimo ciclo di polling
  
  if(c==player || c==guard)
  {
    if((get_speed(object,c)*get_delta(object,c))>2)
    {
      set_delta(0,object,c);
      return 1;
    }
    else
      return 0;
  }
  else
    return -1;
}

int has_vanished(struct trigger *t)//Diminuisce di uno il tempo rimasto al trigger sul terreno di gioco e ritorna 1 se deve svanire, 0 altrimenti
{
  if(--t->time_left)//Se ha un valore diverso da 0, l'oggetto non deve svanire
    return 0;
  else
    return 1;
}

struct list *vanish_handling(struct level *lv, struct list *L)//Gestisce la scadenza di tutti i trigger di una lista e restituisce una lista con tutti i trigger ancora presenti
{
  struct level_square *curr_square;
  struct list *new_L=NULL;
  struct trigger *tr;
  
  while(L)
  {
    tr=get_head(L);//Prendo il primo trigger dalla lista
    curr_square=get_square(lv,get_y(tr,trigger),get_x(tr,trigger));//Ottengo la casella dove dovrebbe trovarsi
    if(curr_square->collectable==tr && has_vanished(tr))//Verifica se il trigger è ancora nella sua casella e che il suo tempo sia scaduto
    {
      clear_square(lv,tr,trigger);//Cancella il carattere dal labirinto
      remove_object(lv,tr,trigger);//Rimuove l'oggetto dalla casella
      free_object(tr,trigger);//Dealloca il trigger
    }
    else if(curr_square->collectable!=tr)//Se l'oggetto è stato preso prima della scadenza, viene deallocato
      free_object(tr,trigger);//Dealloca il trigger
    else//Altrimenti il trigger è ancora in gioco e viene messo nella nuova lista
      new_L=insert_rear(new_L,tr);

    L=delete_head(L);//Viene cancellato il trigger dalla vecchia lista    
  }
  return new_L;//Ritorna la lista con tutti i trigger ancora attivi
}

void expire_handling(struct level *lv, void *object, Class c)//Gestisce gli effetti del personaggio e notifica lo svanire degli effetti nel log del gioco
{
  struct list *new_bonus=NULL,*new_malus=NULL;
  struct mod *m;
  char *effect_name;
  struct character *curr_character=get_character(object,c);
  
  if(curr_character)
  {
    while(curr_character->bonus)//Scorre la lista dei bonus attivi del personaggio
    {
      m=get_head(curr_character->bonus);//Estrae il primo bonus dalla lista
      curr_character->bonus=delete_head(curr_character->bonus);
      if(has_expired(m))//Ne verifica la scadenza
      {
        effect_name=m->effect(lv,object,c,OFF);//Richiama via callback l'effetto del bonus per eliminarne gli effetti benefici
        //Aggiorna il log di sistema dell'avvenuta scadenza
        if(c==player)
          update_log_win(lv,"The player's bonus wears off:",effect_name,GREEN);
        else if(c==guard)
          update_log_win(lv,"One guard's bonus wears off:",effect_name,GREEN);
        free(m);//Deallocazione della struct mod relativa all'effetto attivo
      }
      else
        new_bonus=insert_head(new_bonus,m);//Se non è ancora scaduto, viene messo nella lista di bonus ancora attivi
    }
    curr_character->bonus=new_bonus;//Si assegna la lista con i bonus ancora attivi al personaggio
    
    //Comportamento speculare per i malus
    while(curr_character->malus)
    {
      m=get_head(curr_character->malus);
      curr_character->malus=delete_head(curr_character->malus);
      if(has_expired(m))
      {
        effect_name=m->effect(lv,object,c,OFF);
        if(c==player)
          update_log_win(lv,"The player's malus wears off:",effect_name,RED);
        else if(c==guard)
          update_log_win(lv,"One guard's malus wears off:",effect_name,RED);
        free(m);
      }
      else
        new_malus=insert_head(new_malus,m);
    }
    curr_character->malus=new_malus;
  }
}

void death(struct level *lv)//Rimuove una vita al giocatore e modifica coerentemente lo status del gioco
{
  lv->p->lives--;//Rimozione della vita
  if(lv->p->lives)
  {
    cure(lv,lv->p,player,ON);//Rimuove tutti i malus dal giocatore
    //Riporta il personaggio alla posizione iniziale e ne annulla l'eventuale direzione
    set_y(lv->start_y,lv->p,player); 
    set_x(lv->start_x,lv->p,player);
    set_direction(SPACE,lv->p,player);  
    move_object(lv,lv->p,player);//Il personaggio viene mosso
    print_square(lv,lv->p,player);//Il personaggio viene stampato 
    lv->p->invincibility=3;//Per tre secondi il giocatore diviene invincibile
    update_log_win(lv,"The player lost","one life",RED);
  }
  else
    lv->status=GAME_OVER;//Se il giocatore ha terminato le vite apparirà la schermata di gameover
}

void add_random_collectable(struct level *lv)//Aggiunge un oggetto collectable casuale al terreno di gioco
{
  void *o=NULL;
  int choice;
  struct pos *r_pos;
  struct level_square *square;
  
  choice=rand()%8;//Genera un numero casuale fra 0 e 7: se è compreso fra 0 e 2, viene generato un oggetto
  if(choice<3)
  {
    r_pos=random_pos(lv);//Genera la posizione di una casella random
    if(r_pos)//Verifica che la casella individuata sia valida 
    {
      square=get_square(lv,r_pos->y,r_pos->x);
      if(square->collectable)
      {
        remove_object(lv,square->collectable,square->collectable_Class);
        if(square->collectable_Class==item)
          free_object(square->collectable,square->collectable_Class);
      } 
      switch(choice)
      {
        case 0://Item casuale
          o=create_item(r_pos->y,r_pos->x,rand_item());
          add_object(lv,o,item);
          print_square(lv,o,item);
        break;
      
        case 1://Bonus casuale
          o=create_trigger(r_pos->y,r_pos->x,BONUS,rand_bonus());
          add_object(lv,o,trigger);
          print_square(lv,o,trigger);
        break;
      
        case 2://Malus casuale
          o=create_trigger(r_pos->y,r_pos->x,MALUS,rand_malus());
          add_object(lv,o,trigger);
          print_square(lv,o,trigger);
        break;
      }
      free(r_pos);
    }
  }
}