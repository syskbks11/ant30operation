#include	<stdio.h>
#include	<math.h>

#define		EPS	1.0e-10
#define		MAX_ITERATION	1000

double	f(x,z,abu)
double	x,z,abu;
{
double	y;

y=(1.0 - exp(-x))/(1.0 - exp(-x/abu))-z;

return(y);
}


double	df(x,abu)
double	x,abu;
{
double	y;

y= 1.0/(exp(x)*(1.0 - exp(-x/abu))) 
	- (1.0 - exp(-x))/(abu*exp(x/abu)*pow((1.0 - exp(-x/abu)),2.0));

return(y);
}


int	Tau(y,abu,t)
double	y, abu, *t;
{
int	i;
double	x, xn;

x=1.0;
for(i=1;i<=MAX_ITERATION;i++)
{
xn=x-f(x,y,abu)/df(x,abu);
if(fabs((xn-x)/x)<EPS)	break;
if(finite(xn)==0){
return(-1);
}
x=xn;
}

*t=xn;
return(0);

}

