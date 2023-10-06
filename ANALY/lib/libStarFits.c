/*
Library for FITS file hundler

1993/9/1	Tomoharu OKA	:	Coding Jn
1993/9/2	Tomoharu OKA	:	Ver.1.00 
1994/9/7	Tomoharu OKA	:	Bug fix for pixel inner structure
1995/6/20	Tomoharu OKA	:	FITS -> COLOMNS
1995/9/23	Tomoharu OKA	:	FITS files -> Matrix
*/

#define	DEBUG 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <float.h>

#include <fitshead.h>
#include <astro.h>



FILE*	open_Fits(char	filehead[])
{
char		in_filename[80];
char		buffer[80];
char		*token;
FILE	*fp_in;

strcpy(buffer,filehead);
if((strstr(buffer,".fits")==NULL)&&(strstr(buffer,".FITS")==NULL)){	
	sprintf(in_filename,"%s.FITS",buffer);
	strcpy(filehead,buffer);
	fp_in=fopen(in_filename,"r");
	if(fp_in==NULL){
		sprintf(in_filename,"%s.fits",filehead);
		fp_in=fopen(in_filename,"r");
		if(fp_in==NULL){
		    fprintf(stderr,"Can't open %s !\n",in_filename);
			return(fp_in);
		}
	}
}
else{	strncpy(filehead,buffer,strlen(buffer)-5);
	filehead[strlen(buffer)-5]='\0';
	sprintf(in_filename,"%s",buffer);
	fp_in=fopen(in_filename,"r");
	if(fp_in==NULL){
		fprintf(stderr,"Can't open %s !\n",in_filename);
		return(fp_in);
	}
}

return(fp_in);
}







int	read_Fits(fits, fp)
FITS	*fits;
FILE	*fp;
{
int	ii, jj, kk;
int	offset;
AXIS_INDEX	axis;
int	err_code;

err_code=read_FitsHeader(&((*fits).head),&offset,&axis,fp);
if(err_code!=0){
	fprintf(stderr,"Error in reading FITS header!\n");
	fprintf(stderr,"error code=%d\n",err_code);
	return(101);
}

err_code=read_FitsData(fits,offset,&axis,fp);
if(err_code!=0){
	fprintf(stderr,"Error in reading FITS data!\n");
	fprintf(stderr,"error code=%d\n",err_code);
	return(201);
}

return(0);
}	



int	read_FitsHeader(fits_head,offset,axis,fp_in)
FITS_HEAD 	*fits_head;
int		*offset;
AXIS_INDEX	*axis;
FILE		*fp_in;
{
int	ii, jj, kk;
char 	line[82];
char 	keyword[10];
char 	value[82];
char	comment[82];
char	buffer[82];

/*	Default Values	*/

(*fits_head).naxis[0]=0;
(*fits_head).bscale=1.0;
(*fits_head).bzero=0.0;
(*fits_head).blank=DEFAULT_BLANK_VALUE;
strcpy((*fits_head).bunit,"\' \'");
strcpy((*fits_head).object,"\' \'");
strcpy((*fits_head).date_obs,"\' \'");
strcpy((*fits_head).date_map,"\' \'");
strcpy((*fits_head).origin,"\' \'");
strcpy((*fits_head).telescop,"\' \'");
strcpy((*fits_head).instrume,"\' \'");
strcpy((*fits_head).observer,"\' \'");

for(ii=1;ii<=MAX_DIMENSION;ii++){
	(*fits_head).crval[ii]=0.0;
	(*fits_head).crpix[ii]=0.0;
	(*fits_head).cdelt[ii]=0.0;
	(*fits_head).crota[ii]=0.0;
    	(*fits_head).ctype[ii]=NULL;
}


*offset=80;
while(1){
	fread(line,80,1,fp_in);
	line[80] = (char)NULL;

#if DEBUG
fprintf(stderr,"%s\n",line);
#endif
	divide_Line(line,keyword,value,comment);
#if DEBUG
fprintf(stderr,"%s(%d) %s(%d) : %s(%d)\n",
keyword,strlen(keyword),value,strlen(value),comment,strlen(comment));
#endif
	if(strncmp(keyword,"SIMPLE",6)==0){
		if(strncasecmp(value,"T",1)==0)		(*fits_head).simple=1;
		else if(strncasecmp(value,"F",1)==0)	(*fits_head).simple=0;
		else					(*fits_head).simple=0;
	}
	if(strncmp(keyword,"BITPIX",6)==0)
		(*fits_head).bitpix=(short)atoi(value);		
	if((strncmp(keyword,"NAXIS",5)==0)&&(strlen(keyword)==5)){
		(*fits_head).naxis[0]=(unsigned long)atol(value);	
	}
	for(ii=1;ii<=(*fits_head).naxis[0];ii++){
		sprintf(buffer,"%s%d","NAXIS",ii);
		if(strncmp(keyword,buffer,strlen(buffer))==0)
			(*fits_head).naxis[ii]=(unsigned long)atol(value);
	}
	if(strncmp(keyword,"BSCALE",6)==0)
		(*fits_head).bscale=(double)atof(value);
	if(strncmp(keyword,"BZERO",5)==0)
		(*fits_head).bzero=(double)atof(value);
	if(strncmp(keyword,"BUNIT",5)==0)
		strcpy((*fits_head).bunit,value);	
	if(strncmp(keyword,"BLANK",5)==0)
		(*fits_head).blank=(double)atof(value);
	if(strncmp(keyword,"OBJECT",6)==0)
		strcpy((*fits_head).object,value);	
	if(strncmp(keyword,"DATE-OBS",8)==0)
		strcpy((*fits_head).date_obs,value);	
	if(strncmp(keyword,"DATE-MAP",4)==0)
		strcpy((*fits_head).date_map,value);	
	if(strncmp(keyword,"ORIGIN",6)==0)
		strcpy((*fits_head).origin,value);	
	if(strncmp(keyword,"INSTRUME",8)==0)
		strcpy((*fits_head).instrume,value);	
	if(strncmp(keyword,"TELESCOP",8)==0)
		strcpy((*fits_head).telescop,value);	
	if(strncmp(keyword,"OBSERVER",8)==0)
		strcpy((*fits_head).observer,value);	
	for(ii=1;ii<=(*fits_head).naxis[0];ii++){
		sprintf(buffer,"%s%d","CRVAL",ii);
		if(strncmp(keyword,buffer,strlen(buffer))==0)
			(*fits_head).crval[ii]=(double)atof(value);
		sprintf(buffer,"%s%d","CRPIX",ii);
		if(strncmp(keyword,buffer,strlen(buffer))==0)
			(*fits_head).crpix[ii]=(double)atof(value);
		sprintf(buffer,"%s%d","CDELT",ii);
		if(strncmp(keyword,buffer,strlen(buffer))==0){
			(*fits_head).cdelt[ii]=(double)atof(value);
			(*fits_head).cd_mode=0;
		}
		for(jj=1;jj<=(*fits_head).naxis[0];jj++){
			sprintf(buffer,"%s%d_%d","CD",ii,jj);
			if(strncmp(keyword,buffer,strlen(buffer))==0){
				(*fits_head).cd[ii][jj]=(double)atof(value);
				(*fits_head).cd_mode=1;
			}
		}

		sprintf(buffer,"%s%d","CROTA",ii);
	    	if(strncmp(keyword,buffer,strlen(buffer))==0)
		    (*fits_head).crota[ii]=(double)atof(value);
		sprintf(buffer,"%s%d","CTYPE",ii);
		if(strncmp(keyword,buffer,strlen(buffer))==0){
			(*fits_head).ctype[ii]=malloc(strlen(value)+1);
			strcpy((*fits_head).ctype[ii],value);
		}
	}
	if(strncmp(keyword,"DATAMAX",7)==0)		
		(*fits_head).datamax=(double)atof(value);
	if(strncmp(keyword,"DATAMIN",7)==0)		
		(*fits_head).datamin=(double)atof(value);
	if((strncmp(keyword,"EQUINOX",7)==0)||(strncmp(keyword,"EPOCH",5)==0))		
		(*fits_head).epoch=(double)atof(value);
	if(strncmp(keyword,"END",3)==0){
#if DEBUG
	
	fprintf(stderr,"\n*****************************************\n");
#endif
		break;
	}
	*offset=(*offset)+80;
}	/*	while loop	*/


for(ii=1;ii<=(*fits_head).naxis[0];ii++){
	if((*fits_head).ctype[ii]==NULL)	continue;
	if(strncmp((*fits_head).ctype[ii],"VELO-LSR",8)==0)	
		(*axis).v=ii;
	else if(strncmp((*fits_head).ctype[ii],"GLAT",4)==0)	
		(*axis).y=ii;
	else if(strncmp((*fits_head).ctype[ii],"GLON",4)==0)	
		(*axis).x=ii;
}

/*
for(ii=1;ii<=(*fits_head).naxis[0];ii++){
	if(fabs((*fits_head).cdelt[ii])<1.0e-300){
		(*fits_head).crpix[ii]=0.0;
		(*fits_head).crval[ii]=0.0;
		(*fits_head).cdelt[ii]=1.0;
		sprintf((*fits_head).ctype[ii],"PIX%d",ii);
	}
}
*/



return(0);
}	/*	end of read_FitsHeader	*/






