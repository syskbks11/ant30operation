#include <stdio.h>
#include "onlyl.h"

#define USAGE "��Ȓ�����:interval {��Ւ��������̾} \n"

void

main( int argc, char *argv[])
{
 int size,i;
 int t=0;

 if( argc != 2){
   fputs(USAGE,stderr);
 }else{

   size = init_light( argv[1]);                     /*��ɒ�ߒ�В��*/
   for(i=0;i<size;i++){
      fps[i].x=light[i].x;
      fps[i].y=light[i].y;
      fps[i].dir=light[i].dir;
   }
   do{
      move_light( size);
      cal_dir(size);                                /*��쒤��������*/
      t++;
      if(t>2000){
         goto overt;
      }
      printf("%d:",t);
      printf("%d\n", comp_light( size));
   }while( comp_light( size) );
   printf("������������%d\n",t);
   overt :;
 }
}
