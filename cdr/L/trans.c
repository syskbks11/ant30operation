#include <stdio.h>
#include "onlyl.h"

#define USAGE "�Ȥ���:trans {�ե�����̾} {x��ʬ} {y��ʬ}\n"

void

main( int argc, char *argv[])
{
 int size;
 int x;
 int y;

 if( argc != 4){
   fputs(USAGE,stderr);
 }else{

 size = init_light( argv[1]);                     /*�ɤ߽Ф�*/
 x = atoi(argv[2]);
 y = atoi(argv[3]);
 
 if( x == 0 && y == 0 ){
   fputs(USAGE,stderr);
 }
 trans_light(x,y,size); /*ʿ�԰�ư*/
 load_light( argv[1],size);                       /*�񤭹���*/
 }
}
