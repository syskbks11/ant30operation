/*
FITS range viewer

1993/9/1	Tomoharu OKA	:	Coding Jn
1993/9/2	Tomoharu OKA	:	Ver.1.00 
1994/9/7	Tomoharu OKA	:	Bug fix for pixel inner structure
1995/6/20	Tomoharu OKA	:	FITS -> COLOMNS
1997/9/18	Tomoharu OKA	:	Print xy range
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0

#ifdef	_NO_PROTO
extern int read_FitsHeader();
extern int divide_Line();
extern int rm_Quot();
extern int get_MatrixData();
extern int OutputMatrixData();
#else
extern int read_FitsHeader(FITS_HEAD *, int *, AXIS_INDEX *, FILE *);
extern int divide_Line(char *, char *, char *, char *);
extern int rm_Quot(char *, char *);
extern int get_MatrixData(FITS_HEAD *, int, MATRIX *, AXIS_INDEX *, FILE *);
extern int OutputMatrixData(MATRIX *, FILE *);
#endif


int	me;
char	filename[80];

int	option(argc,argv)
int	argc;
char	*argv[];
{
int	ii, jj, count;
char	*token;

/* default values */
me=0;

for(ii=1;ii<argc;ii++){
	if(argv[ii][0]=='-'){
		if(strcmp(argv[ii],"-me")==0){
			me=1;
		}
	}
	else{	strcpy(filename,argv[ii]);
	}
}

return(0);
}




main(argc,argv)
int	argc;
char	*argv[];
{
FILE		*fp_in, *fp_out;
int		err_code;
AXIS_INDEX	axis;
XY_POS		position;
FITS		fits;
int		offset;
char		in_filename[80];
char		filehead[80];
char		buffer[80];
char		*token;

/*	t@CI[v	*/

if(argc<2){
	fprintf(stderr,"Usage: Fxy (filename)\n");
	exit(1);
}
option(argc,argv);
/*fp_in=stdin;*/
strcpy(buffer,filename);
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


err_code=read_FitsHeader(&(fits.head),&offset,&axis,fp_in);
/*err_code=read_Fits(&(fits.head), fp_in);*/
if(err_code!=0){
	fprintf(stderr,
	"Error in reading a FITS. Error code=%d\n"
	,err_code);	
	exit(999);
}
fclose(fp_in);


err_code=OutputXYrange(&fits,fp_out);
if(err_code!=0){
	fprintf(stderr,"Error in outputing the Matrix data. Error code=%d\n",err_code);
	exit(999);
}


}	/*main*/



int OutputXYrange(fits,fp_out)
FITS	*fits;
FILE		*fp_out;
{
int	ii, jj;
int	ra, dec;
int	min[MAX_DIMENSION], max[MAX_DIMENSION];
double	xmin[MAX_DIMENSION], xmax[MAX_DIMENSION], tmp;

for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	min[ii-1]=0;	max[ii-1]=(*fits).head.naxis[ii]-1;
}
if(me==1){	PIXtoX(fits,xmin,min);
	PIXtoX(fits,xmax,max);}
else{	PIXtoTX(fits,xmin,min);
	PIXtoTX(fits,xmax,max);}

for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	fprintf(stdout,"ch#%d: (1,%d)\t%s: (%lf,%lf):\t%lf\n", 
		ii, (*fits).head.naxis[ii], (*fits).head.ctype[ii], 
		xmin[ii-1], xmax[ii-1], (*fits).head.cdelt[ii]);
}
return(0);

}
	




