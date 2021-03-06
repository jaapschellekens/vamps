/* $Header: /home/schj/src/vamps_0.99g/src/ts.lib/RCS/ts_spl.c,v 1.7 1999/01/06 12:13:01 schj Alpha $ */

/*  $RCSfile: ts_spl.c,v $
 *  $Author: schj $
 *  $Date: 1999/01/06 12:13:01 $ */

static char RCSid[] =
"$Id: ts_spl.c,v 1.7 1999/01/06 12:13:01 schj Alpha $";

/*C:ts_slopes
 *@double *ts_slopes(XY xy[], int np)
 *
 * Calculate the slopes of datapoints in array @xy@ of length
 * @np@ (@np >= 3@). For any point the slope is set to zero if
 * one or both of the segments has a zero slope or the segment
 * slopes have opposite signs. Hence, curve shape is preserved.
 * Return: pointer to double array of length @np@ holding the slopes,
 * retval may be passed to @free()@
 */
/*C:ts_meval
 *@int ts_meval(XY val[], XY tab[], double mtab[], int nt, int nv)
 *
 * Controls evaluation of an osculatory quadratic spline.
 * val is of length @nv@ and holds the ascending x values for
 * which the y values are evaluated from the @nt@ ascending x
 * and y values in @tab@. @mtab@ is of length @nt@ and holds the
 * slopes (possibly from @ts_slopes()@)
 * Return: 0 on normal run, 1 if extrapolation has occured.
 *
 * Reference: D.F. McAllister & J.A. Roulier, 1983, Shape Preserving
 * Quadratic Splines. Algorithm 574, ACM TOMS (7) 384-386.
 *
 * Note: Translated by f2c and cleaned up, but still a mess.
 */
#include "ts.h"

#define dabs(d)	((d) < 0.0 ? -(d) : (d))

static double spline (double xv, double z1, double z2, XY *xy0, XY *xy1,
	double y1, double y2, double e2, double w2, double v2, int ncase);
static int choose (XY *p, double m1, double m2, XY *q, double eps);
static int cases (XY *p, double m1, double m2, XY *q,
	double *e1, double *e2, double *v1, double *v2, double *w1, double *w2,
	double *z1, double *z2, double *y1, double *y2, int ncase); 

static int search(XY *tab,int np,double s,int *lcn);

double *
ts_slopes(XY *xy,int np)
{
	int	i;
	double	*mp;
	double	dy0, dy1, dy2, m1, m2;
	double	xmid, yxmid, xbar, xhat;

/* we need at least 3 points */
	if(np < 3)
		return((double *)NULL);
	
	mp = ts_memory((void *)NULL, np * sizeof(double), "ts_slopes");

	dy0 = dy1 = xy[1].y - xy[0].y;

/* all points between 0 and np - 1 */
	for(i = 1; i < np - 1; i++) {
		dy2 = xy[i+1].y - xy[i].y;
		if(dy1 * dy2 > 0.0) {
			m1 = dy1 / (xy[i].x - xy[i-1].x);
			m2 = dy2 / (xy[i+1].x - xy[i].x);
			if(m1 * m1 > m2 * m2) {
				xbar = dy2 / m1 + xy[i].x;
				xhat = (xbar + xy[i+1].x) / 2.0;
				mp[i] = dy2 / (xhat - xy[i].x);
			}
			else {
				xbar = -dy1 / m2 + xy[i].x;
				xhat = (xy[i-1].x + xbar) / 2.0;
				mp[i] = dy1 / (xy[i].x - xhat);
			}
		}
		else
			mp[i] = 0.0;
		dy1 = dy2;
	}

/* last point */
	m1 = (xy[i-1].y - xy[i-2].y) / (xy[i-1].x - xy[i-2].x);
	m2 = dy1 / (xy[i].x - xy[i-1].x);
	if(m1 * m2 < 0.0)
		mp[i] = 2.0 * m2;
	else {
		xmid = (xy[i-1].x - xy[i].x) / 2.0;
		yxmid = mp[i-1] * (xmid - xy[i-1].x) + xy[i-1].y;
		mp[i] = (xy[i].y - yxmid) / (xy[i].x - xmid);
		if(mp[i] * m2 < 0.0)
			mp[i] = 0.0;
	}

/* first point */
	m1 = dy0 / (xy[1].x - xy[0].x);
	m2 = (xy[2].y - xy[1].y) / (xy[2].x - xy[1].x);
	if(m1 * m2 < 0.0)
		mp[0] = 2.0 * m1;
	else {
		xmid = (xy[0].x + xy[1].x) / 2.0;
		yxmid = mp[1] * (xmid - xy[1].x) + xy[1].y;
		mp[0] = (yxmid - xy[0].y) / (xmid - xy[0].x);
		if(mp[0] * m1 < 0.0)
			mp[0] = 0.0;
	}

	return(mp);
}

