#include	<stdio.h>
#include	<math.h>

double	kaijou(i)
long	i;
{
long	j;
double	term;

if(i<0){
	fprintf(stderr,"kaijou DOMAIN error!\n");
	return(1.0);
}
else if(i==0){
	return(1.0);
}
else{
	term=1.0;
	for(j=0;j<i;j++){
		term=term*(double)(j+1);
	}
	return(term);
}

}


double	bekijou(x,i)
double	x;
long	i;
{

long	j;
double	term;


if(i<0){
	term=1.0;
	for(j=0;j<-i;j++){
		term=term/x;
	}
	return(term);
}
else if(i==0){
	return(1.0);
}
else{
	term=1.0;
	for(j=0;j<i;j++){
		term=term*x;
	}
	return(term);
}

}
