/*
CfA CO(J=1-0) FITS file reader

1993/9/1	Tomoharu OKA	:	Coding 開始
1993/9/2	Tomoharu OKA	:	Ver.1.00 完成
1994/9/7	Tomoharu OKA	:	Bug fix for pixel inner structure
*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#ifdef	_NO_PROTO
extern int options();
extern int get_ProfHeader();
extern int default_ProfHeader();
extern int read_FitsHeader();
extern int divide_Line();
extern int rm_Quot();
extern int get_ProfData();
extern int calc_DataPosition();
#else
extern int options(int, char **, char *, char *, XY_POS *);
extern int get_ProfHeader(FITS_HEAD *, PROFILE *, AXIS_INDEX *, XY_POS *, FILE *);
extern int default_ProfHeader(PROFILE *);
extern int read_FitsHeader(FITS_HEAD *, FILE *);
extern int divide_Line(char *, char *, char *, char *);
extern int rm_Quot(char *, char *);
extern int get_ProfData(FITS_HEAD *, PROFILE *, AXIS_INDEX *, XY_POS *, FILE *);
extern int calc_DataPosition(FITS_HEAD *, unsigned int *);
#endif



main(argc,argv)
int	argc;
char	*argv[];
{
FILE		*fp_in, *fp_out;
char		in_filename[MAX_FILE_NAME_LENGTH];
char		out_filename[MAX_FILE_NAME_LENGTH];
int		err_code;
AXIS_INDEX	axis;
XY_POS		position;
FITS_HEAD	fits_head;
PROFILE		profile;
HISTORY		history;


/*	オプションスイッチの解釈	*/

err_code=options(argc,argv,in_filename,out_filename,&position);
if(err_code!=0){
	fprintf(stderr,"Invalid option!\n");
	exit(999);
}


/*	入力ファイルのオープン	*/

if(in_filename[0]==NULL){	
	/*fp_in=stdin;*/
	fprintf(stderr,"Standard input is not available.\n");
	fprintf(stderr,"Specify input file name with -in option.\n");
	exit(999);
}

else{
	fp_in=fopen(in_filename,"rb");
	if(fp_in==NULL){
		fprintf(stderr,"Can't open %s.\n",in_filename);
		exit(999);
	}
}

/*	出力ファイルのオープン	*/

if(out_filename[0]==NULL)	
	fp_out=stdout;
else{
	fp_out=fopen(out_filename,"wb");
	if(fp_out==NULL){
		fprintf(stderr,"Can't create %s.\n",out_filename);
		exit(999);
	}
}


/*	PROFILE header部分の構成	*/

err_code=get_ProfHeader(&fits_head, &profile, &axis, &position, fp_in);
if(err_code!=0){
	fprintf(stderr,"Error in creating PROFILE header. Error code=%d\n",err_code);
	exit(999);
}

/*	PROFILE data部分の構成	*/

err_code=get_ProfData(&fits_head, &profile, &axis, &position, fp_in);
if(err_code!=0){
	fprintf(stderr,"Error in creating PROFILE data. Error code=%d\n",err_code);
	exit(999);
}

/*	PROFILEの出力	*/

history.size=1;
strcpy(history.step,"IMPORT");
sprintf(history.prm[0],"%s",NULL);
STAR_AddHistory(&profile,&history);


if(fp_in!=stdin)	fclose(fp_in);


if(fp_out==stdout){
	err_code=profile_out(&profile);
	if(err_code!=0){
		fprintf(stderr,"Standard output error! Broken PIPE.\n",out_filename);
		exit(999);
	}
}
else{
	err_code=profile_save(fp_out,&profile);
	if(err_code!=0){
		fprintf(stderr,"FILE output error! Can't write to %s\n",out_filename);
		fclose(fp_out);
		exit(999);
	}
	fclose(fp_out);
}


}	/*main*/