int
ts_meval(XY *val,XY *tab,double *mtab,int nt,int nv)
{
	int	i,err;
	int ncase=0;
	int	start, start1, end, fnd, ind, lcn, lcn1;
	int nt1=0;
	double	e1, e2, v1, v2, w1, w2, z1, z2, y1, y2;
	double	eps = 1e-4;	/* set to fixed value */

/*   EPS IS A RELATIVE ERROR TOLERANCE USED IN SUBROUTINE 'CHOOSE' */
/*   TO DISTINGUISH THE SITUATION MTAB(I) OR MTAB(I+1) IS RELATIVELY */
/*   CLOSE TO THE SLOPE OR TWICE THE SLOPE OF THE LINEAR SEGMENT */
/*   BETWEEN XTAB(I) AND XTAB(I+1).  IF THIS SITUATION OCCURS, */
/*   ROUNDOFF MAY CAUSE A CHANGE IN CONVEXITY OR MONOTONICITY OF THE */
/*   RESULTING SPLINE AND A CHANGE IN THE CASE NUMBER PROVIDED BY */
/*   CHOOSE.  IF EPS IS NOT EQUAL TO ZERO, THEN EPS SHOULD BE GREATER */
/*   THAN OR EQUAL TO MACHINE EPSILON. */

    /* Parameter adjustments */
    --mtab;
    --tab;
    --val;

    /* Function Body */
    start = 1;
    end = nv;
    err = 0;
    if (nv == 1) {
	goto L20;
    }

/* IF XVAL(I) .LT. XTAB(1), THEN XVAL(I)=YTAB(1). */
/* IF XVAL(I) .GT. XTAB(NUM), THEN XVAL(I)=YTAB(NUM). */

/* DETERMINE IF ANY OF THE POINTS IN XVAL ARE LESS THAN THE ABSCISSA OF */
/* THE FIRST DATA POINT. */
L20:
    for (i = 1; i <= nv; ++i) {
	if (val[i].x >= tab[1].x) {
	    goto L40;
	}
	start = i + 1;
    }

L40:
/* DETERMINE IF ANY OF THE POINTS IN XVAL ARE GREATER THAN THE ABSCISSA */
/* OF THE LAST DATA POINT. */
    for (i = 1; i <= nv; ++i) {
	ind = nv + 1 - i;
	if (val[ind].x <= tab[nt].x) {
	    goto L60;
	}
	end = ind - 1;
    }

/* CALCULATE THE IMAGES OF POINTS OF EVALUATION WHOSE ABSCISSAS */
/* ARE LESS THAN THE ABSCISSA OF THE FIRST DATA POINT. */
L60:
    if (start == 1) {
	goto L80;
    }
/* SET THE ERROR PARAMETER TO INDICATE THAT EXTRAPOLATION HAS OCCURRED. */
    err = 1;
    ncase = choose(&tab[1], mtab[1], mtab[2], &tab[2], eps);
    cases(&tab[1], mtab[1], mtab[2], &tab[2],
	    &e1, &e2, &v1, &v2, &w1, &w2, &z1, &z2, &y1, &y2, ncase);
    start1 = start - 1;
    for (i = 1; i <= start1; ++i) {
	val[i].y = spline(val[i].x, z1, z2, &tab[1], &tab[2],
		y1, y2, e2, w2, v2, ncase);
    }
    if (nv == 1) {
	return(err);
    }

/* SEARCH LOCATES THE INTERVAL IN WHICH THE FIRST IN-RANGE POINT OF */
/* EVALUATION LIES. */
L80:
    if (nv == 1 && end != nv) {
	goto L200;
    }
    fnd = search(&tab[1], nt, val[start].x, &lcn);
    lcn++;	/* fortan crap */
    lcn1 = lcn + 1;

/* IF THE FIRST IN-RANGE POINT OF EVALUATION IS EQUAL TO ONE OF THE DATA */
/* POINTS, ASSIGN THE APPROPRIATE VALUE FROM YTAB.  CONTINUE UNTIL A */
/* POINT OF EVALUATION IS FOUND WHICH IS NOT EQUAL TO A DATA POINT. */
    if (fnd == 0) {
	goto L130;
    }
L90:
    val[start].y = tab[lcn].y;
    start1 = start;
    ++start;
    if (start > nv) {
	return(err);
    }
    if (val[start1].x == val[start].x) {
	goto L90;
    }

L100:
    if (val[start].x - tab[lcn1].x < 0.) {
	goto L130;
    } else if (val[start].x == tab[lcn1].x) {
	goto L110;
    } else {
	goto L120;
    }

L110:
    val[start].y = tab[lcn1].y;
    start1 = start;
    ++start;
    if (start > nv) {
	return(err);
    }
    if (val[start].x != val[start1].x) {
	goto L120;
    }
    goto L110;

L120:
    lcn = lcn1;
    ++lcn1;
    goto L100;

/* CALCULATE THE IMAGES OF ALL THE POINTS WHICH LIE WITHIN RANGE */

L130:
    if (lcn == 1 && err == 1) {
	goto L140;
    }
    ncase = choose(&tab[lcn], mtab[lcn], mtab[lcn1], &tab[lcn1], eps);
    cases(&tab[lcn], mtab[lcn], mtab[lcn1], &tab[lcn1],
	    &e1, &e2, &v1, &v2, &w1, &w2, &z1, &z2, &y1, &y2, ncase);

L140:
    for (i = start; i <= end; ++i) {

/* IF XVAL(I) -XTAB(LCN1) IS NEGATIVE, DO NOT RECALCULATE THE PARAMETERS */
/* FOR THIS SECTION OF THE SPLINE SINCE THEY ARE ALREADY KNOWN. */
	if (val[i].x - tab[lcn1].x < 0.) {
	    goto L150;
	} else if (val[i].x == tab[lcn1].x) {
	    goto L160;
	} else {
	    goto L170;
	}

L150:
	val[i].y = spline(val[i].x, z1, z2, &tab[lcn], &tab[lcn1],
		y1, y2, e2, w2, v2, ncase);
	goto L190;

/*  IF XVAL(I) IS A DATA POINT, ITS IMAGE IS KNOWN. */
L160:
	val[i].y = tab[lcn1].y;
	goto L190;

/* INCREMENT THE POINTERS WHICH GIVE THE LOCATION IN THE DATA VECTOR. */
L170:
	lcn = lcn1;
	lcn1 = lcn + 1;

/* DETERMINE THAT THE ROUTINE IS IN THE CORRECT PART OF THE SPLINE. */
	if (val[i].x - tab[lcn1].x < 0.) {
	    goto L180;
	} else if (val[i].x == tab[lcn1].x) {
	    goto L160;
	} else {
	    goto L170;
	}

/* CALL CHOOSE TO DETERMINE THE APPROPRIATE CASE AND THEN CALL */
/* CASES TO COMPUTE THE PARAMETERS OF THE SPLINE. */
L180:
	ncase = choose(&tab[lcn], mtab[lcn], mtab[lcn1], &tab[lcn1], eps);
	cases(&tab[lcn], mtab[lcn], mtab[lcn1], &tab[lcn1],
		&e1, &e2, &v1, &v2, &w1, &w2, &z1, &z2, &y1, &y2, ncase);
	goto L150;
L190:
	;
    }

/* CALCULATE THE IMAGES OF THE POINTS OF EVALUATION WHOSE ABSCISSAS */
/* ARE GREATER THAN THE ABSCISSA OF THE LAST DATA POINT. */
    if (end == nv) {
	return(err);
    }
    if (lcn1 == nt && val[end].x != tab[nt].x) {
	goto L210;
    }
/* SET THE ERROR PRARMETER TO INDICATE THAT EXTRAPOLATION HAS OCCURRED. */
L200:
    err = 1;
    nt1 = nt - 1;

/* BUG: arg 3 was set to tab[nt].x [XTAB(NUM)] */
    ncase = choose(&tab[nt1], mtab[nt1], mtab[nt], &tab[nt], eps);
    cases(&tab[nt1], mtab[nt1], mtab[nt], &tab[nt],
	    &e1, &e2, &v1, &v2, &w1, &w2, &z1, &z2, &y1, &y2, ncase);
L210:
    for (i = end + 1; i <= nv; ++i) {
	val[i].y = spline(val[i].x, z1, z2, &tab[nt1], &tab[nt],
		y1, y2, e2, w2, v2, ncase);
    }
    return(err);
}

