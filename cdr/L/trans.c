#include <stdio.h>
#include "onlyl.h"

#define USAGE "使い方:trans {ファイル名} {x成分} {y成分}\n"

void

main( int argc, char *argv[])
{
 int size;
 int x;
 int y;

 if( argc != 4){
   fputs(USAGE,stderr);
 }else{

 size = init_light( argv[1]);                     /*読み出し*/
 x = atoi(argv[2]);
 y = atoi(argv[3]);
 
 if( x == 0 && y == 0 ){
   fputs(USAGE,stderr);
 }
 trans_light(x,y,size); /*平行移動*/
 load_light( argv[1],size);                       /*書き込み*/
 }
}