int	read_FitsData(fits,offset,axis,fp_in)
FITS 	*fits;
int		offset;
AXIS_INDEX	*axis;
FILE		*fp_in;
{
int	ii, jj;
int	data_bytepix;	/*	[byte]	*/
int	data_position;
int	header_record_length;
int	data_record_start;
int	err_code, blnk_flag;
int	data_number;
char	c;
short	data_short;
long	data_long;
float	data_float;
float	data_min;
int	byte;


data_bytepix=abs((*fits).head.bitpix)/8;	

header_record_length=(int)ceil((double)offset/((double)BIT_RECORD/8.0));
data_record_start=(header_record_length)*BIT_RECORD/8;
#if DEBUG
fprintf(stderr,"Header record length=%ld [record] (%ld [byte])\n"
		,header_record_length,offset);
fprintf(stderr,"Data record start position=%ld [Bytes]\n",data_record_start);
#endif


byte=0;
while(byte+offset<data_record_start){
	fread(&data_long,sizeof(data_long),1,fp_in);
	byte=byte+sizeof(data_long);
}

data_number=1;
for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	data_number=data_number*(*fits).head.naxis[ii];
}
(*fits).data=(float *)calloc(data_number,sizeof(data_float));
(*fits).head.data_num = data_number;
for(ii=0;ii<data_number;ii++){
	blnk_flag=0;
	if((*fits).head.bitpix==32){
		fread(&data_long,sizeof(data_long),1,fp_in);
		if(fabs(((double)data_long-(*fits).head.blank)/(double)data_long)<1.0e-5)		
/*		if(data_long==(long)((*fits).head.blank))	*/
			blnk_flag=1;
		data_float=(float)data_long;	
	}
	if((*fits).head.bitpix==(-32)){
		fread(&data_float,sizeof(data_float),1,fp_in);
/*		if(fabs(((double)data_float-(*fits).head.blank)/(double)data_float)<1.0e-5)*/
		if(data_float==(float)(*fits).head.blank)	
			blnk_flag=1;
	}
	else if((*fits).head.bitpix==16){
		fread(&data_short,sizeof(data_short),1,fp_in);	
/*		if(fabs(((double)data_short-(*fits).head.blank)/(double)data_short)<1.0e-5)*/
		if(data_short==(short)(*fits).head.blank)	
			blnk_flag=1;
		data_float=(float)data_short;	
	}
	if(blnk_flag==1)
		(*fits).data[ii]=(float)DEFAULT_BLANK_VALUE;
	else
		(*fits).data[ii]=(float)(*fits).head.bscale*data_float
				+(float)(*fits).head.bzero;
}

return(0);
}




int	copy_Fits(fits1, fits2)
FITS	*fits1, *fits2;
{
int	ii, err_code;

err_code=copy_FitsHeader(fits1, fits2);
if(err_code!=0){
	fprintf(stderr,"Error in copying FITS header!\n");
	fprintf(stderr,"error code=%d\n",err_code);
	return(101);
}
(*fits2).data=(float *)calloc((*fits1).head.data_num,sizeof((*fits1).data));
for(ii=0;ii<(*fits1).head.data_num;ii++){
	(*fits2).data[ii]=(*fits1).data[ii];
}

return(0);
}



