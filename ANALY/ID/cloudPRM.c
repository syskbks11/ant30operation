/*	Cloud indntify code
1998/3/17	Tomo OKA	Coding	
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0
#define	MAX_CLOUD_NUM	1024
#define	DISTANCE	8.5e3
#define	FP	2.9

typedef struct	{	int	dim;
			double	peak[MAX_DIMENSION];
			double	cent[MAX_DIMENSION];
			double	disp[MAX_DIMENSION];
			double	peak_val;
			double	bound;
			double	size;
			double	quad;
			double	mvt;
			double	lumi;			
		} CLOUD;


int	cloudID();
long	fill_a_cloud();
long	fill_2D();
long	SortFillMatrix();
int	PhysParams();
int	OutputCloudParams();

main(argc,argv)
int	argc;
char	*argv[];
{
FILE		*fp_in, *fp_out, *fp_cloud;
int		err_code;
FITS		fits;
int		i, j, ii, jj;
double		w[MAX_DIMENSION];
int		dim, count;
char		in_filename[80];
char		out_filename[80];
char		filehead[80];
char		buffer[80];
char		*token;
int		clip, crit;
double		bound, peak;
double		distance, xp[MAX_DIMENSION];
CLOUD		cloud[MAX_CLOUD_NUM];



/*	Input FITS filename	*/
fprintf(stderr,"Input FITS filename : ");
fscanf(stdin,"%s",buffer);
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


/*	Distance to the Source	*/
fprintf(stderr,"Distance to the Source [pc]: ");
fscanf(stdin,"%lf",&distance);


/*	Reading FITS file	*/
fprintf(stderr,"Reading %s\n",in_filename);
err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,
	"Error in reading the FITS file. Error code=%d\n"
	,err_code);	
	exit(999);
}
fclose(fp_in);

if((fits.head.naxis[0]!=2)&&(fits.head.naxis[0]!=3)){
	fprintf(stderr,"Input FITS must be 2 or 3 dimension!\n");
	return(1);
}


/*	Cloud Center	*/
again:;
fprintf(stderr,"Cloud Center: ");
fscanf(stdin,"%s",buffer);
token=strtok(buffer,", \t");
for(jj=0;jj<fits.head.naxis[0];jj++){
	if(token==NULL){goto again;}
	else{	xp[jj]=atof(token);
		token=strtok(NULL,", \t");}	
}

/*	Cloud identification	*/
fprintf(stderr,"Identifying Clouds\n");
count=cloudID(&fits, xp, distance, cloud);

/*	Output cloud parameters	*/

for(i=0;i<count;i++){
	crit=0;	for(j=0;j<cloud[i].dim;j++){ if(cloud[i].disp[j]==0.0) crit=1;}
	if(crit==0)
	err_code=OutputCloudParams(&(cloud[i]),stdout);
}


}	/*main*/





int	cloudID(fits,xp,distance,cloud)
FITS	*fits;
double	distance, xp[];
CLOUD	cloud[];
{
int	i, j, k, err_code, count;
int	pix[MAX_DIMENSION];
long	ii, jj, kk, n, nn;
long	*fill;
long	pos, max_ch, num;
float	max;	
double	pbratio;
double	bound;



nn=0;
fill=(long *)calloc((*fits).head.data_num*4,4);

err_code=XtoPIX(fits,xp,pix);
max=(*fits).data[PIXtoPOS(fits,pix)];
fprintf(stderr,"%lf\n",max);
bound=max*0.5;
nn=fill_a_cloud(fits,bound,pix,fill); 

/*	Search the intensity maximum	*/
for(ii=0;ii<nn;ii++){
	if((*fits).data[fill[ii]]>max){
		max=(*fits).data[fill[ii]];
		max_ch=fill[ii];	
	}
}
err_code=POStoPIX(fits,max_ch,pix);
err_code=PIXtoX(fits,xp,pix);



for(count=0;count<=7;count++){

for(i=0;i<(*fits).head.naxis[0];i++){	cloud[count].peak[i]=xp[i];	}
cloud[count].peak_val=max;
bound=max*(double)(9-count)/10.0;
cloud[count].bound=bound;

/*	Fill a cloud	*/
nn=fill_a_cloud(fits,bound,pix,fill); 

/*	Calculate physical parameters	*/
err_code=PhysParams(fits,bound,distance,pix,fill,nn,&(cloud[count]));

/*	Output cloud parameters to stderr	*/
err_code=OutputCloudParams(&(cloud[count]),stderr);

}	/*	count loop	*/
free(fill);
return(count);
}	/*	cloudID		*/





