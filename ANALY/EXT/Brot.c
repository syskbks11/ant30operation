#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>

#define		MAX_CHAR_LENGTH	80
#define		DATA_NUM	2000	/*Ch. map:357, b-V map:1020*/
#define		GRID		0.125

double	sankaku(x1,x2,dx,grid)
double	x1, x2, dx, grid;
{
double	y;

if(fabs(x1-dx-x2)<=grid){
	y=(grid-fabs(x1-dx-x2))/grid;
}
else	y=0.0;

return(y);
}





main(argc,argv)
int	argc;
char	*argv[];
{
int	ii, jj;
int	data_num;
double	l[DATA_NUM], b[DATA_NUM], 
	vmin[DATA_NUM], vmax[DATA_NUM], 
	t10[DATA_NUM], t21[DATA_NUM], r[DATA_NUM];
double	new_t21[DATA_NUM];
double	delta_l, delta_b;


if(argc<2){
	fprintf(stderr,"Usage :	Brot [Delta B]\n");
	exit(1);
}
delta_b=atof(argv[1]);

ii=0;
while(fscanf(stdin,"%lf %lf %lf %lf %lf %lf %lf"
	,&l[ii],&b[ii],&vmin[ii],&vmax[ii],&t10[ii],&t21[ii],&r[ii])==7){
	ii++;
}	/*	while	*/
data_num=ii;

for(ii=0;ii<data_num;ii++){
	new_t21[ii]=0.0;
	for(jj=0;jj<data_num;jj++){
		if((l[ii]==l[jj])&&(vmin[ii]==vmin[jj])&&(vmax[ii]==vmax[jj])){
			new_t21[ii]=new_t21[ii]+
					t21[jj]*sankaku(b[ii],b[jj],delta_b,GRID);
		}				
	}
	r[ii]=new_t21[ii]/t10[ii];
fprintf(stdout,"%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n",
		l[ii],b[ii],vmin[ii],vmax[ii],t10[ii],new_t21[ii],r[ii]);
}


}	/*	main	*/
