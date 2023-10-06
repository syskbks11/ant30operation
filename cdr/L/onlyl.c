#include <stdio.h>
#include <curses.h>
#include "onlyl.h"

#define USAGE "使い方:light {ファイル名}\n"
#define FILE "ファイル:2rt,3rt,4rt,5rt,6rt\n"

void

main( int argc, char *argv[])
{

 char c;
 int size;                                     /*光の個数*/

 if( argc != 2){                               /*誤った使い方の時*/
    fputs(USAGE,stderr);
    fputs(FILE,stderr);
 }
 size  = init_light(argv[1]);

 initscr();                                    /*UNIX用の画面用意*/
 raw();
 noecho();

 while( (c=getch() ) != ESC){                  /*終了は^[か、ESC*/
 move_light(size);
 cal_dir(size);

 printf("\x1b[2J");
 display_light(size);
 }
 endwin();                                     /*UNIX用の画面終了*/
}
