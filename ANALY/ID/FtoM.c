/*
CfA CO(J=1-0) FITS file reader

1993/9/1	Tomoharu OKA	:	Coding 開始
1993/9/2	Tomoharu OKA	:	Ver.1.00 完成
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



main(argc,argv)
int	argc;
char	*argv[];
{
FILE		*fp_in, *fp_out;
int		err_code;
AXIS_INDEX	axis;
XY_POS		position;
FITS		fits;
MATRIX		matrix;
int		offset;

/*	入力ファイルのオープン	*/

fp_in=stdin;
fp_out=stdout;



err_code=read_Fits(&fits, fp_in);
if(err_code!=0){
	fprintf(stderr,"Error in reading the FITS header. Error code=%d\n",err_code);
	exit(999);
}

/*	Matrix data部分の構成	*/

err_code=get_MatrixData(&fits, &matrix, fp_in);
if(err_code!=0){
	fprintf(stderr,"Error in creating the Matrix data. Error code=%d\n",err_code);
	if(err_code==101){
		fprintf(stderr,"FITS dimension must be 2.\n");
	}
	exit(999);
}

err_code=OutputMatrixData(&matrix,fp_out);
if(err_code!=0){
	fprintf(stderr,"Error in outputing the Matrix data. Error code=%d\n",err_code);
	exit(999);
}


}	/*main*/






