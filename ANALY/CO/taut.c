#include	<stdio.h>
#include	<math.h>


main()
{
double	x, xn, y, t, abu;

fprintf(stderr,"12/13=");
fscanf(stdin,"%lf",&abu);
fprintf(stderr,"T(12)/T(13)=");
fscanf(stdin,"%lf",&y);

Tau(y,abu,&t);

fprintf(stdout,"tau=%lf\n",t);

}