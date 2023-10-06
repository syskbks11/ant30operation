#include	<stdio.h>
#include	<string.h>
#include	<math.h>
//#include	<strdef.h>
#include        <profhead.h>
#include        <starlib.h>

#define		R_COLUMN_NUM	80
#define		T_COLUMN_NUM	80
//#define		MAX_CHAR_LENGTH	80
#define		DEFAULT_TRANGE_MIN	(-999.9)

#define		R_STEP	0.1
#define		R_MIN	0.0
#define		R_MAX	2.0
#define		T_STEP	4.0
#define		T_MIN	0.0
#define		T_MAX	20.0


extern int	options();

main(argc,argv)
int	argc;
char	*argv[];
{
FILE	*fpi,*fpo;
int	ii,jj;
int	err_code;
int	r_col, t_col;
double	l,b,v_min,v_max,t21,t10,r;
double	t_min,t_max,r_min,r_max;
RANGE	t_range, r_range;
double	t_step, r_step,t_lower_limit;
double	vwidth;
static double	T[T_COLUMN_NUM];
static double	R[R_COLUMN_NUM], R_freq[T_COLUMN_NUM][R_COLUMN_NUM];
char	buffer[CHAR_LENGTH];
char	in_file[CHAR_LENGTH], out_file[CHAR_LENGTH];

err_code=options(argc,argv,&t_range,&t_step,&t_lower_limit,&r_range,&r_step,&vwidth,in_file,out_file);

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
t_min=t_lower_limit;
t_max=t_range.min+t_step;
while(t_max<=t_range.max){
	jj=0;
	r_min=r_range.min;
	r_max=r_min+r_step;
	fprintf(stderr,"Temperature range : %lf to %lf\n",t_min,t_max);
	while(r_max<=r_range.max){
		fseek(fpi,0L,0);
		R_freq[ii][jj]=0.0;
		fprintf(stderr,"Ratio range : %lf to %lf\t",r_min,r_max);
		while(fscanf(fpi,"%lf %lf %lf %lf %lf %lf %lf",&l,&b,&v_min,&v_max,&t10,&t21,&r)==7){
			if(((t10>=t_min)&&(t10<t_max)&&(t21>=t_lower_limit)) && 
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





int options(argc,argv,t_range,t_step,t_lower_limit,r_range,r_step,vwidth,in_file,out_file)
/*
オプションスイッチを変数に代入する
*/
int	argc;
char	*argv[];
RANGE	*t_range, *r_range;
double	*t_step, *t_lower_limit, *r_step;
double	*vwidth;
char	in_file[], out_file[];
{
	int ii,jj;
	int	err_code;
	char	*token;


/* default values */
	
(*t_range).min=T_MIN;
(*t_range).max=T_MAX;
*t_step=T_STEP;
*t_lower_limit=DEFAULT_TRANGE_MIN;
(*r_range).min=R_MIN;
(*r_range).max=R_MAX;
*r_step=R_STEP;
*vwidth=1.0;
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
			else if(strcmp(argv[ii],"-vwidth")==0){
				ii++;
				*vwidth=atof(argv[ii]);
			}
			else if(strcmp(argv[ii],"-r_step")==0){
				ii++;
				*r_step=atof(argv[ii]);
			}
			else if(strcmp(argv[ii],"-r_range")==0){
				if(ii<argc){	
					ii++;
					token = strtok(argv[ii],",");
					(*r_range).min = atof( token );

					token = strtok(NULL,",");
					(*r_range).max = atof( token );
				}
			}
			else if(strcmp(argv[ii],"-t_step")==0){
				ii++;
				*t_step=atof(argv[ii]);
			}
			else if(strcmp(argv[ii],"-t_limit")==0){
				ii++;
				*t_lower_limit=atof(argv[ii]);
			}
			else if(strcmp(argv[ii],"-t_range")==0){
				if(ii<argc){	
					ii++;
					token = strtok(argv[ii],",");
					(*t_range).min = atof( token );

					token = strtok(NULL,",");
					(*t_range).max = atof( token );
				}
			}

		}
	}
/* 範囲の大小の整え */
	STAR_SortRange(r_range);
	STAR_SortRange(t_range);

if(in_file[0]==NULL){
	fprintf(stderr,"Input filename is not specified.\n");
	fprintf(stderr,"Use the option switch -in (filename).\n\n");
	return(100);
}
if(in_file[0]==NULL){
	fprintf(stderr,"Output filename is not specified.\n");
	fprintf(stderr,"Use the option switch -out (filename).\n\n");
	return(200);
}


	return(0);
}
