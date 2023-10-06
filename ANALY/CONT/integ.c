#include	<stdio.h>
#include	<math.h>
#include	<string.h>

#define	MAX_CHAR_LENGTH	8192
#define	DATA_POINT	256

main(argc,argv)
int     argc;
char    *argv[];
{
char    buffer[MAX_CHAR_LENGTH], *token;
int     i, ii, j, jj, n[DATA_POINT];
int	iflag;
double	ra_d, dec_d, ta_d;  
double	ra[DATA_POINT], dec[DATA_POINT];
double	ta[DATA_POINT], ta2[DATA_POINT], rms[DATA_POINT];

for(jj=0;jj<5;jj++){
	fgets(buffer,MAX_CHAR_LENGTH,stdin);
}

i=0;	j=0;
for(ii=0;ii<DATA_POINT;ii++){	n[ii]=0; ta[ii]=0.0; ta2[ii]=0.0;	}

while(fgets(buffer,MAX_CHAR_LENGTH,stdin)!=NULL)
{
	token=strtok(buffer,"\t\n ,");
	if(token==NULL){ fprintf(stderr,"Error in reading RA.\n"); break;}	
	else ra_d=atof(token);

	token=strtok(NULL,"\t\n ,");
	if(token==NULL){ fprintf(stderr,"Error in reading DEC.\n"); break;}	
	else dec_d=atof(token);

	token=strtok(NULL,"\t\n ,");
	if(token==NULL){ fprintf(stderr,"Error in reading TA.\n"); break;}	
	else ta_d=atof(token);

/*	fprintf(stderr,"%lf\t%lf\t%lf\n",ra_d,dec_d,ta_d);
*/
	if(i==0){
		ra[i]=ra_d;	dec[i]=dec_d;
		ta[i]=ta[i]+ta_d;	ta2[i]=ta2[i]+ta_d*ta_d;	
		n[i]++; i++;
	}
	else{
		iflag=0;
		for(ii=0;ii<i;ii++)
		{
			if((ra[ii]==ra_d)&&(dec[ii]==dec_d))
			{
				n[ii]++; iflag++;
				ta[ii]=ta[ii]+ta_d;	ta2[ii]=ta2[ii]+ta_d*ta_d;
				break;
			}
		}
		if(iflag==0){
			ra[i]=ra_d;	dec[i]=dec_d;
			ta[i]=ta[i]+ta_d;	ta2[i]=ta2[i]+ta_d*ta_d;	
			n[i]++;
			i++;
		}
	}

/*
fprintf(stderr,"%lf\t%lf\t%lf\t%lf\t%lf\n",ta[0],ta[1],ta[2],ta[3],ta[4]);		
*/
}

for(ii=0;ii<i;ii++){
	ta[ii]=ta[ii]/((double)n[ii]);
	ta2[ii]=ta2[ii]/((double)n[ii]);	
	rms[ii]=pow(ta2[ii]-ta[ii]*ta[ii], 0.5);
}

for(ii=0;ii<i;ii++){
	fprintf(stderr,"%lf\t%lf\t%lf\t%lf\n",ra[ii],dec[ii],ta[ii],rms[ii]);
}



}