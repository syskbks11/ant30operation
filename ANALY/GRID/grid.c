/*	
Integrated Intensity Map あるいは Channel Map を再グリッドするプログラム。
入力ファイル形式:	STAR標準ratioファイル形式
出力ファイル形式:	同上

1995/6/22	岡　朋治
*/


#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>
//#include	<strdef.h>
#include	<profhead.h>
#include	<starlib.h>

#define	DEBUG 0

#define	EPS	0.001
#define	PI	3.1415927

#define	L_MIN	(-180.0)	
#define	L_MAX	180.0
#define	B_MIN	(-90.0)	
#define	B_MAX	90.0
#define	V_MIN	(-200.0)	
#define	V_MAX	200.0
#define	T_STEP	4.0

#define	MAX_ROW	8192
#define	MAX_DATA	512
#define	MAX_V_DATA	1

extern int	options();



main(argc,argv)
int	argc;
char	*argv[];
{
FILE	*fpi,*fpo;
int	ii,jj,kk,i,j;
int	err_code;
int	data_num;
int	l_num, b_num, v_num;
int	l_num_new, b_num_new;
int	mode;
int	r_col, t_col;
double	buff;
double	l[MAX_ROW], b[MAX_ROW], 	ll, bb;
double	v_min,v_max, vwidth,	t21[MAX_ROW], t10[MAX_ROW], r;
double	tt1, tt2;
double	dist;
double	grid_old;
double	grid, width;
double	sum_exp;
double	int_old, int_new;
RANGE	l_range, b_range, v_range;
char	in_file[CHAR_LENGTH], out_file[CHAR_LENGTH];
double	L[MAX_DATA], B[MAX_DATA], V;
double	T1[MAX_DATA][MAX_DATA];
double	T2[MAX_DATA][MAX_DATA];


err_code=options(argc,argv,&mode,&l_range,&b_range,&v_range,&grid,&width,in_file,out_file);

if(err_code!=0){
	fprintf(stderr,"Command option is not correct.\n\n");
	exit(999);
}


if(in_file[0]==NULL)	fpi=stdin;
else{
	fpi=fopen(in_file,"rb");
	if(fpi==NULL){
		fprintf(stderr,"Can't open %s.\n",in_file);
		exit(999);
	}
}

if(out_file[0]==NULL)	fpo=stdout;
else{
	fpo=fopen(out_file,"w");
	if(fpo==NULL){
		fprintf(stderr,"Can't create %s.\n",out_file);
		exit(999);
	}
}


kk=0;
while(fscanf(fpi,"%lf %lf %lf %lf %lf %lf %lf"
,&(l[kk]),&(b[kk]),&v_min,&v_max,&(t10[kk]),&(t21[kk]),&r)==7){
	kk++;
}
vwidth=fabs(v_max-v_min);
data_num=kk;

#if DEBUG
fprintf(stderr,"data_num=%d\n",data_num);
#endif

/*	L, B, Vの順に sort	*/

for(ii=0;ii<data_num;ii++){
	for(jj=ii+1;jj<data_num;jj++){
			if(l[ii]>l[jj]){
				buff=l[ii]; l[ii]=l[jj]; l[jj]=buff;
				buff=b[ii]; b[ii]=b[jj]; b[jj]=buff;
				buff=t10[ii]; t10[ii]=t10[jj]; t10[jj]=buff;
				buff=t21[ii]; t21[ii]=t21[jj]; t21[jj]=buff;
				}
		}
}for(ii=0;ii<data_num;ii++){
	for(jj=ii+1;jj<data_num;jj++){
			if((b[ii]>b[jj])&&(l[ii]==l[jj])){
				buff=l[ii]; l[ii]=l[jj]; l[jj]=buff;
				buff=b[ii]; b[ii]=b[jj]; b[jj]=buff;
				buff=t10[ii]; t10[ii]=t10[jj]; t10[jj]=buff;
				buff=t21[ii]; t21[ii]=t21[jj]; t21[jj]=buff;
				}
		}
}


/*	count	*/

L[0]=l[0];  kk=1;
for(ii=1;ii<data_num;ii++){
	for(jj=0;jj<kk;jj++){
		if(l[ii]==L[jj])	break;
	}
	if(jj==kk){
		L[kk]=l[ii];	kk++;
	}
}
l_num=kk;

B[0]=b[0];  kk=1;
for(ii=1;ii<data_num;ii++){
	for(jj=0;jj<kk;jj++){
		if(b[ii]==B[jj])	break;
	}
	if(jj==kk){
		B[kk]=b[ii];	kk++;
	}
}
b_num=kk;
grid_old=fabs(L[0]-L[1]);

if(grid_old/2.0>width){
	fprintf(stderr,"Smoothing width must be larger than a half of the grid spacing.\n");
	exit(1);
}


#if DEBUG
fprintf(stderr,"(l,b)=(%d,%d)\n",l_num,b_num);
#endif

int_old=0.0;
for(ii=0;ii<l_num;ii++){
	for(jj=0;jj<b_num;jj++){
			T1[ii][jj]=t10[ii*b_num+jj];
			T2[ii][jj]=t21[ii*b_num+jj];
			if(  ((L[ii]>=l_range.min)&&(L[ii]<=l_range.max))&&
				((B[jj]>=b_range.min)&&(B[jj]<=b_range.max))  ){
				int_old=int_old+	T2[ii][jj];
			}
	}
}


/*	Regridding and output data	*/

int_new=0.0;
l_num_new=fabs(l_range.max-l_range.min)/grid+1;
b_num_new=fabs(b_range.max-b_range.min)/grid+1;
bb=b_range.min;
for(jj=0;jj<b_num_new;jj++){
	ll=l_range.max; 
	for(ii=0;ii<l_num_new;ii++){
		tt1=0.0;
		tt2=0.0;	
		sum_exp=0.0;
		for(i=0;i<l_num;i++){
			for(j=0;j<b_num;j++){
				dist=pow((ll-L[i]),2.0)+pow((bb-B[j]),2.0);
				sum_exp=sum_exp + exp(-(dist/(width*width)));
				tt1=tt1+T1[i][j]*exp(-(dist/(width*width)));
				tt2=tt2+T2[i][j]*exp(-(dist/(width*width)));
			}
		}
		tt1=tt1/sum_exp;
		tt2=tt2/sum_exp;
		fprintf(stdout,"%9.6lf\t%9.6lf\t%9.3lf\t%9.3lf\t%9.3lf\t%9.3lf\t%9.3lf\n",
			ll,bb,v_min,v_max,tt1,tt2,tt2/tt1);
		int_new=int_new+tt2;
		ll=ll-grid; 
	}
	bb=bb+grid;
}


#if DEBUG
fprintf(stderr,"Integrated intensity check\n");
fprintf(stderr,"Before regridding : %lf\n",int_old);
fprintf(stderr,"After regridding : %lf\n",int_new/pow(grid_old/grid,2.0));
#endif


if(fpi!=stdin)	fclose(fpi);
if(fpo!=stdout)	fclose(fpo);
}





