/*
機能)
calratio で作成されたファイルをもとに、Column3,Column4の二つの全積分強度とそれらの比 Column4/Column3を標準出力に書き出す。

使用法)
cat (input file name) | integrate

出力ファイル書式)
I(1-0)	I(2-1)	(2-1)/(1-0)

*/




#include <math.h>
#include <stdio.h>
#include <stdlib.h>

main(argc,argv)
int	argc;
char	*argv[];
{

int	ii;
double	vmin, vmax;
double	x,y,z_mot,z_son,zz_mot, zz_son;
double 	ratio;

zz_son=0.0;
zz_mot=0.0;

while(fscanf(stdin,"%lf %lf %lf %lf %lf %lf %lf",&x,&y,&vmin,&vmax,&z_mot,&z_son,&ratio)==7){
zz_son=zz_son+z_son;
zz_mot=zz_mot+z_mot;
}
fprintf(stdout,"%14.6lf %14.6lf %14.6lf\n",
zz_mot,zz_son,zz_son/zz_mot);

}
