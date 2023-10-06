#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<strdef.h>
#include        <profhead.h>
#include        <starlib.h>
#include	<math.h>

#define		EPS	0.001
#define		MAX_CHAR_LENGTH	80

#define		V_MIN	(-150.0)
#define		V_MAX	(150.0)
#define		T_STEP	4.0


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
double	ref_l, ref_b;
RANGE	v_range;
double	vwidth, dummy_v;
double	int10, int21;
char	in_file[MAX_CHAR_LENGTH], out_file[MAX_CHAR_LENGTH];



err_code=options(argc,argv,&v_range,in_file,out_file);

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


int10=0.0;
int21=0.0;
err_code=fscanf(fpi,"%lf %lf %lf %lf %lf %lf %lf",&l,&b,&v_min,&v_max,&t10,&t21,&r);
if(err_code==EOF)	exit(1);
ref_l=l; ref_b=b; vwidth=fabs(v_max-v_min);
if((v_min>=v_range.min)&&(v_max<=v_range.max)){
		int10=t10; 
		int21=t21;
}
while(1){
	err_code=fscanf(fpi,"%lf %lf %lf %lf %lf %lf %lf",&l,&b,&v_min,&v_max,&t10,&t21,&r);
	if(err_code==EOF)	break;
	if((fabs(l-ref_l)>EPS)||(fabs(b-ref_b)>EPS)){
		fprintf(fpo,"%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n",
		ref_l,ref_b,v_range.min,v_range.max,
		int10*vwidth,int21*vwidth,int21/int10);
		ref_l=l; ref_b=b;
		if((v_min>=v_range.min)&&(v_max<=v_range.max)){
			int10=t10;
			int21=t21;
		}
		else{
			int10=0.0;
			int21=0.0;
		}
		continue;
	}
	if((v_min>=v_range.min)&&(v_max<=v_range.max)){
		int10=int10+t10;
		int21=int21+t21;
	}
					
}

fclose(fpi);
fclose(fpo);


}





int options(argc,argv,v_range,in_file,out_file)
/*
オプションスイッチを変数に代入する
*/
int	argc;
char	*argv[];
RANGE	*v_range;
char	in_file[], out_file[];
{
	int ii,jj;
	int	err_code;
	char	*token;


/* default values */
	
(*v_range).min=V_MIN;
(*v_range).max=V_MAX;
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
	STAR_SortRange(v_range);

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
