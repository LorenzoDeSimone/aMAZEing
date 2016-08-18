//Lorenzo De Simone N86/1008

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <math.h>
#include "maze.h"

/*----------------------------------*/
//Funzioni di base sul labirinto

struct maze *init_maze(int height, int width)//Inizializza un labirinto
{
  int i;
  struct maze *new_maze=(struct maze*) malloc(sizeof(struct maze)); 
  
  if(height>0 && width>0)
  {
    new_maze->height=height;
    new_maze->width=width;
    new_maze->matrix=(struct square***) calloc(height,sizeof(struct square**));
  
    for(i=0;i<height;i++)
      new_maze->matrix[i]=(struct square**) calloc(width,sizeof(struct square*));
  }
  return new_maze;
}

void free_maze(struct maze *m)//Dealloca un labirinto
{
  int i,j;
  
  for(i=0;i<m->height;i++)
  {
    for(j=0;j<m->width;j++)
      free(m->matrix[i][j]);//Dealloca le struct square
    free(m->matrix[i]);
  }
  free(m->matrix);
  free(m);
}

void add_vertex(struct maze *m, int y, int x)//Inserisce un corridoio alla posizione (y,x)
{
  struct pos **adj=NULL;
  
  if((y<m->height) && (x<m->width))
  {
    if(!m->matrix[y][x])//Verifica che non ci sia già un corridoio
    {
      m->matrix[y][x]=(struct square*) malloc(sizeof(struct square)); 
      adj=get_adj(m,y,x);
 
      //Si inizializzano gli archi uscenti della nuova casella a 1 per i corridoi adiacenti e a 0
      //per i muri adiacenti
      //Vengono inoltre aggiornati gli archi entranti a 1, poichè prima c'era sicuramente un muro
       
      //Entranti ed uscenti di sopra
      if(adj[UP])//Verifica la presenza dell'adiacenza
      {
	set_weight(m,y,x,1,UP);//Arco uscente verso l'alto impostato a 1
	set_weight(m,adj[UP]->y,adj[UP]->x,1,DOWN);//Arco entrante dall'alto verso il basso impostato a 1
      }
      else
	set_weight(m,y,x,0,UP);//Se sopra c'è un muro, l'arco uscente verso l'alto viene impostato a 0

      //Procedimento speculare per le altre adiacenze
      
      //Entranti ed uscenti di destra
      if(adj[RIGHT])
      {
	set_weight(m,y,x,1,RIGHT);
	set_weight(m,adj[RIGHT]->y,adj[RIGHT]->x,1,LEFT);
      }
      else
	set_weight(m,y,x,0,RIGHT);
      
      //Entranti ed uscenti di sotto
      if(adj[DOWN])
      {
	set_weight(m,y,x,1,DOWN);
	set_weight(m,adj[DOWN]->y,adj[DOWN]->x,1,UP);
      }
      else
	set_weight(m,y,x,0,DOWN);
      
      //Entranti ed uscenti di sinistra
      if(adj[LEFT])
      {
	set_weight(m,y,x,1,LEFT);
        set_weight(m,adj[LEFT]->y,adj[LEFT]->x,1,RIGHT);
      }
      else
	set_weight(m,y,x,0,LEFT);
      
      free_adj(adj);
    }
  }
}

void del_vertex(struct maze *m, int y, int x)//Inserisce un muro alla posizione (y,x) (N.B. l'info NON viene deallocato)
{
  struct pos **adj;
 
  //Assegno 0 a tutti gli archi entranti nella casella da cancellare
  adj=get_adj(m,y,x);
  
  if(adj)
  {
    if(adj[UP])
      set_weight(m,adj[UP]->y,adj[UP]->x,0,DOWN);
    if(adj[RIGHT])
      set_weight(m,adj[RIGHT]->y,adj[RIGHT]->x,0,LEFT);
    if(adj[DOWN])
      set_weight(m,adj[DOWN]->y,adj[DOWN]->x,0,UP);
    if(adj[LEFT])
      set_weight(m,adj[LEFT]->y,adj[LEFT]->x,0,RIGHT);
    
    free_adj(adj);
  }
  
  free(m->matrix[y][x]);
  m->matrix[y][x]=NULL;
}

