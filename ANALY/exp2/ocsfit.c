#include	<stdio.h>
#include	<math.h>

#define	MAX_DATA_NUM	2048
#define	MAX_CHAR_LENGTH	256

#define BLTM    (1.38054e-23)
#define PLNK    (6.62559e-34)
#define LIGHT   (2.99792458e8)

#define	WAVE_GUIDE	(4.925e-3)

int	main(argc,argv)
int	argc;
char	*argv[];
{
double	x[MAX_DATA_NUM], y[MAX_DATA_NUM], sigma[MAX_DATA_NUM];
double	sigma_childa, kai2;
double	S, Sx, Sy, Sxx, Sxy, Delta;
double	a, b, sigma_a, sigma_b;
int	n;

double	v, v2, uhf, hf, nu;
int	ii, jj, m;
char	sw[MAX_CHAR_LENGTH], buffer[MAX_CHAR_LENGTH];

double	nu0, mu, sigma_mu;
double	d, B;

d = WAVE_GUIDE;



ii=0;
fgets(buffer,80,stdin);
while(fscanf(stdin,"%lf %lf %s %d %lf %lf %lf",&v,&v2,sw,&m,&uhf,&hf,&nu)==7){
	if(m==0){
		x[ii]=(-8.0/105.0)*v*v;
		sigma[ii]=1.0;
	}
	else if(m==1){
		x[ii]=(13.0/210.0)*v*v;
		sigma[ii]=1.0;/*(13.0/16.0);*/		
	}
	else{	fprintf(stderr,"Invalid M number! M=%d\n",m);
		continue;	}

	y[ii]=nu;

/* fprintf(stdout,"%e\t%e\n",x[ii],y[ii]); */

	ii++;
}	n=ii;

S=0.0; Sx=0.0; Sy=0.0; Sxx=0.0; Sxy=0.0;
for(ii=0;ii<n;ii++){
	S += 1.0/(sigma[ii]*sigma[ii]);
	Sx += x[ii]/(sigma[ii]*sigma[ii]);
	Sy += y[ii]/(sigma[ii]*sigma[ii]);
	Sxx += (x[ii]*x[ii])/(sigma[ii]*sigma[ii]);
	Sxy += (x[ii]*y[ii])/(sigma[ii]*sigma[ii]);
}
Delta = S*Sxx-Sx*Sx;
a = (Sxx*Sy-Sx*Sxy)/Delta;
b = (S*Sxy-Sx*Sy)/Delta;

/*
sigma_childa=0.0;
for(ii=0;ii<n;ii++){	sigma_childa +=  pow((y[ii]-(a+b*x[ii]))/sigma[ii],2.0);}
sigma_childa /= (double)n;
sigma_childa = pow(sigma_childa,0.5);
fprintf(stderr,"sigma_childa = %e\n",sigma_childa);
*/

kai2=0.0;
for(ii=0;ii<n;ii++){
/*	sigma[ii] *= sigma_childa;	*/
	kai2 += pow((y[ii]-(a+b*x[ii]))/sigma[ii],2.0);
}
fprintf(stderr,"kai2 = %e\n",kai2);

sigma_a = pow(Sxx/Delta*kai2/(double)(n-2),0.5);
sigma_b = pow(S/Delta*kai2/(double)(n-2),0.5);

/*
sigma_a = pow(Sxx/Delta,0.5);
sigma_b = pow(S/Delta,0.5);
*/

fprintf(stderr,"a = %e\terr=%e\n",a,sigma_a);
fprintf(stderr,"b = %e\terr=%e\n",b,sigma_b);
fprintf(stderr,"\n");

nu0 = a*1.0e6;
B = nu0/4.0;
mu = PLNK*d*pow((B*b*1.0e6),0.5);
sigma_mu=PLNK*d*pow((B*b),-0.5)*B*1.0e3*sigma_b/2.0;

fprintf(stderr,"nu0 = %lf +- %lf [MHz]\n",a,sigma_a);
fprintf(stderr,"mu = %e +- %e [Cm]\n",mu,sigma_mu);
fprintf(stderr,"mu = %lf +- %lf [Debye]\n",mu*1.0e21*LIGHT,sigma_mu*1.0e21*LIGHT);

}