#include <iostream.h>
#include <math.h>
#include "/home/yamaguch/lib/xgraphc.h"
tree(double x,double y,double dir,double fan,double fanr,double len,double lenr)
{
    if (len<1) return 0;
    double nx = x+len*sin(2*3.14159/360*dir);
    double ny = y+len*cos(2*3.14159/360*dir);
    line(int(x),int(y),int(nx),int(ny));
    tree(nx,ny,dir+fan,fan/fanr,fanr,len/lenr,lenr);
    tree(nx,ny,dir-fan,fan/fanr,fanr,len/lenr,lenr);
}
main()
{
    double dir; cout << "direction="; cin >> dir; // 0
    double fan; cout << "fan="; cin >> fan; // 30 
    double fanr; cout << "fan reduction rate="; cin >> fanr; // 2
    double len; cout << "length="; cin >> len; // 90 
    double lenr; cout << "length reduction rate="; cin >> lenr; // 2
    opengraph();
    tree(400,400,dir,fan,fanr,len,lenr);
    readln(); // remove return key after cin >> len
    readln(); // wait return key
    closegraph();
}
// Copyright 1996 Kazunori Yamaguchi