void set_weight(struct maze *m, int y, int x, float weight, int d)//Imposta il peso dell'arco uscente in input
{
  if((y<m->height) && (x<m->width) && weight>0)
  {
    if(m->matrix[y][x])//Verifica che ci sia un corridoio
    {
      if(d==UP)
        m->matrix[y][x]->u_weight=weight;
      else if(d==RIGHT)
	m->matrix[y][x]->r_weight=weight;
      else if(d==DOWN)
	m->matrix[y][x]->d_weight=weight;
      else if(d=LEFT)
	m->matrix[y][x]->l_weight=weight;
    }
  }
}

struct pos **get_adj(struct maze *m, int y, int x)//Restituisce una struct adj contenente tutte le adiacenze della casella in posizione (y,x)
{
  int new_y,new_x;
  struct pos **adj=NULL;
  
  if((y<m->height) && (x<m->width))
  {
    if(m->matrix[y][x])//Verifica che non ci sia un muro in input
    {
      adj=(struct pos**) calloc(4,sizeof(struct pos*));
      
      //Inserisce in ad tutte le adiacenze controllando i limiti della matrice
      //Se c'è un corridoio adiacente, viene allocato un campo pos con le coordinate dell'adiacenza
      //altrimenti il campo della struct adj viene impostato a NULL

      //UP
      new_y=(y+m->height-1)%m->height;
      if(m->matrix[new_y][x])//Verifica che l'adiacente non sia un muro
        adj[UP]=init_pos(new_y,x);
      else
	adj[UP]=NULL;
      
      //Procedimento speculare per le altre adiacenze
      //RIGHT
      new_x=(x+m->width+1)%m->width;
      if(m->matrix[y][new_x])
        adj[RIGHT]=init_pos(y,new_x);
      else
        adj[RIGHT]=NULL;	
	
      //DOWN
      new_y=(y+m->height+1)%m->height;
      if(m->matrix[new_y][x])
	adj[DOWN]=init_pos(new_y,x);
      else
        adj[DOWN]=NULL;	
	
      //LEFT
      new_x=(x+m->width-1)%m->width;
      if(m->matrix[y][new_x])
        adj[LEFT]=init_pos(y,new_x);
      else
        adj[LEFT]=NULL;	
    }
  }
  return adj;
}

void free_adj(struct pos **adj)//Dealloca la struttura adj in input
{
  free(adj[UP]);
  free(adj[RIGHT]);
  free(adj[DOWN]);
  free(adj[LEFT]);
  free(adj);
}

struct maze *read_maze(char *path)//Legge un labirinto da file
{
  FILE *file;
  struct maze *res=NULL;
  struct parsed_maze *p_ma; 
  int i;
  
  if(file=fopen(path,"rb"))
  {
    if(p_ma=maze_parse_matrix(file))//Eseguo il parsing sul file ed ottengo una struttura intermedia di tipo parsed_maze
    {
      res=init_maze(p_ma->height, p_ma->width);
      
      for(i=0;i<p_ma->length;i+=2)
        add_vertex(res,p_ma->array[i],p_ma->array[i+1]);
      
      free(p_ma->array);
      free(p_ma);
    }
    fclose(file);
  }
  else
    mvprintw(LINES/2,(COLS-19)/2,"Maze file not found");
  
  return res;
}

/*----------------------------------*/
//Algoritmo per la generazione di labirinti random e ausiliari

struct maze *random_maze(int height, int width)//Genera un labirinto random di altezza e larghezza fissata
{
  struct maze *m=init_maze(height,width);
  
  color **col=paint_white(m->height,m->width);//Inizializzo la matrice colore
  random_maze_visit(m,col,rand()%height,rand()%width);//Eseguo una dfs visit partendo da un nodo random
  free_color(col,m->height);//Dealloco la matrice colore
  return m;  
}

void random_maze_visit(struct maze *m, color **col, int y, int x)//Visita ricorsiva ausiliaria a random_maze
{
  struct pos **adj;
  int i;
  
  col[y][x]=GRAY;
  add_vertex(m,y,x);//Aggiungo il vertice
  adj=get_wall_adj(m,y,x);//Ottengo le adiacenze di una locazione posizionate in maniera random in un array
  
  for(i=0;i<4;i++)
  {
    if(i==0)//L'adiacenza al primo posto viene posta nera e quindi rimarrà sempre muro
      col[adj[i]->y][adj[i]->x]=BLACK;
    else if(col[adj[i]->y][adj[i]->x]==WHITE)//Se bianchi, espando gli altri 3 vertici con la visita in profondità
      random_maze_visit(m,col,adj[i]->y,adj[i]->x);
  }
  col[y][x]=BLACK;
}

