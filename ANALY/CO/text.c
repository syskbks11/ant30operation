#include	<stdio.h>
#include	<math.h>

#define		PLNK	6.62559e-27
#define		BLTM	1.38054e-16



main()
{
double	c, nu, tb, t, tex;

fprintf(stderr,"nu[GHz]=");
fscanf(stdin,"%lf",&nu);
fprintf(stderr,"Tb[K]=");
fscanf(stdin,"%lf",&tb);
fprintf(stderr,"tau=");
fscanf(stdin,"%lf",&t);

Tex(nu,tb,t,&tex);

fprintf(stderr,"Tex=%lf\n",tex);

}

