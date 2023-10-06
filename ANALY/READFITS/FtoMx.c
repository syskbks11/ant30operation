/*
CfA CO(J=1-0) FITS file reader

1993/9/1	Tomoharu OKA	:	Coding ŠJŽn
1993/9/2	Tomoharu OKA	:	Ver.1.00 Š®¬
1994/9/7	Tomoharu OKA	:	Bug fix for pixel inner structure
1995/6/20	Tomoharu OKA	:	FITS -> MATRIX
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <profhead.h>
#include <starlib.h>
#include <fitshead.h>

#define	DEBUG	0

#ifdef	_NO_PROTO
extern int read_FitsHeader();
extern int divide_Line();
extern int rm_Quot();
extern int get_MatrixData();
extern int OutputMatrixData();
#else
extern int read_FitsHeader(FITS_HEAD *, int *, AXIS_INDEX *, FILE *);
extern int divide_Line(char *, char *, char *, char *);
extern int rm_Quot(char *, char *);
extern int get_MatrixData(FITS_HEAD *, int, MATRIX *, AXIS_INDEX *, FILE *);
extern int OutputMatrixData(MATRIX *, FILE *);
#endif






int options(argc,argv,box)
int	argc;
char	*argv[];
BOX_RANGE_long	*box;
{
	int ii,jj;
	int	err_code;
	char	*token;


/* default values */
(*box).x.min=0;
(*box).x.max=0;
(*box).y.min=0;
(*box).y.max=0;
	
/* IvVXCb`ððßµÏÉãü·éB */
	for(ii=1;ii<argc;ii++)
	{
		if(argv[ii][0]=='-')	/* option switch */
		{
			if(strcmp(argv[ii],"-x")==0){
				ii++;
				token = strtok(argv[ii],",");
                                        (*box).x.min = atof( token );
				token = strtok(NULL,",");
                                        (*box).x.max = atof( token );
			}
			else if(strcmp(argv[ii],"-y")==0){
				ii++;
				token = strtok(argv[ii],",");
                                        (*box).y.min = atof( token );
				token = strtok(NULL,",");
                                        (*box).y.max = atof( token );				
			}
		}
	}
if(((*box).x.max==0)||((*box).y.max==0))	return(101);


STAR_SortRangeLong(&(*box).x);
STAR_SortRangeLong(&(*box).y);

return(0);
}	








main(argc,argv)
int	argc;
char	*argv[];
{
FILE	*fp_in, *fp_out;
int	err_code;
FITS	fits;
MATRIX	matrix;
BOX_RANGE_long	box;


fp_in=stdin;
fp_out=stdout;


err_code=options(argc,argv,&box);
if(err_code!=0){
	fprintf(stderr,"Errorin reading option switches.  Specify data ranges.\n");
	exit(999);
}

fprintf(stderr,"Reading Fits data.\n");
err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,"Error in reading FITS file. Error code=%d\n",err_code);
	exit(999);
}

fprintf(stderr,"Getting Matrix data.\n");
err_code=get_MatrixDataX(&fits,&matrix);
if(err_code!=0){
	fprintf(stderr,"Error in creating Matrix data. Error code=%d\n",err_code);
	exit(999);
}

err_code=OutputMatrixData(&matrix,fp_out);
if(err_code!=0){
	fprintf(stderr,"Error in outputing the Matrix data. Error code=%d\n",err_code);
	exit(999);
}


}	/*main*/