/* returns the case for parameter calculation */
static
int
choose(XY *p,double m1,double m2,XY  *q,double eps)
{
    double	r__1, r__2;
    double	mref, prod, mref1, mref2, prod1, prod2, spq;

/* ON INPUT-- */

/*   (P1,P2) GIVES THE COORDINATES OF ONE OF THE POINTS OF INTERPOLATION. */

/*   M1 SPECIFIES THE DERIVATIVE CONDITION AT (P1,P2). */

/*   (Q1,Q2) GIVES THE COORDINATES OF ONE OF THE POINTS OF INTERPOLATION. */

/*   M2 SPECIFIES THE DERIVATIVE CONDITION AT (Q1,Q2). */

/*   EPS IS AN ERROR TOLERANCE USED TO DISTINGUISH CASES WHEN M1 OR M2 IS */
/*   RELATIVELY CLOSE TO THE SLOPE OR TWICE THE SLOPE OF THE LINE */
/*   SEGMENT JOINING (P1,P2) AND (Q1,Q2).  IF EPS IS NOT EQUAL TO ZERO, */
/*   THEN EPS SHOULD BE GREATER THAN OR EQUAL TO MACHINE EPSILON. */
/* ----------------------------------------------------------------------- */

/* CALCULATE THE SLOPE SPQ OF THE LINE JOINING (P1,P2),(Q1,Q2). */
    spq = (q->y - p->y) / (q->x - p->x);

/* CHECK WHETHER OR NOT SPQ IS 0. */
    if (spq != 0.) {
	goto L20;
    }
    if (m1 * m2 >= 0.) {
	return(2);
    }
    return(1);

L20:
    prod1 = spq * m1;
    prod2 = spq * m2;

/* FIND THE ABSOLUTE VALUES OF THE SLOPES SPQ,M1,AND M2. */
    mref = dabs(spq);
    mref1 = dabs(m1);
    mref2 = dabs(m2);

/* IF THE RELATIVE DEVIATION OF M1 OR M2 FROM SPQ IS LESS THAN EPS, THEN 
*/
/* CHOOSE CASE 2 OR CASE 3. */
    if ((r__1 = spq - m1, dabs(r__1)) <= eps * mref || (r__2 = spq - m2, 
	    dabs(r__2)) <= eps * mref) {
	goto L30;
    }

/* COMPARE THE SIGNS OF THE SLOPES SPQ,M1, AND M2. */
    if (prod1 < 0. || prod2 < 0.) {
	goto L80;
    }
    prod = (mref - mref1) * (mref - mref2);
    if (prod >= 0.) {
	goto L40;
    }

/* L1, THE LINE THROUGH (P1,P2) WITH SLOPE M1, AND L2, THE LINE THROUGH */
/* (Q1,Q2) WITH SLOPE M2, INTERSECT AT A POINT WHOSE ABSCISSA IS BETWEEN 
*/
/* P1 AND Q1.  THE ABSCISSA BECOMES A KNOT OF THE SPLINE. */
    return(1);

L30:
    if (prod1 < 0. || prod2 < 0.) {
	goto L80;
    }
L40:
    if (mref1 > mref * 2.) {
	goto L50;
    }
    if (mref2 > mref * 2.) {
	goto L60;
    }

/* BOTH L1 AND L2 CROSS THE LINE THROUGH (P1+Q1/2.,P2) AND (P1+Q1/2.,Q2), 
*/
/* WHICH IS THE MIDLINE OF THE RECTANGLE FORMED BY (P1,P2),(Q1,P2), */
/* (Q1,Q2), AND (P1,Q2), OR BOTH M1 AND M2 HAVE SIGNS DIFFERENT THAN THE 
*/
/* SIGN OF SPQ, OR ONE OF M1 AND M2 HAS OPPOSITE SIGN FROM SPQ AND L1 */
/* AND L2 INTERSECT TO THE LEFT OF P1 OR TO THE RIGHT OF Q1.  THE POINT */
/* (P1+Q1)/2. IS A KNOT OF THE SPLINE. */
    return(2);

/* CHOOSE CASE 4 IF MREF2 IS GREATER THAN (2.-EPS)*MREF; OTHERWISE, */
/* CHOOSE CASE 3. */
L50:
    if (mref2 > (2. - eps) * mref) {
	goto L70;
    }
    return(3);

/* IN CASES 3 AND 4, SIGN(M1)=SIGN(M2)=SIGN(SPQ). */

/* EITHER L1 OR L2 CROSSES THE MIDLINE, BUT NOT BOTH. */
/* CHOOSE CASE 4 IF MREF1 IS GREATER THAN (2.-EPS)*MREF; OTHERWISE, */
/* CHOOSE CASE 3. */
L60:
    if (mref1 > (2. - eps) * mref) {
	goto L70;
    }
    return(3);

/* IF NEITHER L1 NOR L2 CROSSES THE MIDLINE, THE SPLINE REQUIRES TWO */
/* KNOTS BETWEEN P1 AND Q1. */
L70:
    return(4);

/* THE SIGN OF AT LEAST ONE OF THE SLOPES M1,M2 DOES NOT AGREE WITH THE */
/* SIGN OF THE SLOPE SPQ. */
L80:
    if (prod1 < 0. && prod2 < 0.) {
	return(2);
    }

    if (prod1 < 0.) {
	goto L90;
    }
    goto L110;

L90:
    if (mref2 > (eps + 1.) * mref) {
	return(1);
    }
    return(2);

L110:
    if (mref1 > (eps + 1.) * mref) {
	return(1);
    }
    return(2);
}