struct pos** get_wall_adj(struct maze *m, int y, int x)//Ottiene le adiacenze in un labirinto ignorando la presenza di corridoi o muri
{
  int new_y,new_x;
  struct pos **adj=(struct pos**) calloc(4,sizeof(struct pos*));
  
  //UP
  new_y=(y+m->height-1)%m->height;  
  adj[rand_index(adj)]=init_pos(new_y,x);

  //RIGHT
  new_x=(x+m->width+1)%m->width;
  adj[rand_index(adj)]=init_pos(y,new_x);

  //DOWN
  new_y=(y+m->height+1)%m->height;
  adj[rand_index(adj)]=init_pos(new_y,x);

  //LEFT
  new_x=(x+m->width-1)%m->width;
  adj[rand_index(adj)]=init_pos(y,new_x);

  return adj;    
}

int rand_index(struct pos **adj)//Restituisce un indice random da 0 a 3 che non è ancora stato riempito nell'array adj
{
  int i=rand()%4;
  while(adj[i])
    i=(i+1)%4;
  
  return i;
}

/*----------------------------------*/
//Algoritmi di ricerca e ausiliari

color **paint_white(int height, int width)//Restituisce una matrice allocata di dimensione height*width con il colore di ogni locazione inizializzato a bianco
{
  //In posizione (y,x) ci sarà il colore del vertice in posizione (y,x)
  color **res=(color**) malloc(height*(sizeof(color*)));
  int i,j;
 
  for(i=0;i<height;i++)
  {
    res[i]=(color*) malloc(width*(sizeof(color)));
    for(j=0;j<width;j++)
      res[i][j]=WHITE;
  }
  return res;
}

void free_color(color **col, int height)//Dealloca la matrice colore
{
  int i;
 
  for(i=0;i<height;i++)
    free(col[i]);
  free(col);
}

struct pos ***init_pred(int height, int width)//Inizializza la matrice pred contenente puntatori a struct pos
{
  //In posizione (y,x) ci sarà il predecessore del vertice in posizione (x,y)
  
  struct pos ***res=(struct pos***) calloc(height,(sizeof(struct pos**)));
  int i;
  
  for(i=0;i<height;i++)
    res[i]=(struct pos**) calloc(width,(sizeof(struct pos*)));
  
  return res;
}

void free_pred(struct pos ***pred, int height, int width)//Dealloca pred e tutte le struct pos allocate 
{
  int i,j;
  if(pred)
  {
    for(i=0;i<height;i++)
    {
      for(j=0;j<width;j++)
      {
        if(pred[i][j])
          free(pred[i][j]);
      }
      free(pred[i]);
    }
    free(pred);
  }
}

struct pos *find_first_vertex(struct maze *m)//Ritorna le coordinate del primo corridoio del labirinto, NULL se il labirinto contiene solo muri
{
  int i,j;
    
  for(i=0;i<m->height;i++)
  {
    for(j=0;j<m->width;j++)
    {
      if(m->matrix[i][j])
        return init_pos(i,j);
    }
  }  
  return NULL;
}

int is_connected(struct maze *m)//Restituisce 1 se il labirinto è una componente connessa, 0 altrimenti
{
  int i,j;
  struct pos ***pred;
  //Trova un corridoio del labirinto
  struct pos* source;
  
  if(!(source=find_first_vertex(m)))
      return 0;
  
  pred=BFS(m,source->y,source->x);
  
  for(i=0;i<m->height;i++)
  {
    for(j=0;j<m->width;j++)
    {
      //Verifica se un corridoio diverso dalla sorgente della BFS abbia pred==NULL
      if(m->matrix[i][j] && !pred[i][j] && ((i!=source->y || j!=source->x)))
      {
        free(source);
        free_pred(pred,m->height,m->width);
        return 0;
      }
    }
  }
  free(source);
  free_pred(pred,m->height,m->width);
  return 1;
}

float h(struct maze *m, int start_y, int start_x, int end_y, int end_x)//Funzione di stima euristica per un labirinto toroidale
{  
  float dy,dx;
  //La funzione euristica è la distanza di Manhattan modificata per funzionare
  //in un ambiente toroidale
  
  dy=fminf(abs(start_y-end_y),m->height - abs(end_y-start_y));
  dx=fminf(abs(start_x-end_x),m->width  - abs(end_x-start_x));
  return dy + dx;
}

//Algoritmi di ricerca

