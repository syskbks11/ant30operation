#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <profhead.h>
#include <starlib.h>

/*
general perpus library

update history of the file
1993/1/12 T.Handa
1992/12/21 T.Handa
*/

double ipow(p, n)
/*
float型変数pの整数冪上を求める。

Programmed by H. Tanioka
Editted by Neko
*/
float	p;
int  	n;
	{
	int		k;
	double 	s = 1;
	
	for(k = 1; k <= n; k++)
		{
		s = s * p;
		}
	
	return(s);
	}

void LeastSquareFit (x, y, ndata, order, coefficient)
/*
Function LeastSquareFit
与えられたxとyの２組の１次元配列（データ数はndata）について、
orderで決められる次数の多項式でフィッティングを行ない、係数
coefficientを返す

Programmed by H. Tanioka
Editted by Neko
*/

float x[], y[]; /* 作業用１次元配列 */
int ndata;	/* データの数 */
int order;	/* 多項式の次数 */
float coefficient[];	/* 多項式の係数 */

{
	double a[10][12];	/* 拡大係数行列 */
	double s[21], t[11];	/* 作業用配列 */
	
	int i, j, k;
	double p, d, px;

/* Initializing the work-matrix s[] and t[] */

	for (i = 0; i <= 2*order; ++i)
		s[i] = 0;

	for (i = 0; i <= order; ++i)
		t[i] = 0;

/* Initializing the coefficient matrix a[][] */

	for (i = 0; i < order; ++i)
	{
		for (j = 0; j < order + 2; ++j)
		{
			a[i][j] = 0;
		}
	}


/* Calculating the value of s[] and t[] */

	for (i = 0; i < ndata; ++i)
	{
		for (j = 0; j <= 2*order; ++j)
		{
			s[j] = s[j] + ipow (x[i], j);
		}
		for (j = 0; j <= order; ++j)
		{
			t[j] = t[j] + ipow (x[i], j) * y[i];
		}
	}

/* Put the value of s[] and t[] into a[][] */

	for (i = 0; i <= order; ++i)
	{
		for (j = 0; j <= order; ++j)
		{
			a[i][j] = s[i+j];
		}
		a[i][order+1] = t[i];
	}

/* Sweep-out the matrix */

	for (k = 0; k <= order; ++k)
	{
		p = a[k][k];

		for (j = k; j <= order+1; ++j)
			a[k][j] = a[k][j] / p;

		for (i = 0; i <= order; ++i)
		{				
			if (i != k)
			{
				d = a[i][k];
	
				for (j = k; j <= order+1; ++j)
					a[i][j] = a[i][j] - d * a[k][j];
			}
		}
	}


/* Put the results into the coefficient vector */

	for (i = 0; i <= order; ++i)
	{
		coefficient[i] = a[i][order+1];
	}
}


void STAR_SortRange(range)
/*
範囲を示すRANGE構造体の内容のソート
1992/12/21 T.Handa
*/
RANGE	*range;
{
	double dummy;

	if((*range).min>(*range).max)
	{
		dummy=(*range).min;
		(*range).min=(*range).max;
		(*range).max=dummy;
	}
}
void STAR_SortRangeLong(range)
/*
範囲を示すRANGE_long構造体の内容のソート
1992/12/21 T.Handa
*/
RANGE_long	*range;
{
	long dummy;

	if((*range).min>(*range).max)
	{
		dummy=(*range).min;
		(*range).min=(*range).max;
		(*range).max=dummy;
	}
}
void STAR_SortBoxRange(range)
/*
範囲を示すBOX_RANGE構造体の内容のソート
1992/12/21 T.Handa
*/
BOX_RANGE	*range;
{
	STAR_SortRange(&((*range).x));
	STAR_SortRange(&((*range).y));
}
void STAR_SortBoxRangeLong(range)
/*
範囲を示すBOX_RANGE_long構造体の内容のソート
1992/12/21 T.Handa
*/
BOX_RANGE_long	*range;
{
	STAR_SortRangeLong(&((*range).x));
	STAR_SortRangeLong(&((*range).y));
}