static
int
cases (XY *p,double m1,double m2,XY *q,double *e1,double *e2,double *v1,double *v2,double *w1,double *w2,double *z1,double *z2,double *y1,double *y2,int ncase)
{
	double	mbar1, mbar2, mbar3, ztwo, c1, d1, h1, j1, k1;

/* CASES COMPUTES THE KNOTS AND OTHER PARAMETERS OF THE SPLINE ON THE */
/* INTERVAL (P1,Q1). */

/* ON INPUT-- */

/*   (P1,P2) AND (Q1,Q2) ARE THE COORDINATES OF THE POINTS OF */
/*   INTERPOLATION. */

/*   M1 IS THE SLOPE AT (P1,P2). */

/*   M2 IS THE SLOPE AT (Q1,Q2) */

/*   NCASE CONTROLS THE NUMBER AND LOCATION OF THE KNOTS. */


/* ON OUTPUT-- */

/*   (V1,V2),(W1,W2),(Z1,Z2), AND (E1,E2) ARE THE COORDINATES OF THE */
/*   KNOTS AND OTHER PARAMETERS OF THE SPLINE ON (P1,Q1).  (E1,E2) */
/*   AND (Y1,Y2) ARE USED ONLY IF NCASE=4. */

/* -----------------------------------------------------------------------
 */

    if (ncase == 3 || ncase == 4) {
	goto L20;
    }
    if (ncase == 2) {
	goto L10;
    }

/* CALCULATE THE PARAMETERS FOR CASE 1. */
    *z1 = (p->y - q->y + m2 * q->x - m1 * p->x) / (m2 - m1);
    ztwo = p->y + m1 * (*z1 - p->x);
    *v1 = (p->x + *z1) / 2.;
    *v2 = (p->y + ztwo) / 2.;
    *w1 = (*z1 + q->x) / 2.;
    *w2 = (ztwo + q->y) / 2.;
    *z2 = *v2 + (*w2 - *v2) / (*w1 - *v1) * (*z1 - *v1);
    return(0);

/* CALCULATE THE PARAMETERS FOR CASE 2. */
L10:
    *z1 = (p->x + q->x) / 2.;
    *v1 = (p->x + *z1) / 2.;
    *v2 = p->y + m1 * (*v1 - p->x);
    *w1 = (*z1 + q->x) / 2.;
    *w2 = q->y + m2 * (*w1 - q->x);
    *z2 = (*v2 + *w2) / 2.;
    return(0);

/* CALCULATE THE PARAMETERS USED IN BOTH CASES 3 AND 4. */
L20:
    c1 = p->x + (q->y - p->y) / m1;
    d1 = q->x + (p->y - q->y) / m2;
    h1 = c1 * 2. - p->x;
    j1 = d1 * 2. - q->x;
    mbar1 = (q->y - p->y) / (h1 - p->x);
    mbar2 = (p->y - q->y) / (j1 - q->x);

    if (ncase == 4) {
	goto L50;
    }

/* CALCULATE THE PARAMETERS FOR CASE 3. */
    k1 = (p->y - q->y + q->x * mbar2 - p->x * mbar1) / (mbar2 - mbar1);
    if (dabs(m1) > dabs(m2)) {
	goto L30;
    }
    *z1 = (k1 + q->x) / 2.;
    goto L40;
L30:
    *z1 = (k1 + p->x) / 2.;
L40:
    *v1 = (p->x + *z1) / 2.;
    *v2 = p->y + m1 * (*v1 - p->x);
    *w1 = (q->x + *z1) / 2.;
    *w2 = q->y + m2 * (*w1 - q->x);
    *z2 = *v2 + (*w2 - *v2) / (*w1 - *v1) * (*z1 - *v1);
    return(0);

/* CALCULATE THE PARAMETERS FOR CASE 4. */
L50:
    *y1 = (p->x + c1) / 2.;
    *v1 = (p->x + *y1) / 2.;
    *v2 = m1 * (*v1 - p->x) + p->y;
    *z1 = (d1 + q->x) / 2.;
    *w1 = (q->x + *z1) / 2.;
    *w2 = m2 * (*w1 - q->x) + q->y;
    mbar3 = (*w2 - *v2) / (*w1 - *v1);
    *y2 = mbar3 * (*y1 - *v1) + *v2;
    *z2 = mbar3 * (*z1 - *v1) + *v2;
    *e1 = (*y1 + *z1) / 2.;
    *e2 = mbar3 * (*e1 - *v1) + *v2;
    return(0);
}

