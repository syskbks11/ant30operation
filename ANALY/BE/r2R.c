/************************************************/
/*						*/
/*	2D projection of a radial distribution  */
/*		2002/9/3 by Tomo OKA		*/
/*						*/
/*	Usage: cat (input) | r2R [>(out)]	*/
/*						*/
/************************************************/

#include	<stdio.h>
#include	<math.h>
#include	<string.h>
#include	<malloc.h>

#include	"poly.h"

#define	TMPFILE	"r2R.tmp"


main(argc,argv)
int	argc;
char	*argv[];
{
FILE	*tmp;
int	ii, jj, kk;
int	data_num;
int	ir;
char	command[MAX_CHAR_LENGTH];
double	xi_buff, theta_buff, rho_buff;
double	*xi, *rho, *sigma, dxi, xir;



tmp=fopen(TMPFILE,"w");
if(tmp==NULL){fprintf(stderr,"Can't open %s!\n",TMPFILE); exit(0);}

data_num=0;
while(fscanf(stdin,"%lf %lf %lf",&xi_buff,&theta_buff,&rho_buff)==3){
	data_num++;
	fprintf(tmp,"%e\t%e\t%e\n",xi_buff,theta_buff,rho_buff);
}
fclose(tmp);

xi=calloc(data_num,sizeof(xi_buff));
rho=calloc(data_num,sizeof(rho_buff));
sigma=calloc(data_num,sizeof(rho_buff));

tmp=fopen(TMPFILE,"r");
if(tmp==NULL){fprintf(stderr,"Can't open %s!\n",TMPFILE); exit(0);}

for(ii=0;ii<data_num;ii++){
	fscanf(tmp,"%lf %lf %lf",(xi+ii),&theta_buff,(rho+ii));
/*	fprintf(stderr,"%lf\t%lf\t%lf\n",*(xi+ii),theta_buff,*(rho+ii));	*/
}
fclose(tmp);

dxi = fabs(*(xi+1) - *xi);

for(ii=0;ii<data_num;ii++){
*(sigma+ii) = 0.0;
for(jj=0;jj<data_num;jj++){
	xir = pow(pow(*(xi+ii),2.0)+pow(*(xi+jj),2.0),0.5);
	ir = (int)floor(xir/dxi);
	if(ir<data_num)	*(sigma+ii) += *(rho+ir);
}
*(sigma+ii) *= 2.0*dxi;
fprintf(stdout,"%e\t%e\t%e\n",*(xi+ii),*(rho+ii),*(sigma+ii));
}

sprintf(command,"rm %s\n",TMPFILE);
fprintf(stderr,"%s",command);
system(command);

}


