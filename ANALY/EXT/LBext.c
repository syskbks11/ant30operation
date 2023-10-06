#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<strdef.h>
#include        <profhead.h>
#include        <starlib.h>
#include	<math.h>

#define		EPS	0.001
#define		MAX_CHAR_LENGTH	80

#define		L_MIN	(-180.0)	
#define		L_MAX	180.0
#define		B_MIN	(-90.0)	
#define		B_MAX	90.0
#define		V_MIN	(-200.0)	
#define		V_MAX	200.0
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
RANGE	l_range, b_range, v_range;
char	in_file[MAX_CHAR_LENGTH], out_file[MAX_CHAR_LENGTH];



err_code=options(argc,argv,&l_range,&b_range,&v_range,in_file,out_file);

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


if(err_code==EOF)	exit(1);

while(1){
	err_code=fscanf(fpi,"%lf %lf %lf %lf %lf %lf %lf",&l,&b,&v_min,&v_max,&t10,&t21,&r);
	if(err_code==EOF)	break;
	if((l>=l_range.min)&&(l<=l_range.max) && 
		(b>=b_range.min)&&(b<=b_range.max) &&
		(v_min>=v_range.min)&&(v_max<=v_range.max) ){
		fprintf(fpo,"%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n",
				l,b,v_min,v_max,t10,t21,r);
	}
}

if(fpi!=stdin)	fclose(fpi);
if(fpo!=stdout)	fclose(fpo);


}





int options(argc,argv,l_range,b_range,v_range,in_file,out_file)
/*
オプションスイッチを変数に代入する
*/
int	argc;
char	*argv[];
RANGE	*l_range, *b_range, *v_range;
char	in_file[], out_file[];
{
	int ii,jj;
	int	err_code;
	char	*token;


/* default values */
	
(*l_range).min=L_MIN;
(*l_range).max=L_MAX;
(*b_range).min=B_MIN;
(*b_range).max=B_MAX;
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
