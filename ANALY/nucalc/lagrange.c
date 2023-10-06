#include	<math.h>
#include	<nucalc.h>

short	Lagrange_interpolation(x,y,n,x0,y0)
double	*x, *y;
unsigned short	n;
double	x0;
double	*y0;

{
short	i, j;
double	L, P;


P=0.0;
for(i=0;i<n;i++){
	L=(*(y+i));
	for(j=0;j<n;j++){
		if(i==j)	continue;
		else{
			L=L*(x0-*(x+j))/(*(x+i)-*(x+j));
		}
	}
	P=P+L;
}
*y0=P;

return(0);

}
