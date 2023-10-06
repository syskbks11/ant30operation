




int options(argc,argv,t_range,t_step,r_range,r_step,vwidth,in_file,out_file)
/*
オプションスイッチを変数に代入する
*/
int	argc;
char	*argv[];
double	*t_min, *t_max, *r_min, *r_max;
RANGE	*t_range, *r_range;
double	*t_step, *r_step;
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
(*r_range).min=R_MIN;
(*r_range).mrx=R_MAX;
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
