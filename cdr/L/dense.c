#include <stdio.h>
#include "onlyl.h"

#define USAGE "’»È’¤¤’Êý:dense {’¥Õ’¥¡’¥¤’¥ë’Ì¾} \n"
#define XLIM 18
#define YLIM 15


int dens[XLIM][YLIM];

int

add_dense( int size)
{

 int i,x,y;

 for(i=0;i<size;i++){
    x=light[i].x+light[i].y%2;
    x/=2;
    y=light[i].y;
    if( x >= XLIM || y >=YLIM ){
      return 1;
    }
    dens[x][y]++;
 }
 return 0;
}

void

display_dense( void )
{
 int i,j;

 printf("\x1b[2J");

 for( i=0;i<XLIM;i++){
    for( j=0;j<YLIM;j++){
         gotoxy( 2*(2*i-j%2)+3,2*j+1);
         printf("%2d",dens[i][j]);
    }
 }
}
   



void

main( int argc, char *argv[])
{
 int size,i,j;
 int t=0;

 for(i=0;i<XLIM;i++){
      for(j=0;j<YLIM;j++){
	dens[i][j]=0;
      }
 }
     
 if( argc != 2){
   fputs(USAGE,stderr);
 }else{

   size = init_light( argv[1]);                     /*’ÆÉ’¤ß’½Ð’¤·*/
   for(i=0;i<size;i++){
      fps[i].x=light[i].x;
      fps[i].y=light[i].y;
      fps[i].dir=light[i].dir;
   }
   do{
      move_light( size);
      cal_dir(size);                                /*’¾ì’¤ò’·ü’¤±’¤ë*/
      t++;
      if( add_dense( size) || t>2000){
         goto overt;
      }
   }while( comp_light( size) );
   display_dense();
   gotoxy(1,2*YLIM+1);
   printf("’¸÷’¼þ’´ü’¡§%d\n",t);
   overt :;
 }
}
