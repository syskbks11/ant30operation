#include <stdlib.h>
#include <stdio.h>
#include <curses.h>

int main()
{
  char ch = ' ';

  if(initscr() == NULL){
    fprintf(stderr, "initscr failure\n");
    exit(EXIT_FAILURE);
  }

#ifdef CBREAK
  cbreak();
#else
  nocbreak();
#endif

  mvprintw(6, 5, "input = ");

  while(ch != 'q'){
    mvprintw(5, 5, "please input: ");
    ch = getch();
    move(5, 5);
    clrtoeol();
    mvinsch(6, 15, ch);
    /*
    mvprintw(6, 5, "input %c", ch);
    */
    refresh();
  }

  endwin();
  exit(EXIT_SUCCESS);
}






