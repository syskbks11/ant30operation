/*
One-dimensional Fast Fourier Transform by Cooley-Tukey algorithm

1994/6/30	Tomo OKA

Return values:
0:	Normal
1:	Inconsistency between n and iter
999:	Insufficient data number (n<2)

Variables:
xr:	Real part of input and output data
xi:	Imaginary part of input and output data
n:	Number of the input data
iter:	n=2^(iter)
flag:	FT:0
	IFT:1
*/



#include	<stdio.h>
#include	<math.h>
#include	<nucalc.h>

#define	FT	0
#define	IFT	1


short	fft(xr, xi, n, iter, flag)
double	xr[];
double	xi[];
int	n;
int	iter;
int	flag;
{
int	ii;
int	i, j, it, xp, xp2, k, j1, j2;
double	sgn, w, wr, wi;
double	dr1, dr2, di1, di2, tr, ti, arg;

/********************************/
/*	Parameter check		*/
/********************************/

if(n<2){
	return(999);
}

if(iter<=0){
	iter=0;
	i=n;
	while(i!=0){
		i=i/2;
		iter=iter+1;
	}
}

j=1;
for(i=1;i<=iter;i++){
	j=2*j;
}

if(n!=j){
	return(1);
}


/********************************/
/*	FT or Inverse FT	*/
/********************************/

if(flag==IFT){
	sgn=1.0;
}
else{
	sgn=(-1.0);
}



/********************************/
/*	Main section		*/
/********************************/

xp2=n;
for(it=1;it<=iter;it++){
	xp=xp2;
	xp2=xp/2;
	w=PI/(double)xp2;
	for(k=1;k<=xp2;k++){
		arg=(double)(k-1)*w;
		wr=cos(arg);
		wi=sgn*sin(arg);
		i=k-xp;
		for(j=xp;j<=n;j=j+xp){
			j1=j+i;
			j2=j1+xp2;
			dr1=xr[j1-1];
			dr2=xr[j2-1];
			di1=xi[j1-1];
			di2=xi[j2-1];
			tr=dr1-dr2;
			ti=di1-di2;
			xr[j1-1]=dr1+dr2;
			xi[j1-1]=di1+di2;
			xr[j2-1]=tr*wr-ti*wi;
			xi[j2-1]=ti*wr+tr*wi;
		}
	}
}

j1=n/2;
j2=n-1;
j=1;
for(i=1;i<=j2;i++){
	if(i<j){
		tr=xr[j-1];
		ti=xi[j-1];
		xr[j-1]=xr[i-1];
		xi[j-1]=xi[i-1];
		xr[i-1]=tr;
		xi[i-1]=ti;
	}
	k=j1;
	while(k<j){
		j=j-k;
		k=k/2;
	}
	j=j+k;
}


/********************************/
/*	FT or Inverse FT	*/
/********************************/

if(flag==IFT){
	w=(double)n;
	for(i=1;i<=n;i++){
		xr[i-1]=xr[i-1]/w;
		xi[i-1]=xi[i-1]/w;
		
	}
}

return(0);

}
