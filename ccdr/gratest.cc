#include <X11/Xlib.h>             //Xlibに必要なインクルード
#include <X11/Xutil.h>
#include <stdio.h>

int main( void )
{
  Display* dis;                       //Display ID
  Window   win;                       //Window  ID
  XSetWindowAttributes att;
  XEvent ev;

  dis = XOpenDisplay( NULL );
  win = XCreateSimpleWindow( dis, RootWindow(dis,0), 100, 100,
	      256, 256, 3, WhitePixel(dis,0), BlackPixel(dis,0) );

  att.backing_store = WhenMapped;
  XChangeWindowAttributes( dis, win, CWBackingStore, &att );

  XMapWindow( dis, win );
  XFlush( dis );

  XSelectInput( dis, win, ExposureMask );
  do{
    XNextEvent( dis, &ev);
  }while( ev.type != Expose );

  getchar();

  XDestroyWindow( dis, win );
  XCloseDisplay( dis );

  return(0);
}