int	copy_FitsHeader(fits1, fits2)	/* copy FITS (fits1 -> fits2)	*/
FITS	*fits1, *fits2;
{
int	ii, jj;

(*fits2).head.simple=(*fits1).head.simple;
(*fits2).head.bitpix=(*fits1).head.bitpix;
(*fits2).head.naxis[0]=(*fits1).head.naxis[0];
for(ii=1;ii<=(*fits1).head.naxis[0];ii++){
	(*fits2).head.naxis[ii]=(*fits1).head.naxis[ii];
}

(*fits2).head.bscale=(*fits1).head.bscale;
(*fits2).head.bzero=(*fits1).head.bzero;
strcpy((*fits2).head.bunit,(*fits1).head.bunit);
(*fits2).head.blank=(*fits1).head.blank;
strcpy((*fits2).head.object,(*fits1).head.object);
strcpy((*fits2).head.date_obs,(*fits1).head.date_obs);
strcpy((*fits2).head.date_map,(*fits1).head.date_map);
strcpy((*fits2).head.origin,(*fits1).head.origin);
strcpy((*fits2).head.instrume,(*fits1).head.instrume);
strcpy((*fits2).head.telescop,(*fits1).head.telescop);
strcpy((*fits2).head.observer,(*fits1).head.observer);
for(ii=1;ii<=(*fits1).head.naxis[0];ii++){
	(*fits2).head.crval[ii]=(*fits1).head.crval[ii];
	(*fits2).head.crpix[ii]=(*fits1).head.crpix[ii];
	(*fits2).head.cdelt[ii]=(*fits1).head.cdelt[ii];
	(*fits2).head.crota[ii]=(*fits1).head.crota[ii];
	(*fits2).head.ctype[ii]=malloc(strlen((*fits1).head.ctype[ii])+1);
	strcpy((*fits2).head.ctype[ii],(*fits1).head.ctype[ii]);
}
for(ii=1;ii<=(*fits1).head.naxis[0];ii++){
for(jj=1;jj<=(*fits1).head.naxis[0];jj++){
	(*fits2).head.cd[ii][jj]=(*fits1).head.cd[ii][jj];
}}
(*fits2).head.cd_mode=(*fits1).head.cd_mode;

(*fits2).head.datamax=(*fits1).head.datamax;
(*fits2).head.datamin=(*fits1).head.datamin;
(*fits2).head.data_num=(*fits1).head.data_num;
(*fits2).head.epoch=(*fits1).head.epoch;

/*
for(jj=1;jj<=(*fits2).head.naxis[0];jj++){
	if((*fits2).head.naxis[jj]<=1)	(*fits2).head.naxis[0]--;	
}
*/

return(0);
}	/*	end of read_FitsData	*/









int output_Fits(fits,fp)
FITS		*fits;
FILE		*fp;
{
int	ii, jj, kk;
int	count;
long	data_num;
char 	buffer[82];
char 	line[82];
int	header_length;
int	header_record_length;
int	data_record_start;
int	add_space;
short	data_short;
long	ch, data_long;
float	data_float;
int	minch,maxch;


data_num=1;
for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	data_num=data_num*(*fits).head.naxis[ii];
}
(*fits).head.data_num=data_num;

(*fits).head.datamin = fabs(DEFAULT_BLANK_VALUE);
(*fits).head.datamax = DEFAULT_BLANK_VALUE;
for(ch=0;ch<data_num;ch++){
	if(((*fits).data[ch]<(*fits).head.datamin)&&(Blank((*fits).data[ch])==0))
		{(*fits).head.datamin=(*fits).data[ch];	minch=ch;}
	if(((*fits).data[ch]>(*fits).head.datamax)&&(Blank((*fits).data[ch])==0))	
		{(*fits).head.datamax=(*fits).data[ch]; maxch=ch;}
}
/*
fprintf(stderr,"datamin=%f(%d)\tdatamax=%f(%d)\n",(*fits).head.datamin,minch,(*fits).head.datamax,maxch);
fprintf(stderr,"data_num=%d\n",data_num);
*/
count=0;

sprintf(line,"SIMPLE  =      T /");	
space_pad(line,80);	fwrite(line,80,1,fp);	count++;
sprintf(line,"BITPIX  =      %d /",(*fits).head.bitpix); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;
sprintf(line,"NAXIS   =      %d /",(*fits).head.naxis[0]); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;
for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	sprintf(line,"NAXIS%d  =      %d /",ii,(*fits).head.naxis[ii]); 
	space_pad(line,80);	fwrite(line,80,1,fp);	count++;
}

sprintf(line,"OBJECT  =      %s /",(*fits).head.object); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;
{
struct tm	*t;
long	clock;

time(&clock);
t=localtime(&clock);
sprintf((*fits).head.date_map,"\'%04d-%02d-%02d\'"
,(*t).tm_year+1900,(*t).tm_mon+1,(*t).tm_mday); 
}
sprintf(line,"DATE-OBS=      %s /",(*fits).head.date_obs); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;
sprintf(line,"DATE-MAP=      %s /",(*fits).head.date_map); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;
strncpy((*fits).head.origin,"\'STAR UTokyo\'",14);
sprintf(line,"ORIGIN  =      %s /",(*fits).head.origin); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;
sprintf(line,"INSTRUME=      %s /",(*fits).head.instrume); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;
sprintf(line,"TELESCOP=      %s /",(*fits).head.telescop); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;
sprintf(line,"OBSERVER=      %s /",(*fits).head.observer); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;

if((*fits).head.bitpix==16){
	(*fits).head.bscale=((*fits).head.datamax-(*fits).head.datamin)/(double)SHRT_MAX;
	(*fits).head.bzero=(*fits).head.datamin;}
else if((*fits).head.bitpix==32){
	(*fits).head.bscale=((*fits).head.datamax-(*fits).head.datamin)/(double)LONG_MAX;
	(*fits).head.bzero=(*fits).head.datamin;}
else if((*fits).head.bitpix==(-32)){
	(*fits).head.bscale=1.0;(*fits).head.bzero=0.0;}


/*
fprintf(stderr,"bscale=%f\tbzero=%f\n",(*fits).head.bscale,(*fits).head.bzero);
fprintf(stderr,"(double)LONG_MIN=%lf\tLONG_MIN=%d\n",(double)LONG_MIN,LONG_MIN);
*/

sprintf(line,"BSCALE  =      %E /",(*fits).head.bscale); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;
sprintf(line,"BZERO   =      %E /",(*fits).head.bzero); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;
sprintf(line,"BUNIT   =      %s /",(*fits).head.bunit); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;
(*fits).head.blank=(double)((-1.0)*pow(2.0,(double)(abs((*fits).head.bitpix)))/2.0)+1.0;
sprintf(line,"BLANK   =      %ld /",(long)(*fits).head.blank); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;


for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	sprintf(line,"CRVAL%1d  =      %E /",ii,(*fits).head.crval[ii]); 	
	space_pad(line,80);	fwrite(line,80,1,fp);	count++;
	sprintf(line,"CRPIX%1d  =      %E /",ii,(*fits).head.crpix[ii]); 	
	space_pad(line,80);	fwrite(line,80,1,fp);	count++;
	sprintf(line,"CDELT%1d  =      %E /",ii,(*fits).head.cdelt[ii]); 	
	space_pad(line,80);	fwrite(line,80,1,fp);	count++;
	sprintf(line,"CROTA%1d  =      %E /",ii,(*fits).head.crota[ii]); 	
	space_pad(line,80);	fwrite(line,80,1,fp);	count++;
	sprintf(line,"CTYPE%1d  =      %s /",ii,(*fits).head.ctype[ii]); 	
	space_pad(line,80);	fwrite(line,80,1,fp);	count++;
}
sprintf(line,"DATAMAX =      %E /",(*fits).head.datamax); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;
sprintf(line,"DATAMIN =      %E /",(*fits).head.datamin); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;
sprintf(line,"EPOCH   =      %E /",(*fits).head.epoch); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;
sprintf(line,"END"); 
space_pad(line,80);	fwrite(line,80,1,fp);	count++;

