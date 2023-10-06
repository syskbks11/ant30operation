/*	Radial density distribution 	*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0
#define	RESTRICT	3.0
#define	GRID		3.0


int	option(argc,argv,crip)
int	argc;
char	*argv[];
double	*crip;
{
int	ii, jj, count;
char	*token;

/* default values */
*crip = 0.0;

for(ii=1;ii<argc;ii++){
	if(argv[ii][0]=='-'){
		if(strcmp(argv[ii],"-crip")==0){
			ii++;
			*crip=atof(argv[ii]);
		}
	}
}


return(0);
}




int	radial_distribution(fits, fp, crip)
FITS	*fits;
FILE	*fp;
double	crip;
{
int	ii, jj;
long	ch, n, pix[MAX_DIMENSION];
double	x[MAX_DIMENSION];
double	xm[MAX_DIMENSION], x2[MAX_DIMENSION];
double	r, dr, rr, integ;

for(ii=0;ii<2;ii++){xm[ii]=0.0;	x2[ii]=0.0;}
n=0;	integ=0.0;
for(pix[0]=0;pix[0]<=(*fits).head.naxis[1]-1;pix[0]++){
for(pix[1]=0;pix[1]<=(*fits).head.naxis[2]-1;pix[1]++){
	PIXtoTX(fits,x,pix);
	if((*fits).data[PIXtoPOS(fits,pix)]<crip)	continue;

	for(ii=0;ii<2;ii++){
		xm[ii]+=(*fits).data[PIXtoPOS(fits,pix)]*x[ii];	
		x2[ii]+=(*fits).data[PIXtoPOS(fits,pix)]*x[ii]*x[ii];
	}
	integ+=(*fits).data[PIXtoPOS(fits,pix)];

}}
for(ii=0;ii<2;ii++){
	xm[ii] /= integ;	
	x2[ii] /= integ;
}


r = 0.0;	
dr = fabs((*fits).head.cdelt[1]);
for(ii=0;ii<MAX_DATA_NUM;ii++){
	n=0;	integ=0.0;
	for(pix[0]=0;pix[0]<=(*fits).head.naxis[1]-1;pix[0]++){
	for(pix[1]=0;pix[1]<=(*fits).head.naxis[2]-1;pix[1]++){
		PIXtoTX(fits,x,pix);
		if((*fits).data[PIXtoPOS(fits,pix)]<crip)	continue;

		rr = pow(pow((x[0]-xm[0]),2.0)+pow((x[1]-xm[1]),2.0), 0.5);
		if((r<=rr)&&(rr<r+dr)){
			integ += (*fits).data[PIXtoPOS(fits,pix)];
			n++;
		}	
	}}
	if(n==0)	break;
	integ /= (double)n;
	fprintf(fp,"%lf\t%lf\t%lf\n",r+dr/2.0,integ,(double)n );
	r += dr;
}


return(0);
}







main(argc,argv)
int	argc;
char	*argv[];
{
FILE	*fp_in, *fp_out;
FITS	fits;
int	ii, jj, err_code;
double	crip;




err_code=option(argc,argv,&crip);
if(err_code!=0){
	fprintf(stderr,"Error in reading option switch. Error code=%d\n",err_code);	
	exit(999);
}

fp_in=stdin;
fp_out=stdout;

fprintf(stderr,"Reading the FITS file.\n");
err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading a FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}
for(jj=1;jj<=fits.head.naxis[0];jj++){
	if(fits.head.naxis[jj]<=1)	fits.head.naxis[0]--;	
}
if(fits.head.naxis[0]!=2){
	fprintf(stderr,"FITS dimension must be 2.\n");
	exit(990);
}


fprintf(stderr,"Writing a radial distribution.\n");
err_code=radial_distribution(&fits, fp_out, crip);
if(err_code!=0){
	fprintf(stderr,
	"Error in writing to output file. Error code=%d\n"
	,err_code);	
	exit(999);
}


}	/*main*/





