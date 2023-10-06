#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>

int main(void)
{
  char buf[BUFSIZ];

  if(initscr() == NULL){
    fprintf(stderr, "initscr failure\n");
    exit(EXIT_FAILURE);
  }

  memset(buf, '\0', BUFSIZ);

#ifdef CBREAK
  cbreak();
#else
  nocbreak();
#endif
  /* 入力が"exit"なら終了 */
  while(strcmp(buf, "exit") != 0){
    mvprintw(5, 5, "please input: ");
    getnstr(buf, BUFSIZ);
    move(5, 5);
    /* カーソル位置から行末までを削除 */
    clrtoeol();
    /* 入力文字列を表示 */
    mvprintw(6, 5, "input = %s", buf);
    refresh();
  }

  endwin();
  exit(EXIT_SUCCESS);
}