int default_ProfHeader(profile)
PROFILE *profile;
{
	int	ii;
	long	now;

/* default parameters */
	strcpy((*profile).file_id,"STAR.scn");

	(*profile).sys_head.format_id=1;
	(*profile).sys_head.version=2;
	(*profile).sys_head.data_type=1;
	(*profile).sys_head.hist_step=0;
	(*profile).sys_head.hist_line=1;
	(*profile).sys_head.scan=0;
	(*profile).sys_head.backup=0;
	(*profile).sys_head.num_scan=1;

	(*profile).sys_head.l_head=637;	/* the size before history for ver.1 */
	(*profile).sys_head.l_head
		=(*profile).sys_head.l_head+(*profile).sys_head.hist_line*16;
	(*profile).sys_head.l_file=(*profile).sys_head.l_head;

	strcpy((*profile).obs_prm.project,"Imported");
	strcpy((*profile).obs_prm.scd_file,"   ");
	(*profile).obs_prm.mjd=0;
	(*profile).obs_prm.start=0;
	(*profile).obs_prm.stop=0;
	(*profile).obs_prm.on_time=0;
	(*profile).obs_prm.off_time=0;
	strcpy((*profile).obs_prm.observer1,"Someone");
	(*profile).obs_prm.observer2[0]=NULL;
	(*profile).obs_prm.observer3[0]=NULL;
	(*profile).obs_prm.observer4[0]=NULL;

	strcpy((*profile).object.name,"SomeRegion");
	(*profile).object.cood_sys=0;
	(*profile).object.on_lb.x=0.0;
	(*profile).object.on_lb.y=0.0;
	(*profile).object.on_radec.x=0.0;
	(*profile).object.on_radec.y=0.0;
	(*profile).object.on_azel.x=0.0;
	(*profile).object.on_azel.y=0.0;
	(*profile).object.off_lb.x=0.0;
	(*profile).object.off_lb.y=0.0;
	(*profile).object.off_radec.x=0.0;
	(*profile).object.off_radec.y=0.0;
	(*profile).object.orig_xy.x=0.0;
	(*profile).object.orig_xy.y=0.0;
	(*profile).object.pa_xy=0.0;

	strcpy((*profile).ant_status.tele,"Telescop");
	(*profile).ant_status.ap_eff=(float)0.0;
	(*profile).ant_status.mb_eff=(float)0.0;
	(*profile).ant_status.fss_eff=(float)0.0;
	(*profile).ant_status.hpbw=(float)0.0;

	strcpy((*profile).rx_status.name,"SomeRX");
	(*profile).rx_status.sb_mode=0;
	(*profile).rx_status.v_lsr=0.0;
	(*profile).rx_status.rest_freq=0.0;
	(*profile).rx_status.obs_freq=0.0;
	(*profile).rx_status.lo_freq1=0.0;
	(*profile).rx_status.if_freq1=0.0;
	(*profile).rx_status.lo_freq2=0.0;
	(*profile).rx_status.if_freq2=0.0;
	(*profile).rx_status.lo_freq3=0.0;
	(*profile).rx_status.if_freq3=0.0;
	(*profile).rx_status.mltplx1=0;
	(*profile).rx_status.mltplx2=0;
	(*profile).rx_status.mltplx3=0;
	(*profile).rx_status.tsys=(float)0.0;
	(*profile).rx_status.trx=(float)0.0;
	(*profile).rx_status.rms=(float)0.0;
	(*profile).rx_status.calib=(float)300.0;
	(*profile).rx_status.weight=(float)1.0;
	(*profile).rx_status.scale=(float)1.0;

	strcpy((*profile).be_status.name,"SomeBE");
	(*profile).be_status.be_ch=0;
	(*profile).be_status.basis_ch=1;
	(*profile).be_status.sign=1;
	(*profile).be_status.data_ch=0;
	(*profile).be_status.center_freq=0.0;
	(*profile).be_status.disp0=0.0;
	(*profile).be_status.disp1=0.0;
	(*profile).be_status.disp2=0.0;
	(*profile).be_status.disp3=0.0;
	(*profile).be_status.disp4=0.0;
	(*profile).be_status.freq_res=0.0;
	(*profile).be_status.ch_width=(float)1.0;
	(*profile).be_status.ch_orig=(float)1.0;

	strcpy((*profile).obs_log.site_name,"Anywhere");
	(*profile).obs_log.meteo.tmp=(float)0.0;
	(*profile).obs_log.meteo.water=(float)0.0;
	(*profile).obs_log.meteo.press=(float)0.0;
	(*profile).obs_log.tau0=(float)0.0;
	strcpy((*profile).obs_log.memo,"Imported");
	(*profile).obs_log.comment[0]=NULL;

	(*profile).history[0].size=(unsigned char)1;
	strcpy((*profile).history[0].step,"END");
	time(&now);
	(*profile).history[0].time=now;

	for(ii=0;ii<MAX_FLAG_CH;ii++)
		(*profile).spurious[ii]=0x00;
	for(ii=0;ii<MAX_FLAG_CH;ii++)
		(*profile).baseline[ii]=0x00;
	for(ii=0;ii<MAX_CH;ii++)
		(*profile).data[ii]=(float)0.0;

	return(0);
}	/*	end of this function	*/
	


