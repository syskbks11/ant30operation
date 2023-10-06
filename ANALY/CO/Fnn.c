#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include 	<malloc.h>
#include        <profhead.h>
#include        <starlib.h>
#include	<fitshead.h>


#define	PLNK	6.62559e-27
#define	BLTM	1.38054e-16
#define	TBB	2.73


double  J(T,nu)
double  T, nu;
{
double  J, c;
c=PLNK/BLTM*(nu*1.0e9);
J=c/(exp(c/T)-1.0);
return(J);
}





main(argc,argv)
int	argc;
char	*argv[];
{
FILE	*fp_in1, *fp_in2, *fp_out;
int	ii, jj;
int	err_code;
int	data_num, data_num2;
FITS	fits1, fits2;
char	in_filename1[MAX_CHAR_LENGTH], in_filename2[MAX_CHAR_LENGTH];
char	out_filename[MAX_CHAR_LENGTH];
char	filehead[MAX_CHAR_LENGTH];
char	buffer[MAX_CHAR_LENGTH];
char	*token;
double	abu, tau, tex, nu1, nu2, ncol, rat;
double	c1, c2;
double	rms, rms2;
double	Nco, Nci;
float	data_float;



/*	Default values	*/

fprintf(stderr,"IN_FITS(CO),IN_FITS(CI) -> OUT_TEXT(Nco-Nci)\n");
fprintf(stderr,"Input FITS filename(CO) : ");
fscanf(stdin,"%s",buffer);
if((strstr(buffer,"fits")==NULL)&&(strstr(buffer,"FITS")==NULL)){	
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

fprintf(stderr,"Input FITS filename(CI) : ");
fscanf(stdin,"%s",buffer);
if((strstr(buffer,"fits")==NULL)&&(strstr(buffer,"FITS")==NULL)){	
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

fprintf(stderr,"Excitation Temperature [K] = ");
fscanf(stdin,"%lf",&tex);

fprintf(stderr,"Frequency(1) [GHz] = ");
fscanf(stdin,"%lf",&nu1);

/* fprintf(stderr,"Frequency(2) [GHz] = "); 
fscanf(stdin,"%lf",&nu2); */
nu2 = 492.160651;


/*fp_out=stdout;*/
fprintf(stderr,"Output TEXT filename : ");
fscanf(stdin,"%s",buffer);
if((strstr(buffer,"txt")==NULL)&&(strstr(buffer,"TXT")==NULL)){	
	sprintf(out_filename,"%s.TXT",buffer);
	strcpy(filehead,buffer);
	fp_out=fopen(out_filename,"w");
	if(fp_out==NULL){
		sprintf(out_filename,"%s.txt",filehead);
		fp_out=fopen(out_filename,"w");
		if(fp_out==NULL){
			fprintf(stderr,"Can't open %s !\n",out_filename);
			exit(1);
		}
	}
}
else{	strncpy(filehead,buffer,strlen(buffer)-4);
	sprintf(out_filename,"%s",buffer);
	fp_out=fopen(out_filename,"w");
	if(fp_out==NULL){
		fprintf(stderr,"Can't open %s !\n",out_filename);
		exit(1);
	}
}


/*	Reading input FITS files	*/
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





/*	Calculate data numbers	*/
data_num=1;
for(jj=1;jj<=fits1.head.naxis[0];jj++){
	data_num = data_num * fits1.head.naxis[jj];
}
data_num2=1;
for(jj=1;jj<=fits2.head.naxis[0];jj++){
	data_num2 = data_num2 * fits2.head.naxis[jj];
}
if(data_num!=data_num2){
	fprintf(stderr,"FITS files must have the same matrix. (%d!=%d)\n",data_num,data_num2);
	exit(0);
}


{double	x,x2; int n;
n=0; x=0.0; x2=0.0;
for(ii=0;ii<data_num;ii++){
	if((fits1.data[ii]<0.0)&&(Blank(fits1.data[ii])==0)){
		x=x+fits1.data[ii]; x2=x2+pow(fits1.data[ii],2.0); n++;}
}
rms=pow(x2/(double)n,0.5);
n=0; x=0.0; x2=0.0;
for(ii=0;ii<data_num2;ii++){
	if((fits2.data[ii]<0.0)&&(Blank(fits2.data[ii])==0)){
		x=x+fits2.data[ii]; x2=x2+pow(fits2.data[ii],2.0); n++;}
}
rms2=pow(x2/(double)n,0.5);
fprintf(stderr,"RMS of data(1) = %lf\n",rms);
fprintf(stderr,"RMS of data(2) = %lf\n",rms2);}

c1=PLNK/BLTM*(nu1*1.0e9);
c2=PLNK/BLTM*(nu2*1.0e9);


/*	Calculate New Data 	*/
for(ii=0;ii<data_num;ii++){
	if( /*(fits1.data[ii]<=(rms*5.0))||(fits2.data[ii]<=(rms2*5.0))||*/
	(Blank(fits1.data[ii])==(-1))||(Blank(fits2.data[ii])==(-1)))	
		continue;
	else{		
		Nco = 5.557e17*tex/pow(nu1,2.0)*
			exp((double)(2)*PLNK*nu1*1.0e9/(2.0*BLTM*tex))
        		*fits1.data[ii]/(1.0-(J(TBB,nu1)/J(tex,nu1)));
		Nci = 1.98e15*(exp(23.6/tex)+3.0+5.0*exp(-38.9/tex))
       			*fits2.data[ii]/(1.0-(J(TBB,nu2)/J(tex,nu2)) );
		fprintf(fp_out,"%e\t%e\n",Nco,Nci);
	}
}



/*	Output TEXT file	*/
fprintf(stderr,"Writing to %s\n",out_filename);

fclose(fp_out);

}	/*	end of the main	function	*/



