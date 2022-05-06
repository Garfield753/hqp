
/**************************************************************************
**
** Copyright (C) 1993 David E. Steward & Zbigniew Leyk, all rights reserved.
**
**			     Meschach Library
** 
** This Meschach Library is provided "as is" without any express 
** or implied warranty of any kind with respect to this software. 
** In particular the authors shall not be liable for any direct, 
** indirect, special, incidental or consequential damages arising 
** in any way from use of the software.
** 
** Everyone is granted permission to copy, modify and redistribute this
** Meschach Library, provided:
**  1.  All copies contain this copyright notice.
**  2.  All modified copies shall carry a notice stating who
**      made the last modification and the date of such modification.
**  3.  No charge is made for this software or works derived from it.  
**      This clause shall not be construed as constraining other software
**      distributed on the same medium as this software, nor is a
**      distribution fee considered a charge.
**
***************************************************************************/


/*
	A collection of functions for computing norms: scaled and unscaled
*/
static	char	rcsid[] = "$Id: norm.c,v 1.3 2002/12/09 10:57:47 e_arnold Exp $";

#include	<stdio.h>
#include	<math.h>
#include	"matrix.h"


/* _v_norm1 -- computes (scaled) 1-norms of vectors */
Real	_v_norm1(x,scale)
const VEC *x, *scale;
{
	int	i, dim;
	Real	s, sum;

	if ( x == (VEC *)NULL )
		m_error(E_NULL,"_v_norm1");
	dim = x->dim;

	sum = 0.0;
	if ( scale == (VEC *)NULL )
		for ( i = 0; i < dim; i++ )
			sum += fabs(x->ve[i]);
	else if ( (int) scale->dim < dim )
		m_error(E_SIZES,"_v_norm1");
	else
		for ( i = 0; i < dim; i++ )
		{	s = scale->ve[i];
			sum += ( s== 0.0 ) ? fabs(x->ve[i]) : fabs(x->ve[i]/s);
		}

	return sum;
}

/* square -- returns x^2 */
Real	square(x)
Real	x;
{	return x*x;	}

#if 0
/* cube -- returns x^3 */
Real cube(x)
Real x;
{  return x*x*x;   }
#endif

/* _v_norm2 -- computes (scaled) 2-norm (Euclidean norm) of vectors */
Real	_v_norm2(x,scale)
const VEC *x, *scale;
{
	int	i, dim;
	Real	s, sum;

	if ( x == (VEC *)NULL )
		m_error(E_NULL,"_v_norm2");
	dim = x->dim;

	sum = 0.0;
	if ( scale == (VEC *)NULL )
		for ( i = 0; i < dim; i++ )
			sum += square(x->ve[i]);
	else if ( (int) scale->dim < dim )
		m_error(E_SIZES,"_v_norm2");
	else
		for ( i = 0; i < dim; i++ )
		{	s = scale->ve[i];
			sum += ( s== 0.0 ) ? square(x->ve[i]) :
							square(x->ve[i]/s);
		}

	return sqrt(sum);
}

/* _v_norm_inf -- computes (scaled) infinity-norm (supremum norm) of vectors */
Real	_v_norm_inf(x,scale)
const VEC *x, *scale;
{
	int	i, dim;
	Real	s, maxval, tmp;

	if ( x == (VEC *)NULL )
		m_error(E_NULL,"_v_norm_inf");
	dim = x->dim;

	maxval = 0.0;
	if ( scale == (VEC *)NULL )
		for ( i = 0; i < dim; i++ )
		{	tmp = fabs(x->ve[i]);
			maxval = hqp_max(maxval,tmp);
		}
	else if ( (int) scale->dim < dim )
		m_error(E_SIZES,"_v_norm_inf");
	else
		for ( i = 0; i < dim; i++ )
		{	s = scale->ve[i];
			tmp = ( s== 0.0 ) ? fabs(x->ve[i]) : fabs(x->ve[i]/s);
			maxval = hqp_max(maxval,tmp);
		}

	return maxval;
}

/* m_norm1 -- compute matrix 1-norm -- unscaled */
Real	m_norm1(A)
const MAT *A;
{
	int	i, j, m, n;
	Real	maxval, sum;

	if ( A == (MAT *)NULL )
		m_error(E_NULL,"m_norm1");

	m = A->m;	n = A->n;
	maxval = 0.0;

	for ( j = 0; j < n; j++ )
	{
		sum = 0.0;
		for ( i = 0; i < m; i ++ )
			sum += fabs(A->me[i][j]);
		maxval = hqp_max(maxval,sum);
	}

	return maxval;
}

/* m_norm_inf -- compute matrix infinity-norm -- unscaled */
Real	m_norm_inf(A)
const MAT *A;
{
	int	i, j, m, n;
	Real	maxval, sum;

	if ( A == (MAT *)NULL )
		m_error(E_NULL,"m_norm_inf");

	m = A->m;	n = A->n;
	maxval = 0.0;

	for ( i = 0; i < m; i++ )
	{
		sum = 0.0;
		for ( j = 0; j < n; j ++ )
			sum += fabs(A->me[i][j]);
		maxval = hqp_max(maxval,sum);
	}

	return maxval;
}

/* m_norm_frob -- compute matrix frobenius-norm -- unscaled */
Real	m_norm_frob(A)
const MAT *A;
{
	int	i, j, m, n;
	Real	sum;

	if ( A == (MAT *)NULL )
		m_error(E_NULL,"m_norm_frob");

	m = A->m;	n = A->n;
	sum = 0.0;

	for ( i = 0; i < m; i++ )
		for ( j = 0; j < n; j ++ )
			sum += square(A->me[i][j]);

	return sqrt(sum);
}

