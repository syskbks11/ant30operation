/*  ���󤸤礦�� �����ߥ�졼�������  *
 *             header file             *
 *              for UNIX               *
 *           1999.9.30 16:07           */

#include <stdio.h>

#define gotoxy(x,y)  printf("\x1b[%d;%dH",(y),(x))
#define MAXSIZE 30

/*����ɽ����¤��*/
struct{
   int x;
   int y;    /*��ɸ*/
   int dir;  /*����*/
} light[ MAXSIZE];

/*���Τ�ɽ����¤��*/
struct{
   int x;
   int y;    /*��ɸ*/
   int mass; /*����*/
} solid[ MAXSIZE];

/*���򼡤λ����ư�����ؿ�*/
void

move_light(int size)
{
 int i;

 for(i = 0; i<size; i++){
   switch( light[i].dir){
                 /*�����Ǿ��ʬ��*/
   case 0:       /*��*/
      light[i].x +=2;break;
   case 1:       /*����*/
      light[i].x++;
      light[i].y--;break;
   case 2:       /*����*/
      light[i].x--;
      light[i].y--;break;
   case 3:       /*��*/
      light[i].x -=2;break;
   case 4:       /*����*/
      light[i].x--;
      light[i].y++;break;
   case 5:       /*����*/
      light[i].x++;
      light[i].y++;break;
   default:break;
   }
 }
}

/*����ɽ������ؿ�*/
void

display_light(int size)
{
 int i;

 for( i=size-1; i>=0; i--){
    gotoxy( light[i].x, light[i].y);
    printf("%d",light[i].dir);
    /*������ɽ��������ɽ��*/
 }
 fflush(stdout);
}

/*���Τ�ɽ������ؿ�*/
void

display_solid(int size)
{
 int i;

 for( i=size-1; i>=0; i--){
    gotoxy( solid[i].x, solid[i].y);
    printf("\x1b[7m%d\x1b[0m",solid[i].mass);
    /*���̤�ɽ��������ɽ��*/
 }
 fflush(stdout);
}

/*����ե����뤫���ɤ߼��ؿ�*/
int

init_light( char *fname)
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
   return i;   /*���θĿ����֤�*/
 }
}

/*���Τ�ե����뤫���ɤ߼��ؿ�*/
int

init_solid( char *fname)
{
 FILE *fp;
 int i,x,y,m;

 if( (fp=fopen(fname,"r")) == NULL){
     fprintf(stderr,"%s�������ץ�Ǥ��ޤ���\n",fname);
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
   return i;   /*���ΤθĿ����֤�*/
 }
}

