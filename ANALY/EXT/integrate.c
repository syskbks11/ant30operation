/*
�@�\)
calratio �ō쐬���ꂽ�t�@�C�������ƂɁAColumn3,Column4�̓�̑S�ϕ����x�Ƃ����̔� Column4/Column3��W���o�͂ɏ����o���B

�g�p�@)
cat (input file name) | integrate

�o�̓t�@�C������)
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
