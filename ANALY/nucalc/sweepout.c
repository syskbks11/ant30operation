#include	<math.h>
#include	<stdio.h>
#include	<nucalc.h>

#define		DEBUG	0

short		sweepout(a,b,n,m,eps)

double			*a,	*b;
unsigned short		n,m;
double			eps;

{
unsigned short	i,j,k,p,q,iw;
unsigned short	iwork[SWEEPOUT_MAX_DIM];
double		buffer[SWEEPOUT_MAX_DIM];
double		w,max;


#if DEBUG
for(i=0;i<m;i++){
	for(j=0;j<m;j++){
		fprintf(stderr,"%5.2lf ",*(a+i*n+j));
	}
	fprintf(stderr,"%5.2lf\n",*(b+i));
}
#endif


for(i=0;i<m;i++){
	iwork[i]=i;
}


for(k=0;k<m;k++){

	max=fabs(*(a+n*k+k));
	p=k;
	q=k;
	for(j=k;j<m;j++){
		for(i=k;i<m;i++){
			if(max<fabs(*(a+n*i+j))){
				max=fabs(*(a+n*i+j));
				p=i;
				q=j;
			}
		}
	}

	if(max<=eps){
/*		fprintf(stderr,"max=%e\n",max);
*/		return(999);
	}

	for(i=0;i<m;i++){
		w = *(a+n*i+k);
		*(a+n*i+k) = *(a+n*i+q);
		*(a+n*i+q) = w;
	}

	for(j=0;j<m;j++){
		w = *(a+n*k+j);
		*(a+n*k+j) = *(a+n*p+j);
		*(a+n*p+j) = w;
	}
		w = *(b+k);
		*(b+k) = *(b+p);
		*(b+p) = w;
	
	i=iwork[k];
	iwork[k]=iwork[q];
	iwork[q]=i;

	
	for(j=k+1;j<m;j++){
		*(a+n*k+j) = (*(a+n*k+j)) 
								/ (*(a+n*k+k));
	}
		*(b+k) = (*(b+k)) / (*(a+n*k+k));
		
	for(i=0;i<m;i++){
		if(i!=k){
			for(j=k+1;j<m;j++){
				*(a+n*i+j) = *(a+n*i+j) 
				- (*(a+n*i+k)) * (*(a+n*k+j));
			}
				*(b+i)=(*(b+i))
				- (*(a+n*i+k)) * (*(b+k));
		}
		
	}
}



for(i=0;i<m;i++){
	iw=iwork[i];
	buffer[iw]= *(b+i);
}
for(i=0;i<m;i++){
	*(b+i)=buffer[i];
}

#if DEBUG
for(i=0;i<m;i++){
	for(j=0;j<m;j++){
		fprintf(stderr,"%5.2lf ",*(a+i*n+j));
	}
	fprintf(stderr,"%5.2lf\n",*(b+i));
}
#endif


return(0);

}
