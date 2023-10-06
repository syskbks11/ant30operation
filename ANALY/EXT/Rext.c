#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<strdef.h>
#include        <profhead.h>
#include        <starlib.h>
#include	<math.h>

#define		EPS	0.001
#define		MAX_CHAR_LENGTH	80

#define		R_MIN	0.0
#define		R_MAX	2.0
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
RANGE	r_range;
double	vwidth;
double	intensity;
char	in_file[MAX_CHAR_LENGTH], out_file[MAX_CHAR_LENGTH];



err_code=options(argc,argv,&vwidth,&r_range,in_file,out_file);

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


intensity=0.0;
err_code=fscanf(fpi,"%lf %lf %lf %lf %lf %lf %lf",&l,&b,&v_min,&v_max,&t10,&t21,&r);
if(err_code==EOF)	exit(1);
ref_l=l; ref_b=b;
if((r>=r_range.min)&&(r<=r_range.max))	intensity=t10;
while(1){
	err_code=fscanf(fpi,"%lf %lf %lf %lf %lf %lf %lf",&l,&b,&v_min,&v_max,&t10,&t21,&r);
	if(err_code==EOF)	break;
	if((fabs(l-ref_l)>EPS)||(fabs(b-ref_b)>EPS)){
		fprintf(fpo,"%lf\t%lf\t%lf\n",ref_l,ref_b,intensity*vwidth);
		ref_l=l; ref_b=b;
		intensity=0.0;
		continue;
	}
	if((r>=r_range.min)&&(r<=r_range.max))	intensity=intensity+t10;					
}

fclose(fpi);
fclose(fpo);


}





int options(argc,argv,vwidth,r_range,in_file,out_file)
/*
オプションスイッチを変数に代入する
*/
int	argc;
char	*argv[];
double	*vwidth;
RANGE	*r_range;
char	in_file[], out_file[];
{
	int ii,jj;
	int	err_code;
	char	*token;


/* default values */
	
(*r_range).min=R_MIN;
(*r_range).max=R_MAX;
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
			else if(strcmp(argv[ii],"-r_range")==0){
				if(ii<argc){	
					ii++;
					token = strtok(argv[ii],",");
					(*r_range).min = atof( token );

					token = strtok(NULL,",");
					(*r_range).max = atof( token );
				}
			}

		}
	}
/* 範囲の大小の整え */
	STAR_SortRange(r_range);

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
