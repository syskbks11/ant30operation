#include <stdio.h>
#include "onlyl.h"

#define USAGE "使い方:rot {ファイル名} {回転数}\n"

void

main( int argc, char *argv[])
{
 int size;
 int d;

 if( argc != 3){
   fputs(USAGE,stderr);
 }else{

 size = init_light( argv[1]);                     /*読み出し*/
 d = atoi(argv[2]);
 rotate_light( d,size);                           /*回転*/
 load_light( argv[1],size);                       /*書き込み*/
 }
}
