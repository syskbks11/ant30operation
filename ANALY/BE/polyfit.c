/************************************************/
/*						*/
/*	Fitting to a Bonner-Ebert sphere	*/
/*		2002/10/16 by Tomo OKA		*/
/*						*/
/*	Usage: polyfit -n (index) -data (file) 	*/
/*		-alpha () -ximax () [-rhoc]	*/
/*						*/
/************************************************/

#include	<stdio.h>
#include	<math.h>
#include	<nucalc.h>
#include	"poly.h"

#define	DEV	(1.0e-4);
#define	SOFT	(1.0e-1);
#define	UNIT	1.0	

extern void	options();
extern double	column();
extern double	kai2();
extern int	initialize_parameter();
extern double	fitting_BEsphere();
extern void	scaling_matrix();
extern void	de_scaling();



main(argc,argv)
int	argc;
char	*argv[];
{
int	ii, jj;
int	debug;
double	n;
double	kai, xi, r, rho, theta;
char	mdl_file[MAX_CHAR_LENGTH], data_file[MAX_CHAR_LENGTH];
FILE	*mf, *df;

MODEL	model;
DATA	data;
PARAM	param, param_opt;
double	xi_dum, theta_dum, rho_dum;
double	r_dum, sigma_dum, n_dum;


/*	Option switch interpretation	*/
options(argc,argv,&n,&debug,data_file,&param_opt);
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


/*	Reading a data file	*/
df=fopen(data_file,"r");
if(df==NULL){ fprintf(stderr,"Can't open %s!\n",data_file); exit(2);}

for(ii=0;ii<MAX_DATA_NUM;ii++){
if(fscanf(df,"%lf %lf %lf",&(r_dum),&(sigma_dum),&(n_dum))!=3)	break;
}	data.num=ii;
rewind(df);
data.r = (double *)calloc(data.num,8);
data.sigma = (double *)calloc(data.num,8);
data.n = (double *)calloc(data.num,8);
for(ii=0;ii<data.num;ii++){
if(fscanf(df,"%lf %lf %lf",&(data.r[ii]),&(data.sigma[ii]),&(data.n[ii]))!=3)	break;
/*fprintf(stderr,"%lf\t%lf\t%lf\n",data.r[ii],data.sigma[ii],data.n[ii]);*/
}	data.num=ii;
fclose(df);
fprintf(stderr,"observed data number = %d\n",data.num);


/*	Fitting to a Bonner-Ebert sphere	*/
initialize_parameter(&model, &data, &param, &param_opt);
kai = fitting_BEsphere(&model, &data, &param);

fprintf(stdout,"kai2 = %lfalpha = %lf\tximax=%lf\trhoc = %lf\t\n"
	,kai,param.alpha,param.ximax,param.rhoc);
}



int	initialize_parameter(model, data, param, param_opt)
MODEL 	*model;
DATA	*data;
PARAM	*param, *param_opt;
{
int	ii, jj, mid;
double	width;

mid = (*model).num/2;
if((*param_opt).alpha!=0.0)	(*param).alpha=(*param_opt).alpha;
else				(*param).alpha = (*data).r[(*data).num-1]/(*model).xi[mid];

if((*param_opt).ximax!=0.0)	(*param).ximax=(*param_opt).ximax;
else				(*param).ximax = (*model).xi[mid];

if((*param_opt).rhoc!=0.0)	(*param).rhoc=(*param_opt).rhoc;
else{				(*param).rhoc = 1.0;
				(*param).rhoc = (*data).sigma[0]/column(model,param,0);	}

return(0);
}