header_length = 80*count;
header_record_length = (int)ceil((double)header_length/(double)(BIT_RECORD/8));
data_record_start = (header_record_length)*BIT_RECORD/8;
add_space = data_record_start-header_length;
for(ii=1;ii<=add_space;ii++){
	fwrite(" ",1,1,fp);
}

for(ii=0;ii<data_num;ii++){		
	if((*fits).head.bitpix==16){
		if(Blank((*fits).data[ii])==(-1))
			data_short=(short)(*fits).head.blank;
		else
			data_short=(short)( ((*fits).data[ii]-(*fits).head.bzero)/(*fits).head.bscale ) ;
		fwrite(&data_short,2,1,fp);
	}
	else if((*fits).head.bitpix==32){
		if(Blank((*fits).data[ii])==(-1))
			data_long=(long)(*fits).head.blank;
		else
			data_long=(long)( ((*fits).data[ii]-(*fits).head.bzero)/(*fits).head.bscale );
		fwrite(&data_long,4,1,fp);
	}
	else if((*fits).head.bitpix==(-32)){
		if(Blank((*fits).data[ii])==(-1))
			data_float=(float)(*fits).head.blank;
		else
			data_float=(float)( ((*fits).data[ii]-(*fits).head.bzero)/(*fits).head.bscale );
		fwrite(&data_float,4,1,fp);
	}	
}
return(0);
}	/*	end of outputFITS	*/






int OutputColumnData(fits,fp)
FITS		*fits;
FILE		*fp;
{
int	i, ra, dec;
double	cosd[MAX_DIMENSION];
long	ii, err_code;
int	pix[MAX_DIMENSION];
double	x[MAX_DIMENSION];


for(ii=0;ii<(*fits).head.data_num;ii++){
	err_code=POStoPIX(fits,ii,pix);	
	if(Blank((*fits).data[ii])==(-1))	continue;
	PIXtoTX(fits,x,pix);
	for(i=0;i<(*fits).head.naxis[0];i++){
		fprintf(fp,"%lf\t",x[i]);
	}
	fprintf(fp,"%lf\n",(*fits).data[ii]);
/*	if(Blank((*fits).data[ii])==(-1))	fprintf(stderr,"(blank)\n");*/	
}

return(0);
}	/*	end of OutputCoulumnData	*/


int OutputColumnData2(fits,fp)
FITS		*fits;
FILE		*fp;
{
int	i, ra, dec;
double	cosd[MAX_DIMENSION];
long	ii, err_code;
int	pix[MAX_DIMENSION];
double	x[MAX_DIMENSION];
double	xd[MAX_DIMENSION];
int	pixd[MAX_DIMENSION];


for(ii=0;ii<(*fits).head.data_num;ii++){
	err_code=POStoPIX(fits,ii,pix);	
	if(Blank((*fits).data[ii])==(-1))	continue;
	PIXtoTX(fits,x,pix);
	TXtoPIX(fits,x,pixd);
	PIXtoTX(fits,xd,pixd);
	for(i=0;i<(*fits).head.naxis[0];i++){
		fprintf(fp,"%lf\t",x[i]);
		fprintf(fp,"%d\t",pix[i]-pixd[i]);		
	}
	fprintf(fp,"%lf\n",(*fits).data[ii]);
/*	if(Blank((*fits).data[ii])==(-1))	fprintf(stderr,"(blank)\n");*/	
}

return(0);
}	/*	end of OutputCoulumnData	*/






int OutputMatrixData(matrix,fp_out)
MATRIX		*matrix;
FILE		*fp_out;
{
int	ii, jj;




fprintf(fp_out,"\t");
for(jj=1;jj <= (*matrix).num[1];jj++){
	fprintf(fp_out,"%9.6lf\t",(*matrix).axisx[jj]);
}
fprintf(fp_out,"\n");
for(ii=1;ii <= (*matrix).num[2];ii++){
	fprintf(fp_out,"%9.6lf\t",(*matrix).axisy[ii]);
	for(jj=1;jj <= (*matrix).num[1];jj++){
		fprintf(fp_out,"%12.5lf\t",(*matrix).data[jj][ii]);
	}
	fprintf(fp_out,"\n");
}
return(0);
}	/*	end of OutputMatrixData	*/



	

int get_MatrixData(fits, matrix)
FITS		*fits;
MATRIX		*matrix;
{
int	ii, jj;
float	data_float;
int	count, ra, dec;
double	declination, cos_dec;

if( (*fits).head.naxis[0]!=2){
for(jj=3;jj<=(*fits).head.naxis[0];jj++){
	if((*fits).head.naxis[jj]>1)	return(101);
}}
(*matrix).dim=2;
for(ii=1;ii <= ((*matrix).dim) ;ii++){
	(*matrix).num[ii]=(*fits).head.naxis[ii];
}



ra=(-1);	dec=(-1);
for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	if((strstr((*fits).head.ctype[ii],"RA")!=NULL)||
	(strstr((*fits).head.ctype[ii],"GLON")!=NULL))	ra=ii;
	if((strstr((*fits).head.ctype[ii],"DEC")!=NULL)||
	(strstr((*fits).head.ctype[ii],"GLAT")!=NULL))	dec=ii;
}
if(dec==(-1)){
declination=0.0;
cos_dec=1.0;
}
else
{double	min, max;
min=(*fits).head.crval[dec] + (*fits).head.cdelt[dec]*(1.0-(*fits).head.crpix[dec]);
max=(*fits).head.crval[dec] + (*fits).head.cdelt[dec]*((double)(*fits).head.naxis[dec]-(*fits).head.crpix[dec]);
declination = (min+max)/2.0;
cos_dec=cos(declination*PI/180.0);
}
/*declination=(*fits).head.crval[dec];*/

for(ii=1;ii<=(*matrix).num[1];ii++){
	(*matrix).axisx[ii]=(*fits).head.crval[1]+
			(*fits).head.cdelt[1]/cos_dec*((float)ii-(*fits).head.crpix[1]);
}
for(ii=1;ii<=(*matrix).num[2];ii++){
	(*matrix).axisy[ii]=(*fits).head.crval[2]+
			(*fits).head.cdelt[2]/cos_dec*((float)ii-(*fits).head.crpix[2]);
}
count=0;
for(ii=1;ii<=(*matrix).num[2];ii++){
	for(jj=1;jj<=(*matrix).num[1];jj++){
		data_float=(*fits).data[count];	
	    	if(Blank((*fits).data[count])!=0)
			(*matrix).data[jj][ii]=0.0;	//(float)DEFAULT_BLANK_VALUE;
		else
			(*matrix).data[jj][ii]=(*fits).data[count];
	count++;
	}
}
return(0);
}	/*	end of get_MatrixData	*/






