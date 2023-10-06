#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>

#include <fitshead.h>

#define		MAX_FGET_LENGTH	81920
#define		MAX_COL		2048
#define		MAX_ROW		2048

#define	MAX_CHAR_LENGTH	256
#define		MAX_BRANGE	10


typedef struct{
    int		min[MAX_BRANGE];
    int		max[MAX_BRANGE];
    int		num;
} RANGE ;

typedef struct{
    double	min[MAX_BRANGE];
    double	max[MAX_BRANGE];
    int		num;
} DRANGE;



int	read_matrix(FILE *in, double x[], double y[], int *inum, int *jnum)
{
int	ii, jj;
char	buffer[MAX_FGET_LENGTH], *token;
double	z;

fgets(buffer,MAX_FGET_LENGTH-1,in);
    //fprintf(stderr,"%s\n",buffer);	

ii=0;
token=strtok(buffer,"\t\n ,");
x[ii]=atof(token);	
    //fprintf(stderr,"%lf\t%s\t%d\n",x[ii],token,ii);
ii++;
while(1){
	token=strtok(NULL,"\t\n ,");
	if(token==NULL)	break;
	x[ii]=atof(token);
	// 	fprintf(stderr,"%lf\t%s\t%d\n",x[ii],token,ii);
	ii++;
}
*inum=ii;

for(jj=0;;jj++){
	if(fgets(buffer,MAX_FGET_LENGTH-1,in)==NULL)	break;
	token=strtok(buffer,"\t\n ,");
	if(token==NULL)	break;
	y[jj]=atof(token);
	for(ii=0;ii<*inum;ii++){
		token=strtok(NULL,"\t\n ,");
		z=atof(token);
	    //		fprintf(stdout,"%lf\t%lf\t%lf\n",x[ii],y[jj],z);
	}
}
*jnum=jj;

rewind(in);
return(0);
}



