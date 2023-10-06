/*
1993/9/1	Tomoharu OKA	:	Coding ŠJŽn
1993/9/2	Tomoharu OKA	:	Ver.1.00 Š®¬
1994/9/7	Tomoharu OKA	:	Bug fix for pixel inner structure
1995/6/20	Tomoharu OKA	:	FITS -> MATRIX
1997/10/30	Tomo OKA	:	JCMT FITS -> Normal FITS
1998/3/19	Tomo OKA	:	CfA FITS -> Normal FITS
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
FITS		fits;
int		offset;
char		in_filename[80];
char		out_filename[80];
char		filehead[80];
char		buffer[80];
char		*token;
char		command[80];



if(argc<2){
	fprintf(stderr,"Usage: FJtoF (filename)\n");
	exit(1);
}


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
	sprintf(in_filename,"%s",buffer);
	fp_in=fopen(in_filename,"r");
	if(fp_in==NULL){
		fprintf(stderr,"Can't open %s !\n",in_filename);
		exit(1);
	}
}

err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,"Error in reading the FITS header. Error code=%d\n",err_code);
	exit(999);
}
fclose(fp_in);

sprintf(command,"cp %s %s.bak\n",in_filename,in_filename);
system(command);


err_code=correct_Fits(&fits);

strcpy(out_filename,in_filename);
fp_out=fopen(out_filename,"w");
if(fp_out==NULL){
	fprintf(stderr,"Can't open %s !\n",out_filename);
	exit(2);
}
err_code=output_Fits(&fits, fp_out);
if(err_code!=0){
	fprintf(stderr,"Error in outputing the Matrix data. Error code=%d\n",err_code);
	exit(999);
}
fclose(fp_out);



}	/*main*/



int	correct_Fits(fits)
FITS	*fits;
{
int	i, j, err_code;
long	pos, pos_new;
FITS	fits_new;
char	buffer[MAX_CHAR_NUMBER];
int	pix[MAX_DIMENSION], pix_new[MAX_DIMENSION];


err_code=copy_Fits(fits,&fits_new);

fits_new.head.naxis[1]=(*fits).head.naxis[3];
fits_new.head.naxis[3]=(*fits).head.naxis[1];
fits_new.head.crval[1]=(*fits).head.crval[3];
fits_new.head.crval[2]=0.0;
fits_new.head.crval[3]=(*fits).head.crval[1]/1.0e3;
fits_new.head.cdelt[1]=(-1.0)*(*fits).head.cdelt[3];
fits_new.head.cdelt[3]=(*fits).head.cdelt[1]/1.0e3;
fits_new.head.crpix[1]=(double)(1+(*fits).head.naxis[3])-(*fits).head.crpix[3];
fits_new.head.crpix[2]=((*fits).head.crpix[2]-(*fits).head.crval[2]/(*fits).head.cdelt[2]);
fits_new.head.crpix[3]=(*fits).head.crpix[1];

fits_new.head.crpix[1]=(fits_new.head.crpix[1]-fits_new.head.crval[1]/fits_new.head.cdelt[1]);
fits_new.head.crval[1]=0.0;

strcpy(fits_new.head.ctype[1],(*fits).head.ctype[3]);
strcpy(fits_new.head.ctype[3],(*fits).head.ctype[1]);



for(pos=0;pos<(*fits).head.data_num;pos++){
	err_code=POStoPIX(fits,(long)pos,pix);
	pix_new[0]=fits_new.head.naxis[1]-pix[2]-1; 
	pix_new[1]=pix[1]; 
	pix_new[2]=pix[0];
	pos_new=PIXtoPOS(&fits_new,pix_new);
	fits_new.data[pos_new]=(*fits).data[pos];
}

err_code=copy_Fits(&fits_new,fits);

return(0);
}