int	slice_Fits(fits, slice, ch)
FITS 	*fits;
int	slice, ch;
{
int	ii, jj, num;
float	min, max;
unsigned int	pix[MAX_DIMENSION], pixn[MAX_DIMENSION];
FITS	fits_new;


if((*fits).head.naxis[0]!=3)	return(-20);

copy_Fits(fits,&fits_new);
fits_new.head.naxis[0]=2;

fits_new.head.datamin=999.0;
fits_new.head.datamax=(-999.0);

if(slice==3){
fits_new.head.naxis[1]=(*fits).head.naxis[1];
fits_new.head.crpix[1]=(*fits).head.crpix[1];
fits_new.head.crval[1]=(*fits).head.crval[1];
fits_new.head.cdelt[1]=(*fits).head.cdelt[1];
fits_new.head.crota[1]=(*fits).head.crota[1];
fits_new.head.ctype[1]=malloc(strlen((*fits).head.ctype[1])+1);
strcpy(fits_new.head.ctype[1],(*fits).head.ctype[1]);
fits_new.head.naxis[2]=(*fits).head.naxis[2];
fits_new.head.crpix[2]=(*fits).head.crpix[2];
fits_new.head.crval[2]=(*fits).head.crval[2];
fits_new.head.cdelt[2]=(*fits).head.cdelt[2];
fits_new.head.crota[2]=(*fits).head.crota[2];
fits_new.head.ctype[2]=malloc(strlen((*fits).head.ctype[2])+1);
strcpy(fits_new.head.ctype[2],(*fits).head.ctype[2]);


for(pixn[0]=0;pixn[0]<=(*fits).head.naxis[1]-1;pixn[0]++){
for(pixn[1]=0;pixn[1]<=(*fits).head.naxis[2]-1;pixn[1]++){
    pix[0]=pixn[0]; pix[1]=pixn[1]; pix[2]=ch-1;
    	fits_new.data[PIXtoPOS(&fits_new,pixn)]=(*fits).data[PIXtoPOS(fits,pix)];
    if(Blank((*fits).data[PIXtoPOS(fits,pix)])!=0)	continue;
	if((*fits).data[PIXtoPOS(fits,pix)]>fits_new.head.datamax)	
		fits_new.head.datamax=(*fits).data[PIXtoPOS(fits,pix)];
	if((*fits).data[PIXtoPOS(fits,pix)]<fits_new.head.datamin)	
		fits_new.head.datamin=(*fits).data[PIXtoPOS(fits,pix)];
}}
}
else if(slice==2){
fits_new.head.naxis[1]=(*fits).head.naxis[1];
fits_new.head.crpix[1]=(*fits).head.crpix[1];
fits_new.head.crval[1]=(*fits).head.crval[1];
fits_new.head.cdelt[1]=(*fits).head.cdelt[1];
fits_new.head.crota[1]=(*fits).head.crota[1];
fits_new.head.ctype[1]=malloc(strlen((*fits).head.ctype[1])+1);
strcpy(fits_new.head.ctype[1],(*fits).head.ctype[1]);
fits_new.head.naxis[2]=(*fits).head.naxis[3];
fits_new.head.crpix[2]=(*fits).head.crpix[3];
fits_new.head.crval[2]=(*fits).head.crval[3];
fits_new.head.cdelt[2]=(*fits).head.cdelt[3];
fits_new.head.crota[2]=(*fits).head.crota[3];
fits_new.head.ctype[2]=malloc(strlen((*fits).head.ctype[3])+1);
strcpy(fits_new.head.ctype[2],(*fits).head.ctype[3]);

for(pixn[0]=0;pixn[0]<=(*fits).head.naxis[1]-1;pixn[0]++){
for(pixn[1]=0;pixn[1]<=(*fits).head.naxis[3]-1;pixn[1]++){
    pix[0]=pixn[0]; pix[1]=ch-1; pix[2]=pixn[1];
    	fits_new.data[PIXtoPOS(&fits_new,pixn)]=(*fits).data[PIXtoPOS(fits,pix)];
    if(Blank((*fits).data[PIXtoPOS(fits,pix)])!=0)	continue;
	if((*fits).data[PIXtoPOS(fits,pix)]>fits_new.head.datamax)	
		fits_new.head.datamax=(*fits).data[PIXtoPOS(fits,pix)];
	if((*fits).data[PIXtoPOS(fits,pix)]<fits_new.head.datamin)	
		fits_new.head.datamin=(*fits).data[PIXtoPOS(fits,pix)];
}}
}
else if(slice==1){
fits_new.head.naxis[1]=(*fits).head.naxis[2];
fits_new.head.crpix[1]=(*fits).head.crpix[2];
fits_new.head.crval[1]=(*fits).head.crval[2];
fits_new.head.cdelt[1]=(*fits).head.cdelt[2];
fits_new.head.crota[1]=(*fits).head.crota[2];
fits_new.head.ctype[1]=malloc(strlen((*fits).head.ctype[2])+1);
strcpy(fits_new.head.ctype[1],(*fits).head.ctype[2]);
fits_new.head.naxis[2]=(*fits).head.naxis[3];
fits_new.head.crpix[2]=(*fits).head.crpix[3];
fits_new.head.crval[2]=(*fits).head.crval[3];
fits_new.head.cdelt[2]=(*fits).head.cdelt[3];
fits_new.head.crota[2]=(*fits).head.crota[3];
fits_new.head.ctype[2]=malloc(strlen((*fits).head.ctype[3])+1);
strcpy(fits_new.head.ctype[2],(*fits).head.ctype[3]);

for(pixn[0]=0;pixn[0]<=(*fits).head.naxis[2]-1;pixn[0]++){
for(pixn[1]=0;pixn[1]<=(*fits).head.naxis[3]-1;pixn[1]++){
    pix[0]=ch-1; pix[1]=pixn[0]; pix[2]=pixn[1];
    	fits_new.data[PIXtoPOS(&fits_new,pixn)]=(*fits).data[PIXtoPOS(fits,pix)];
    if(Blank((*fits).data[PIXtoPOS(fits,pix)])!=0)	continue;
	if((*fits).data[PIXtoPOS(fits,pix)]>fits_new.head.datamax)	
		fits_new.head.datamax=(*fits).data[PIXtoPOS(fits,pix)];
	if((*fits).data[PIXtoPOS(fits,pix)]<fits_new.head.datamin)	
		fits_new.head.datamin=(*fits).data[PIXtoPOS(fits,pix)];
}}
}
else	return(-21);

copy_Fits(&fits_new,fits);

return(0);
}










