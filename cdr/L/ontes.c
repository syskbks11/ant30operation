#include <stdio.h>
#include <curses.h>
#include "onlyl.h"

#define USAGE "使い方:light {ファイル１}\n"
#define LFILE "ファイル:l_*,2rt,3rt,4rt,5rt,6rt\n"


struct{
   int x;
   int y;
} Vec[5];




void

move_lightV(int size)                             /*光を次の時刻に動かす関数*/
{
 int i;
 Vec[0].x=2;
 Vec[0].y=0;                    /*右*/
 Vec[1].x=1;
 Vec[1].y=-1;                   /*右上*/
 Vec[2].x=-1;
 Vec[2].y=-1;                   /*左上*/
 Vec[3].x=-Vec[0].x;
 Vec[3].y=-Vec[0].y;            /*左*/
 Vec[4].x=-Vec[1].x;
 Vec[4].y=-Vec[1].y;            /*左下*/
 Vec[5].x=-Vec[2].x;
 Vec[5].y=-Vec[2].y;            /*右下*/

 for(i = 0; i<size; i++){
   light[i].x += Vec[light[i].dir].x;
   light[i].y += Vec[light[i].dir].y;
 }
}



void

main( int argc, char *argv[])
{

 char c;
 int size;                                     /*光の個数*/

 if( argc != 1){                               /*誤った使い方の時*/
    fputs(USAGE,stderr);
    fputs(LFILE,stderr);
 }
 size  = init_light(argv[1]);

 initscr();                                    /*UNIX用の画面用意*/
 raw();
 noecho();

 while( (c=getch() ) != ESC){                  /*終了は^[か、ESC*/
 move_lightV(size);
 cal_dir(size);

 printf("\x1b[2J");
 display_light(size);
 }
 endwin();                                     /*UNIX用の画面終了*/
}
