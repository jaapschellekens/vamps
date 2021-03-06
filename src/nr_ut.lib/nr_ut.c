#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "nrutil.h"


#ifdef NR_LIST
/*C:todvec
 *@double *todvec(void *mat,int col)
 *
 * Extract column @col@ from matrix @mat@ and return a newly
 * allocated double vector. If @mat@ is a vector the column field
 * should be 0 and a copy of the vector is returned. All types are
 * handled. If @mat@ is not in the list or the column is invalid
 * NULL is returned.*/
double *todvec(void *mat,int col)
{
	int	nr,i;
	mdata_type *d;
	double *retv;

	if ((nr = get_id((void *)mat)) == -1)
		return NULL;
	else{
		d = &md[nr];

		if (col > d->lc || col < d->fc)
			return NULL;

		retv = dvector(d->fr,d->lr);

		switch (d->t){
			case NR_DM:	 
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = d->d.dm[i][col];
				break;
			case NR_FM: 
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (double)d->d.fm[i][col];
				break;
			case NR_IM:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (double)d->d.im[i][col];
				break;
			case NR_DV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = d->d.dv[i];
				break;
			case NR_FV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (double)d->d.fv[i];
				break;
			case NR_IV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (double)d->d.iv[i];
				break;
			case NR_CV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (double)d->d.cv[i];
				break;
			case NR_LV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (double)d->d.lv[i];
				break;
			default:
				break;
		}
	}

	return retv;
}

/*C:todxy
 *@double **todxy(void *mat,int x, int y)
 *
 * Extract column @x@ and @y@ from matrix @mat@ and returns a newly
 * allocated matrix. 
 * If @mat@ is not in the list or the column is invalid
 * NULL is returned.*/
double **todxy(void *mat,int x, int y)
{
	int	nr,i;
	mdata_type *d;
	double **retv;

	if ((nr = get_id((void *)mat)) == -1)
		return NULL;
	else{
		d = &md[nr];

		if (x > d->lc || x < d->fc || y > d->lc || y < d->fc)
			return NULL;

		retv = dmatrix(d->fr,d->lr,0,1);
		d = &md[nr];

		switch (d->t){
			case NR_DM:	 
				for (i=d->fr;i<=d->lr;i++){
 					retv[i][0] = d->d.dm[i][x];
					retv[i][1] = d->d.dm[i][y];
				}
				break;
			case NR_FM:	 
				for (i=d->fr;i<=d->lr;i++){
 					retv[i][0] = (double)d->d.fm[i][x];
					retv[i][1] = (double)d->d.fm[i][y];
				}
				break;
			case NR_IM:
				for (i=d->fr;i<=d->lr;i++){
 					retv[i][0] = (double)d->d.im[i][x];
					retv[i][1] = (double)d->d.im[i][y];
				}
				break;
			default:
				break;
		}
	}

	return retv;
}

/*C:tocvec
 *@unsigned char *tocvec(void *mat,int col)
 *
 * Extract column @col@ from matrix @mat@ and return a newly
 * allocated char vector. If @mat@ is a vector the column field
 * should be 0 and a copy of the vector is returned. All types are
 * handled. If @mat@ is not in the list or the column is invalid
 * NULL is returned.*/
unsigned char *tocvec(void *mat,int col)
{
	int	nr,i;
	mdata_type *d;
	unsigned char *retv;

	if ((nr = get_id((void *)mat)) == -1)
		return NULL;
	else{
		d = &md[nr];

		if (col > d->lc || col < d->fc)
			return NULL;

		retv = cvector(d->fr,d->lr);

		switch (d->t){
			case NR_DM:	 
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (char)d->d.dm[i][col];
				break;
			case NR_FM: 
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (char)d->d.fm[i][col];
				break;
			case NR_IM:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (char)d->d.im[i][col];
				break;
			case NR_DV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = d->d.dv[i];
				break;
			case NR_FV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (char)d->d.fv[i];
				break;
			case NR_IV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (char)d->d.iv[i];
				break;
			case NR_CV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (char)d->d.cv[i];
				break;
			case NR_LV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (char)d->d.lv[i];
				break;
			default:
				break;
		}
	}

	return retv;
}

/*C:tofvec
 *@float *tofvec(void *mat,int col)
 *
 * Extract column @col@ from matrix @mat@ and return a newly
 * allocated float vector. If @mat@ is a vector the column field
 * should be 0 and a copy of the vector is returned. All types are
 * handled. If @mat@ is not in the list or the column is invalid
 * NULL is returned.*/
