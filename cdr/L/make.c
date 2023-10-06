/*   光シミュレーション用 （光のみ）   *
 *      Editer 試作品  for UNIX        *
 *      1999.11.1    By 永井 誠        */

#include <stdio.h>
#include <curses.h>
#include "onlyl.h"

#define USAGE "使い方:make {ファイル１}\n"
#define YMAX    2*20                /*画面範囲の指定*/
#define YMIN    1
#define XMAX    2*35
#define XMIN    1

int

func1( int x,int y, int c ,int i)   /*方向を入力されたときの処理*/
{

  int p;
  printf("%d\x1b[1D",c);            /*方向標示*/
  p = check_light(x,y,i);           /*光の有無を調べる*/
  if( p >=0 ){
         light[p].dir = c;
	 priminibuf("over","write");/*上書きする*/
  }else{  
         light[i].x = x;            /*追加する*/
         light[i].y = y;
         light[i].dir = c;
	 i++;
  }
  return i;
}

void

main( int argc, char *argv[])
{

 int x,y;
 int c,p;
 int i=0;

 if( argc != 2){                      /*引数は作成するファイル名*/
   fputs(USAGE,stderr);
 }else{


 initscr();
 raw();
 noecho();

 x = XMIN;
 y = YMIN;

 while( ( c=getch() ) != ESC ){
     switch( c ){
       /*光の方向の入力*/
         case '0': i=func1(x,y,0,i);break;
         case '1': i=func1(x,y,1,i);break;
         case '2': i=func1(x,y,2,i);break;
         case '3': i=func1(x,y,3,i);break;
         case '4': i=func1(x,y,4,i);break;
         case '5': i=func1(x,y,5,i);break;
       /*方向キー*/
         case 'y': y= (y<=YMIN)?YMAX:y-1;
	           x= (x<=XMIN)?XMAX:x-1;break;
         case 'u': y= (y<=YMIN)?YMAX:y-1;
	           x= (x>=XMAX)?XMIN:x+1;break;
         case 'j': x= (x>=XMAX-1)?XMIN+1-(y%2):x+2;break;
         case 'h': x= (x<=XMIN+1)?XMAX-(y%2):x-2;break;
         case 'm': y= (y>=YMAX)?YMIN:y+1;
	           x= (x>=XMAX)?XMIN:x+1;break;
         case 'n': y= (y>=YMAX)?YMIN:y+1;
	           x= (x<=XMIN)?XMAX:x-1;break;
         case ' ':                 /*消すとき*/
	   p= check_light(x,y,i);
	   if(p >= 0){
	     del_light(p,i);
	     i--;
	     printf(" \x1b[1D");
	   }
	   break;
         case  Cs:                 /*ファイルに保存するとき*/
            load_light(argv[1],i);
	    priminibuf("Wrote ",argv[1]);
	    break;
         default : break;
     }
     prinminibuf("size:",i);       /*入力された総数の表示*/
     gotoxy(x,y);
     fflush(stdout);               /*カーソルを動かすため*/
     if(i>=MAXSIZE-1){
       priminibuf("OVER","SIZE");  /*光の数が多すぎるとき*/
     }
   }
 }
 endwin();                         /*UNIX用の画面終了*/
}
