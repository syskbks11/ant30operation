/* げんじょう版 光シミュレーションプログラム *
 *             in C for UNIX                 *
 *            1999.9.30 16:12                */

#include <stdio.h>
#include <curses.h>
#include "light.h"

#define USAGE "使い方:light {ファイル１} {ファイル２}\n"
#define LFILE "ファイル１:l_*,2rt,3rt,4rt,5rt,6rt\n"
#define SFILE "ファイル２:s_*\n"
#define ESC '\x1b'
#define VecX 1  /*場の計算に使うベクトル*/ 
#define VecY 4  /*Xは右,Yは左上*/

  /*光に懸る場を計算する*/
int

check_field( int i,int size,int ssize)
{
 int field=0;
 int dx,dy;
 int n;

 for(n = 0; n<size; n++){
   dx = light[n].x-light[i].x;
   dy = light[n].y-light[i].y;
   switch( dx*dy ){
     case  1: field -= VecY * dy;break;
     case -1: field -= (VecX+VecY)*dy;break;
     case  0:
       switch( dx ){
         case  2: field += VecX;break;
         case -2: field -= VecX;break;
         case  0:
           if( dy == 0 && i != n){
	      printf("\x1b[%d;%dH衝突",1,39);
           }
         default: break;
       }
     default:break;
   }
 }
 for(n = 0; n<ssize; n++){
   dx = solid[n].x-light[i].x;
   dy = solid[n].y-light[i].y;
   switch( dx*dy ){
     case  1:      /*左上,右下*/
        field -= VecY * dy;break;
     case -1:      /*左下,右上*/
        field -= (VecY+VecX)*dy;break;
     case  0:      
       switch( dx ){
         case  2:  /*右*/
             field += VecX;break;
         case -2:  /*左*/
             field -= VecX;break;
         case  0:
           if( dy == 0 ){
               printf("\x1b[%d;%dH通過",1,40);
           }
         default: break;
       }
     default:break;
   }
 }
 return field;
}

/*場から方向を決定する関数*/
void

cal_dir(int size,int ssize)
{
 int i;
 int f;
 int field;

 for( i=0 ; i<size ; i++){
    field = check_field( i ,size,ssize);
    if( field != 0){
     if(field<=0){
        field *=-1;
     }
     switch( field ){
     /*fの値で作用が決まる*/
     case      VecX: case 2*VecX:
          f=9;  break;
     case VecX+VecY: case 2*(VecX+VecY):
          f=11; break;
     case      VecY: case 2*VecY:
          f=7;  break;
     case 2*VecX+VecY:
          f=10; break;
     case VecX+VecY*2:
          f=6;  break;
     case   VecY-VecX:
          f=8;  break;
     default:   break;
     }
     /*場の作用を表す式*/
     light[i].dir=(f-light[i].dir)%6;
    }
 }
}

void

main( int argc, char *argv[])
{

 char c;
 int size;     /*光の個数*/
 int ssize;  /*固体の個数*/

 if( argc != 2){
 /*誤った使い方の時*/
    fputs(USAGE,stderr);
    fputs(LFILE,stderr);
    fputs(SFILE,stderr);
 }
 size  = init_light(argv[1]);
 ssize = init_solid(argv[2]);

 /*UNIX用の画面用意*/
 initscr();
 raw();
 noecho();

 while( (c=getch() ) != ESC){
   /*終了は^[*/
 move_light(size);
 cal_dir(size,ssize);

 printf("\x1b[2J");
 display_solid(ssize);
 display_light(size);
 }
 /*UNIX用の画面終了*/
 endwin();
}
