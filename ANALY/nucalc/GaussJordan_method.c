#include	<math.h>
#include	<nucalc.h>

short		GaussJordan_method(a,inv_a,n,eps)

double			*a,	*inv_a;
unsigned short		n;
double			eps;

{
unsigned short	i,j,k,p,q,iw,jw;
unsigned short	iwork[SWEEPOUT_MAX_DIM],	jwork[SWEEPOUT_MAX_DIM];
double		buffer[SWEEPOUT_MAX_DIM];
double		w,max;


for(i=0;i<n;i++){
	iwork[i]=i;
	jwork[i]=i;
}

for(i=0;i<n;i++){
	for(j=0;j<n;j++){
		if(i==j)	*(inv_a+n*i+j)=1.0;
		else		*(inv_a+n*i+j)=0.0;
	}
}

for(k=0;k<n;k++){

	max=fabs(*(a+n*k+k));
	p=k;
	q=k;
	for(j=k;j<n;j++){
		for(i=k;i<n;i++){
			if(max<fabs(*(a+n*i+j))){
				max=fabs(*(a+n*i+j));
				p=i;
				q=j;
			}
		}
	}

	if(max<=eps){
		return(999);
	}

	for(i=0;i<n;i++){
		w = *(a+n*i+k);
		*(a+n*i+k) = *(a+n*i+q);
		*(a+n*i+q) = w;
	}

	for(j=k;j<n;j++){
		w = *(a+n*k+j);
		*(a+n*k+j) = *(a+n*p+j);
		*(a+n*p+j) = w;
	}
	for(j=0;j<k;j++){
		w = *(inv_a+n*k+j);
		*(inv_a+n*k+j) = *(inv_a+n*p+j);
		*(inv_a+n*p+j) = w;
	}
	
	i=iwork[k];
	iwork[k]=iwork[p];
	iwork[p]=i;
	
	j=jwork[k];
	jwork[k]=jwork[q];
	jwork[q]=j;
	
	for(j=k+1;j<n;j++){
		*(a+n*k+j) = (*(a+n*k+j)) / (*(a+n*k+k));
	}
	for(j=0;j<n;j++){
		*(inv_a+n*k+j) = (*(inv_a+n*k+j)) / (*(a+n*k+k));
	}
		*(a+n*k+k) = 1.0;
		
	for(i=0;i<n;i++){
		if(i!=k){
			for(j=k+1;j<n;j++){
				*(a+n*i+j) = *(a+n*i+j) - (*(a+n*i+k)) * (*(a+n*k+j));
			}
			for(j=0;j<n;j++){
				*(inv_a+n*i+j) = *(inv_a+n*i+j) 
				- (*(a+n*i+k)) * (*(inv_a+n*k+j));
			}
				*(a+n*i+k) = 0.0;
		}
		
	}
}



for(j=0;j<n;j++){

	for(i=0;i<n;i++){
		iw=jwork[i];
		buffer[iw]= *(inv_a+n*i+j);
	}
	for(i=0;i<n;i++){
		*(inv_a+n*i+j)=buffer[i];
	}
}

for(i=0;i<n;i++){
	for(j=0;j<n;j++){
		jw=iwork[j];
		buffer[jw]= *(inv_a+n*i+j);
	}
	for(j=0;j<n;j++){
		*(inv_a+n*i+j)=buffer[j];
	}
}			

return(0);

}
