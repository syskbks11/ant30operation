/*
Two-dimensional Fast Fourier Transform by Cooley-Tukey algorithm

1994/6/30	Tomo OKA

Return values:
0:	Normal
1:	Inconsistency between n and iter
10:	Error in the function fft()
999:	Insufficient data number (n<2) or too many data (n>NDATA)

Variables:
xr:	Real part of input and output data
xi:	Imaginary part of input and output data
n:	Number of the input data
flag:	FT:0
	IFT:1
*/



#include	<stdio.h>
#include	<math.h>
#include	<nucalc.h>

#define	FT	0
#define	IFT	1


short	fft2(xr, xi, n, nmax, flag)
int	n, nmax, flag;
double	xr[][FFT_MAX_DATA_NUM], xi[][FFT_MAX_DATA_NUM];
{
int	i, j, iter;
short	err_code;
double	wr[FFT_MAX_DATA_NUM], wi[FFT_MAX_DATA_NUM];

/********************************/
/*	Parameter check		*/
/********************************/

if((n<2)||(n>FFT_MAX_DATA_NUM)){
	return(999);
	
}

iter=0;
i=n;
i=i/2;
while(i!=0){
	iter=iter+1;
	i=i/2;
}

j=1;
for(i=1;i<=iter;i++){
	j=2*j;
}

if(n!=j){
	return(1);
}



/********************************/
/*	Main section		*/
/********************************/

for(j=1;j<=n;j++){
	for(i=1;i<=n;i++){
		wr[i-1]=xr[i-1][j-1];
		wi[i-1]=xi[i-1][j-1];
	}

	err_code=fft(wr,wi,n,iter,flag);
	if(err_code!=0){
		return(10);
	}
	
	for(i=1;i<=n;i++){
		xr[i-1][j-1]=wr[i-1];
		xi[i-1][j-1]=wi[i-1];
	}
}

for(i=1;i<=n;i++){
	for(j=1;j<=n;j++){
		wr[j-1]=xr[i-1][j-1];
		wi[j-1]=xi[i-1][j-1];
	}

	err_code=fft(wr,wi,n,iter,flag);
	if(err_code!=0){
		return(10);
	}
	
	for(j=1;j<=n;j++){
		xr[i-1][j-1]=wr[j-1];
		xi[i-1][j-1]=wi[j-1];
	}
}

return(0);


}	
