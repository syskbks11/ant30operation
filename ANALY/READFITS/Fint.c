/*	Velocity integration of a FITS DATA 	*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0




int	integ_Fits(fits, fp_out)
FITS	*fits;
FILE	*fp_out;
{
int	ii;
long	ch, n,  pix[MAX_DIMENSION];
double	w, x, x2, rms;




w=0.0; n=0; x=0.0; x2=0.0;
for(ch=0;ch<(*fits).head.data_num;ch++){
	if(Blank((*fits).data[ch])==(-1)){
		continue;
	}
	else{
		if((*fits).data[ch]<0.0){x=x+(*fits).data[ch]; x2=x2+pow((*fits).data[ch],2.0); n++;}
		w=w+(double)(*fits).data[ch];
	}
}

rms=pow(x2/(double)n,0.5);

for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	fprintf(fp_out,"CDELT(%d) =  %lf \n",ii,(*fits).head.cdelt[ii]);
}
fprintf(fp_out,"Integrated Intensity: %lf \n",w);
fprintf(fp_out,"RMS noise: %lf \n",rms);

return(0);
}







main(argc,argv)
int	argc;
char	*argv[];
{
FILE		*fp_in, *fp_out;
int		err_code;
FITS		fits, fits_new;
int		ii, jj;
int		dim;
RANGE		v;
char		in_filename[80];
char		filehead[80];
char		buffer[80];
char		*token;


if(argc<2){
	fprintf(stderr,"Usage: Fint (filename)\n");
	exit(1);
}

/*fp_in=stdin;*/
strcpy(buffer,argv[1]);
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
	filehead[strlen(buffer)-5]='\0';
	sprintf(in_filename,"%s",buffer);
	fp_in=fopen(in_filename,"r");
	if(fp_in==NULL){
		fprintf(stderr,"Can't open %s !\n",in_filename);
		exit(1);
	}
}fp_out=stdout;

err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading the FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}


err_code=integ_Fits(&fits, fp_out);
if(err_code!=0){
	fprintf(stderr,
	"Error in calculating the total integrated intensity. Error code=%d\n"
	,err_code);	
	exit(999);
}


}	/*main*/


