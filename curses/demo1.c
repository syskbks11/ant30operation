#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <time.h>

#define STAR_COUNT_MAX 500
#define DELAY 10000

int main(int argc, char *argv[])
{
  int i,j;
  int istep, jstep;
  int star_count;
  int loop_count;

  if(argc != 2){
    fprintf(stderr,"Usage %s loopcount\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  loop_count = atoi(argv[1]);

  if(initscr() == NULL){
    fprintf(stderr, "initscr failure\n");
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));
  while(loop_count > 0){
    star_count = 0;
    i = rand() % LINES;
    j = rand() % COLS;
    istep = jstep = 1;
    if(i == 0){
      istep = -1;
    }
    if(j == 0){
      jstep = -1;
    }
    do{
      /* i,jの位置に*を出力 */
      mvaddch(i, j, '*');
      star_count++;
      refresh();
      usleep(DELAY);
      if(i == LINES - 1 || i == 0){
	istep = -istep;
      }
      if(j == COLS - 1 || j == 0){
	jstep = -jstep;
      }
      i += istep;
      j += jstep;
    }while(star_count < STAR_COUNT_MAX);
    loop_count--;
    /* 画面をクリアする */
    clear();
  }

  endwin();
  exit(EXIT_SUCCESS);
}