long	fill_a_cloud(fits,bound,max_pix,fill)
FITS	*fits;
double	bound;
int	max_pix[];
long	*fill;
{
int	i, j, err_code;
int	pix[MAX_DIMENSION];
int	pix_min[MAX_DIMENSION], pix_max[MAX_DIMENSION], pix_cen[MAX_DIMENSION];
long	n, nn;
float	cent1, cent2, weight, data;

n=0;
if((*fits).head.naxis[0]==2)
	n=fill_2D(fits,bound,max_pix,2,fill);

else if((*fits).head.naxis[0]==3){
for(i=0;i<(*fits).head.naxis[0];i++){pix[i]=max_pix[i];}
for(pix[2]=max_pix[2];pix[2]<(*fits).head.naxis[3];pix[2]++){
	nn=fill_2D(fits,bound,pix,2,(fill+n));
	if(nn==0)	break;
	n=n+nn; 
	cent1=0.0; cent2=0.0;  weight=0.0;
	for(i=1;i<=nn;i++){
		data=(*fits).data[*(fill+n-i)];
		if(data<0.0)	continue;
		err_code=POStoPIX(fits,*(fill+n-i),pix);
		cent1=cent1+data*(float)pix[0];
		cent2=cent2+data*(float)pix[1];
		weight=weight+data;
	}	
	cent1=cent1/weight;	cent2=cent2/weight;	
	pix[0]=(int)cent1;	pix[1]=(int)cent2;
}
for(i=0;i<(*fits).head.naxis[0];i++){pix[i]=max_pix[i];}
for(pix[2]=max_pix[2]-1;pix[2]>=0;pix[2]--){
	nn=fill_2D(fits,bound,pix,2,(fill+n));
	if(nn==0)	break;
	n=n+nn; 
	cent1=0.0; cent2=0.0;  weight=0.0;
	for(i=1;i<=nn;i++){
		data=(*fits).data[*(fill+n-i)];
		if(data<0.0)	continue;
		err_code=POStoPIX(fits,*(fill+n-i),pix);
		cent1=cent1+data*(float)pix[0];
		cent2=cent2+data*(float)pix[1];
		weight=weight+data;
	}	
	cent1=cent1/weight;	cent2=cent2/weight;
	pix[0]=(int)cent1;	pix[1]=(int)cent2;
}/*	for(pix[2]	*/

for(pix[0]=max_pix[0];pix[0]<(*fits).head.naxis[1];pix[0]++){
	nn=fill_2D(fits,bound,pix,0,(fill+n));
	if(nn==0)	break;
	n=n+nn; 
	cent1=0.0; cent2=0.0;  weight=0.0;
	for(i=1;i<=nn;i++){
		data=(*fits).data[*(fill+n-i)];
		if(data<0.0)	continue;
		err_code=POStoPIX(fits,*(fill+n-i),pix);
		cent1=cent1+data*(float)pix[1];
		cent2=cent2+data*(float)pix[2];
		weight=weight+data;
	}	
	cent1=cent1/weight;	cent2=cent2/weight;	
	pix[1]=(int)cent1;	pix[2]=(int)cent2;
}
for(i=0;i<(*fits).head.naxis[0];i++){pix[i]=max_pix[i];}
for(pix[0]=max_pix[0]-1;pix[0]>=0;pix[0]--){
	nn=fill_2D(fits,bound,pix,0,(fill+n));
	if(nn==0)	break;
	n=n+nn; 
	cent1=0.0; cent2=0.0;  weight=0.0;
	for(i=1;i<=nn;i++){
		data=(*fits).data[*(fill+n-i)];
		if(data<0.0)	continue;
		err_code=POStoPIX(fits,*(fill+n-i),pix);
		cent1=cent1+data*(float)pix[1];
		cent2=cent2+data*(float)pix[2];
		weight=weight+data;
	}	
	cent1=cent1/weight;	cent2=cent2/weight;
	pix[1]=(int)cent1;	pix[2]=(int)cent2;
}/*	for(pix[0]	*/

n=SortFillMatrix(fill,n);


} /*	else if		*/

return(n);
}	/*	fill_a_cloud	*/