int	Blank(data)
float	data;
{
int	blank;

if((data==DEFAULT_BLANK_VALUE)
	/*||(isnormal(data)!=1)*/)	blank=(-1);
else	blank=0;

return(blank);
}






float	Data(fits,pix)
FITS	*fits;
int	pix[];
{
int	ii;
float	fdata;
int	flag;
long	ch;

flag=0;
for(ii=0;ii<(*fits).head.naxis[0];ii++){
	if(pix[ii]<0)	flag=1;
	else if(pix[ii]>=(*fits).head.naxis[ii+1])	flag=1;
}
if(flag==1)	fdata=DEFAULT_BLANK_VALUE;
else{
	ch=PIXtoPOS(fits,pix);
	fdata=(*fits).data[ch];
}
return(fdata);
}


/****************************************/
/* pix: 0 to fits.head.naxis[]-1        */
/* pos:	0 to data_number-1              */
/****************************************/

long	PIXtoPOS(fits,pix)
FITS	*fits;
int	pix[];		
{
int	ii, jj;
long	pos;

pos=(long)pix[(*fits).head.naxis[0]-1];
for(ii=(*fits).head.naxis[0]-1;ii>0;ii--){
	pos = pos*(long)(*fits).head.naxis[ii] + (long)pix[ii-1]; 
}
return(pos);
}	/*	end of PIXtoPOS		*/


int	POStoPIX(fits,pos,pix)
FITS	*fits;
long	pos;
int	pix[];
{
int	i, dim;
long	rem, mpix[MAX_DIMENSION];

mpix[0]=(*fits).head.naxis[1];
for(i=1;i<(*fits).head.naxis[0];i++){
	mpix[i]=mpix[i-1]*(*fits).head.naxis[i+1];
}
rem=pos;
for(i=(*fits).head.naxis[0]-1;i>=1;i--){
	pix[i]=(int)floor((double)rem/(double)mpix[i-1]);
	rem=rem-(long)pix[i]*mpix[i-1];
}
pix[0]=rem;
return(0);
}	/*	end of POStoPIX		*/





long	DataPosition(fits,pix)
FITS	*fits; long	pix[];	
{int	ii,jj;
long	dim, pos;
dim=(*fits).head.naxis[0]; pos=pix[dim-1];
for(ii=dim-1;ii>0;ii--){pos = pos*(*fits).head.naxis[ii] + pix[ii-1]; }
return(pos);}





/*	Hundling FITS keywords	*/

int	divide_Line(line, keyword, value, comment)
char	line[];
char	*keyword;
char	*value;
char	*comment;
{
char	*token;
char	*buffer;
char	*value1;
char	*buffer1;


//buffer=malloc(9); buffer[8]=NULL;
buffer=(char *)malloc(9);
strncpy(buffer,line,8);
compress(buffer,keyword);
free(buffer);

//buffer=(char *)malloc(MAX_CHAR_NUMBER);
buffer=(line+8);
if(buffer==NULL){	value[0]=NULL;	comment[0]=NULL;	}
else			strcpy(value,buffer);	


if((strncmp(keyword,"COMMENT",7)==0)||(strncmp(keyword,"HISTORY",7)==0)){
	comment[0]=NULL;
}
else if(strncmp(keyword,"END",3)==0){
	value[0]=NULL;
	comment[0]=NULL;
}
else{
	if(value[0]=='='){
	    	buffer=(value+1);
		if(buffer==NULL){	value[0]=NULL;	comment[0]=NULL;	}
		else			strcpy(value,buffer);
	}
    	if(strpbrk(value,"/")==NULL)	value[strlen(value)-1]='/';
	if(strpbrk(value,"\'")==NULL){
		value1=malloc(strlen(value)+1);		strcpy(value1,value);
		buffer=strtok(value,"/");
		if(buffer==NULL){	value[0]=NULL;	comment[0]=NULL;	}
		else{	strcpy(value,buffer);
			buffer=strpbrk(value1,"/");	free(value1);
			buffer=(buffer+1);
			if((buffer==NULL)||(strspn(buffer," ")==strlen(buffer))){
				comment[0]=NULL;
			}
			else	strcpy(comment,buffer);		
		}
	}
	else{
	    	buffer=strtok(value,"\'");	
		if(buffer==NULL){	value[0]=NULL;	comment[0]=NULL;	}
	    	if(strncmp(buffer," ",1)==0)	    	buffer=strtok(NULL,"\'");
	    
		if(buffer==NULL){	value[0]=NULL;	comment[0]=NULL;	}
		else{	sprintf(value,"\'%s\'",buffer);
			buffer=strtok(NULL,"/");
			if((buffer==NULL)||(strspn(buffer," ")==strlen(buffer))){
				comment[0]=NULL;	
			}
			else{	buffer=strpbrk(buffer,"/");
				buffer=(buffer+1);
				if((buffer[0]==NULL)||(strspn(buffer," ")==strlen(buffer)))
							comment[0]==NULL;	
				else			strcpy(comment,buffer);	
			}					
		}
	}
}

/*
fprintf(stderr,"%s(%d) %s(%d) : %s(%d)\n",
keyword,strlen(keyword),value,strlen(value),comment,strlen(comment));
*/
//free(buffer);
return(0);
}







int compress(original, compressed)
char	original[];
char	compressed[];
{
int	ii, jj;

ii=0;	jj=0;
while(original[ii]!=(char)NULL){
	if((original[ii]!=' ')&&(original[ii]!='\'')&&(original[ii]!='\"')){		
	compressed[jj]=original[ii];
		ii++;	jj++;
	}
	else	ii++;

}	/*	while loop	*/
compressed[jj]=(char)NULL;

return(0);
}	/*	end of this function	*/




char *space_pad(line,num)
char	*line;
unsigned int	num;
{
int	ii, add;
char	*new;

new=malloc(num+1);
add=num-strlen(line);
strcpy(new,line);
for(ii=1;ii<=add;ii++){
	strcat(new," ");
}	
strcpy(line,new);
free(new);
/*	fprintf(stderr,"%d\n",strlen(line));	*/
return(new);
}



int tint(x)
double	x;
{
double	xl, xg;
xl=floor(x);	xg=ceil(x);	
if((x-xl)>=0.5)	return((int)xg);
else		return((int)xl);
}


int	XtoPIX(fits,x,pix)
FITS	*fits;
double	x[];
int	pix[];
{
int	i;
for(i=0;i<(*fits).head.naxis[0];i++){
	pix[i] = tint((*fits).head.crpix[i+1]+(x[i]-(*fits).head.crval[i+1])/(*fits).head.cdelt[i+1]) - 1;
}
return(0);
}




