/*	Draw a color image with PGPLOT 	*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>

/*#include <profhead.h>
#include <starlib.h>*/
#include <fitshead.h>
#include <cpgplot.h>

#define	DEBUG	0
#define	MAX_CHAR_LENGTH	256


double	trunc(x)
double	x;
{
    double	lx, rx, y;
    lx=floor(log10(fabs(x)));
    if(x>=0.0)	rx=floor(x/pow(10.0,lx));
    else	rx=ceil(x/pow(10.0,lx));
    y=rx*pow(10.0,lx);
    return(y);
}


void	options(argc,argv,fh1,fh2,slice,chmin,chmax,device)
int	argc;
char	*argv[];
int	*slice, *chmin, *chmax;
char	fh1[], fh2[], device[];
{
int 	ii, jj, tmp;
int	err_code;
char	*token;
char	buffer[80];

/* default values */
fh1[0]='\0';
fh2[0]='\0';
strcpy(device,"/xw");
*slice=(-1);
*chmin=(-1); *chmax=(-1);

/* interpret options */
for(ii=1;ii<argc;ii++){
    	if(argv[ii][0]=='-'){
	    if(strcmp(argv[ii],"-d")==0){
		ii++;
		strcpy(device,argv[ii]);
	    }
	    else if(strcmp(argv[ii],"-s")==0){
		ii++;
		*slice=atoi(argv[ii]);
	    }
	    else if(strcmp(argv[ii],"-ch")==0){
	    	ii++;
		token=strtok(argv[ii],",");
		if(token!=NULL)	*chmin=atoi(token);
		else	continue;
		token=strtok(NULL,",");
		if(token!=NULL)	*chmax=atoi(token);
		else	continue;
		if(*chmin>*chmax){tmp=*chmin; *chmin=*chmax; *chmax=tmp;}
	    }
	    else if(strcmp(argv[ii],"-f1")==0){
		ii++;
		strcpy(fh1,argv[ii]);
	    }
	    else if(strcmp(argv[ii],"-f2")==0){
		ii++;
		strcpy(fh2,argv[ii]);
	    }
	}
 	else	continue;
}

}



void	palette(type, contra, bright)
int	type;
float	contra, bright;
{
float 	rl[9]={-0.5,0.0,0.17,0.33,0.50,0.67,0.83,1.0,1.7},
    	rr[9]={0.0,0.0,0.0,0.0,0.6,1.0,1.0,1.0,1.0},
    	rg[9]={0.0,0.0,0.0,1.0,1.0,1.0,0.6,0.0,1.0},
    	rb[9]={0.0,0.3,0.8,1.0,0.3,0.0,0.0,0.0,1.0};

float	al[20]={0.0,0.1,0.1,0.2,0.2,0.3,0.3,0.4,0.4,0.5,0.5,0.6,0.6,0.7,0.7,0.8,0.8,0.9,0.9,1.0},
    	ar[20]={0.0,0.0,0.3,0.3,0.5,0.5,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,1.0,1.0,1.0,1.0,1.0},
    	ag[20]={0.0,0.0,0.3,0.3,0.0,0.0,0.0,0.0,0.8,0.8,0.6,0.6,1.0,1.0,1.0,1.0,0.8,0.8,0.0,0.0},
    	ab[20]={0.0,0.0,0.3,0.3,0.7,0.7,0.7,0.7,0.9,0.9,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};	


if(type==2)
    cpgctab(rl, rr, rg, rb, 9, contra, bright);
/*else if(type==3)
    cpgctab(hl, hr, hg, hb, 5, contra, bright);
*/
else if(type==5)
    cpgctab(al, ar, ag, ab, 20, contra, bright); 
}



void	pgplot_setup(device,d1,d2)
char	device[];
int	d1, d2;
{
cpgopen(device);
cpgsubp(d1,d2);
cpgask(0);
cpgsch(1.5);
}