long	fill_2D(fits,bound,max_pix,fax,fill)
FITS	*fits;
double	bound;
int	max_pix[], fax;
long	*fill;
{
int	i, j;
int	pix[MAX_DIMENSION];
int	pix_min[MAX_DIMENSION], pix_max[MAX_DIMENSION], pix_cen[MAX_DIMENSION];
int	ax0, ax1;
long	n, nn;
float	center, weight, data;

if((*fits).data[PIXtoPOS(fits,max_pix)]<bound)	return(0);
if(fax==0){		ax0=1; ax1=2;	}
else if(fax==1){	ax0=0; ax1=2;	}
else if(fax==2){	ax0=0; ax1=1;	}
else	return(0);

n=0; if((*fits).head.naxis[0]>2) pix[fax]=max_pix[fax];
for(i=0;i<(*fits).head.naxis[0];i++){pix_cen[i]=max_pix[i];}
for(pix[ax1]=pix_cen[ax1];pix[ax1]<(*fits).head.naxis[ax1+1];pix[ax1]++){nn=0;
	for(pix[ax0]=pix_cen[ax0];pix[ax0]<(*fits).head.naxis[ax0+1];pix[ax0]++){
		if((*fits).data[PIXtoPOS(fits,pix)]<bound)	break;
		else{*(fill+n)=PIXtoPOS(fits,pix);	n++; nn++;}
	} pix_max[ax0]=pix[ax0]-1;
	for(pix[ax0]=pix_cen[ax0]-1;pix[ax0]>=0;pix[ax0]--){
		if((*fits).data[PIXtoPOS(fits,pix)]<bound)	break;
		else{*(fill+n)=PIXtoPOS(fits,pix);	n++; nn++;}
	} pix_min[ax0]=pix[ax0]+1; 
	if(nn==0) break; 
	center=0.0; weight=0.0;
	for(pix[ax0]=pix_min[ax0];pix[ax0]<=pix_max[ax0];pix[ax0]++){
		data=(*fits).data[PIXtoPOS(fits,pix)]/(*fits).head.datamax;
		if(data<0.0)	continue;
		center=center+data*(float)pix[ax0];
		weight=weight+data;
	}
	center=center/weight;	pix_cen[ax0]=(int)center;
} pix_max[ax1]=pix[ax1];

for(i=0;i<(*fits).head.naxis[0];i++){pix_cen[i]=max_pix[i];}
for(pix[ax1]=pix_cen[ax1]-1;pix[ax1]>=0;pix[ax1]--){nn=0;
	for(pix[ax0]=pix_cen[ax0];pix[ax0]<(*fits).head.naxis[ax0+1];pix[ax0]++){
		if((*fits).data[PIXtoPOS(fits,pix)]<bound)	break;
		else{*(fill+n)=PIXtoPOS(fits,pix);	n++; nn++;}
	} pix_max[ax0]=pix[ax0]-1;
	for(pix[ax0]=pix_cen[ax0]-1;pix[ax0]>=0;pix[ax0]--){
		if((*fits).data[PIXtoPOS(fits,pix)]<bound)	break;
		else{*(fill+n)=PIXtoPOS(fits,pix);	n++; nn++;}
	} pix_min[ax0]=pix[ax0]+1; 
	if(nn==0) break; 
	center=0.0; weight=0.0;
	for(pix[ax0]=pix_min[ax0];pix[ax0]<=pix_max[ax0];pix[ax0]++){
		data=(*fits).data[PIXtoPOS(fits,pix)]/(*fits).head.datamax;
		if(data<0.0)	continue;
		center=center+data*(float)pix[ax0];
		weight=weight+data;
	}
	center=center/weight;	pix_cen[ax0]=(int)center;
} pix_min[ax1]=pix[ax1]; pix_cen[ax1]=(pix_max[ax1]+pix_min[ax1])/2;

/*	*/
for(i=0;i<(*fits).head.naxis[0];i++){pix_cen[i]=max_pix[i];}
for(pix[ax0]=pix_cen[ax0];pix[ax0]<(*fits).head.naxis[ax0+1];pix[ax0]++){nn=0;
	for(pix[ax1]=pix_cen[ax1];pix[ax1]<(*fits).head.naxis[ax1+1];pix[ax1]++){
		if((*fits).data[PIXtoPOS(fits,pix)]<bound)	break;
		else{*(fill+n)=PIXtoPOS(fits,pix);	n++; nn++;}
	} pix_max[ax1]=pix[ax1]-1;
	for(pix[ax1]=pix_cen[ax1]-1;pix[ax1]>=0;pix[ax1]--){
		if((*fits).data[PIXtoPOS(fits,pix)]<bound)	break;
		else{*(fill+n)=PIXtoPOS(fits,pix);	n++; nn++;}
	} pix_min[ax1]=pix[ax1]+1; 
	if(nn==0) break; 
	center=0.0; weight=0.0;
	for(pix[ax1]=pix_min[ax1];pix[ax1]<=pix_max[ax1];pix[ax1]++){
		data=(*fits).data[PIXtoPOS(fits,pix)]/(*fits).head.datamax;
		if(data<0.0)	continue;
		center=center+data*(float)pix[ax1];
		weight=weight+data;
	}
	center=center/weight;	pix_cen[ax1]=(int)center;
} pix_max[ax0]=pix[ax0];

for(i=0;i<(*fits).head.naxis[0];i++){pix_cen[i]=max_pix[i];}
for(pix[ax0]=pix_cen[ax0]-1;pix[ax0]>=0;pix[ax0]--){
	for(pix[ax1]=pix_cen[ax1];pix[ax1]<(*fits).head.naxis[ax1+1];pix[ax1]++){nn=0;
		if((*fits).data[PIXtoPOS(fits,pix)]<bound)	break;
		else{*(fill+n)=PIXtoPOS(fits,pix);	n++; nn++;}
	} pix_max[ax1]=pix[ax1]-1;
	for(pix[ax1]=pix_cen[ax1]-1;pix[ax1]>=0;pix[ax1]--){
		if((*fits).data[PIXtoPOS(fits,pix)]<bound)	break;
		else{*(fill+n)=PIXtoPOS(fits,pix);	n++; nn++;}
	} pix_min[ax1]=pix[ax1]+1; 
	if(nn==0) break; 
	center=0.0; weight=0.0;
	for(pix[ax1]=pix_min[ax1];pix[ax1]<=pix_max[ax1];pix[ax1]++){
		data=(*fits).data[PIXtoPOS(fits,pix)]/(*fits).head.datamax;
		if(data<0.0)	continue;
		center=center+data*(float)pix[ax1];
		weight=weight+data;
	}
	center=center/weight;	pix_cen[ax1]=(int)center;
} pix_min[ax0]=pix[ax0]; pix_cen[ax0]=(pix_max[ax0]+pix_min[ax0])/2;

n=SortFillMatrix(fill,n);

return(n);
}	/*	fill_2D	*/




