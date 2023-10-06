/*
cc -o rhist rhist.c -lm
*/


#include	<stdio.h>
#include	<math.h>

#include	<strdef.h>
#include        <profhead.h>
#include        <starlib.h>

#define		R_COLUMN_NUM	80
#define		T_COLUMN_NUM	80
#define		MAX_CHAR_LENGTH	80

#define		R_STEP	0.1
#define		R_MIN	0.0
#define		R_MAX	2.0
#define		T_STEP	4
#define		T_MIN	(-999.9)
#define		T_MAX	20


extern int	options();

main(argc,argv)
int	argc;
char	*argv[];
{
FILE	*fpi,*fpo;
int	ii,jj;
int	err_code;
int	r_col, t_col;
double	l,v,t21,t10,r;
double	t_min,t_max,r_min,r_max;
RANGE	t_range, r_range;
double	t_step, r_step;
double	vwidth;
static double	T[T_COLUMN_NUM];
static double	R[R_COLUMN_NUM], R_freq[T_COLUMN_NUM][R_COLUMN_NUM];
char	buffer[MAX_CHAR_LENGTH];
char	in_file[MAX_CHAR_LENGTH], out_file[MAX_CHAR_LENGTH];

err_code=options(argc,argv,t_range,&t_step,r_range,&r_step,&vwidth,in_file,out_file);

if(err_code!=0){
	fprintf(stderr,"Command option is not correct.\n\n");
	exit(999);
}
	

fpi=fopen(in_file,"rb");
if(fpi==NULL){
	fprintf(stderr,"Can't open %s.\n",in_file);
	exit(999);
}

fpo=fopen(out_file,"w");
if(fpo==NULL){
	fprintf(stderr,"Can't create %s.\n",out_file);
	exit(999);
}

ii=0;
t_min=t_range.min;
t_max=t_min+t_step;
while(t_max<=t_range.max){
	jj=0;
	r_min=r_range.min;
	r_max=r_min+r_step;
	fprintf(stderr,"Temperature range : %lf to %lf\n",t_min,t_max);
	while(r_max<=r_range.max){
		fseek(fpi,0L,0);
		R_freq[ii][jj]=0.0;
		fprintf(stderr,"Ratio range : %lf to %lf\t",r_min,r_max);
		while(fscanf(fpi,"%lf %lf %lf %lf %lf",&l,&v,&t10,&t21,&r)==5){
			if(((t10>=t_min)&&(t10<t_max)) && 
				((r>=r_min)&&(r<r_max)) ){
					R_freq[ii][jj]=R_freq[ii][jj]+t10*vwidth;
			}
		}
		fprintf(stderr,"R_freq[%d][%d]=%lf\n",ii,jj,R_freq[ii][jj]);
		R[jj]=r_min;
		r_min=r_max;
		r_max=r_min+r_step;
		jj++;
	}
		r_col=jj;

	T[ii]=t_min;
	t_min=t_max;
	t_max=t_min+t_step;
	ii++;
	fprintf(stderr,"\n");
}
	t_col=ii;

fclose(fpi);


fprintf(fpo,"%s\t","Ratio_range");
for(ii=0;ii<t_col;ii++){
	fprintf(fpo,"%lf -\t",T[ii]);
}
fprintf(fpo,"\n");


for(jj=0;jj<r_col;jj++){
	fprintf(fpo,"%lf\t",R[jj]);
	for(ii=0;ii<t_col;ii++){
		fprintf(fpo,"%lf\t",R_freq[ii][jj]);
	}
	fprintf(fpo,"\n");
}
fclose(fpo);


}