int get_ProfData(fits_head, profile, axis, position, fp_in)
FITS_HEAD	*fits_head;
PROFILE		*profile;
AXIS_INDEX	*axis;
XY_POS		*position;
FILE		*fp_in;

{
int	ii, jj;
int	data_bytepix;	/*	[byte]	*/
int	data_position;
int	offset;
int	header_record_length;
int	data_record_start;
int	err_code,blnk_flag;
char	c;
short	data_short;
long	data_long;
float	data_float;
unsigned int	array_pos[MAX_DIMENSION];


data_bytepix=(*fits_head).bitpix/8;	
array_pos[(*axis).x]=(unsigned int)(  ( (*position).x-(*fits_head).crval[(*axis).x] )
			/(*fits_head).cdelt[(*axis).x] 
				+ (*fits_head).crpix[(*axis).x]  );
array_pos[(*axis).y]=(unsigned int)(  ( (*position).y-(*fits_head).crval[(*axis).y] )
			/(*fits_head).cdelt[(*axis).y] 
				+ (*fits_head).crpix[(*axis).y]  );

fprintf(stderr,"LB position=(%lf, %lf)\n",(*position).x,(*position).y);	
fprintf(stderr,"Array position=(%d, %d)\n",array_pos[(*axis).x],array_pos[(*axis).y]);	


offset=ftell(fp_in);
header_record_length=(int)ceil((double)offset/((double)BIT_RECORD/8.0));
data_record_start=header_record_length*BIT_RECORD/8;
fprintf(stderr,"Header record length=%ld [record]\n",header_record_length);
fprintf(stderr,"Data record start position=%ld [Bytes]\n",data_record_start);

for(ii=1;ii<=(*fits_head).naxis[(*axis).v];ii++){
	array_pos[(*axis).v]=ii;
	data_position=data_bytepix * calc_DataPosition(fits_head,array_pos)
			 + data_record_start;
	if(ii==1){	
	fprintf(stderr,"Data start position=%ld [Bytes]\n",data_position);
	}
	fseek(fp_in,data_position,0);
	blnk_flag=0;
	if((*fits_head).bitpix==32){
		fread(&data_long,sizeof(data_long),1,fp_in);
		data_float=(float)data_long;
		if(data_long==(*fits_head).blank)	blnk_flag=1;	
/*		fprintf(stderr,"data_position=%ld : data=%ld\n",
		data_position,data_long);
*/	}
	else{
		fread(&data_short,sizeof(data_short),1,fp_in);
		if((long)data_short==(*fits_head).blank)	blnk_flag=1;	
		data_float=(float)data_short;	
	}
	if(blnk_flag==1)
		(*profile).data[ii]=(float)DEFAULT_BLANK_VALUE;
	else
		(*profile).data[ii]=(float)(*fits_head).bscale*data_float
			+(float)(*fits_head).bzero;
}


return(0);
}




