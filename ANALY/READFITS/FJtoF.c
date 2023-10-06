/*
1993/9/1	Tomoharu OKA	:	Coding 開始
1993/9/2	Tomoharu OKA	:	Ver.1.00 完成
1994/9/7	Tomoharu OKA	:	Bug fix for pixel inner structure
1995/6/20	Tomoharu OKA	:	FITS -> MATRIX
1997/10/30	Tomo OKA	:	JCMT FITS -> Normal FITS
1998/12/23	Tomo OKA	:	W44 JCMT FITS -> Normal FITS
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0


void	options(argc,argv,in_filename,mode)
int	argc;
char	*argv[];
char	in_filename[];
int	*mode;
{
int ii,jj;
int	err_code;
char	*token;

/* default values */
	
*mode=0;

/* interpret options */
for(ii=1;ii<argc;ii++){
	if(argv[ii][0]=='-'){
		if((strstr(argv[ii],"-GC")!=NULL)||(strstr(argv[ii],"-gc")!=NULL)){
			*mode=0;
		}
		else if((strstr(argv[ii],"-w44")!=NULL)||(strstr(argv[ii],"-W44")!=NULL)){	
			*mode=1;
		}
		else{
			*mode=0;
		}
	}
	else	strcpy(in_filename,argv[ii]);
}

}





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
int		mode;


/*	入力ファイルのオープン	*/

if(argc<2){
	fprintf(stderr,"Usage: FJtoF [option:-GC/-W44] (filename)\n");
	exit(1);
}

options(argc,argv,in_filename,&mode);


/*fp_in=stdin;*/
strcpy(buffer,in_filename);
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


/*	Matrix data部分の構成	*/
err_code=correct_Fits(&fits,mode);



/*fp_out=stdout;*/
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



int	correct_Fits(fits,mode)
FITS	*fits;
int	mode;
{
XY_POS	radec, lb;
double	v, dv, f, df;
int	ii, jj;
int     pix[MAX_DIMENSION], pixn[MAX_DIMENSION];
long    data_num, pos;
FITS	fits_new;


data_num=1;
for(jj=1;jj<=(*fits).head.naxis[0];jj++){
	data_num=data_num*(*fits).head.naxis[jj];
}


if(mode==0){	/*	mode==0	: GC and other cases	*/
radec.x=(*fits).head.crval[1];
radec.y=(*fits).head.crval[2];

lb_radec(&radec,&lb,1,1);
/* RD2LB(&radec,1,1); */

if(lb.x>270.0)	lb.x=lb.x-360.0;

(*fits).head.crval[2]=lb.x;
(*fits).head.crval[1]=lb.y;

(*fits).head.cdelt[1]=(-1.0)*(*fits).head.cdelt[1];

(*fits).head.ctype[1]=malloc(8);
(*fits).head.ctype[2]=malloc(8);
strcpy((*fits).head.ctype[2],"\'GLON\'");
strcpy((*fits).head.ctype[1],"\'GLAT\'");

/*	flip x<->y coordinates	*/
copy_Fits(fits,&fits_new);

fits_new.head.naxis[1]=(*fits).head.naxis[2];
fits_new.head.crval[1]=(*fits).head.crval[2];
fits_new.head.crpix[1]=(*fits).head.crpix[2];
fits_new.head.cdelt[1]=(*fits).head.cdelt[2];
fits_new.head.crota[1]=(*fits).head.crota[2];
fits_new.head.ctype[1]=malloc(strlen((*fits).head.ctype[2])+1);
strcpy(fits_new.head.ctype[1],(*fits).head.ctype[2]);
fits_new.head.naxis[2]=(*fits).head.naxis[1];
fits_new.head.crval[2]=(*fits).head.crval[1];
fits_new.head.crpix[2]=(*fits).head.crpix[1];
fits_new.head.cdelt[2]=(*fits).head.cdelt[1];
fits_new.head.crota[2]=(*fits).head.crota[1];
fits_new.head.ctype[2]=malloc(strlen((*fits).head.ctype[1])+1);
strcpy(fits_new.head.ctype[2],(*fits).head.ctype[1]);	

for(ii=0;ii<data_num;ii++){
	POStoPIX(fits,ii,pix);
	pixn[0]=pix[1];
	pixn[1]=pix[0];
	for(jj=2;jj<=(*fits).head.naxis[0];jj++){
		pixn[jj]=pix[jj];
	}
	pos=PIXtoPOS(&fits_new,pixn);
	fits_new.data[pos]=(*fits).data[ii];
}

copy_Fits(&fits_new,fits);

}	/*	mode==0	: GC and other cases	*/


else if(mode==1){	/*	mode==1	: W44 case	*/
radec.x=(*fits).head.crval[1];
radec.y=(*fits).head.crval[2];

lb_radec(&radec,&lb,1,1);
if(lb.x>270.0)	lb.x=lb.x-360.0;

(*fits).head.crval[1]=lb.x;
(*fits).head.crval[2]=lb.y;

strcpy((*fits).head.ctype[1],"\'GLON\'");
strcpy((*fits).head.ctype[2],"\'GLAT\'");
}	/*	mode==1	: W44 case	*/





if((*fits).head.naxis[0]==3){
if(mode==1){	
/*	f=345.795989e9;
	(*fits).head.crpix[3]=(*fits).head.crpix[3]
		+(f-(*fits).head.crval[3])/(*fits).head.cdelt[3];	*/
	(*fits).head.crpix[3]=(double)(*fits).head.naxis[3]/2.0 + 0.5;
	(*fits).head.crval[3]=37.1547;
	(*fits).head.cdelt[3]=(-0.54178987805156);
}
else{
	f=(*fits).head.crval[3];
	df=(*fits).head.cdelt[3];
	v=0.0;
	dv=(-1.0)*df/f*2.9979246e5;
	(*fits).head.crval[3]=v;
	(*fits).head.cdelt[3]=dv;
}
	(*fits).head.ctype[3]=malloc(8);
	strcpy((*fits).head.ctype[3],"\'V-LSR\'");
}




(*fits).head.blank=DEFAULT_BLANK_VALUE;
for(ii=0;ii<data_num;ii++){	
	if(((*fits).data[ii]<(*fits).head.datamin)||((*fits).data[ii]>(*fits).head.datamax))
		(*fits).data[ii]=DEFAULT_BLANK_VALUE;
}





return(0);
}



