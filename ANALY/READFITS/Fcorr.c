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




main(argc,argv)
int	argc;
char	*argv[];
{
FILE		*fp_in1, *fp_in2, *fp_out;
int		err_code;
XY_POS		position;
FITS		fits1, fits2, fitso;
char		in_filename1[80], in_filename2[80];
char		out_filename[80];
char		filehead[80];
char		buffer[80];
char		*token;
int		mode;




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


sprintf(out_filename,"%s.corr.FITS",filehead);
fp_out=fopen(out_filename,"w");
if(fp_out==NULL){
	fprintf(stderr,"Can't open %s !\n",out_filename);
	exit(1);
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


err_code=CalculateCorreation(&fits1, &fits2, &fitso);
if(err_code!=0){
	fprintf(stderr,"Error in calculating crrelation function. Error code=%d\n",err_code);
	exit(999);
}

fprintf(stderr,"Wrinting to %s\n",out_filename);
err_code=output_Fits(fitso,fp_out);
if(err_code!=0){
	fprintf(stderr,
	"Error in writing the FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}
fclose(fp_out);

}	/*main*/





int CalculateCorreation(fits1, fits2, fitso)
FITS		*fits1, *fits2, *fitso;
{
int	i, j, ii, jj, kk, flag;
int	pos1, pos2, poso;
int	data_num1, data_num2, data_num;
int	data_point;
int	pix1[MAX_DIMENSION], pix2[MAX_DIMENSION], pixo[MAX_DIMENSION];
double	x1[MAX_DIMENSION], x2[MAX_DIMENSION], dx[MAX_DIMENSION];
double	data1, data2, datao, integ;


data_num1=1.0;
for(jj=1;jj<=(*fits1).head.naxis[0];jj++){
	data_num1=data_num1*(*fits1).head.naxis[jj];
}
data_num2=1.0;
for(jj=1;jj<=(*fits2).head.naxis[0];jj++){
 	data_num2=data_num2*(*fits2).head.naxis[jj];
}

copy_FitsHeader(fits1, fitso);
for(jj=1;jj<=(*fitso).head.naxis[0];jj++){
	(*fitso).head.naxis[jj]=(*fits1).head.naxis[jj]/2;
	(*fitso).head.cdelt[jj]=(*fits1).head.cdelt[jj];
	(*fitso).head.crval[jj]=0.0;
	(*fitso).head.crpix[jj]=(*fitso).head.naxis[jj]/2;
}

data_num=1;
for(jj=1;jj<=(*fitso).head.naxis[0];jj++){
	data_num=data_num*(*fitso).head.naxis[jj];
}
(*fitso).data=calloc(data_num,4);

fprintf(stderr,"data_num1=%d data_num2=%d data_num=%d\n",data_num1,data_num2,data_num); 

integ = 0.0;
for(pos2=0;pos2<data_num2;pos2++){
	data2=(double)(*fits2).data[pos2];
	integ = integ + data2*data2;
}

for(poso=0;poso<data_num;poso++){
	POStoPIX(fitso,poso,pixo);
	PIXtoX(fitso,dx,pixo);
	datao=0.0;	data_point=0;
	for(pos2=0;pos2<data_num2;pos2++){
		data2=(*fits2).data[pos2];
		POStoPIX(fits2,pos2,pix2);
		PIXtoX(fits2,x2,pix2);
		for(jj=0;jj<(*fits2).head.naxis[0];jj++){
			x1[jj] = x2[jj] + dx[jj];
		}
		XtoPIX(fits1,x1,pix1);
		flag=0;
		for(jj=1;jj<=(*fits1).head.naxis[0];jj++){
			if((pix1[jj-1]<0)||(pix1[jj-1]>=(*fits1).head.naxis[jj]))	flag++;
		}
		if(flag!=0)	continue;

		pos1=PIXtoPOS(fits1,pix1);

		if( (Blank((*fits1).data[pos1])==(-1) ) ||
			(Blank((*fits2).data[pos2])==(-1) ) )	continue;

		data1=(double)(*fits1).data[pos1];
		datao = datao + data1*data2/integ;
		data_point++;
	}
	if(data_point>0)	(*fitso).data[poso] = (float)datao; 
	else			(*fitso).data[poso] = 0.0;

if(pixo[0]==0)	fprintf(stderr,"%d\t",poso);
}
fprintf(stderr,"\n");

return(0);

}



