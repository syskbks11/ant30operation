#include <stdio.h>
#include "onlyl.h"

#define USAGE "�Ȥ���:rot {�ե�����̾} {��ž��}\n"

void

main( int argc, char *argv[])
{
 int size;
 int d;

 if( argc != 3){
   fputs(USAGE,stderr);
 }else{

 size = init_light( argv[1]);                     /*�ɤ߽Ф�*/
 d = atoi(argv[2]);
 rotate_light( d,size);                           /*��ž*/
 load_light( argv[1],size);                       /*�񤭹���*/
 }
}
