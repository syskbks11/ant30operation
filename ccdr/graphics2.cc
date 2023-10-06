#include <fstream.h>
#include <iostream.h>
#include "/home/yamaguch/lib/xgraphc.h"
show(fstream fin,int x)
{
    double v;
    fin >> v;
    if (!fin) return 0;
    line(x,getmaxy(),x,getmaxy()-int(v));
    show(fin,x+10);
}
main()
{
    opengraph();
    fstream fin("immig5.dat",ios::in);
    if (!fin) cout << "File not found" << "\n";
    show(fin,0);
    fin.close();
    readln();
    closegraph();
}
