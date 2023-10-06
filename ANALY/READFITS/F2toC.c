/*
1993/9/1	Tomoharu OKA	:	Coding Jn
1993/9/2	Tomoharu OKA	:	Ver.1.00 
1994/9/7	Tomoharu OKA	:	Bug fix for pixel inner structure
1995/6/20	Tomoharu OKA	:	FITS -> COLOMNS
1998/3/3	Tomoharu OKA	:	2 FITS -> COLOMNS
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0


void	options(argc,argv,mode)
int	argc;
char	*argv[];
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
		if(strstr(argv[ii],"ax")!=NULL){
			*mode=1;
		}
		else{
			*mode=0;
		}
	}
}
}



main(argc,argv)
int	argc;
char	*argv[];
{
FILE		*fp_in1, *fp_in2, *fp_out;
int		err_code;
XY_POS		position;
FITS		fits1, fits2;
char		in_filename1[80], in_filename2[80];
char		out_filename[80];
char		filehead[80];
char		buffer[80];
char		*token;
int		mode;


options(argc,argv,&mode);


/*	t@CI[v	*/

/*fp_in=stdin;*/

fprintf(stderr,"Input FITS filename(1) : ");
fscanf(stdin,"%s",buffer);
if((strstr(buffer,".fits")==NULL)&&(strstr(buffer,".FITS")==NULL)){	
	sprintf(in_filename1,"%s.FITS",buffer);
	strcpy(filehead,buffer);
	fp_in1=fopen(in_filename1,"r");
	if(fp_in1==NULL){
		sprintf(in_filename1,"%s.fits",filehead);
		fp_in1=fopen(in_filename1,"r");
		if(fp_in1==NULL){
			fprintf(stderr,"Can't open %s !\n",in_filename1);
			exit(1);
		}
	}
}
else{	strncpy(filehead,buffer,strlen(buffer)-5);
	sprintf(in_filename1,"%s",buffer);
	fp_in1=fopen(in_filename1,"r");
	if(fp_in1==NULL){
		fprintf(stderr,"Can't open %s !\n",in_filename1);
		exit(1);
	}
}

fprintf(stderr,"Input FITS filename(2) : ");
fscanf(stdin,"%s",buffer);
if((strstr(buffer,".fits")==NULL)&&(strstr(buffer,".FITS")==NULL)){	
	sprintf(in_filename2,"%s.FITS",buffer);
	strcpy(filehead,buffer);
	fp_in2=fopen(in_filename2,"r");
	if(fp_in2==NULL){
		sprintf(in_filename2,"%s.fits",filehead);
		fp_in2=fopen(in_filename2,"r");
		if(fp_in2==NULL){
			fprintf(stderr,"Can't open %s !\n",in_filename2);
			exit(1);
		}
	}
}
else{	strncpy(filehead,buffer,strlen(buffer)-5);
	sprintf(in_filename2,"%s",buffer);
	fp_in2=fopen(in_filename2,"r");
	if(fp_in2==NULL){
		fprintf(stderr,"Can't open %s !\n",in_filename2);
		exit(1);
	}
}


fprintf(stderr,"Reading %s\n",in_filename1);
err_code=read_Fits(&fits1, fp_in1);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading %s. Error code=%d\n"
	,err_code,in_filename1);	
	exit(999);
}
fclose(fp_in1);
fprintf(stderr,"Reading %s\n",in_filename2);
err_code=read_Fits(&fits2, fp_in2);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading %s. Error code=%d\n"
	,err_code,in_filename2);	
	exit(999);
}
fclose(fp_in2);



fp_out=stdout;
err_code=Output2ColumnData(&fits1, &fits2, fp_out, mode);
if(err_code!=0){
	fprintf(stderr,"Error in writing a column data. Error code=%d\n",err_code);
	exit(999);
}


}	/*main*/





int Output2ColumnData(fits1, fits2, fp_out, mode)
FITS		*fits1, *fits2;
FILE		*fp_out;
int		mode;
{
int	i, j, ii, jj, kk;
int	data_num;
int	ch[MAX_DIMENSION];
double	x[MAX_DIMENSION];
double	declination;
int	ra, dec;


for(i=1;i<=(*fits1).head.naxis[0];i++){
	if((*fits1).head.naxis[i]!=(*fits2).head.naxis[i]){
		fprintf(stderr,"FITS matrices must be the same (%d).\n",i);
		return(1);
	}
}

ra=(-1);	dec=(-1);
for(ii=1;ii<=(*fits1).head.naxis[0];ii++){
	if((strstr((*fits1).head.ctype[ii],"RA")!=NULL)||
	(strstr((*fits1).head.ctype[ii],"GLON")!=NULL))	ra=ii;
	if((strstr((*fits1).head.ctype[ii],"DEC")!=NULL)||
	(strstr((*fits1).head.ctype[ii],"GLAT")!=NULL))	dec=ii;
}
if(dec==(-1))	declination=0.0;
else		declination=(*fits1).head.crval[dec];

data_num=1;
for(jj=1;jj<=(*fits1).head.naxis[0];jj++){
	data_num=data_num*(*fits1).head.naxis[jj];
}

for(ii=0;ii<data_num;ii++){
	if((Blank((*fits1).data[ii])==(-1))||(Blank((*fits2).data[ii])==(-1)))	
		continue;
	if(mode==0)
		fprintf(fp_out,"%lf\t%lf\n",(*fits1).data[ii],(*fits2).data[ii]);
	else {
		POStoPIX(fits1,ii,ch);
		for(jj=1;jj<=(*fits1).head.naxis[0];jj++){
			if(jj==ra){
			x[jj-1]=(*fits1).head.crval[jj] 
			+ (*fits1).head.cdelt[jj]/cos(declination*PI/180.0)
				*((double)(ch[jj-1]+1)-(*fits1).head.crpix[jj]);
			}
			else{
			x[jj-1]=(*fits1).head.crval[jj] 
			+ (*fits1).head.cdelt[jj]*((double)(ch[jj-1]+1)-(*fits1).head.crpix[jj]);
			}		
			fprintf(fp_out,"%lf\t",x[jj-1]);
		}

		fprintf(fp_out,"%lf\t%lf\n",(*fits1).data[ii],(*fits2).data[ii]);
	}
}

return(0);

}



