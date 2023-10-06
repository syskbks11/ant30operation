/*     �����ߥ�졼������ѡʸ��Τߡ�    *
 *         haeder file  for UNIX         *
 *        1999.10.31  By �ʰ� ��         */

/* �ؿ�����
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

/* �ޥ������ */

#define gotoxy(x,y)      printf("\x1b[%d;%dH",(y),(x))
#define priminibuf(a,b)  gotoxy(1,YMAX+1);printf("%s %s",a,b)
#define prinminibuf(a,b) gotoxy(1,YMAX+1);printf("%s %d",a,b)

/* ��� */

#define MAXSIZE 85                   /*���κ����*/
#define VecX 1                       /*��η׻��˻Ȥ��٥��ȥ�*/ 
#define VecY 4                       /*X�ϱ�,Y�Ϻ���*/
#define ESC     '\x1b'
#define Cs      '\x13'

/* ��¤�Τ���� */

struct tri{
   int x;
   int y;                            /*��ɸ*/
   int dir;                          /*����*/
};

struct tri light[ MAXSIZE];          /*����ɽ����¤��*/
struct tri fps[ MAXSIZE];            /*������֤ε�Ͽ*/

/* �ؿ���� */

int

init_light( char *fname)             /*����ե����뤫���ɤ߼��ؿ�*/
{
 FILE *fp;
 int i,x,y,d;

 if( (fp=fopen(fname,"r")) == NULL){
     fprintf(stderr,"%s�������ץ�Ǥ��ޤ���\n",fname);
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
   return i;                          /*���θĿ����֤�*/
 }
}

void

move_light(int size)                  /*���򼡤λ����ư�����ؿ�*/
{
 int i;

 for(i = 0; i<size; i++){
   switch( light[i].dir){             /*�����Ǿ��ʬ��*/
   case 0:                            /*��*/
      light[i].x +=2;break;
   case 1:                            /*����*/
      light[i].x++;
      light[i].y--;break;
   case 2:                            /*����*/
      light[i].x--;
      light[i].y--;break;
   case 3:                            /*��*/
      light[i].x -=2;break;
   case 4:                            /*����*/
      light[i].x--;
      light[i].y++;break;
   case 5:                            /*����*/
      light[i].x++;
      light[i].y++;break;
   default:break;
   }
 }
}

void

display_light(int size)               /*����ɽ������ؿ�*/
{
 int i;

 for( i=size-1; i>=0; i--){
    gotoxy( light[i].x, light[i].y);
    printf("%d",light[i].dir);        /*������ɽ��������ɽ��*/
 }
 fflush(stdout);
}

int

check_field( int i,int size)          /*���˷�����׻�����ؿ�*/
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
	      printf("\x1b[%d;%dH����",1,39);
           }
         default: break;
       }
     default:break;
   }
 }
 return field;
}

void

cal_dir(int size)                      /*�줫����������ꤹ��ؿ�*/    
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
     switch( field ){                  /*f���ͤǺ��Ѥ���ޤ�*/
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
     light[i].dir=(f-light[i].dir)%6;   /*��κ��Ѥ�ɽ����*/
    }
 }
}

void

rotate_light( int d, int size )         /*������������ͤ��Ѥ���ؿ�*/
{
 int i;

 for ( i = 0; i< size; i++){
    light[i].dir = (light[i].dir += d )%6;
 }
}

void

load_light( char *fname, int size)      /*�ե�����˸���񤭹���ؿ�*/
{
  FILE *fp;
  int i;

 if( (fp=fopen(fname,"w")) == NULL){
     fprintf(stderr,"%s�������ץ�Ǥ��ޤ���\n",fname);
     exit();
 }else{
     for( i=0; i<size; i++){
         fprintf( fp,"%d %d %d\n",light[i].x,light[i].y,light[i].dir);
     }
     fclose(fp);
 }
}

void

trans_light( int x, int y, int size )    /*����ʿ�԰�ư����ؿ�*/
{
 int i;

 for ( i = 0; i< size; i++){
    light[i].x += x;
    light[i].y += y;
 }
}

/*���ΰ��֤�����Ʊ���ʤ飰���ۤʤ�У����֤��ؿ�*/

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

check_light(int x,int y,int size)         /*����̵ͭ��Ĵ�٤�ؿ�*/
{
  int i;

  for(i=0;i<size;i++){
    if( x== light[i].x ){
      if( y == light[i].y ){
	return i;                         /*�����ֹ���֤�*/
      }
    }
  }
  return -1;                              /*̵�����-1���֤�*/
}

void

del_light( int i,int size)                /*����������ؿ�*/
{
  int j;

  for(j=i;j<size-1;j++){
    light[j].x = light[j+1].x;
    light[j].y = light[j+1].y;
    light[j].dir = light[j+1].dir;
  }
}