float *tofvec(void *mat,int col)
{
	int	nr,i;
	mdata_type *d;
	float *retv;

	if ((nr = get_id((void *)mat)) == -1)
		return NULL;
	else{
		d = &md[nr];

		if (col > d->lc || col < d->fc)
			return NULL;

		retv = vector(d->fr,d->lr);

		switch (d->t){
			case NR_DM:	 
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (float)d->d.dm[i][col];
				break;
			case NR_FM: 
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (float)d->d.fm[i][col];
				break;
			case NR_IM:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (float)d->d.im[i][col];
				break;
			case NR_DV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = d->d.dv[i];
				break;
			case NR_FV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (float)d->d.fv[i];
				break;
			case NR_IV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (float)d->d.iv[i];
				break;
			case NR_CV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (float)d->d.cv[i];
				break;
			case NR_LV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (float)d->d.lv[i];
				break;
			default:
				break;
		}
	}

	return retv;
}

/*C:toivec
 *@int *toivec(void *mat,int col)
 *
 * Extract column @col@ from matrix @mat@ and return a newly
 * allocated int vector. If @mat@ is a vector the column field
 * should be 0 and a copy of the vector is returned. All types are
 * handled. If @mat@ is not in the list or the column is invalid
 * NULL is returned.*/
int *toivec(void *mat,int col)
{
	int	nr,i;
	mdata_type *d;
	int *retv;

	if ((nr = get_id((void *)mat)) == -1)
		return NULL;
	else{
		d = &md[nr];

		if (col > d->lc || col < d->fc)
			return NULL;

		retv = ivector(d->fr,d->lr);

		switch (d->t){
			case NR_DM:	 
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (int)d->d.dm[i][col];
				break;
			case NR_FM: 
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (int)d->d.fm[i][col];
				break;
			case NR_IM:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (int)d->d.im[i][col];
				break;
			case NR_DV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = d->d.dv[i];
				break;
			case NR_FV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (int)d->d.fv[i];
				break;
			case NR_IV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (int)d->d.iv[i];
				break;
			case NR_CV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (int)d->d.cv[i];
				break;
			case NR_LV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (int)d->d.lv[i];
				break;
			default:
				break;
		}
	}

	return retv;
}


/*C:tolvec
 *@unsigned long *tolvec(void *mat,int col)
 *
 * Extract column @col@ from matrix @mat@ and return a newly
 * allocated long vector. If @mat@ is a vector the column field
 * should be 0 and a copy of the vector is returned. All types are
 * handled. If @mat@ is not in the list or the column is invalid
 * NULL is returned.*/
unsigned long *tolvec(void *mat,int col)
{
	int	nr,i;
	mdata_type *d;
	unsigned long *retv;

	if ((nr = get_id((void *)mat)) == -1)
		return NULL;
	else{
		d = &md[nr];

		if (col > d->lc || col < d->fc)
			return NULL;

		retv = lvector(d->fr,d->lr);

		switch (d->t){
			case NR_DM:	 
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (long)d->d.dm[i][col];
				break;
			case NR_FM: 
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (long)d->d.fm[i][col];
				break;
			case NR_IM:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (long)d->d.im[i][col];
				break;
			case NR_DV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = d->d.dv[i];
				break;
			case NR_FV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (long)d->d.fv[i];
				break;
			case NR_IV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (long)d->d.iv[i];
				break;
			case NR_CV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (long)d->d.cv[i];
				break;
			case NR_LV:
				for (i=d->fr;i<=d->lr;i++)
					retv[i] = (long)d->d.lv[i];
				break;
			default:
				break;
		}
	}

	return retv;
}



/*C:nr_getval
 *@ double nr_getval(double **tab, double xval, int xcol, int ycol,
 *@			int ys, int ye, int lin)
 * 
 * gets the y value in @ycol@ from xval from matrix tab. @ycol@ is searched
 * between index @ys@ and @ye@. Linear interpolation
 * is used for increased accuracy if @lin@ > 0. @xcol@ should be sorted!
 */
double nr_getval(double **tab, double xval, int xcol, int ycol, int ys, int ye, int lin)
{
	unsigned long index,mid,lo,up,n;
	int ascnd;
	double rico;

  	/* first check high and low points */
	
	if (xval <= tab[xcol][ys])
		return tab[ycol][ys];
	else if (xval >= tab[xcol][ye])
		return tab[xcol][ye];

	/* Find position in array: x[index] < xval < x[index +1]*/
	n = ye - ys + 1;
	lo = ys-1; up = ye;
	ascnd = (tab[xcol][ye] >= tab[xcol][ys]);
	while (up - lo > 1){
		mid = (up + lo) >> 1;
		if (xval >= tab[xcol][mid] == ascnd)
			lo = mid;
		else
			up = mid;
	}
	if ( xval == tab[xcol][ys])  index = ys;
	else if (xval == tab[xcol][n-1]) index =  n - 2;
	else index = lo;

	if (xval == tab[xcol][index])
		return tab[ycol][index]; /* exact match */

	if (lin){
		/* linear interpolation */
		rico = (tab[ycol][index + 1] - tab[ycol][index]) / (tab[xcol][index + 1] - tab[xcol][index]);
		return (rico * (xval - tab[xcol][index])) + tab[ycol][index];
	}else
		return tab[ycol][index];
}

#endif