long	SortFillMatrix(fill,n)
long	*fill;
long	n;
{
long	ii, jj, new_n;
long	*fill_new, tmp;

fill_new=(long *)calloc(n,4);

for(ii=0;ii<n-1;ii++){
for(jj=ii+1;jj<n;jj++){
	if((*(fill+jj))<(*(fill+ii))){
		tmp=(*(fill+ii)); 
		(*(fill+ii))=(*(fill+jj)); 
		(*(fill+jj))=tmp;
	}
}}

(*(fill_new))=(*fill); new_n=1;

for(ii=1;ii<n;ii++){
	if((*(fill+ii))==(*(fill+ii-1)))	continue;
	else{	(*(fill_new+new_n))=(*(fill+ii));	new_n++;}
}
for(ii=0;ii<new_n;ii++){
	(*(fill+ii))=(*(fill_new+ii));
}
free(fill_new);

return(new_n);
}




int	PhysParams(fits,bound,distance,pix_cen,fill,nn,cloud)
FITS	*fits;
double	bound, distance;
int	pix_cen[];
long	*fill;
long	nn;
CLOUD	*cloud;
{
int	i, j, err_code;
int	pix[MAX_DIMENSION];
double	wint, lumi;
double	x[MAX_DIMENSION], xx[MAX_DIMENSION], xc[MAX_DIMENSION];
double	quad[MAX_DIMENSION][MAX_DIMENSION];
double	volume;
long	ii, jj;

bound=0.0;

PIXtoX(fits,xc,pix_cen);
for(i=0;i<(*fits).head.naxis[0];i++){	
	x[i]=0.0; 
	for(j=0;j<(*fits).head.naxis[0];j++){	quad[i][j]=0.0;	}		
}
lumi=0.0; 
for(ii=0;ii<nn;ii++){
	if(POStoPIX(fits,*(fill+ii),pix)!=0)	return(1);
	wint=((*fits).data[*(fill+ii)]-bound);
	lumi=lumi+wint;
	PIXtoX(fits,xx,pix);
	for(i=0;i<(*fits).head.naxis[0];i++){
		x[i]=x[i]+(xx[i]-xc[i])*wint;	
		for(j=0;j<(*fits).head.naxis[0];j++){	quad[i][j]=quad[i][j]+(xx[i]-xc[i])*(xx[j]-xc[j])*wint;	}		
	}
}
for(i=0;i<(*fits).head.naxis[0];i++){	
	x[i]=x[i]/lumi;	
	for(j=0;j<(*fits).head.naxis[0];j++){	quad[i][j]=quad[i][j]/lumi;	}		
}

(*cloud).dim=(*fits).head.naxis[0];
for(i=0;i<(*fits).head.naxis[0];i++){
	(*cloud).peak[i] = xc[i];
	(*cloud).cent[i] = x[i]+xc[i];
	(*cloud).disp[i] = pow((quad[i][i]-pow(x[i],2.0)),0.5);
}
(*cloud).size=distance*tan(pow((*cloud).disp[0]*(*cloud).disp[1],0.5)*(PI/180.0));
volume=1.0;for(i=1;i<=(*fits).head.naxis[0];i++){volume=volume*(*fits).head.cdelt[i];}
(*cloud).lumi=lumi*fabs(volume)*pow(distance*(PI/180.0),2.0);
if((*fits).head.naxis[0]>=3)
	(*cloud).mvt=3.0*FP*(*cloud).size*pow((*cloud).disp[2],2.0)*232.0;
else	(*cloud).mvt=0.0;

/*{double	qmax, q;	qmax=0.0;
for(i=0;i<(*fits).head.naxis[0]-1;i++){	
	for(j=i+1;j<(*fits).head.naxis[0];j++){
		q=fabs(quad[i][j]/pow(quad[i][i]*quad[j][j],0.5));	if(q>qmax) qmax=q;
	}
}(*cloud).quad=qmax;}*/

return(0);
}





