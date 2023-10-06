/*   �����ߥ�졼������� �ʸ��Τߡ�   *
 *      Editer �����  for UNIX        *
 *      1999.11.1    By �ʰ� ��        */

#include <stdio.h>
#include <curses.h>
#include "onlyl.h"

#define USAGE "�Ȥ���:make {�ե����룱}\n"
#define YMAX    2*20                /*�����ϰϤλ���*/
#define YMIN    1
#define XMAX    2*35
#define XMIN    1

int

func1( int x,int y, int c ,int i)   /*���������Ϥ��줿�Ȥ��ν���*/
{

  int p;
  printf("%d\x1b[1D",c);            /*����ɸ��*/
  p = check_light(x,y,i);           /*����̵ͭ��Ĵ�٤�*/
  if( p >=0 ){
         light[p].dir = c;
	 priminibuf("over","write");/*��񤭤���*/
  }else{  
         light[i].x = x;            /*�ɲä���*/
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

 if( argc != 2){                      /*�����Ϻ�������ե�����̾*/
   fputs(USAGE,stderr);
 }else{


 initscr();
 raw();
 noecho();

 x = XMIN;
 y = YMIN;

 while( ( c=getch() ) != ESC ){
     switch( c ){
       /*��������������*/
         case '0': i=func1(x,y,0,i);break;
         case '1': i=func1(x,y,1,i);break;
         case '2': i=func1(x,y,2,i);break;
         case '3': i=func1(x,y,3,i);break;
         case '4': i=func1(x,y,4,i);break;
         case '5': i=func1(x,y,5,i);break;
       /*��������*/
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
         case ' ':                 /*�ä��Ȥ�*/
	   p= check_light(x,y,i);
	   if(p >= 0){
	     del_light(p,i);
	     i--;
	     printf(" \x1b[1D");
	   }
	   break;
         case  Cs:                 /*�ե��������¸����Ȥ�*/
            load_light(argv[1],i);
	    priminibuf("Wrote ",argv[1]);
	    break;
         default : break;
     }
     prinminibuf("size:",i);       /*���Ϥ��줿�����ɽ��*/
     gotoxy(x,y);
     fflush(stdout);               /*���������ư��������*/
     if(i>=MAXSIZE-1){
       priminibuf("OVER","SIZE");  /*���ο���¿������Ȥ�*/
     }
   }
 }
 endwin();                         /*UNIX�Ѥβ��̽�λ*/
}