int	plot_fits(fits, min, max, title, wdg, cp)
FITS	*fits;
double	min, max;
char	title[];
int	wdg, cp;
{
int	i, j, ii, jj;
float	*ta;
float	xmin, ymin, xmax, ymax, buff;
float	xrange, yrange;
float	ixmin, iymin, ixmax, iymax;
int	ixrange, iyrange, ixtic, iytic;
char	xlabel[MAX_CHAR_LENGTH], ylabel[MAX_CHAR_LENGTH];
double	az, el, dd;
int	pix[MAX_DIMENSION];
float	fg, bg, tr[6];

if((*fits).head.naxis[0]!=2)	return(1);


ta = calloc((*fits).head.naxis[1]*(*fits).head.naxis[2],sizeof(ta));

xmin=(*fits).head.crval[1]+(*fits).head.cdelt[1]*(1-(*fits).head.crpix[1]);
xmax=(*fits).head.crval[1]+(*fits).head.cdelt[1]*((*fits).head.naxis[1]-(*fits).head.crpix[1]);
ymin=(*fits).head.crval[2]+(*fits).head.cdelt[2]*(1-(*fits).head.crpix[2]);
ymax=(*fits).head.crval[2]+(*fits).head.cdelt[2]*((*fits).head.naxis[2]-(*fits).head.crpix[2]);
if(xmin>xmax){buff=xmin; xmin=xmax; xmax=buff;}
if(ymin>ymax){buff=ymin; ymin=ymax; ymax=buff;}
 
xrange=xmax-xmin;	ixrange=(int)xrange;
yrange=ymax-ymin;	iyrange=(int)yrange;
ixmin=(xmin-xrange/20.0); ixmax=(xmax+xrange/20.0); 
iymin=(ymin-yrange/20.0); iymax=(ymax+yrange/20.0);
ixtic=(int)(pow(10.0,floor(log10(xrange))));	
if((ixrange/ixtic)>=3)	ixtic=ixtic*2;
if((ixrange/ixtic)<=1)	ixtic=ixtic/2;

iytic=(int)(pow(10.0,floor(log10(yrange))));	
if((iyrange/iytic)>=3)	iytic=iytic*2;
if((iyrange/iytic)<=1)	iytic=iytic/2;

sprintf(xlabel,"%s [deg]",(*fits).head.ctype[1]);
sprintf(ylabel,"%s [deg]",(*fits).head.ctype[2]);

for(pix[0]=0;pix[0]<(*fits).head.naxis[1];pix[0]++){
for(pix[1]=0;pix[1]<(*fits).head.naxis[2];pix[1]++){
    if((*fits).data[PIXtoPOS(fits,pix)]>=fabs((*fits).head.datamin))
 	*(ta + pix[1]*(*fits).head.naxis[1] + pix[0]) = (float)(*fits).data[PIXtoPOS(fits,pix)];
    else	*(ta + pix[1]*(*fits).head.naxis[1] + pix[0]) = (-5.0)*fabs((*fits).head.datamin);
    //    fprintf(stderr,"%lf\n",*(ta + pix[0]*(*fits).head.naxis[2] + pix[1]));
}}

if((strstr((*fits).head.ctype[1],"LON")!=0)||(strstr((*fits).head.ctype[1],"RA")!=0)){
    buff=ixmin; ixmin=ixmax; ixmax=buff;
}

cpgeras();
    //cpgbbuf();
    //cpgsave();
cpgsvp(0.2, 0.85, 0.2, 0.85);
cpgswin(ixmin,ixmax,iymin,iymax);

cpglab(xlabel, ylabel, title);
cpgbox("BCINTS",ixtic,5,"BCINTS",iytic,5);


fg=min;
bg=max;


tr[0]=(*fits).head.crval[1]+(*fits).head.cdelt[1]*(1-(*fits).head.crpix[1]);
tr[1]=(*fits).head.cdelt[1];  
tr[2]=0.0;
tr[3]=(*fits).head.crval[2]+(*fits).head.cdelt[2]*(1-(*fits).head.crpix[2]);
tr[4]=0.0;
tr[5]=(*fits).head.cdelt[2];


cpgsci(1);
palette(cp,1.0,0.5);

if(wdg==1)	cpgwedg("RI",1.0,2.0,fg,bg,(*fits).head.bunit);
cpgimag(ta,(*fits).head.naxis[1],(*fits).head.naxis[2],
	1,(*fits).head.naxis[1],1,(*fits).head.naxis[2],
	fg, bg, tr);
    //cpgebuf();

return(0);
}









main(argc,argv)
int	argc;