int	PIXtoX(fits,x,pix)
FITS	*fits;
double	x[];
int	pix[];
{
int	i;
for(i=0;i<(*fits).head.naxis[0];i++){
	x[i] = (*fits).head.crval[i+1]+(*fits).head.cdelt[i+1]*((double)(pix[i]+1)-(*fits).head.crpix[i+1]);
}
return(0);
}





int	PIXtoTX(fits,x,pix)
FITS	*fits;
double	x[];
int	pix[];
{
int	i, j, ii, jj;
int     ra, dec;
XY_POS	radec, prj;
DIRECT_COS	dir;
double	r, theta, declination;

ra=(-1);        dec=(-1);	declination=0.0;
for(ii=1;ii<=(*fits).head.naxis[0];ii++){
        if((strstr((*fits).head.ctype[ii],"RA")!=NULL)||
        (strstr((*fits).head.ctype[ii],"GLON")!=NULL))  ra=ii;
        if((strstr((*fits).head.ctype[ii],"DEC")!=NULL)||
        (strstr((*fits).head.ctype[ii],"GLAT")!=NULL))  dec=ii;
}
if(dec!=(-1)){
    declination=(*fits).head.crval[dec]*RAD;
}

for(ii=1;ii<=(*fits).head.naxis[0];ii++){
    if(ii==ra)
	x[ii-1] = (*fits).head.crval[ii]+
			(*fits).head.cdelt[ii]/cos(declination)*((double)(pix[ii-1]+1)-(*fits).head.crpix[ii]);
    else
	x[ii-1] = (*fits).head.crval[ii]+
			(*fits).head.cdelt[ii]*((double)(pix[ii-1]+1)-(*fits).head.crpix[ii]);
}

return(0);
}



int	PIXtoWX(fits,x,pix)
FITS	*fits;
double	x[];
int	pix[];
{
int	i, j, ii, jj;
int     ra, dec;
XY_POS	radec, prj;
DIRECT_COS	dir;
double	r, theta;

ra=(-1);        dec=(-1);
for(ii=1;ii<=(*fits).head.naxis[0];ii++){
        if((strstr((*fits).head.ctype[ii],"RA")!=NULL)||
        (strstr((*fits).head.ctype[ii],"GLON")!=NULL))  ra=ii;
        if((strstr((*fits).head.ctype[ii],"DEC")!=NULL)||
        (strstr((*fits).head.ctype[ii],"GLAT")!=NULL))  dec=ii;
}
if((ra==(-1))||(dec==(-1))){
	ra=1; dec=2;
}
for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	if((ii!=ra)&&(ii!=dec))
	x[ii-1] = (*fits).head.crval[ii]+
		(*fits).head.cdelt[ii]*((double)(pix[ii-1]+1)-(*fits).head.crpix[ii]);
}

PIXtoPRJ(fits,pix,&prj,ra,dec);

r = atan(sqrt(prj.x*prj.x+prj.y*prj.y));
if(prj.x==0.0){	if(prj.y>0.0)	theta = PI/2.0; 
		else		theta = (-1.0)*PI/2.0; }
else if(prj.x>0.0)	theta = atan(prj.y/prj.x);
else			theta = atan(prj.y/prj.x) + PI;

dir.l=(-1.0)*sin(theta)*sin(r);
dir.m=cos(theta)*sin(r);
dir.n=cos(r);

rotate_zx(&dir,(90.0-(*fits).head.crval[dec]));
rotate_xy(&dir,(*fits).head.crval[ra]);

if(dir.l==0.0){	if(dir.m>0.0)	radec.x = PI/2.0; 
		else		radec.x = (-1.0)*PI/2.0; }
else if(dir.l>0.0)	radec.x = atan(dir.m/dir.l)/RAD;
else		radec.x = ( PI+atan(dir.m/dir.l) )/RAD;
if(dir.n>=0.0)	radec.y = 90.0 - asin(sqrt(dir.l*dir.l+dir.m*dir.m))/RAD;
else		radec.y = asin(sqrt(dir.l*dir.l+dir.m*dir.m))/RAD - 90.0;

x[ra-1] = radec.x;
x[dec-1] = radec.y;


return(0);
}




int	TXtoPIX(fits,x,pix)
FITS	*fits;
double	x[];
int	pix[];
{
int	i, j, ii, jj;
int     ra, dec;
XY_POS	radec, prj;
DIRECT_COS	dir;
double	r, theta, declination;


ra=(-1);        dec=(-1);
for(ii=1;ii<=(*fits).head.naxis[0];ii++){
        if((strstr((*fits).head.ctype[ii],"RA")!=NULL)||
        (strstr((*fits).head.ctype[ii],"GLON")!=NULL))  ra=ii;
        if((strstr((*fits).head.ctype[ii],"DEC")!=NULL)||
        (strstr((*fits).head.ctype[ii],"GLAT")!=NULL))  dec=ii;
}

if(dec!=(-1)){
    declination=(*fits).head.crval[dec]*RAD;
}

for(ii=1;ii<=(*fits).head.naxis[0];ii++){
    if(ii==ra)
	pix[ii-1] = tint(  (*fits).head.crpix[ii]+
			 (x[ii-1]-(*fits).head.crval[ii])/((*fits).head.cdelt[ii]/cos(declination))  ) - 1;
    else
	pix[ii-1] = tint(  (*fits).head.crpix[ii]+
			 (x[ii-1]-(*fits).head.crval[ii])/((*fits).head.cdelt[ii])  ) - 1;	
}

return(0);
}



int	WXtoPIX(fits,x,pix)
FITS	*fits;
double	x[];
int	pix[];
{
int	i, j, ii, jj;
int     ra, dec;
XY_POS	radec, prj;
DIRECT_COS	dir;
double	r, theta;


ra=(-1);        dec=(-1);
for(ii=1;ii<=(*fits).head.naxis[0];ii++){
        if((strstr((*fits).head.ctype[ii],"RA")!=NULL)||
        (strstr((*fits).head.ctype[ii],"GLON")!=NULL))  ra=ii;
        if((strstr((*fits).head.ctype[ii],"DEC")!=NULL)||
        (strstr((*fits).head.ctype[ii],"GLAT")!=NULL))  dec=ii;
}
if((ra==(-1))||(dec==(-1))){
	ra=1; dec=2;
}

for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	if((ii!=ra)&&(ii!=dec))
	pix[ii-1] = tint(  (*fits).head.crpix[ii]+
			(x[ii-1]-(*fits).head.crval[ii])/(*fits).head.cdelt[ii]  ) - 1;
}


radec.x=x[ra-1];
radec.y=x[dec-1];

dir.l=cos(radec.x*RAD)*sin((90.0-radec.y)*RAD);
dir.m=sin(radec.x*RAD)*sin((90.0-radec.y)*RAD);
dir.n=cos((90.0-radec.y)*RAD);