struct pos ***BFS(struct maze *m, int y, int x)//Visita in ampiezza del labirinto che riempie una matrice di puntatori a struct pos con le posizioni dei nodi visitati
{ 
  return BFS_custom(m,y,x,get_adj);
}

struct pos ***BFS_custom(struct maze *m, int y, int x, FNADJ my_adj)//BFS con funzione di adiacenza custom
{
  struct list *queue=NULL;
  struct pos **adj;
  color **col;
  struct pos *curr_pos,***pred=NULL;
  int i;
  
  if((y<m->height) && (x<m->width))//Verifico l'esattezza delle coordinate
  {
    col=paint_white(m->height,m->width);//Inizializzo la matrice colore
    col[y][x]=GRAY;//Coloro la sorgente di grigio    
    pred=init_pred(m->height,m->width);//Inizializzo la matrice pred
    queue=insert_head(queue,init_pos(y,x));//Inserisco in coda una struttura contenente le coordinate della sorgente

    while(queue)
    {
      curr_pos=get_rear(queue);//Ottengo la struct pos dalla coda
      adj=my_adj(m,curr_pos->y,curr_pos->x);
      
      for(i=0;i<4;i++)
      {
	if(adj[i] && col[adj[i]->y][adj[i]->x]==WHITE)
	{
	  pred[adj[i]->y][adj[i]->x]=init_pos(curr_pos->y,curr_pos->x);
	  col[adj[i]->y][adj[i]->x]=GRAY;
	  queue=insert_head(queue,adj[i]);
	}
      }
	
      col[curr_pos->y][curr_pos->x]=BLACK;
      queue=delete_rear(queue);
      free(adj);//Dealloco l'array: i singoli campi sono nella coda e verranno deallocati uno ad uno
      free(curr_pos);
    }
    free_color(col,m->height);
  }
  return pred;   
}

struct list *nBFS(struct maze *m, int y, int x, int n)//Visita in ampiezza del labirinto fino ad una distanza n
{
  return nBFS_custom(m,y,x,n,get_adj);
}

struct list *nBFS_custom(struct maze *m, int y, int x, int n, FNADJ my_adj)//nBFS con funzione di adiacenza custom
{
  struct list *L=NULL,*queue=NULL;
  struct pos **adj;
  color **col;
  struct pos *curr_pos;
  int dist[m->height][m->width];
  int i;
  
  if((y<m->height) && (x<m->width))//Verifico l'esattezza delle coordinate
  {
    col=paint_white(m->height,m->width);//Inizializzo la matrice colore
    dist[y][x]=0;
    col[y][x]=GRAY;//Coloro la sorgente di grigio  
    queue=insert_head(queue,init_pos(y,x));//Inserisco in coda una struttura contenente le coordinate della sorgente

    while(queue)
    {
      curr_pos=get_rear(queue);//Ottengo la struct pos dalla coda
      if(dist[curr_pos->y][curr_pos->x]>n)
      {
        free_color(col,m->height);
        while(queue)
        {
          free(get_head(queue));
          queue=delete_head(queue);
        }
        return L;
      }
      adj=my_adj(m,curr_pos->y,curr_pos->x);
      
      for(i=0;i<4;i++)
      {
        dist[adj[i]->y][adj[i]->x]=dist[curr_pos->y][curr_pos->x]+1;
	if(adj[i] && col[adj[i]->y][adj[i]->x]==WHITE)
	{
          L=insert_head(L,init_pos(adj[i]->y,adj[i]->x));
	  col[adj[i]->y][adj[i]->x]=GRAY;
	  queue=insert_head(queue,adj[i]);
	}
      }
	
      col[curr_pos->y][curr_pos->x]=BLACK;
      queue=delete_rear(queue);
      free(adj);//Dealloco l'array: i singoli campi sono nella coda e verranno deallocati uno ad uno
      free(curr_pos);
    }
    free_color(col,m->height);
  }
  return L;      
}

struct list *get_path_list(struct pos*** pred, int start_y, int start_x, int end_y, int end_x)//Crea una lista contenente i nodi di un percorso minimo
{
  struct list *L=NULL;

  if((start_y==end_y) && (start_x==end_x))
    L=insert_head(L,init_pos(start_y,start_x));
  else
  {
    L=get_path_list(pred,start_y,start_x,pred[end_y][end_x]->y,pred[end_y][end_x]->x); 
    L=insert_head(L,init_pos(pred[end_y][end_x]->y,pred[end_y][end_x]->x));
  }
  return L;
}

