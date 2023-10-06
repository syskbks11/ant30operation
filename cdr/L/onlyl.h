/*     光シミュレーション用（光のみ）    *
 *         haeder file  for UNIX         *
 *        1999.10.31  By 永井 誠         */

/* 関数一覧
 * 
 * int  init_light( char *fname)
 * void move_light( int size)
 * void desplay_light( int size)
 * int  check_field( int i, int size)
 * void cal_dir( int size)
 * void rotate_light( int d, int size)
 * void load_light( char *fname, int size)
 * void trans_light( int x, int y, int size)
 * int  comp_light( int size)
 * int  check_light( int x, int y, int size)
 * void del_light( int i, int size)
 */

/* マクロ定義 */

#define gotoxy(x,y)      printf("\x1b[%d;%dH",(y),(x))
#define priminibuf(a,b)  gotoxy(1,YMAX+1);printf("%s %s",a,b)
#define prinminibuf(a,b) gotoxy(1,YMAX+1);printf("%s %d",a,b)

/* 定数 */

#define MAXSIZE 85                   /*光の最大数*/
#define VecX 1                       /*場の計算に使うベクトル*/ 
#define VecY 4                       /*Xは右,Yは左上*/
#define ESC     '\x1b'
#define Cs      '\x13'

/* 構造体の宣言 */

struct tri{
   int x;
   int y;                            /*座標*/
   int dir;                          /*方向*/
};

struct tri light[ MAXSIZE];          /*光を表す構造体*/
struct tri fps[ MAXSIZE];            /*初期位置の記録*/

/* 関数定義 */

int

init_light( char *fname)             /*光をファイルから読み取る関数*/
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
   return i;                          /*光の個数を返す*/
 }
}

void

move_light(int size)                  /*光を次の時刻に動かす関数*/
{
 int i;

 for(i = 0; i<size; i++){
   switch( light[i].dir){             /*方向で場合分け*/
   case 0:                            /*右*/
      light[i].x +=2;break;
   case 1:                            /*右上*/
      light[i].x++;
      light[i].y--;break;
   case 2:                            /*左上*/
      light[i].x--;
      light[i].y--;break;
   case 3:                            /*左*/
      light[i].x -=2;break;
   case 4:                            /*左下*/
      light[i].x--;
      light[i].y++;break;
   case 5:                            /*右下*/
      light[i].x++;
      light[i].y++;break;
   default:break;
   }
 }
}

void

display_light(int size)               /*光を表示する関数*/
{
 int i;

 for( i=size-1; i>=0; i--){
    gotoxy( light[i].x, light[i].y);
    printf("%d",light[i].dir);        /*方向を表す数字で表示*/
 }
 fflush(stdout);
}

int

check_field( int i,int size)          /*光に懸る場を計算する関数*/
{
 int field=0;
 int dx,dy;
 int n;

 for(n = 0; n<size; n++){
   dx = light[n].x-light[i].x;
   dy = light[n].y-light[i].y;
   switch( dx*dy ){
     case  1: field -= VecY * dy;break;
     case -1: field -= (VecX+VecY)*dy;break;
     case  0:
       switch( dx ){
         case  2: field += VecX;break;
         case -2: field -= VecX;break;
         case  0:
           if( dy == 0 && i != n){
	      printf("\x1b[%d;%dH衝突",1,39);
           }
         default: break;
       }
     default:break;
   }
 }
 return field;
}

void

cal_dir(int size)                      /*場から方向を決定する関数*/    
{
 int i;
 int f;
 int field;

 for( i=0 ; i<size ; i++){
    field = check_field( i ,size);
    if( field != 0){
     if(field<=0){
        field *=-1;
     }
     switch( field ){                  /*fの値で作用が決まる*/
     case      VecX: case 2*VecX:
          f=9;  break;
     case VecX+VecY: case 2*(VecX+VecY):
          f=11; break;
     case      VecY: case 2*VecY:
          f=7;  break;
     case 2*VecX+VecY:
          f=10; break;
     case VecX+VecY*2:
          f=6;  break;
     case   VecY-VecX:
          f=8;  break;
     default:   break;
     }
     light[i].dir=(f-light[i].dir)%6;   /*場の作用を表す式*/
    }
 }
}

void

rotate_light( int d, int size )         /*光の方向を一様に変える関数*/
{
 int i;

 for ( i = 0; i< size; i++){
    light[i].dir = (light[i].dir += d )%6;
 }
}

void

load_light( char *fname, int size)      /*ファイルに光を書き込む関数*/
{
  FILE *fp;
  int i;

 if( (fp=fopen(fname,"w")) == NULL){
     fprintf(stderr,"%sがオープンできません。\n",fname);
     exit();
 }else{
     for( i=0; i<size; i++){
         fprintf( fp,"%d %d %d\n",light[i].x,light[i].y,light[i].dir);
     }
     fclose(fp);
 }
}

void

trans_light( int x, int y, int size )    /*光を平行移動する関数*/
{
 int i;

 for ( i = 0; i< size; i++){
    light[i].x += x;
    light[i].y += y;
 }
}

/*光の位置が初めと同じなら０、異なれば１を返す関数*/

int

comp_light( int size )                        
{
 int i;

 for ( i = 0; i< size; i++){
   if( fps[i].x != light[i].x ){
       i = size;
   }else{
       if( fps[i].y != light[i].y ){
           i = size;
       }else{
           if( fps[i].dir != light[i].dir ){
                i = size;
           }
       }
   }
 }
 return (i-size);
}

int

check_light(int x,int y,int size)         /*光の有無を調べる関数*/
{
  int i;

  for(i=0;i<size;i++){
    if( x== light[i].x ){
      if( y == light[i].y ){
	return i;                         /*光の番号を返す*/
      }
    }
  }
  return -1;                              /*無ければ-1を返す*/
}

void

del_light( int i,int size)                /*光を削除する関数*/
{
  int j;

  for(j=i;j<size-1;j++){
    light[j].x = light[j+1].x;
    light[j].y = light[j+1].y;
    light[j].dir = light[j+1].dir;
  }
}
