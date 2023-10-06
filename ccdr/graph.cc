#include <iostream.h>
#include "/home/yamaguch/lib/xgraphc.h"

main()
{
   opengraph();
   int r;

   cin >> r; readln();
   circle(400-200,200,r);
   line(400-150,450,400+150,450);
   readln();
   closegraph();
}
