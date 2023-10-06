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



fp_in=stdin;
fp_out=stdout;

fprintf(stderr,"Reading the FITS file.\n");
err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading the FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}


fprintf(stderr,"Writing the FITS file.\n");
err_code=output_Fits(&fits, fp_out);
if(err_code!=0){
	fprintf(stderr,
	"Error in writing FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}




}	/*main*/


