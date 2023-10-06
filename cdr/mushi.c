#include <stdio.h>
#include <curses.h>

#define SIZE 60
#define YMAX 23
#define YMIN 2
#define XMAX 75
#define XMIN 2
#define ESC '\x1b'
#define gotoxy(x,y)  printf("\x1b[%d;%dH",(y),(x))

struct{
   int x;
   int y;
   char c;
} worm[ SIZE];

void

init_worm(void)
{
 int i;

 for(i=0; i < SIZE-1; i++){
   worm[i].x = XMIN;
   worm[i].y = YMIN;
   worm[i].c = 'A'+i;
 }
 worm[SIZE-1].c = ' ';
}

void

move_worm( int x, int y)
{
 int i;

 for(i = SIZE-1; i >= 1; i--){
      worm[i].x = worm[i-1].x;
      worm[i].y = worm[i-1].y;
 }
 worm[0].x = x;
 worm[0].y = y;
}

void

display_worm(void)
{
 int i;

 for( i=SIZE-1; i>=0; i--){
    gotoxy( worm[i].x, worm[i].y);
    putchar( worm[i].c);
 }
 fflush(stdout);
}
/*
void

drawframe( int x, int y)
{
 
}
*/
void

main()
{

 int x, y, c;

 initscr();
 raw();
 noecho();

 x = XMIN;
 y = YMIN;

 /* drawframe(XMAX, YMAX); */
 init_worm();

 while( (c=getch() ) != ESC){
   switch( c ){
   case 'i': y= (y<=YMIN)?YMAX:y-1;break;
   case 'k': x= (x>=XMAX)?XMIN:x+1;break;
   case 'j': x= (x<=XMIN)?XMAX:x-1;break;
   case 'm': y= (y>=YMAX)?YMIN:y+1;break;
   default : break;
   }
 move_worm(x,y);
 display_worm();
 }
 endwin();
}
