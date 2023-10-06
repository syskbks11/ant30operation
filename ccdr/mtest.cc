/* mtest.cc */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>

unsigned long GetColor( Display* dis, char* color_name )
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

  dis = XOpenDisplay( NULL );
  win = XCreateSimpleWindow( dis, RootWindow(dis,0), 100,100,
    256, 256, 3, WhitePixel(dis,0), BlackPixel(dis,0) );

  att.backing_store = WhenMapped;
  XChangeWindowAttributes( dis, win, CWBackingStore, &att );

  XSelectInput( dis, win, ExposureMask|ButtonPressMask );
  XMapWindow( dis, win );

  do{
    XNextEvent( dis, &ev);
  }while( ev.type != Expose );
  
  gc = XCreateGC( dis, DefaultRootWindow(dis), 0, 0 );
  XSetForeground( dis, gc, GetColor( dis, "yellow")  );
  
  int ox=0,oy=0;

  while(1){
    XNextEvent( dis, &ev );
    if( ev.type != ButtonPress ) continue;
    if( ev.xbutton.button == 1 ){
      XDrawLine( dis, win, gc, ox, oy, ev.xbutton.x, ev.xbutton.y );
      ox = ev.xbutton.x;
      oy = ev.xbutton.y;
      XFlush( dis );
      continue;
    }
    if( ev.xbutton.button == 3 ){
      break;
    }
  }
  
  XDestroyWindow( dis , win );
  XCloseDisplay( dis );
  
  return(0);
}
