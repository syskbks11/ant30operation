/* anime.cc */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WIN_WIDTH   256
#define WIN_HEIGHT  256
#define SQUARE_SIZE  16

#define AREA_X_MIN  (0)
#define AREA_X_MAX  (WIN_WIDTH-SQUARE_SIZE)
#define AREA_Y_MIN  (0)
#define AREA_Y_MAX  (WIN_HEIGHT-SQUARE_SIZE)


u_long GetColor( Display* dis, char* color_name )
{
  Colormap cmap;
  XColor near_color, true_color;

  cmap = DefaultColormap( dis, 0 );
  XAllocNamedColor( dis, cmap, color_name, &near_color, &true_color );
  return( near_color.pixel );
}


int main( void )
{
  Display* dis;
  Window win;
  XSetWindowAttributes att;
  GC gc;
  XEvent ev;

  int t;
  double x,y,ox,oy;
  double vx,vy;

  dis = XOpenDisplay( NULL );
  win = XCreateSimpleWindow( dis, RootWindow(dis,0), 100, 100,
      WIN_WIDTH, WIN_HEIGHT, 3, WhitePixel(dis,0), BlackPixel(dis,0) );

  att.backing_store = WhenMapped;
  XChangeWindowAttributes( dis, win, CWBackingStore, &att );
  
  XSelectInput( dis, win, ExposureMask );
  XMapWindow( dis, win );

  do{
    XNextEvent( dis, &ev);
  }while( ev.type != Expose );

  gc = XCreateGC( dis, RootWindow(dis,0), 0, 0 );
  XSetFunction( dis, gc, GXxor );
  XSetForeground( dis, gc, BlackPixel(dis,0)^GetColor( dis, "red")  );

  ox = x = WIN_WIDTH/2;
  oy = y = WIN_HEIGHT/3;
  vx = +1.0;
  vy = +1.2;

  XFillRectangle( dis, win, gc, (int)x, (int)y, SQUARE_SIZE, SQUARE_SIZE );

  for( t=0 ; t<2048 ; t++ ){
    usleep(1024);
    
    x += vx;
    y += vy;
    if( x < AREA_X_MIN ){ x=AREA_X_MIN; vx = -vx; }
    if( x > AREA_X_MAX ){ x=AREA_X_MAX; vx = -vx; }
    if( y < AREA_Y_MIN ){ y=AREA_Y_MIN; vy = -vy; }
    if( y > AREA_Y_MAX ){ y=AREA_Y_MAX; vy = -vy; }
    
    XFillRectangle( dis, win, gc, (int)ox, (int)oy, SQUARE_SIZE, SQUARE_SIZE );
    XFillRectangle( dis, win, gc, (int) x, (int) y, SQUARE_SIZE, SQUARE_SIZE );
    
    XFlush( dis );
    
    ox = x;
    oy = y;
  }

  XDestroyWindow( dis , win );
  XCloseDisplay( dis );

  return(0);
}