char	*argv[];
{
FILE	*fp_in1, *fp_in2, *fp_out;
int	err_code;
int	ii, jj, id1, id2, pnum, ch;
FITS	fits1, fits2, fitsn;
int	slice, chmin, chmax, wdg, cp;	
char	filehead1[80],filehead2[80];
char	device[MAX_CHAR_LENGTH], dummy[MAX_CHAR_LENGTH], title[MAX_CHAR_LENGTH];
double	zz, aspect, tmp, dmax;
char	buffer[MAX_CHAR_LENGTH], *token;



options(argc,argv,filehead1,filehead2,&slice,&chmin,&chmax,device);


if(filehead1[0]=='\0'){
fprintf(stderr,"Input FITS(1) filename : ");
fscanf(stdin,"%s",filehead1);}
fp_in1=open_Fits(filehead1);
if(fp_in1==NULL)	exit(1);
fp_out=stdout;

err_code=read_Fits(&fits1, fp_in1);
if(err_code!=0){
	fprintf(stderr,"Error in reading the FITS(1) file. Error code=%d\n",err_code);	
	exit(999);
}

if(filehead2[0]=='\0'){
fprintf(stderr,"Input FITS(2) filename : ");
fscanf(stdin,"%s",filehead2);}
fp_in2=open_Fits(filehead2);
if(fp_in2==NULL)	exit(1);

err_code=read_Fits(&fits2, fp_in2);
if(err_code!=0){
	fprintf(stderr,"Error in reading the FITS(1) file. Error code=%d\n",err_code);	
	exit(999);
}


if(slice<0){
    fprintf(stderr,"Slicing dimension (1-%d): ",fits1.head.naxis[0]);
    fscanf(stdin,"%d",&slice);
}
again:;
if((chmin<0)||(chmax<0)){
    fprintf(stderr,"Channel range (1-%d; min,max): ",fits1.head.naxis[slice]);
    fscanf(stdin,"%s",buffer);
    token=strtok(buffer,", \t");
    if(token==NULL)	goto again;
    chmin=atoi(token);
    token=strtok(NULL,", \t");
    if(token==NULL)	goto again;
    chmax=atoi(token);   
    if(chmin>chmax){tmp=chmin; chmin=chmax; chmax=tmp;}
}



pnum=(chmax-chmin)+1;
id1=(int)ceil(pow((double)pnum*2,0.5));
id2=(int)ceil((double)pnum*2/(double)id1);
if(fmod((double)id1,2)!=0)	id1++;

fprintf(stderr,"dim=(%d,%d)\n",id1,id2);

pgplot_setup(device,id1,id2);


for(ii=1;ii<=id1*2;ii++){
for(jj=1;jj<=id2;jj++){
    wdg=0; cp=2;
    ch=(ii-1)/2*id2+(jj-1)+chmin;	
    if(ch>chmax)	break;
    fprintf(stderr,"%d %d : %d\n",ii,jj,ch);

    if(fmod((double)ii,2)==1){
	zz = fits1.head.crval[slice]+fits1.head.cdelt[slice]*(ch-fits1.head.crpix[slice]);
	sprintf(title,"%s:%s=%lf",filehead1,fits1.head.ctype[slice],zz);
	dmax=0.9*fits1.head.datamax;
	copy_Fits(&fits1,&fitsn);
    }
    else{
	zz = fits2.head.crval[slice]+fits2.head.cdelt[slice]*(ch-fits2.head.crpix[slice]);
	sprintf(title,"%s:%s=%lf",filehead2,fits2.head.ctype[slice],zz);
	dmax=0.9*fits2.head.datamax;
	copy_Fits(&fits2,&fitsn);
    }
    slice_Fits(&fitsn,slice,ch);
//	output_Fits(&fits2,stdout);
    if((ii==1)&&(jj==1)){
	aspect=(double)fitsn.head.naxis[2]/(double)fitsn.head.naxis[1];
	cpgpap(0.0,(float)aspect);
//	fprintf(stderr,"aspect=%lf\t%d %d\n",aspect,fitsn.head.naxis[1],fitsn.head.naxis[2]);
    }
    if(ch==chmin)	wdg=1;
    
    cpgpanl(ii,jj);
    plot_fits(&fitsn,0.0,dmax,title,wdg,cp);
    
}}


if(strcmp(device,"/xw")==0)	sleep(20);


cpgclos();


}	/*main*/