int	OutputCloudParams(cloud,fp_cloud)
CLOUD	*cloud;
FILE	*fp_cloud;
{
int	i;
static int	c=0;

if((c==0)&&(fp_cloud==stdout)){
	for(i=0;i<(*cloud).dim;i++){fprintf(fp_cloud,"cent%d\t",i+1);}
	for(i=0;i<(*cloud).dim;i++){fprintf(fp_cloud,"disp%d\t",i+1);}
	fprintf(fp_cloud,"S\t");
/*	fprintf(fp_cloud,"q\t");*/
	fprintf(fp_cloud,"p_value\t");
	fprintf(fp_cloud,"bound\t");
	fprintf(fp_cloud,"Lco\t");
	fprintf(fp_cloud,"Mvt\n");
	c++;
}
for(i=0;i<(*cloud).dim;i++){
	fprintf(fp_cloud,"%lf\t",(*cloud).cent[i]);
}
for(i=0;i<(*cloud).dim;i++){
	fprintf(fp_cloud,"%lf\t",(*cloud).disp[i]);
}
fprintf(fp_cloud,"%lf\t",(*cloud).size);
/*fprintf(fp_cloud,"%.2lf\t",(*cloud).quad);*/
fprintf(fp_cloud,"%.3lf\t",(*cloud).peak_val);
fprintf(fp_cloud,"%.1lf\t",(*cloud).bound);
fprintf(fp_cloud,"%.4e\t",(*cloud).lumi);
fprintf(fp_cloud,"%.4e\n",(*cloud).mvt);

return(0);
}