int options(argc,argv,mode,l_range,b_range,v_range,grid,width,in_file,out_file)
/*
オプションスイッチを変数に代入する
*/
int	argc;
char	*argv[];
int	*mode;
RANGE	*l_range, *b_range, *v_range;
double	*grid, *width;
char	in_file[], out_file[];
{
	int ii,jj;
	int	err_code;
	char	*token;
	

/* default values */
	
*mode=0;	/*	No smoothing	*/
(*l_range).min=L_MIN;
(*l_range).max=L_MAX;
(*b_range).min=B_MIN;
(*b_range).max=B_MAX;
(*v_range).min=V_MIN;
(*v_range).max=V_MAX;
*grid=0.125;
*width=9.0/60.0/2.0/log(2.0);
in_file[0]=NULL;
out_file[0]=NULL;


/* オプションスイッチを解釈し変数に代入する。 */
	for(ii=1;ii<argc;ii++)
	{
		if(argv[ii][0]=='-')	/* option switch */
		{
			if(strcmp(argv[ii],"-in")==0){
				ii++;
				strcpy(in_file,argv[ii]);
			}
			else if(strcmp(argv[ii],"-out")==0){
				ii++;
				strcpy(out_file,argv[ii]);
			}
			else if(strcmp(argv[ii],"-grid")==0){
				ii++;
				*grid=atof(argv[ii]);
			}
			else if(strcmp(argv[ii],"-width")==0){
				ii++;
				*width=atof(argv[ii]);
				*width=(*width)/2.0/log(2.0);
				*mode=1;
			}
			else if(strcmp(argv[ii],"-l")==0){
				if(ii<argc){	
					ii++;
					token = strtok(argv[ii],",");
					(*l_range).min = atof( token );

					token = strtok(NULL,",");
					(*l_range).max = atof( token );
				}
			}
			else if(strcmp(argv[ii],"-b")==0){
				if(ii<argc){
					ii++;
					token = strtok(argv[ii],",");
					(*b_range).min = atof( token );
					
					token = strtok(NULL,",");
					(*b_range).max = atof( token );
				}
			}
			else if(strcmp(argv[ii],"-v")==0){
				if(ii<argc){
					ii++;
					token = strtok(argv[ii],",");
					(*v_range).min = atof( token );

					token = strtok(NULL,",");
					(*v_range).max = atof( token );
				}
			}
		}
	}
/* 範囲の大小の整え */
	STAR_SortRange(l_range);
	STAR_SortRange(b_range);
	STAR_SortRange(v_range);


	return(0);
}
