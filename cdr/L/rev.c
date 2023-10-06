#include <stdio.h>
#include "onlyl.h"

#define USAGE "使い方:rev {ファイル名} \n"


void

main( int argc, char *argv[])
{
 int size;

 if( argc != 2){
   fputs(USAGE,stderr);
 }else{

 size = init_light( argv[1]);                     /*読み出し*/
 rotate_light( 3,size);                           /*反転*/
 cal_dir(size);                                   /*場を懸ける*/
 load_light( argv[1],size);                       /*書き込み*/
 }
}