static double
spline(double xv,double z1,double z2,XY *xy0,XY *xy1,double y1,double y2,double e2,double w2,double v2,int ncase)
{
    double	r__1, r__2, r__3;

/*   SPLINE FINDS THE IMAGE OF A POINT IN XVAL. */

/*   XVALS CONTAINS THE VALUE AT WHICH THE SPLINE IS EVALUATED. */

/*   (XTABS,YTABS) ARE THE COORDINATES OF THE LEFT-HAND DATA POINT */
/*   USED IN THE EVALUATION OF XVALS. */

/*   (XTABS1,YTABS1) ARE THE COORDINATES OF THE RIGHT-HAND DATA POINT */
/*   USED IN THE EVALUATION OF XVALS. */

/*   Z1,Z2,Y1,Y2,E2,W2,V2 ARE THE PARAMETERS OF THE SPLINE. */

/*   NCASE CONTROLS THE EVALUATION OF THE SPLINE BY INDICATING WHETHER */
/*   ONE OR TWO KNOTS WERE PLACED IN THE INTERVAL (XTABS,XTABS1). */
/* ----------------------------------------------------------------------- */

/* IF NCASE .EQ. 4, MORE THAN ONE KNOT WAS PLACED IN THE INTERVAL. */
	if(ncase == 4)
		goto L40;

/* CASES 1,2, OR 3. */

/* DETERMINE THE LOCATION OF XVALS RELATIVE TO THE KNOT. */
	if(z1 - xv < 0.0) {
		r__1 = xy1->x - xv;
		r__2 = xv - z1;
		r__3 = xy1->x - z1;
		return((z2 * (r__1 * r__1) + w2 * 2.0 * (xv - z1) *
		    (xy1->x - xv) + xy1->y * (r__2 * r__2)) / (r__3 * r__3));
	}
	else if(z1 == xv)
		return(z2);
	else {
		r__1 = z1 - xv;
		r__2 = xv - xy0->x;
		r__3 = z1 - xy0->x;
		return((xy0->y * (r__1 * r__1) + v2 * 2.0 * (xv - xy0->x) * 
			(z1 - xv) + z2 * (r__2 * r__2)) / (r__3 * r__3));
	}

/* CASE 4. */

/* DETERMINE THE LOCATION OF XVALS RELATIVE TO THE FIRST KNOT. */
L40:
	if(y1 - xv < 0.0) {
	goto L70;
	}
	else if(y1 == xv)
		return(y2);
	else {
		r__1 = y1 - xv;
		r__2 = xv - xy0->x;
		r__3 = y1 - xy0->x;
		return((xy0->y * (r__1 * r__1) + v2 * 2.0 * (xv - xy0->x) * 
			(y1 - xv) + y2 * (r__2 * r__2)) / (r__3 * r__3));
	}

/* DETERMINE THE LOCATION OF XVALS RELATIVE TO THE SECOND KNOT. */
L70:
	if(z1 - xv < 0.0) {
		r__1 = xy1->x - xv;
		r__2 = xv - z1;
		r__3 = xy1->x - z1;
		return((z2 * (r__1 * r__1) + w2 * 2.0 * (xv - z1) *
		    (xy1->x - xv) + xy1->y * (r__2 * r__2)) / (r__3 * r__3));
	}
	else if(z1 == xv)
		return(z2);
	else {
		r__1 = z1 - xv;
		r__2 = xv - y1;
		r__3 = z1 - y1;
		return((y2 * (r__1 * r__1) + e2 * 2.0 * (xv - y1) *
			(z1 - xv) + z2 * (r__2 * r__2)) / (r__3 * r__3));
	}
}

/*
 * binary search for s compared to tab[i].x. Returns 1 on exact match,
 * else 0. On return lcn holds largest index for which tab[lcn].x <= s
 */
static
int
search(XY *tab,int np,double s,int *lcn)
{
	int	last, first, middle;

	first = 0;
	last = np - 1;

	if(tab[0].x == s) {
		*lcn = 0;
		return(1);
	}
	if(tab[last].x == s) {
		*lcn = last;
		return(1);
	}
/* if last - first == 1, s is out of range; lcn set to 0 */
	while(last - first != 1) {
		middle = (first + last) / 2;
		if(tab[middle].x < s)
			first = middle;
		else if(tab[middle].x > s)
			last = middle;
		else {
			*lcn = middle;
			return(1);
		}
	}
	*lcn = first;
	return(0);
}
