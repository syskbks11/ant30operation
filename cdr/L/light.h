/*  げんじょう版 光シミュレーション用  *
 *             header file             *
 *              for UNIX               *
 *           1999.9.30 16:07           */

#include <stdio.h>

#define gotoxy(x,y)  printf("\x1b[%d;%dH",(y),(x))
#define MAXSIZE 30

/*光を表す構造体*/
struct{
   int x;
   int y;    /*座標*/
   int dir;  /*方向*/
} light[ MAXSIZE];

/*固体を表す構造体*/
struct{
   int x;
   int y;    /*座標*/
   int mass; /*質量*/
} solid[ MAXSIZE];

/*光を次の時刻に動かす関数*/
void

move_light(int size)
{
 int i;

 for(i = 0; i<size; i++){
   switch( light[i].dir){
                 /*方向で場合分け*/
   case 0:       /*右*/
      light[i].x +=2;break;
   case 1:       /*右上*/
      light[i].x++;
      light[i].y--;break;
   case 2:       /*左上*/
      light[i].x--;
      light[i].y--;break;
   case 3:       /*左*/
      light[i].x -=2;break;
   case 4:       /*左下*/
      light[i].x--;
      light[i].y++;break;
   case 5:       /*右下*/
      light[i].x++;
      light[i].y++;break;
   default:break;
   }
 }
}

/*光を表示する関数*/
void

display_light(int size)
{
 int i;

 for( i=size-1; i>=0; i--){
    gotoxy( light[i].x, light[i].y);
    printf("%d",light[i].dir);
    /*方向を表す数字で表示*/
 }
 fflush(stdout);
}

/*固体を表示する関数*/
void

display_solid(int size)
{
 int i;

 for( i=size-1; i>=0; i--){
    gotoxy( solid[i].x, solid[i].y);
    printf("\x1b[7m%d\x1b[0m",solid[i].mass);
    /*質量を表す数字で表示*/
 }
 fflush(stdout);
}

/*光をファイルから読み取る関数*/
int

init_light( char *fname)
{
 FILE *fp;
 int i,x,y,d;

 if( (fp=fopen(fname,"r")) == NULL){
     fprintf(stderr,"%sがオープンできません。\n",fname);
     exit();
 }else{
   i = 0;
   while( fscanf( fp, "%d %d %d *[^\n]",&x,&y,&d) != EOF){
       light[i].x=x;
       light[i].y=y;
       light[i].dir=d;
       i++;
       if(i >= MAXSIZE){
           fclose(fp);
           return i;
       }
   }
   fclose(fp);
   return i;   /*光の個数を返す*/
 }
}

/*固体をファイルから読み取る関数*/
int

init_solid( char *fname)
{
 FILE *fp;
 int i,x,y,m;

 if( (fp=fopen(fname,"r")) == NULL){
     fprintf(stderr,"%sがオープンできません。\n",fname);
     exit();
 }else{
   i = 0;
   while( fscanf( fp, "%d %d %d *[^\n]",&x,&y,&m) != EOF){
       solid[i].x=x;
       solid[i].y=y;
       solid[i].mass=m;
       i++;
       if(i >= MAXSIZE){
           fclose(fp);
           return i;
       }
   }
   fclose(fp);
   return i;   /*固体の個数を返す*/
 }
}

