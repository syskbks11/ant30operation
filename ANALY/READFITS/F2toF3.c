/*
CfA CO(J=1-0) FITS file reader

1993/9/1	Tomoharu OKA	:	Coding Jn
1993/9/2	Tomoharu OKA	:	Ver.1.00 
1994/9/7	Tomoharu OKA	:	Bug fix for pixel inner structure
1997/10/?	Tomo OKA	:	2D FITS -> 3D FITS
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
//#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0

int	is, ie, inum;
double	start_velocity;
double	vel_intvl;




main(int argc, char *argv)
{
FILE		*fp_in, *fp_out;
int		err_code;
XY_POS		position;
FITS		fits, fits_new;
int		offset;
int		ii, jj;
int		data_number;
char		in_filename[80];
char		out_filename[80];
char		filehead[80];
char		directory[80];
char		command[80];
double		v;
int		flag;
int		ch_number;
char		buffer[80];
char		*token;
int		ch[256];



flag=0;
/*
fprintf(stderr,"Data directory : ");
fscanf(stdin,"%s",directory);
*/
fprintf(stderr,"Filename head : ");
fscanf(stdin,"%s",filehead);
fprintf(stderr,"Channel range (1,88): ");
fscanf(stdin,"%s",buffer);
	token=strtok(buffer,",");
	if(token!=NULL) is=atoi(token);	else exit(1);
	token=strtok(NULL,",");
	if(token!=NULL) ie=atoi(token);	else exit(1);
fprintf(stderr,"Center velocity of the first channel (#1) [km/s]: ");
fscanf(stdin,"%lf",&start_velocity);
fprintf(stderr,"Velocity interval [km/s]: ");
fscanf(stdin,"%lf",&vel_intvl);

inum=0;
for(ii=is;ii<=ie;ii++){
	sprintf(in_filename,"%s.%d.FITS",filehead,ii);
	fp_in=fopen(in_filename,"r");
	if(fp_in==NULL){
		sprintf(in_filename,"%s.%02d.FITS",filehead,ii);
		fp_in=fopen(in_filename,"r");
		if(fp_in==NULL){
			sprintf(in_filename,"%s.%02d.fits",filehead,ii);
			fp_in=fopen(in_filename,"r");
			if(fp_in==NULL){	
				/* fprintf(stderr,"Can't open %s !\n",in_filename); */
				continue;
			}
		}
	}
	if(fp_in!=NULL){	ch[inum]=ii;	inum++;	}
	fclose(fp_in);
}	/*	for ii loop	*/



for(ii=0;ii<inum;ii++){
	v = start_velocity + vel_intvl*((double)ii-1.0);
	sprintf(in_filename,"%s.%d.FITS",filehead,ch[ii]);
	fp_in=fopen(in_filename,"r");
	if(fp_in==NULL){
		sprintf(in_filename,"%s.%02d.FITS",filehead,ch[ii]);
		fp_in=fopen(in_filename,"r");
		if(fp_in==NULL){
			sprintf(in_filename,"%s.%02d.fits",filehead,ch[ii]);
			fp_in=fopen(in_filename,"r");
			if(fp_in==NULL){	
				fprintf(stderr,"Can't open %s !\n",in_filename); 
				exit(1);
			}
		}
	}
	err_code=read_Fits(&fits, fp_in);
	if(ii==0)	err_code=create_3DFitsHeader(&fits, &fits_new);
	fprintf(stderr,"Reading %s\n",in_filename);
	err_code=add_FitsData(&fits, &fits_new,vel_intvl);

	fclose(fp_in);
}	/*	for ii loop	*/


sprintf(out_filename,"%s.cube.FITS",filehead);
fp_out=fopen(out_filename,"w");
if(fp_out==NULL){
	fprintf(stderr,"Can't open %s !\n",out_filename);
	exit(1);
}
fprintf(stderr,"Writing to %s\n",out_filename);
output_Fits(&fits_new, fp_out);

}	/*main*/