int	create_Fits(FITS *fits, double x[], double y[], int inum, int jnum, FILE *in)
{
int	ii, jj, vel;
DRANGE	drange;
double	g[MAX_DIMENSION];
char	grids[MAX_CHAR_LENGTH], axis[8];
double	fixed, tmp;
long	pos, data_num;
int	pix[MAX_DIMENSION];
double	xx[MAX_DIMENSION], sum, wgt, z, w;
char	buffer[MAX_FGET_LENGTH], *token;
float	*data;

(*fits).head.bitpix=16;
(*fits).head.naxis[0]=3;
(*fits).head.bscale=1.0;
(*fits).head.bzero=0.0;
(*fits).head.blank=DEFAULT_BLANK_VALUE;
strcpy((*fits).head.bunit,"\' \'");
strcpy((*fits).head.object,"\' \'");
strcpy((*fits).head.date_obs,"\' \'");
strcpy((*fits).head.date_map,"\' \'");
strcpy((*fits).head.origin,"\' \'");
strcpy((*fits).head.telescop,"\' \'");
strcpy((*fits).head.instrume,"\' \'");
strcpy((*fits).head.observer,"\' \'");
(*fits).head.epoch=1950.0;


fprintf(stderr,"Velocity axis (x/y) : ");
fscanf(stdin,"%s",axis);
if(strncasecmp(axis,"y",1)==0)		vel=2;
else if(strncasecmp(axis,"x",1)==0)	vel=1;

fprintf(stderr,"Fixed Value (axis 2) : ");
fscanf(stdin,"%lf",&fixed);

fprintf(stderr,"Grid spacings : ");
fscanf(stdin,"%s",grids);	
token=strtok(grids,", \t");	
    for(jj=0;jj<(*fits).head.naxis[0];jj++){
	if(token!=NULL)	g[jj]=atof(token);
	token=strtok(NULL,", \t");	
}

if(vel==2){
    drange.min[0]=x[0];	drange.max[0]=x[inum-1];
    drange.min[1]=fixed;	drange.max[1]=fixed;   
    drange.min[2]=y[0];	drange.max[2]=y[jnum-1];   
}
else if(vel==1){
    drange.min[0]=y[0];	drange.max[0]=y[jnum-1];
    drange.min[1]=fixed;	drange.max[1]=fixed;   
    drange.min[2]=x[0];	drange.max[2]=x[inum-1];   
}

/*
fprintf(stderr,"%lf\t%lf\t%d\n",x[0],x[inum-1],inum);
fprintf(stderr,"%lf\t%lf\t%d\n",y[0],y[jnum-1],jnum);
*/

for(jj=0;jj<(*fits).head.naxis[0];jj++){
    if(drange.max[jj]<drange.min[jj]){tmp=drange.max[jj]; drange.max[jj]=drange.min[jj]; drange.min[jj]=tmp;}
    //    fprintf(stderr,"%d\t%lf\t%lf\n",jj,drange.min[jj],drange.max[jj]);
}


for(jj=0;jj<(*fits).head.naxis[0];jj++){
	if(g[jj]==0.0)	g[jj]=1.0;
	(*fits).head.naxis[jj+1]=tint((drange.max[jj]-drange.min[jj])/g[jj])+1;
    	(*fits).head.crval[jj+1]=floor((drange.min[jj]+drange.max[jj])/2.0);
	(*fits).head.crpix[jj+1]=(double)tint((*fits).head.naxis[jj+1]/2.0);
	(*fits).head.cdelt[jj+1]=g[jj];
	(*fits).head.crota[jj+1]=0.0;
}

(*fits).head.ctype[1]=malloc(5);	strcpy((*fits).head.ctype[1],"GLON");
(*fits).head.ctype[2]=malloc(5);	strcpy((*fits).head.ctype[2],"GLAT");
(*fits).head.ctype[3]=malloc(8);	strcpy((*fits).head.ctype[3],"LSR-VEL");

data_num=1;
for(jj=1;jj<=(*fits).head.naxis[0];jj++)
    data_num*=(*fits).head.naxis[jj];

(*fits).data=(float *)calloc(data_num,sizeof((*fits).data));
data=(float *)calloc((inum*jnum),sizeof(data));

fgets(buffer,MAX_FGET_LENGTH-1,in);
for(jj=0;jj<jnum;jj++){
	if(fgets(buffer,MAX_FGET_LENGTH-1,in)==NULL)	break;
	token=strtok(buffer,"\t\n ,");
	if(token==NULL)	break;
	y[jj]=atof(token);
	for(ii=0;ii<inum;ii++){
	    token=strtok(NULL,"\t\n ,");
	    *(data+jj*inum+ii) = (float)atof(token);
	    if(vel==2){
		xx[0]=x[ii];	xx[1]=fixed;	xx[2]=y[jj];
	    }
	    else if(vel==1){
		xx[0]=y[jj];	xx[1]=fixed;	xx[2]=x[ii];
	    }
	    TXtoPIX(fits,xx,pix);
	    (*fits).data[PIXtoPOS(fits,pix)]= *(data+jj*inum+ii);
	    //	    fprintf(stdout,"%lf\t%lf\t%lf\n",xx[0],xx[1],xx[2]);
	}
}

return(0);
}






main(int argc, char *argv[])
{
int	ii, jj;
int	inum, jnum;
char	buffer[MAX_FGET_LENGTH], *token;
double	x[MAX_COL], y[MAX_ROW], z;
FILE	*in, *out;
FITS	fits;
char	in_filename[MAX_CHAR_LENGTH], out_filename[MAX_CHAR_LENGTH];
long	pos, data_num;


fprintf(stderr,"Input matrix filename : ");
fscanf(stdin,"%s",in_filename);
in=fopen(in_filename,"r");
if(in==NULL){
    fprintf(stderr,"Can't open %s !\n",in_filename);
    exit(1);
}

sprintf(out_filename,"%s.%s",in_filename,"fits");
out=fopen(out_filename,"w");
if(out==NULL){
    fprintf(stderr,"Can't open %s !\n",out_filename);
    exit(1);
}



read_matrix(in,x,y,&inum,&jnum);
fprintf(stderr,"inum=%d\tjnum=%d\n",inum,jnum);

create_Fits(&fits,x,y,inum,jnum,in);

fprintf(stderr,"Wrinting to %s\n",out_filename);
output_Fits(&fits, out);

fclose(in);
fclose(out);
}