double	fitting_BEsphere(model, data, param)
MODEL 	*model;
DATA	*data;
PARAM	*param;
{
int	ii, jj, kk, error;
double	kai, kaip1, kaip2, kaipp, kain;
double	dx, da, dr;
double	K[3][3], b[3];
double	max, soft;
double	cmax[3], rmax[3];


for(ii=0;ii<MAX_IT;ii++){

dx = fabs((*param).ximax)*DEV;
da = fabs((*param).alpha)*DEV;
dr = fabs((*param).rhoc)*DEV;

kai = kai2(model, data, param);		(*param).ximax += dx;
kaip1 = kai2(model, data, param);	(*param).alpha += da;
kaipp = kai2(model, data, param);	(*param).ximax -= dx;
kaip2 = kai2(model, data, param);	(*param).alpha -= da;
K[0][1]=(kaipp-kaip1-kaip2+kai)/(dx*da);	K[1][0]=K[0][1];

(*param).alpha += da;
kaip1 = kai2(model, data, param);	(*param).rhoc += dr;
kaipp = kai2(model, data, param);	(*param).alpha -= da;
kaip2 = kai2(model, data, param);	(*param).rhoc -= dr;
K[1][2]=(kaipp-kaip1-kaip2+kai)/(da*dr);	K[2][1]=K[1][2];

(*param).rhoc += dr;
kaip1 = kai2(model, data, param);	(*param).ximax += dx;
kaipp = kai2(model, data, param);	(*param).rhoc -= dr;
kaip2 = kai2(model, data, param);	(*param).ximax -= dx;
K[2][0]=(kaipp-kaip1-kaip2+kai)/(dr*dx);	K[0][2]=K[2][0];

(*param).ximax += dx;
kaip1 = kai2(model, data, param);	(*param).ximax += dx;
kaipp = kai2(model, data, param);	(*param).ximax -= 2.0*dx;
K[0][0]=(kaipp-2.0*kaip1+kai)/(dx*dx);
b[0] = (-1.0)*(kaip1 - kai)/dx;

(*param).alpha += da;
kaip1 = kai2(model, data, param);	(*param).alpha += da;
kaipp = kai2(model, data, param);	(*param).alpha -= 2.0*da;
K[1][1]=(kaipp-2.0*kaip1+kai)/(da*da);
b[1] = (-1.0)*(kaip1 - kai)/da;

(*param).rhoc += dr;
kaip1 = kai2(model, data, param);	(*param).rhoc += dr;
kaipp = kai2(model, data, param);	(*param).rhoc -= 2.0*dr;
K[2][2]=(kaipp-2.0*kaip1+kai)/(dr*dr);
b[2] = (-1.0)*(kaip1 - kai)/dr;

/*
for(jj=0;jj<3;jj++){
for(kk=0;kk<3;kk++){
fprintf(stderr,"%e\t",K[jj][kk]);
}fprintf(stderr,"%e\n",b[jj]);
}fprintf(stderr,"\n");
*/


/* scaling_matrix(K,b,cmax,rmax,3,3); */
error=sweepout(K,b,3,3,1.0e-10);
if(error!=0){
	fprintf(stderr,"Error in sweepout()! Error code = %d.\n",error);
}
/* de_scaling(b,cmax,rmax,3); */	


soft=1.0;	
for(kk=0;kk<10;kk++){
(*param).ximax += b[0]*soft;
(*param).alpha += b[1]*soft;
(*param).rhoc += b[2]*soft;
kain = kai2(model, data, param);

fprintf(stderr,"soft = %5.1e\tkai = %lf, %lf\t",soft,kai,kain);
fprintf(stderr,"ximax = %lf\talpha = %lf\trhoc = %lf\n",
	(*param).ximax,(*param).alpha,(*param).rhoc);

if(kain<=kai)	break;
else{	
	(*param).ximax -= b[0]*soft;
	(*param).alpha -= b[1]*soft;
	(*param).rhoc -= b[2]*soft;
	soft *= SOFT;
}
if((*param).ximax>(*model).xi[(*model).num-1])	(*param).ximax=(*model).xi[(*model).num-1];
if((*param).ximax<0.0)	(*param).ximax=(*model).xi[(*model).num/2];

}

max = EPS;
if(max<fabs(b[0]/(*param).ximax))	max=fabs(b[0]/(*param).ximax);
if(max<fabs(b[1]/(*param).alpha))	max=fabs(b[1]/(*param).alpha);
if(max<fabs(b[2]/(*param).rhoc))	max=fabs(b[2]/(*param).rhoc);
if(max<pow(EPS,0.5))	break;

}	/*	iteration loop	*/

return(kai);
}






double	linear_interpolation(model,xi)
MODEL	*model;
double	xi;
{
int	ii, jj, is;
double	xi1, xi2;
double	theta1, theta2, theta;

if(xi<=(*model).xi[0])			theta=(*model).theta[0];
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







void scaling_matrix(H,b,cmax,rmax,n,num)
double	*H, b[];
double	cmax[], rmax[];
int	n, num;
{
int	ii,jj;

for(ii=0;ii<num;ii++){
	rmax[ii]=fabs( *(H+ii*n+0) );
	for(jj=1;jj<num;jj++){
		if(fabs( *(H+ii*n+jj) )>rmax[ii])	rmax[ii]=fabs( *(H+ii*n+jj) );
	}
}
for(ii=0;ii<num;ii++){
	if(rmax[ii]<=0.0){continue;}
	b[ii]=b[ii]/rmax[ii]*UNIT;
	for(jj=0;jj<num;jj++){
		( *(H+ii*n+jj) ) = (*(H+ii*n+jj) )/rmax[ii]*UNIT;
	}
}

for(jj=0;jj<num;jj++){
	cmax[jj]=fabs( *(H+0*n+jj) );
	for(ii=1;ii<num;ii++){
		if(fabs( *(H+ii*n+jj) )>cmax[jj])	cmax[jj]=fabs( *(H+ii*n+jj) );
	}
}
for(jj=0;jj<num;jj++){
	for(ii=0;ii<num;ii++){
		if(cmax[jj]<=0.0){continue;}
		(*(H+ii*n+jj) ) = (*(H+ii*n+jj)) / cmax[jj]*UNIT;
	}
}
}


void de_scaling(t,cmax,rmax,num)
double	t[];
double	cmax[];
double	rmax[];
int	num;
{
int		ii;

for(ii=0;ii<num;ii++){
	if(cmax[ii]<=0.0){continue;}
	t[ii]=t[ii]/cmax[ii]*UNIT;
}

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

