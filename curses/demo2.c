#include <curses.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>

#define DELAY 100000

int main(int argc, char *argv[])
{
  char *msg;
  char *cur;
  int count = 0;

  if(argc != 2){
    fprintf(stderr, "Usage: %s message\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  msg = argv[1];

  if(initscr() == NULL){
    fprintf(stderr, "initscr failure\n");
    exit(EXIT_FAILURE);
  }

  /* 念のためまず0,0にカーソルを移動する */
  move(0,0);

  /* 文字列の最後から順に1文字ずつ挿入していく. */
  cur = msg + strlen(msg);
  while(cur != msg){
    insch(*(--cur));
    count++;
    refresh();
    usleep(DELAY);
  }

  /* さらに行の幅分スペースを挿入 */
  while(count < COLS){
    insch(' ');
    count++;
    refresh();
    usleep(DELAY);
  }

  sleep(1);

  /* 0,0にてdeleteしていく */
  while(count > 0){
    mvdelch(0, 0);
    count--;
    refresh();
    usleep(DELAY);
  }

  endwin();
  exit(EXIT_SUCCESS);
}

