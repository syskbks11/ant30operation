#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <curses.h>

#define MAP_W 80
#define MAP_H 40
#define VIEW_W 17
#define VIEW_H 9
#define hx ((int)(VIEW_W/2)+x)
#define hy ((int)(VIEW_H/2)+y)

char MapDat[ MAP_W][ MAP_H];

#define gotoxy(x,y) printf("\x1b[%d;%dH",(y),(x))
#define cls() printf("\x1b[2J")

int

read_map( const char *fname)
{
  FILE *fp;
  char buf[ MAP_W+2],*cp;
  int l;

  if( (fp=fopen(fname,"r"))==NULL){
    return(0);
  }else{
     memset(MapDat,0,sizeof(MapDat)); 
     l=0; 
     while( l<MAP_H && fgets(buf,sizeof(buf),fp)){
      if( (cp = strchr(buf,'\n'))!=NULL){
	*cp = '\0';
      }
      strncpy(MapDat[l],buf,MAP_W);
      l++;
    }
    fclose(fp);
    return (l);
  }
}

void

disp_map( int sx, int sy, int x,int y)
{
  int i,j;
  char c;

  for(j=y;j<y+VIEW_H;j++){
    gotoxy(sx,sy+j-y);
    for(i=x;i<x+VIEW_W;i++){
      c = MapDat[ j%MAP_H][ i%MAP_W];
      putchar( iscntrl(c)?'.':c);
    }
  }
  fflush(stdout);
}

void

main( int argc,char *argv[])
{
  int x=1,y=1,c=0;

  if(argc!=2 || read_map( argv[argc-1])<=0){
    fputs("テキストファイルを指定して下さい。\n",stderr);
    exit(1);
  }

  initscr();
  raw();
  noecho();

  do{
    switch(c){
      case 'i':if(--y<0) y=MAP_H-1;
	break;
      case 'k':if(++x>MAP_W) x=0;
	break;
      case 'j':if(--x<0) x=MAP_W-1;
	break;
      case 'm':if(++y>=MAP_H) y=0;
	break;
      default:break;
    }
    disp_map(5,3,x,y);
    gotoxy((int)(VIEW_W/2)+5,(int)(VIEW_H/2)+3);
    fflush(stdout);
  }while( (c=getch()) !='.');
  endwin();
}
