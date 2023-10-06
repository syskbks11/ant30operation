#include <stdio.h>
#include <curses.h>
#include "onlyl.h"

#define USAGE "�Ȥ���:light {�ե�����̾}\n"
#define FILE "�ե�����:2rt,3rt,4rt,5rt,6rt\n"

void

main( int argc, char *argv[])
{

 char c;
 int size;                                     /*���θĿ�*/

 if( argc != 2){                               /*��ä��Ȥ����λ�*/
    fputs(USAGE,stderr);
    fputs(FILE,stderr);
 }
 size  = init_light(argv[1]);

 initscr();                                    /*UNIX�Ѥβ����Ѱ�*/
 raw();
 noecho();

 while( (c=getch() ) != ESC){                  /*��λ��^[����ESC*/
 move_light(size);
 cal_dir(size);

 printf("\x1b[2J");
 display_light(size);
 }
 endwin();                                     /*UNIX�Ѥβ��̽�λ*/
}
