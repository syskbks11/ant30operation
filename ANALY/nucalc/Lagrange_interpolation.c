#include	<math.h>
#include	<nucalc.h>

short	Lagrange_interpolation(x,y,n,x0,y0)
double	*x,*y;
short	n;
double	x0;
double	*y0;


{
short	i,j;
double	term,sum;

sum=0.0;
for(i=0;i<n;i++){
	term= *(y+i);
	for(j=0;j<n;j++){
		if(i==j)	continue;
		else{
			term=term*(x0-*(x+j))/(*(x+i)-*(x+j));
		}
	}
	sum=sum+term;
}
*y0=sum;

return(0);

}
