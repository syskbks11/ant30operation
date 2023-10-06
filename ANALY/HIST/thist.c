/*
cc -o thist thist.c -lm
*/



#include	<stdio.h>
#include	<math.h>

#define		NUM	80

main()
{
FILE	*fp;
int		ii;
int		n[NUM];
double	l,v,t21,t10,r;
double	I10[NUM],I21[NUM],R,err_R;
double	baseline,vwidth;
char	buffer[80],data_file[80];

fprintf(stderr,"Input filename : ");
fscanf(stdin,"%s",data_file);

fp=fopen(data_file,"rb");
if(fp==NULL){
	fprintf(stderr,"Can't open %s.\n",data_file);
	exit(999);
}

fprintf(stderr,"Velocity binning width of input file: ");
fscanf(stdin,"%s",buffer);
vwidth=atof(buffer);

fprintf(stderr,"Baseline accuracy : ");
fscanf(stdin,"%s",buffer);
baseline=atof(buffer);


for(ii=0;ii<20;ii++){
	I10[ii]=0.0;
	I21[ii]=0.0;
	n[ii]=0;
	fseek(fp,0L,0);
	fprintf(stderr,"Temperature range : %d to %d\n",ii,ii+1);
	while(fscanf(fp,"%lf %lf %lf %lf %lf",&l,&v,&t10,&t21,&r)==5){
		if(((double)ii<=t10)&&(t10<(double)(ii+1))){
			I10[ii]=I10[ii]+t10*vwidth;
			I21[ii]=I21[ii]+t21*vwidth;
			n[ii]=n[ii]+1;
		}
	}
}
fclose(fp);

for(ii=0;ii<20;ii++){
	R=I21[ii]/I10[ii];
	err_R=baseline*vwidth*pow((double)n[ii],0.5)/I10[ii]*1.41421356;
	fprintf(stdout,"%lf\t%lf\t%lf\t%lf\t%lf\n",
		(double)ii,I10[ii],I21[ii],R,err_R);
}

}