rotate_xy(&dir,(-1.0)*(*fits).head.crval[ra]);
rotate_zx(&dir,(-1.0)*(90.0-(*fits).head.crval[dec]));

r = sqrt(dir.l*dir.l+dir.m*dir.m) ;
if(dir.m==0.0){	if(dir.l<0.0)	theta = PI/2.0;
		else		theta = (-1.0)*PI/2.0;	}
else if(dir.m>0.0)	theta = atan((-1.0)*dir.l/dir.m);
else			theta = PI + atan((-1.0)*dir.l/dir.m);

prj.x = tan(r)*cos(theta);
prj.y = tan(r)*sin(theta);

PRJtoPIX(fits,pix,&prj,ra,dec);

return(0);
}





int	 PIXtoPRJ(fits,pix,prj,ra,dec)
FITS	*fits;
int	pix[], ra, dec;
XY_POS	*prj;
{
int	ii, jj;

if((*fits).head.cd_mode==0){
(*prj).x = (*fits).head.cdelt[ra]*((double)(pix[ra-1]+1)-(*fits).head.crpix[ra]) ;
(*prj).y = (*fits).head.cdelt[dec]*((double)(pix[dec-1]+1)-(*fits).head.crpix[dec]) ;
}
else{
(*prj).x = (*fits).head.cd[ra][ra]*((double)(pix[ra-1]+1)-(*fits).head.crpix[ra]);
(*prj).x += (*fits).head.cd[ra][dec]*((double)(pix[dec-1]+1)-(*fits).head.crpix[dec]);
(*prj).y = (*fits).head.cd[dec][ra]*((double)(pix[ra-1]+1)-(*fits).head.crpix[ra]);
(*prj).y += (*fits).head.cd[dec][dec]*((double)(pix[dec-1]+1)-(*fits).head.crpix[dec]);
}

(*prj).x *= RAD;
(*prj).y *= RAD;

return(0);
}



int	 PRJtoPIX(fits,pix,prj,ra,dec)
FITS	*fits;
int	pix[], ra, dec;
XY_POS	*prj;
{
int	ii, jj;
double	pixd[2];
double	det;


if((*fits).head.cd_mode==0){
pix[ra-1]=tint(  (*fits).head.crpix[ra] +
			((*prj).x/RAD)/(*fits).head.cdelt[ra]  ) - 1;
pix[dec-1]=tint(  (*fits).head.crpix[dec] +
			((*prj).y/RAD)/(*fits).head.cdelt[dec]  ) - 1;
}
else{
det=(*fits).head.cd[ra][ra]*(*fits).head.cd[dec][dec]
	-(*fits).head.cd[ra][dec]*(*fits).head.cd[dec][ra];
pixd[0] = (*fits).head.cd[dec][dec]*((*prj).x/RAD)
	- (*fits).head.cd[ra][dec]*((*prj).y/RAD);
pixd[1] = (*fits).head.cd[ra][ra]*((*prj).y/RAD)
	- (*fits).head.cd[dec][ra]*((*prj).x/RAD);
pixd[0] /= det;
pixd[1] /= det;
pixd[0] += (*fits).head.crpix[ra];
pixd[1] += (*fits).head.crpix[dec];

pix[0] = tint(pixd[0])-1;
pix[1] = tint(pixd[1])-1;

}

return(0);
}



FindDec(fits, cosd)
FITS	*fits;
double	cosd[];
{
double	declination, length; 
int	ra, dec, ii;

ra=(-1);        dec=(-1);
for(ii=1;ii<=(*fits).head.naxis[0];ii++){
        if((strstr((*fits).head.ctype[ii],"RA")!=NULL)||
        (strstr((*fits).head.ctype[ii],"GLON")!=NULL))  ra=ii;
        if((strstr((*fits).head.ctype[ii],"DEC")!=NULL)||
        (strstr((*fits).head.ctype[ii],"GLAT")!=NULL))  dec=ii;
}
if(dec==(-1)){
	for(ii=1;ii<=(*fits).head.naxis[0];ii++){
		cosd[ii-1]=1.0;
	}
}
else{
	for(ii=1;ii<=(*fits).head.naxis[0];ii++){
		if(ii==ra)	cosd[ii-1]=cos((*fits).head.crval[dec]*RAD);
		else		cosd[ii-1]=1.0;
	}
}
}




long	DataNumber(fits)
FITS	*fits;
{
int	ii;
long	data_number;
data_number=1;
for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	data_number=data_number*(*fits).head.naxis[ii];
}
(*fits).head.data_num = data_number;
return(data_number);
}



/*****************************/
/*
Rotation in the x-y plane
*/
/*****************************/


int	rotate_xy(dir,theta)
DIRECT_COS *dir;	/* direction cos [rad] */
double	theta;	/* in [degrees] */
{
int	ii, jj;
double	rot[3][3];
double	x[3], xn[3];

x[0]=(*dir).l;	x[1]=(*dir).m;	x[2]=(*dir).n;

rot[0][0]=cos(theta*RAD);
rot[0][1]=(-1.0)*sin(theta*RAD);
rot[0][2]=0.0;
rot[1][0]=sin(theta*RAD);
rot[1][1]=cos(theta*RAD);
rot[1][2]=0.0;
rot[2][0]=0.0;
rot[2][1]=0.0;
rot[2][2]=1.0;

for(ii=0;ii<3;ii++){
xn[ii]=0.0;
for(jj=0;jj<3;jj++){
	xn[ii] += rot[ii][jj]*x[jj];
}}

(*dir).l=xn[0];	(*dir).m=xn[1];	(*dir).n=xn[2];	

return(0);
}

/*****************************/
/*
Rotation in the xz plane
*/
/*****************************/

int	rotate_zx(dir,theta)
DIRECT_COS *dir;	/* direction cos [rad] */
double	theta;	/* in [degrees] */
{
int	ii, jj;
double	rot[3][3];
double	x[3], xn[3];


x[0]=(*dir).l;	x[1]=(*dir).m;	x[2]=(*dir).n;

rot[0][0]=cos(theta*RAD);
rot[0][1]=0.0;
rot[0][2]=sin(theta*RAD);
rot[1][0]=0.0;
rot[1][1]=1.0;
rot[1][2]=0.0;
rot[2][0]=(-1.0)*sin(theta*RAD);
rot[2][1]=0.0;
rot[2][2]=cos(theta*RAD);

for(ii=0;ii<3;ii++){
xn[ii]=0.0;
for(jj=0;jj<3;jj++){
	xn[ii] += rot[ii][jj]*x[jj];
}}

(*dir).l=xn[0];	(*dir).m=xn[1];	(*dir).n=xn[2];	

return(0);
}