struct list *reach_path(struct maze *m, int y, int x)//Ritorna i nodi del percorso minimo che, se scavati, congiungerebbero il nodo ad una componente connessa già presente
{
  color **col=paint_white(m->height,m->width);//Inizializzo la matrice colore
  struct list *L=NULL,*queue=NULL;
  struct pos **adj;
  struct pos *curr_pos,***pred=NULL;
  int i,connected=0;
  
  col[y][x]=GRAY;//Coloro la sorgente di grigio  
  queue=insert_head(queue,init_pos(y,x));//Inserisco in coda una struttura contenente le coordinate della sorgente
  pred=init_pred(m->height,m->width);//Inizializzo la matrice pred

  while(queue && !connected)//Il ciclo continua finchè non si è raggiunta la componente connessa
  {
    curr_pos=get_rear(queue);//Ottengo la struct pos dalla coda
    if(m->matrix[curr_pos->y][curr_pos->x])//Il ciclo si ferma non appena il nodo estratto è una casella
      connected=1;
    else
    {
      adj=get_wall_adj(m,curr_pos->y,curr_pos->x);
      for(i=0;i<4;i++)
      {
        if(col[adj[i]->y][adj[i]->x]==WHITE)
        {
          pred[adj[i]->y][adj[i]->x]=init_pos(curr_pos->y,curr_pos->x);
          col[adj[i]->y][adj[i]->x]=GRAY;
	  queue=insert_head(queue,adj[i]);
        }
      }
      col[curr_pos->y][curr_pos->x]=BLACK;
      queue=delete_rear(queue);
      free(adj);//Dealloco l'array: i singoli campi sono nella coda e verranno deallocati uno ad uno
      free(curr_pos);
    }
  }
  
  free_color(col,m->height);//Deallocazione della matrice colore
   
  if(connected)
    L=get_path_list(pred,y,x,curr_pos->y,curr_pos->x); 
  
  while(queue)//Deallocazione della coda non esaminata
  {
    free(get_head(queue));
    queue=delete_head(queue);
  }
  
  return L;
}

struct pos ***dijkstra(struct maze *m, int y, int x)//Ricerca del percorso minimo che riempie una matrice di puntatori a struct pos con le posizioni dei nodi visitati
{
  return dijkstra_custom(m,y,x,get_adj);
}

struct pos ***dijkstra_custom(struct maze *m, int y, int x, FNADJ my_adj)//dijkstra con funzione di adiacenza custom
{
  struct pr_queue *queue=NULL;
  struct pos *curr_pos,**adj,***pred=NULL;
  float weight,new_est,priority;
  int i,j;
  
  if((y<m->height) && (x<m->width))//Verifico l'esattezza delle coordinate
  {
    queue=init_pr_queue(m->height,m->width);
    pred=init_pred(m->height,m->width);//Inizializzo la matrice pred
    
    //Inserisco nella coda la sorgente con prioritá 0
    insert_key(queue,init_pos(y,x),0);
    
  //Inserisco nella coda a priorità tutti i vertici del labirinto con distanza infinita 
   for(i=0;i<m->height;i++)
      for(j=0;j<m->width;j++)
	if((m->matrix[i][j]) && (y!=i || x!=j)) 
	    insert_key(queue,init_pos(i,j),INFINITY);
    
    while(queue->root)
    {
      curr_pos=get_min(queue);//Leggo il vertice con prioritá minore
      priority=get_priority(queue,curr_pos);//Ottengo la prioritá del vertice con prioritá minore
      extract_min(queue);//Lo estraggo dalla coda a prioritá     
      
      if(priority!=(INFINITY))//Verifico che il nodo sia raggiungibile dalla sorgente
      {
        adj=my_adj(m,curr_pos->y,curr_pos->x);
	
	for(i=0;i<4;i++)
        {
	  if(adj[i])//Verifico che l'adiacenza esista
	  {
	    //Ottengo il peso dell'arco a seconda della direzione
	    if(i==UP)
	      weight=m->matrix[curr_pos->y][curr_pos->x]->u_weight;
	    else if(i==RIGHT)
	      weight=m->matrix[curr_pos->y][curr_pos->x]->r_weight;
            else if(i==DOWN)
	      weight=m->matrix[curr_pos->y][curr_pos->x]->d_weight;
	    else if(i==LEFT)
	      weight=m->matrix[curr_pos->y][curr_pos->x]->l_weight;
	  
	    new_est=priority+weight;//Calcolo la nuova stima
            
	    if((get_priority(queue,adj[i])!=-1) && new_est<get_priority(queue,adj[i]))
	    {
	      //Se la nuova stima é migliore, diminuisco la prioritá nella coda e modifico il predecessore
              decrease_key(queue,adj[i],new_est);
	      free(pred[adj[i]->y][adj[i]->x]);
	      pred[adj[i]->y][adj[i]->x]=init_pos(curr_pos->y,curr_pos->x);
	    }
          }  	
        }      
      }
    }
  }
  return pred;   
}

