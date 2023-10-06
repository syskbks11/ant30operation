/* ���󤸤礦�� �����ߥ�졼�����ץ���� *
 *             in C for UNIX                 *
 *            1999.9.30 16:12                */

#include <stdio.h>
#include <curses.h>
#include "light.h"

#define USAGE "�Ȥ���:light {�ե����룱} {�ե����룲}\n"
#define LFILE "�ե����룱:l_*,2rt,3rt,4rt,5rt,6rt\n"
#define SFILE "�ե����룲:s_*\n"
#define ESC '\x1b'
#define VecX 1  /*��η׻��˻Ȥ��٥��ȥ�*/ 
#define VecY 4  /*X�ϱ�,Y�Ϻ���*/

  /*���˷�����׻�����*/
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
	      printf("\x1b[%d;%dH����",1,39);
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
     case  1:      /*����,����*/
        field -= VecY * dy;break;
     case -1:      /*����,����*/
        field -= (VecY+VecX)*dy;break;
     case  0:      
       switch( dx ){
         case  2:  /*��*/
             field += VecX;break;
         case -2:  /*��*/
             field -= VecX;break;
         case  0:
           if( dy == 0 ){
               printf("\x1b[%d;%dH�̲�",1,40);
           }
         default: break;
       }
     default:break;
   }
 }
 return field;
}

/*�줫����������ꤹ��ؿ�*/
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
     /*f���ͤǺ��Ѥ���ޤ�*/
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
     /*��κ��Ѥ�ɽ����*/
     light[i].dir=(f-light[i].dir)%6;
    }
 }
}

void

main( int argc, char *argv[])
{

 char c;
 int size;     /*���θĿ�*/
 int ssize;  /*���ΤθĿ�*/

 if( argc != 2){
 /*��ä��Ȥ����λ�*/
    fputs(USAGE,stderr);
    fputs(LFILE,stderr);
    fputs(SFILE,stderr);
 }
 size  = init_light(argv[1]);
 ssize = init_solid(argv[2]);

 /*UNIX�Ѥβ����Ѱ�*/
 initscr();
 raw();
 noecho();

 while( (c=getch() ) != ESC){
   /*��λ��^[*/
 move_light(size);
 cal_dir(size,ssize);

 printf("\x1b[2J");
 display_solid(ssize);
 display_light(size);
 }
 /*UNIX�Ѥβ��̽�λ*/
 endwin();
}