int calc_DataPosition(fits_head,array_pos)
FITS_HEAD	*fits_head;
unsigned int	array_pos[];
{
int	ii,jj;
double	sum, mult;

sum=0.0;
for(ii=1;ii<=(*fits_head).naxis[0];ii++){
	mult=1.0;
	for(jj=1;jj<ii;jj++){
		mult=mult * (double)((*fits_head).naxis[jj]);
	}
	mult=mult * ( (double)array_pos[ii]-1.0);
	sum=sum+mult;
}
return((int)sum);

}

	
int get_ProfHeader(fits_head, profile, axis, position, fp_in)
FITS_HEAD	*fits_head;
PROFILE		*profile;
AXIS_INDEX	*axis;
XY_POS		*position;
FILE		*fp_in;
{
char	buffer[256];
char 	*token;
int	ii;
int	err_code;
unsigned short	bs_ch, sp_ch;
unsigned short	flag_byte;


	
err_code=default_ProfHeader(profile);
if(err_code!=0)		return(10);

err_code=read_FitsHeader(fits_head, fp_in);
if(err_code!=0)		return(20);

/*	座標軸の解釈	*/

for(ii=1;ii<=(*fits_head).naxis[0];ii++){
	if(strncmp((*fits_head).ctype[ii],"VELO-LSR",8)==0)	
		(*axis).v=ii;
	else if(strncmp((*fits_head).ctype[ii],"GLAT",4)==0)	
		(*axis).y=ii;
	else if(strncmp((*fits_head).ctype[ii],"GLON",4)==0)	
		(*axis).x=ii;
}


/* only for the CfA CO(J=1-0) survey */
	(*profile).ant_status.hpbw=(float)9.0;
	(*profile).rx_status.rest_freq=115.271204;
	(*profile).rx_status.obs_freq=(*profile).rx_status.rest_freq;

/* the number of BackEnd chanels */

	strncpy((*profile).ant_status.tele,(*fits_head).telescop,8); 
	(*profile).be_status.be_ch=(*fits_head).naxis[(*axis).v];
	(*profile).be_status.data_ch=(*profile).be_status.be_ch;
	if((*profile).be_status.data_ch>MAX_CH) return(100);
	sp_ch=((*profile).be_status.data_ch-1)
		/(sizeof((*profile).spurious[0])*8)+1;
	bs_ch=sp_ch;
	flag_byte=sp_ch*sizeof((*profile).spurious[0])
                +bs_ch*sizeof((*profile).baseline[0]);
	(*profile).sys_head.l_head
		=(*profile).sys_head.l_head+flag_byte;
	(*profile).sys_head.l_file
		=(*profile).sys_head.l_head
		+(*profile).be_status.data_ch
		*sizeof((*profile).data[0]);


/* freq resolution of the BackEnd [MHz] */
	(*profile).be_status.freq_res=(float)((*fits_head).cdelt[(*axis).v]/(LIGHT_SPEED*1.0e3)*(*profile).rx_status.rest_freq*1.0e3);
	if((*fits_head).cdelt[(*axis).v]<10.0){
	/*	速度が[km/s]で表示されている場合への対処	*/
		(*profile).be_status.freq_res=
			(*profile).be_status.freq_res*1e3;
	}
	(*profile).be_status.disp0=0.0;
	(*profile).be_status.disp1=(*profile).be_status.freq_res;
	(*profile).be_status.disp2=0.0;
	(*profile).be_status.disp3=0.0;
	(*profile).be_status.disp4=0.0;

	(*profile).be_status.center_freq=0.0;
	(*profile).be_status.basis_ch=0;

	(*profile).be_status.sign=(-1);

/* velocity of the 1st ch [km/s] */
	(*profile).rx_status.v_lsr=((*fits_head).crval[(*axis).v]-0.0*(*fits_head).cdelt[(*axis).v])/1.0e3;

/* delta_l (incliment of galactic logitude in deg) */
/*	delta_l=(*fits_head).cdelt[(*axis).x];	*/

/* delta_b (incliment of galactic latitude in deg) */
/*	delta_b=(*fits_head).cdelt[(*axis).y];	*/

/* object */
	strcpy((*profile).object.name,(*fits_head).object);
	(*profile).object.on_lb.x=(*position).x;
	(*profile).object.on_lb.y=(*position).y;

return(0);
}


int options(argc,argv,in_filename,out_filename,position)
int	argc;
char	*argv[];
char	in_filename[];
char	out_filename[];
XY_POS	*position;
{
	int ii,jj;
	int	err_code;
	char	*token;


/* default values */
	
	in_filename[0]=NULL;	
	out_filename[0]=NULL;	

/* オプションスイッチを解釈し変数に代入する。 */
	for(ii=1;ii<argc;ii++)
	{
		if(argv[ii][0]=='-')	/* option switch */
		{
			if(strcmp(argv[ii],"-in")==0){
				ii++;
				strcpy(in_filename,argv[ii]);
			}
			else if(strcmp(argv[ii],"-out")==0){
				ii++;
				strcpy(out_filename,argv[ii]);
			}
			else if(strcmp(argv[ii],"-l")==0){
				ii++;
				(*position).x=atof(argv[ii]);
			}
			else if(strcmp(argv[ii],"-b")==0){
				ii++;
				(*position).y=atof(argv[ii]);
			}
			else if(strcmp(argv[ii],"-a")==0){
				ii++;
				(*position).x=atof(argv[ii]);
			}
			else if(strcmp(argv[ii],"-d")==0){
				ii++;
				(*position).y=atof(argv[ii]);
			}
		}
	}

return(0);
}	