int	create_3DFitsHeader(fits, fits_new)
FITS 	*fits;
FITS 	*fits_new;
{
int	ii, jj;
int	data_number;
float	data_float;


(*fits_new).head.simple=(*fits).head.simple;
(*fits_new).head.bitpix=(*fits).head.bitpix;
for(ii=1;ii<=2;ii++){
	(*fits_new).head.naxis[ii]=(*fits).head.naxis[ii];
}
(*fits_new).head.naxis[0]=3L;
(*fits_new).head.naxis[3]=inum;	/*abs(ie-is)+1;*/
if((*fits_new).head.bitpix==16)
	(*fits_new).head.bscale=((*fits_new).head.datamax-(*fits_new).head.datamin)/(65536.0/2.0);
else if(abs((*fits_new).head.bitpix)==32)

(*fits_new).head.bscale=((*fits_new).head.datamax-(*fits_new).head.datamin)/(4294967296.0/2.0);
(*fits).head.bzero=(*fits).head.datamin;
strcpy((*fits_new).head.bunit,(*fits).head.bunit);
(*fits_new).head.blank=(*fits).head.blank;
strcpy((*fits_new).head.object,(*fits).head.object);
strcpy((*fits_new).head.date_obs,(*fits).head.date_obs);
strcpy((*fits_new).head.date_map,(*fits).head.date_map);
strcpy((*fits_new).head.origin,(*fits).head.origin);
strcpy((*fits_new).head.instrume,(*fits).head.instrume);
strcpy((*fits_new).head.telescop,(*fits).head.telescop);
strcpy((*fits_new).head.observer,(*fits).head.observer);
for(ii=1;ii<=(*fits).head.naxis[0];ii++){
	(*fits_new).head.crval[ii]=(*fits).head.crval[ii];
	(*fits_new).head.crpix[ii]=(*fits).head.crpix[ii];
	(*fits_new).head.cdelt[ii]=(*fits).head.cdelt[ii];
	(*fits_new).head.ctype[ii]=malloc(strlen((*fits).head.ctype[ii])+1);
	strcpy((*fits_new).head.ctype[ii],(*fits).head.ctype[ii]);
}
(*fits_new).head.crval[3]= start_velocity;
(*fits_new).head.crpix[3]= 1.0;
(*fits_new).head.cdelt[3]= vel_intvl;
(*fits_new).head.ctype[3]=malloc(strlen("\'LSR-Vel\'")+1);
strcpy((*fits_new).head.ctype[3],"\'LSR-Vel\'");


(*fits_new).head.datamax=(*fits).head.datamax;
(*fits_new).head.datamin=(*fits).head.datamin;
(*fits_new).head.epoch=(*fits).head.epoch;

data_number=1;
for(jj=1;jj<=(*fits_new).head.naxis[0];jj++){
	data_number = data_number * (*fits_new).head.naxis[jj];
}
(*fits_new).data=(float *)calloc(data_number,sizeof(data_float));


return(0);
}



int add_FitsData(fits, fits_new,vel_intvl)
FITS	*fits, *fits_new;
double	vel_intvl;
{
int	ii;
int	new_data_num;
static int	data_number=0;
double	width;


if(strstr((*fits).head.bunit,"m/s")==NULL)
    	width=1.0;
else	width=vel_intvl;

strcpy((*fits_new).head.bunit,"\'K  \'");

new_data_num=1;
for(ii=1;ii<=2;ii++){
	new_data_num=new_data_num*(*fits).head.naxis[ii];
}

for(ii=0;ii<new_data_num;ii++){
    (*fits_new).data[data_number+ii]=(*fits).data[ii]/width;
    //    if(fabs((*fits).data[ii])>1e3)    fprintf(stderr,"%f\n",(*fits).data[ii]);
}

if((*fits).head.datamax>(*fits_new).head.datamax)
	(*fits_new).head.datamax=(*fits).head.datamax;
if((*fits).head.datamin<(*fits_new).head.datamin)
	(*fits_new).head.datamin=(*fits).head.datamin;

data_number = data_number+new_data_num;

free((*fits).data);

return(0);
}



