#include <stdio.h>
#include "onlyl.h"

#define USAGE "�Ȥ���:rev {�ե�����̾} \n"


void

main( int argc, char *argv[])
{
 int size;

 if( argc != 2){
   fputs(USAGE,stderr);
 }else{

 size = init_light( argv[1]);                     /*�ɤ߽Ф�*/
 rotate_light( 3,size);                           /*ȿž*/
 cal_dir(size);                                   /*��������*/
 load_light( argv[1],size);                       /*�񤭹���*/
 }
}
