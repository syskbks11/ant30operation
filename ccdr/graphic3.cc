#include <iostream.h>
#include "/home/yamaguch/lib/xgraphc.h"
waiting(int t)
{
    if (t>0) waiting(t-1);
}
double bounce(double dx,double x,double s,double g)
{
    if ((x>g) || (x<s)) return -dx;
    return dx;
} 
show(int times,double x,double y,double dx,double dy)
{
    const double rad=30;
    setcolor(White);
    circle(int(x),int(y),int(rad));
    waiting(100);
    setcolor(Black);
    circle(int(x),int(y),int(rad));
    if (times>0) show(times-1,x+dx,y+dy,
        bounce(dx,x+dx,rad,getmaxx()-rad),
        bounce(dy,y+dy,rad,getmaxy()-rad));
}
main()
{
    opengraph();
    show(10000,getmaxx()/2.0,getmaxy()/2.0,4.0,1.0);
    readln();
    closegraph();
}