struct pos ***a_star(struct maze *m, int start_y, int start_x, int end_y, int end_x)//Ricerca del percorso minimo mediante l'algoritmo A*
{
  return a_star_custom(m,start_y,start_x,end_y,end_x,get_adj);
}

struct pos ***a_star_custom(struct maze *m, int start_y, int start_x, int end_y, int end_x, FNADJ my_adj)//a_star con funzione di adiacenza custom
{
  struct pr_queue *open=NULL;
  struct pos *curr_pos,**adj,***pred=NULL;
  float new_g,weight,cost, g[m->height][m->width];
  int i,j, closed[m->height][m->width];
  
  //Open set è implementato come coda a priorità
  //Closed set è implementato come array:
  //alla posizione (x,y) 1 se il vertice é in closed set, 0 altrimenti
  
  //Inizializzo closed vuoto
  for(i=0;i<m->height;i++)
    for(j=0;j<m->width;j++)
      closed[i][j]=0;
  
  //Inizializzo closed vuoto
  for(i=0;i<m->height;i++)
    for(j=0;j<m->width;j++)
      g[i][j]=INFINITY;
  
  g[start_y][start_x]=0;
  
  if((start_y<m->height) && (end_y<m->height) && (start_x<m->width) && (end_x<m->width))
  {
    pred=init_pred(m->height,m->width);//Inizializzo la matrice pred
    open=init_pr_queue(m->height,m->width);//Inizializzo open set con la sola radice
    insert_key(open,init_pos(start_y,start_x),h(m,start_y,start_x,end_y,end_x));
     
    while(open->root)
    {
      curr_pos=get_min(open);
      extract_min(open);
      closed[curr_pos->y][curr_pos->x]=1;//Inserisco il nodo corrente a closed set
      
      if(curr_pos->y==end_y && curr_pos->x==end_x)
	return pred;
      
      adj=my_adj(m,curr_pos->y,curr_pos->x);
       
      for(i=0;i<4;i++)
      {
	if(adj[i] && closed[adj[i]->y][adj[i]->x]==0)//Verifico che l'adiacenza esista e che non sia in closed
        {
	  //Ottengo il peso dell'arco a seconda della direzione
	  if(i==UP)
	    weight=m->matrix[curr_pos->y][curr_pos->x]->u_weight;
	  else if(i==RIGHT)
	    weight=m->matrix[curr_pos->y][curr_pos->x]->r_weight;
          else if(i==DOWN)
	    weight=m->matrix[curr_pos->y][curr_pos->x]->d_weight;
	  else if(i==LEFT)
	    weight=m->matrix[curr_pos->y][curr_pos->x]->l_weight;

	  new_g=g[curr_pos->y][curr_pos->x]+weight;//Nuova distanza per l'adiacenza scoperta

	  if(!find_node(open,adj[i]))//L'adiacenza non é in open
	  {
	    free(pred[adj[i]->y][adj[i]->x]); 
	    pred[adj[i]->y][adj[i]->x]=init_pos(curr_pos->y,curr_pos->x);
	    g[adj[i]->y][adj[i]->x]=new_g;
	    insert_key(open,adj[i],new_g+h(m,adj[i]->y,adj[i]->x,end_y,end_x));
	  }
	  else if(new_g < g[adj[i]->y][adj[i]->x])//L'adiacenza é in open e new_g é minore
	  {
	    free(pred[adj[i]->y][adj[i]->x]); 
	    pred[adj[i]->y][adj[i]->x]=init_pos(curr_pos->y,curr_pos->x);
	    g[adj[i]->y][adj[i]->x]=new_g;
	    decrease_key(open,adj[i],new_g+h(m,adj[i]->y,adj[i]->x,end_y,end_x));
	  }
	}
      }  	   	 
    }
    free_pred(pred,m->height,m->width);
  }
  return NULL;
}
