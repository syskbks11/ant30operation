/************************************************/
/*						*/
/*	Calculate a density-profile 		*/
/*		for a Bonner-Ebert sphere	*/
/*		2002/10/16 by Tomo OKA		*/
/*						*/
/*	Usage: polyfit -n (index) [-d]		*/
/*						*/
/************************************************/

#include	<stdio.h>
#include	<math.h>

#include	"poly.h"

extern void	options();
extern double	column();
extern double	kai2();
extern int	initialize_parameter();
extern double	fitting_BEsphere();



main(argc,argv)
int	argc;
char	*argv[];
{
int	ii, jj;
int	debug;
double	n;
double	kai, xi, dxi, r, rho, theta, sigma;
char	mdl_file[MAX_CHAR_LENGTH], data_file[MAX_CHAR_LENGTH];
FILE	*mf, *df;

MODEL	model;
DATA	data;
PARAM	param;
double	xi_dum, theta_dum, rho_dum;
double	r_dum, sigma_dum, n_dum;

/*	Option switch interpretation	*/
options(argc,argv,&n,&debug,data_file,&param);
param.n = n;

/*	Reading a model file	*/
if(n==MAX_POLYTROPE_INDEX)
	sprintf(mdl_file,"ninf");
else	sprintf(mdl_file,"n%d",(int)n);

mf=fopen(mdl_file,"r");
if(mf==NULL){ fprintf(stderr,"Can't open %s!\n",mdl_file); exit(1);}

for(ii=0;ii<MAX_DATA_NUM;ii++){
if(fscanf(mf,"%lf %lf %lf",&(xi_dum),&(theta_dum),&(rho_dum))!=3)	break;
}	model.num=ii;
rewind(mf);
model.xi = (double *)calloc(model.num,8);
model.theta = (double *)calloc(model.num,8);
model.rho = (double *)calloc(model.num,8);
for(ii=0;ii<model.num;ii++){
if(fscanf(mf,"%lf %lf %lf",&(model.xi[ii]),&(model.theta[ii]),&(model.rho[ii]))!=3)	break;
/*fprintf(stderr,"%lf\t%lf\t%lf\n",model.xi[ii],model.theta[ii],model.rho[ii]);*/
}	model.num=ii;
fclose(mf);
fprintf(stderr,"model data number = %d\n",model.num);



/*	Calculating a density-profile of a Bonner-Ebert sphere	*/

for(ii=0;ii<model.num;ii++){
	r = model.xi[ii]*param.alpha;
	sigma = column(&model, &param, r);
	fprintf(stdout,"%lf\t%e\t%e\n",model.xi[ii],r,sigma);
}


}










double	linear_interpolation(model,xi)
MODEL	*model;
double	xi;
{
int	ii, jj, is;
double	xi1, xi2;
double	theta1, theta2, theta;

if(xi<(*model).xi[0])			theta=(*model).theta[0];
else if(xi>(*model).xi[(*model).num-1])	theta=0.0;
else{
is = (int)(xi/fabs((*model).xi[1]-(*model).xi[0]))-2;	if(is<0)is=0;
for(ii=is;ii<(*model).num-1;ii++){
	xi1 = (*model).xi[ii];	xi2 = (*model).xi[ii+1]; 
	if((xi1<=xi)&&(xi<xi2))	break;
}
theta1 = (*model).theta[ii]; 	theta2 = (*model).theta[ii+1]; 
theta = theta1 + (theta2-theta1)/(xi2-xi1) * (xi-xi1); 
}

return(theta);
}



double	column(model, param, r)
MODEL 	*model;
PARAM	*param;
double	r;
{
double	xi, s, rr, sigma, theta, rho;
double	dxi, ds;
int	ii, jj, err;


if( r>(*param).alpha*(*param).ximax )	sigma=0.0;

else{
sigma=0.0;	rr=r;	s=0.0;
dxi = (*model).xi[1]-(*model).xi[0];
ds = dxi * (*param).alpha;
while( rr<=(*param).alpha*(*param).ximax ){
	rr = pow(r*r + s*s, 0.5);
	xi = rr/(*param).alpha;

	theta = linear_interpolation(model,xi);

	if((*param).n==MAX_POLYTROPE_INDEX)
		rho = (*param).rhoc * exp((-1.0)*theta);
	else	rho = (*param).rhoc * pow(theta,(*param).n);
	sigma += rho*ds ; 
	s += ds; 

fprintf(stderr,"theta = %lf\trhoc = %lf\n",theta,(*param).rhoc);
}
sigma *= 2.0;
}
return(sigma);
}




double	kai2(model, data, param)
MODEL 	*model;
DATA	*data;
PARAM	*param;
{
int	ii, jj;
double	kai, sigma;

kai = 0.0;
for(ii=0;ii<(*data).num;ii++){
	sigma = column(model,param,(*data).r[ii]);
	kai += (*data).n[ii] * pow((*data).sigma[ii]-sigma, 2.0);
}

return(kai);
}






void	options(argc,argv,n,d,filename,param)
int	argc;
char	*argv[];
double	*n;
int	*d;
char	filename[];
PARAM	*param;
{
int	ii, jj;
char	*token;

/* default values */
*n=0.0;
*d=0;
(*param).alpha=0.0;
(*param).ximax=0.0;
(*param).rhoc=0.0;

/* interpret options */
for(ii=1;ii<argc;ii++){
	if(argv[ii][0]=='-'){
		if(strcmp(argv[ii],"-n")==0){
			ii++;
			if((strstr(argv[ii],"inf")!=NULL)||(strstr(argv[ii],"INF")!=NULL)){
				*n=MAX_POLYTROPE_INDEX;
			}
			else	*n=atof(argv[ii]);
		}
		else if(strcmp(argv[ii],"-debug")==0){
			*d=1;
		}
		else if(strcmp(argv[ii],"-data")==0){
			ii++;
			strcpy(filename,argv[ii]);
		}
		else if(strcmp(argv[ii],"-alpha")==0){
			ii++;	
			(*param).alpha=atof(argv[ii]);
		}
		else if(strcmp(argv[ii],"-rhoc")==0){
			ii++;	
			(*param).rhoc=atof(argv[ii]);
		}
		else if(strcmp(argv[ii],"-ximax")==0){
			ii++;	
			(*param).ximax=atof(argv[ii]);
		}

	}
}
}