int	read_FitsHeader(fits_head,fp_in)
FITS_HEAD 	*fits_head;
FILE		*fp_in;

{
int	ii;
char 	line[82];
char 	keyword[10];
char 	value[82];
char	comment[82];
char	buffer[82];

/*	Default Values	*/

(*fits_head).naxis[0]=0;
(*fits_head).bscale=1;
(*fits_head).bzero=0;

while(1){
	fread(line,80,1,fp_in);
	line[80]=NULL;
	fprintf(stderr,"%s\n",line);
	divide_Line(line,keyword,value,comment);
	if(strncmp(keyword,"SIMPLE",6)==0){
		if(strncasecmp(value,"T",1)==0)		(*fits_head).simple=1;
		else if(strncasecmp(value,"F",1)==0)	(*fits_head).simple=0;
		else					(*fits_head).simple=0;
	}
	if(strncmp(keyword,"BITPIX",6)==0)
		(*fits_head).bitpix=(unsigned short)atoi(value);		
	if((strncmp(keyword,"NAXIS",5)==0)&&(strlen(keyword)==5))
		(*fits_head).naxis[0]=(unsigned long)atol(value);	

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
		(*fits_head).blank=(long)atol(value);
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
		if(strncmp(keyword,buffer,strlen(buffer))==0)
			(*fits_head).cdelt[ii]=(double)atof(value);
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
	if(strncmp(keyword,"EPOCH",5)==0)		
		(*fits_head).epoch=(double)atof(value);
	if(strncmp(keyword,"END",3)==0){
		fprintf(stderr,"\n*****************************************\n");
		break;
	}

}	/*	while loop	*/







return(0);
}	/*	end of this function	*/



int	divide_Line(line, keyword, value, comment)
char	line[];
char	*keyword;
char	*value;
char	*comment;
{
char	*token;
char	*remnant;
char	*buffer;


if(strncmp(line,"COMMENT",7)==0){
	value=NULL;
	buffer=strtok(line," ");
	rm_Quot(buffer,keyword);
	buffer=strtok(NULL,"");
	strcpy(comment,buffer);
}	/*	if COMMENT	*/	

else{	

if(strpbrk(line,"/")==NULL){
	if(strpbrk(line,"=")==NULL){
		buffer=strtok(line," ");
		rm_Quot(buffer,keyword);
		value=NULL;
		comment=NULL;
	}
	else{
		buffer=strtok(line,"=");
		rm_Quot(buffer,keyword);
		buffer=strtok(NULL,"=");
		rm_Quot(buffer,value);
		comment=NULL;
	}
}
else{
	remnant=strtok(line,"/");
	buffer=strtok(NULL,"/");
	strcpy(comment,buffer);
	if(strpbrk(remnant,"=")==NULL){
		strncpy(buffer,remnant,8);
		rm_Quot(buffer,keyword);
		value=NULL;
	}
	else{
		buffer=strtok(remnant,"=");
		rm_Quot(buffer,keyword);
		buffer=strtok(NULL,"=");
		rm_Quot(buffer,value);
	}
}

}	/*	else COMMENT	*/


return(0);
}	/*	end of this function	*/



int rm_Quot(original, compressed)
char	original[];
char	compressed[];
{
int	ii, jj;

ii=0;	jj=0;
while(original[ii]!=NULL){
	if((original[ii]!=' ')&&(original[ii]!='\'')&&(original[ii]!='\"')){			compressed[jj]=original[ii];
		ii++;	jj++;
	}
	else	ii++;

}	/*	while loop	*/
compressed[jj]=NULL;

return(0);
}	/*	end of this function	*/






	
