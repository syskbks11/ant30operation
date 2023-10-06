#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include 	<stdlib.h>

#include        <profhead.h>
#include        <starlib.h>
#include	<fitshead.h>


#define		T_STEP	2.0
#define		T_MIN	0
#define		T_MAX	20.0


int options(argc,argv,t_step,t_range,scale)
int	argc;
char	*argv[];
double	*t_step;
RANGE	*t_range;
double	*scale;
{
	int ii,jj;
	int	err_code;
	char	*token;

/* default values */
	
(*t_range).min=T_MIN;
(*t_range).max=T_MAX;
*t_step=T_STEP;
*scale=1.0;


for(ii=1;ii<argc;ii++){
	if(argv[ii][0]=='-'){	/* option switch */	
		if(strcmp(argv[ii],"-step")==0){
			ii++;
			*t_step=atof(argv[ii]);
		}
		else if(strcmp(argv[ii],"-range")==0){
			if(ii<argc){	
				ii++;
				token = strtok(argv[ii],",");
				(*t_range).min = atof( token );
				token = strtok(NULL,",");
				(*t_range).max = atof( token );
			}
		}
		else if(strcmp(argv[ii],"-scale")==0){
			ii++;
			*scale=atof(argv[ii]);
		}
	}
}

STAR_SortRange(t_range);
return(0);

}	/*	end of options()	*/






main(argc,argv)
int	argc;
char	*argv[];
{
FILE	*fp_in, *fp_out;
int	ii, jj;
int	err_code;
double	step;
RANGE	range, intensity;
double	scale;
int	column;
int	data_num;
FITS	fits;
long	*hist_pix;
double	*hist_int;
char	in_filename[80], out_filename[80];
char	filehead[80];
char	buffer[80], *token;


/*fp_in=stdin;*/
fprintf(stderr,"Input FITS filename : ");
fscanf(stdin,"%s",buffer);
if((strstr(buffer,".fits")==NULL)&&(strstr(buffer,".FITS")==NULL)){	
	sprintf(in_filename,"%s.FITS",buffer);
	strcpy(filehead,buffer);
	fp_in=fopen(in_filename,"r");
	if(fp_in==NULL){
		sprintf(in_filename,"%s.fits",filehead);
		fp_in=fopen(in_filename,"r");
		if(fp_in==NULL){
			fprintf(stderr,"Can't open %s !\n",in_filename);
			exit(1);
		}
	}
}
else{	strncpy(filehead,buffer,strlen(buffer)-5);
	sprintf(in_filename,"%s",buffer);
	fp_in=fopen(in_filename,"r");
	if(fp_in==NULL){
		fprintf(stderr,"Can't open %s !\n",in_filename);
		exit(1);
	}
}

fp_out=stdout;
scale=1.0;

/*err_code=options(argc,argv,&step,&range,&scale);*/

fprintf(stderr,"Scaling factor: ");
fscanf(stdin,"%lf",&scale);

again:;
fprintf(stderr,"Intensity range (min,max): ");
fscanf(stdin,"%s",buffer);
token=strtok(buffer,", \t\n");
if(token!=NULL)	range.min=atof(token);
else goto again;
token=strtok(NULL,",");		
if(token!=NULL)	range.max=atof(token);
else goto again;
if(range.min>range.max){double tmp=range.min; range.min=range.max; range.max=range.min;}
//	STAR_SortRange(range);

fprintf(stderr,"Histogram step: ");
fscanf(stdin,"%lf",&step);




column=(int)((range.max-range.min)/step);
hist_pix=(long *)calloc(column, 4);
hist_int=(double *)calloc(column, 8);


/*	Read FITS File from fp_in	*/
err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,"Error in reading the FITS file. Error code=%d\n",err_code);
	exit(999);
}

data_num=1;
for(jj=1;jj<=fits.head.naxis[0];jj++){
	data_num = data_num * fits.head.naxis[jj];
}

/*	Initialize Histogram Array	*/
for(jj=0;jj<column;jj++){
	hist_pix[jj]=0;
	hist_int[jj]=0.0;
}

/*	Create Histogram Array	*/
{
float	data_float;
for(ii=0;ii<data_num;ii++){
	data_float=fits.data[ii]*scale;
	for(jj=0;jj<column;jj++){
		intensity.min = range.min + (double)jj*step;
		intensity.max = intensity.min + step;	
		if((data_float>=intensity.min)&&(data_float<intensity.max)){
			hist_pix[jj]++;
			hist_int[jj] = hist_int[jj] + fits.data[ii];
		}
	}
}
}


/*	Output the Histogram Array	*/
for(jj=0;jj<column;jj++){
	intensity.min = range.min + (double)jj*step;
	intensity.max = intensity.min + step;			
	fprintf(fp_out,"%lf\t%lf\t%ld\t%lf\n",
		intensity.min,intensity.max,hist_pix[jj],hist_int[jj]);
}


}	/*	end of the main	function	*/



