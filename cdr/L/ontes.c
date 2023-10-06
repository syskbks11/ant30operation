#include <stdio.h>
#include <curses.h>
#include "onlyl.h"

#define USAGE "�Ȥ���:light {�ե����룱}\n"
#define LFILE "�ե�����:l_*,2rt,3rt,4rt,5rt,6rt\n"


struct{
   int x;
   int y;
} Vec[5];




void

move_lightV(int size)                             /*���򼡤λ����ư�����ؿ�*/
{
 int i;
 Vec[0].x=2;
 Vec[0].y=0;                    /*��*/
 Vec[1].x=1;
 Vec[1].y=-1;                   /*����*/
 Vec[2].x=-1;
 Vec[2].y=-1;                   /*����*/
 Vec[3].x=-Vec[0].x;
 Vec[3].y=-Vec[0].y;            /*��*/
 Vec[4].x=-Vec[1].x;
 Vec[4].y=-Vec[1].y;            /*����*/
 Vec[5].x=-Vec[2].x;
 Vec[5].y=-Vec[2].y;            /*����*/

 for(i = 0; i<size; i++){
   light[i].x += Vec[light[i].dir].x;
   light[i].y += Vec[light[i].dir].y;
 }
}



void

main( int argc, char *argv[])
{

 char c;
 int size;                                     /*���θĿ�*/

 if( argc != 1){                               /*��ä��Ȥ����λ�*/
    fputs(USAGE,stderr);
    fputs(LFILE,stderr);
 }
 size  = init_light(argv[1]);

 initscr();                                    /*UNIX�Ѥβ����Ѱ�*/
 raw();
 noecho();

 while( (c=getch() ) != ESC){                  /*��λ��^[����ESC*/
 move_lightV(size);
 cal_dir(size);

 printf("\x1b[2J");
 display_light(size);
 }
 endwin();                                     /*UNIX�Ѥβ��̽�λ*/
}
