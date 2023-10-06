#include	<stdio.h>
#include	<math.h>

#define		PLNK	6.62559e-27
#define		BLTM	1.38054e-16
#define		TBB	2.73



main()
{
    double	nu, tb, tex, tau;
    double	c;
    int		it;	

    fprintf(stderr,"nu[GHz]=");
    fscanf(stdin,"%lf",&nu);
    fprintf(stderr,"Tex=");
    fscanf(stdin,"%lf",&tex);

    c=PLNK/BLTM*(nu*1.0e9);

    for(it=0;it<1000;it++){
	tau=(double)it/100.0;
	tb = c*(1.0/(exp(c/tex)-1.0) - 1.0/(exp(c/TBB)-1.0) )*(1.0-exp(-tau));
	fprintf(stdout,"%lf\t%lf\n",tau,tb);
    }



}

