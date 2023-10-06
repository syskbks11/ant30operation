#include	<stdio.h>
#include	<string.h>
#include	<math.h>

#define		MAX_CHAR_LENGTH	81920
#define		MAX_COL		1024
#define		MAX_ROW		1024


main(argc,argv)
int	argc;
char	*argv[];
{
int	ii, jj;
char	buffer[MAX_CHAR_LENGTH], *token;
int	inum, jnum;
double	x[MAX_COL], y[MAX_ROW], z, zz;



fgets(buffer,MAX_CHAR_LENGTH,stdin);
/*	fprintf(stderr,"%s\t",buffer);	*/

jj=0;
token=strtok(buffer,"\t\n ,");
x[jj]=atof(token);
jj++;
while(1){
	token=strtok(NULL,"\t\n ,");
	if(token==NULL)	break;
	x[jj]=atof(token);
/*	fprintf(stderr,"%lf\t",x[jj]);	*/	
	jj++;
}
jnum=jj;

zz=0.0;
for(ii=0;;ii++){
	if(fgets(buffer,MAX_CHAR_LENGTH,stdin)==NULL)	break;
	token=strtok(buffer,"\t\n ,");
	if(token==NULL)	break;
	y[ii]=atof(token);
	for(jj=0;jj<jnum;jj++){
		token=strtok(NULL,"\t\n ,");
		z=atof(token);
		zz=zz+z;
	}
}

fprintf(stdout,"\n%e\n\n",zz);


}
