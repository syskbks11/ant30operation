#include	<stdio.h>
#include	<math.h>

#define	PLNK	6.62559e-27
#define	BLTM	1.38054e-16
#define	TBB	2.73


int	Tex(nu, tb, t, tex)
double	nu, tb, t, *tex;
{
double	c;

c=PLNK/BLTM*(nu*1.0e9);

tb = tb + c/(exp(c/TBB)-1.0)*(1.0-exp(-t));

if(((1.0-exp(-t))*c/tb + 1.0)<0.0){
	*tex=0.0;
	return(-1);
}
else	*tex=c/log((1.0-exp(-t))*c/tb + 1.0);

return(0);
}

