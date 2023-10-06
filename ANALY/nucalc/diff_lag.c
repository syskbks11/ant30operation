#include	<math.h>
#include	<nucalc.h>

short	differential_Lagrange(x,y,n,x0,dy0)
double	x[], y[];
unsigned short	n;
double	x0;
double	*dy0;

{
short	i, j, k;
double	L, S, P;


P=0.0;
for(i=0;i<n;i++){
	S=0.0;
	for(j=0;j<n;j++){
		L=1.0;
		if(i==j)	continue;
		for(k=0;k<n;k++){
			if(i==k)		continue;
			else if(j==k)	L=L/(x[i]-x[k]);
			else			L=L*(x0-x[k])/(x[i]-x[k]);
		}
		S=S+L;
	}
	P=P+y[i]*S;
}
*dy0=P;
return(0);
}
