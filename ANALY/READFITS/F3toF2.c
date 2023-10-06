/*
CfA CO(J=1-0) FITS file reader

1993/9/1	Tomoharu OKA	:	Coding ŠJŽn
1993/9/2	Tomoharu OKA	:	Ver.1.00 Š®¬
1994/9/7	Tomoharu OKA	:	Bug fix for pixel inner structure
1995/6/20	Tomoharu OKA	:	FITS -> COLOMNS
1995/9/23	Tomoharu OKA	:	FITS files -> Matrix
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0


main(argc,argv)
int	argc;
char	*argv[];
{
FILE		*fp_in, *fp_out;
int		err_code;
static FITS	fits;
int		offset;
int		ii, jj;
int	slice, ch;
char	filehead[80];
char	out_filename[80];

fprintf(stderr,"Input FITS filename : ");
fscanf(stdin,"%s",filehead);
fp_in=open_Fits(filehead);
if(fp_in==NULL)	exit(1);



fprintf(stderr,"Reading FITS file.\n");
err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading the FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}

if(fits.head.naxis[0]<3){
	fprintf(stderr,"FITS dimension must be 3!\n");
	exit(2);
}

slice=3;
fprintf(stderr,"Slicing dimension (1-%d) : ", fits.head.naxis[0]);
fscanf(stdin,"%d",&slice);

ch=1;
fprintf(stderr,"Slicing channel (1-%d) : ", fits.head.naxis[slice]);
fscanf(stdin,"%d",&ch);


fprintf(stderr,"Slicing 3D FITS\n");
err_code=slice_Fits(&fits, slice, ch);
if(err_code!=0){
	fprintf(stderr,
	"Error in creating new FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}


sprintf(out_filename,"%s.#%d-%d.FITS",filehead,slice,ch);
fp_out=fopen(out_filename,"w");
if(fp_out==NULL){
	fprintf(stderr,"Can't open %s !\n",out_filename);
	exit(1);
}

fprintf(stderr,"Writing to %s\n",out_filename);
err_code=output_Fits(&fits, fp_out);
if(err_code!=0){
	fprintf(stderr,
	"Error in writing the FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}
fclose(fp_out);


}	/*main*/


