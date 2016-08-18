//Lorenzo De Simone N86/1008

typedef void (*FNLOGO) (int,int);//Funzioni che stampano il logo desiderato

char **init_choices(int n_choices,...);//Crea un array di stringhe per il menu con le stringhe in input
void free_choices(char **choices, int n_choices);//Dealloca l'array di char contenente le scelte possibili del menu

//Stampano un menu con con la possibilità di selezionare l'opzione desiderata
int print_menu(int height, int width, char **choices, int n_choices, int color_pair, FNLOGO title);
void print_choices(WINDOW *menu_win, char **choices, int highlight, int n_choices);