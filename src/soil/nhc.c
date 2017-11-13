/*$Header: /home/schj/src/vamps_0.99g/src/soil/RCS/nhc.c,v 1.2 1999/01/06 12:13:01 schj Alpha $ 
 */
/*  $RCSfile: nhc.c,v $
 *  $Author: schj $
 *  $Date: 1999/01/06 12:13:01 $
 */

#ifdef NHC
static char RCSid[] =
"$Id: nhc.c,v 1.2 1999/01/06 12:13:01 schj Alpha $";

#include "nrutil.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static double *k,*theta,*dkdp,*dkdp1,*dzc,*dzf,dt;
static double *dfa,*dfb,*dfc;
static double *h,*h1,*psit;
static double *cc,*bb; /* temp storage for ntridiag */
static double *f,*e,*ec,*qb,*U,*k1,*U1,*qb1,*dk1,*dt1,*theta1,*res;
static double drain,ddrain;



void init_nhc (int n)
{
	cc = dvector(0, n -1);
	bb = dvector(0, n -1);
	e = dvector(0, n -1);
	f = dvector(0, n -1);
	ec = dvector(0, n -1);
	res = dvector(0, n -1);
	U = dvector(0, n -1);
	dk1 = dvector(0, n -1);
	dt1 = dvector(0, n -1);
	U1 = dvector(0, n -1);
	qb = dvector(0, n -1);
	qb1 = dvector(0, n -1);
	psit = dvector(0, n -1);
	dkdp = dvector(0, n -1);
	dkdp1 = dvector(0, n -1);
	dfb = dvector(0, n -1);
	dfa = dvector(0, n -1);
	dfc = dvector(0, n -1);
}



int nheadcalc(int n, double *t, double *dt)
{
	int itt =0;
	first_est(n, t, dt);

	do{
		itt++;
	}while(nhc(n,t,dt) !=0 && itt < maxit);
}



/*
main()
{
	int i;
	char s[128];

	init_nhc(n);

	dt = 0.01; 
	nn = 2.0 + 3.0/b;

	for (i=0;i < n;i++){
		dzf[i] = dzc[i] = 1.0; 
		h[i] = -90;
	}
	for (i = 0; i < n; i++){
		h1[i] = h[i]; 
	}

	for (i = 0; i < n; i++){
		psit[i] = h[i];
		k[i] = h2k(psit[i]);
	}

	nr_genw("hini",(void *)h);
	for (day=0; day < 4; day++){
		nhc(n);
		sprintf(s,"%d.theta",day);
		nr_genw(s,(void *)theta1);
		sprintf(s,"%d.h",day);
		nr_genw(s,(void *)psit);
		nr_genw("kend",(void *)k1);
		nr_genw("greekc",(void *)greekc);
		sprintf(s,"%d.res",day);
		nr_genw(s,(void *)res);
		sprintf(s,"%d.qb1",day);
		nr_genw(s,(void *)qb1);
		sprintf(s,"%d.f",day);
		nr_genw(s,(void *)f);
		nr_genw("e",(void *)e);
	}
}*/

void 
mkmat(double *theta, double *k, double *dkdp,double *dzc,double *dzf,
		double dt, int n)
{
	int i;
	
	/* only flux controlled upper boundary at moment */
	/* dfdp = thomb, dfdp_min_1 = thomc, dfdp_plus_1 = thoma */
	/* first the top node...*/
	f[0] = qb1[0] + e[0];
	/*	f[0] = 0.0;*/
	dfb[0] = 0.5 * dkdp[0] * sqrt(k[1]/k[0]) + (k[0]/dzf[0]) + 
		(dt1[0] * dzf[0])/(2.0 * dt);
	dfc[0] = 0.5 * dkdp[1] * sqrt(k[0]/k[1]) - (k[1]/dzf[0]);
/* add f's here to .. */	


	/* now the inner nodes...*/
	for (i = 1; i < n-1; i++){ /* page 29 Dawes & Hatton eq: A17-A19 */
		f[i] = qb1[i] - qb1[i-1] + e[i];
		dfa[i] = -0.5 * dkdp[i-1] * sqrt(k[i]/k[i-1]) -
			k[i-1]/dzf[i-1];
		dfb[i] = 0.5 * dkdp[i] * (sqrt(k[i+1]/k[i]) - 
				sqrt(k[i-1]/k[i])) + 
				k[i]*(1.0/dzf[i-1] + 1/dzf[i]) + 
				1.0/dzf[i] + (dt1[i] * dzc[i])/dt;
		dfc[i] = 0.5 * dkdp[i+1] * sqrt(k[i]/k[i+1]) -
				(k[i+1]/dzf[i]); 	
	}

	i = n-1; /* overbodig */
	/* the bottom node...*/
	f[i] = -qb1[i-1] + e[i] + drain;
	dfa[i] = -0.5 * dkdp[i-1] * sqrt(k[i]/k[i-1])-(k[i-1]/dzf[i-1]);
	dfb[i] = -0.5 * dkdp[i] * sqrt(k[i-1]/k[i]) + (k[i]/dzf[i-1]) +
		(dt1[i] * dzf[i-1])/(2.0 * dt) + ddrain;
}


void first_est(int n, double *t, double *dt)
{
	int i=0;

	k[i] = h2k(psit[i]);
	theta[i] = h2t(psit[i]);
	U[i] = h2u(psit[i]);
	/*qb[i] = sqrt(k[i] * k[i+1]) - (U[i+1] - U[i])/dzf[i];*/
	ec[i] = theta[i] * dzf[i] /(2.0 * (*dt));
	for (i = 1; i < n -1; i++){
		k[i] = h2k(psit[i]);
		theta[i] = h2t(psit[i]);
		U[i] = h2u(psit[i]);
		qb[i-1] = sqrt(k[i] * k[i-1]) - (U[i] - U[i-1])/dzf[i-1];
		ec[i] = theta[i] * dzc[i]/(2.0*(*dt));
	}
	i = n-1; /* overbodig */
	k[i] = h2k(psit[i]);
	theta[i] = h2t(psit[i]);
	U[i] = h2u(psit[i]);
	ec[i] = theta[i] * dzf[i-1] /(2.0 *(*dt));
	qb[i-1] = sqrt(k[i] * k[i-1]) - (U[i] - U[i-1])/dzf[i-1];
}

int nhc(int n, double *t, double *dt)
{
	int kk,i = 0;
	int noconv = 0;
	int itt = 0;
	double convcrit = 1.0E-6;


	itt = 0;
	/* is eigelijk nog extra code voor laag-grenzen nodig !! */
	i = 0;

	
	/* is eigelijk nog extra code voor laag-grenzen nodig !! */
	i = 0;
	k1[i] = h2k(psit[i]);
	theta1[i] = h2t(psit[i]);
	U1[i] = h2u(psit[i]);
	e[i] = theta1[i] * dzf[i] /(2.0 *dt) - ec[i];
	for (i = 1; i < n -1; i ++){
		k1[i] = h2k(psit[i]);
		theta1[i] = h2t(psit[i]);
		U1[i] = h2u(psit[i]);
		/* nodal derivatives */
		dk1[i] = h2dkdp(psit[i]);
		dt1[i] = h2dtdp(psit[i]);
		qb1[i-1] = sqrt(k1[i] * k1[i-1]) - (U1[i] - U1[i-1])/dzf[i-1];
		e[i] = theta1[i] * dzc[i]/(2.0*dt) - ec[i];
	}
	i = n-1;
	k1[i] = h2k(psit[i]);
	theta1[i] = h2t(psit[i]);
	U1[i] = h2u(psit[i]);
	/* nodal derivatives */
	dk1[i] = h2dkdp(psit[i]);
	dt1[i] = h2dtdp(psit[i]);
	e[i] = theta1[i] * dzf[i-1] /(2.0 *dt) - ec[i];
	qb1[i-1] = sqrt(k1[i] * k1[i-1]) - (U1[i] - U1[i-1])/dzf[i-1];
	/*qb1[i-1] = 0.0;*/
	
	/* Gravity drainage .expand later...*/
	drain = h2k(psit[i]);
	ddrain = h2dkdp(psit[i]);
	/*drain = 0.0;ddrain = 0.0;		*/
	
	mkmat(theta1, k1, dk1,dzc,dzf, dt, n);
	ntridiag(dfa,dfb,dfc,f,res, bb, cc, n);
	noconv = 0;
	itt++;
	for (i = 0; i < n; i++){
		psit[i] = psit[i] - res[i];
		if (fabs(f[i]) > convcrit)
			noconv = 1;
	}
	
        return noconv;
}


/*C:ntridiag
 *@ int ntridiag(double *fa,double *fb,double *fc, double *f, double
 *@              *res, double *greekb, double *greekc, int n)
 * 
 * Description: Solves the matrix by the simple and fast algorithm for
 * a tridiagonal matrix. If a zero pivot occurs it exits and returns 1
 * and the more general method for solving band-diagonal matrixes
 * should be used. 0 is returned on success
 *
 * Returns: 0 on sucess, 1 on failure*/

int
ntridiag (double *fa, double *fb, double *fc, double *f,double *res, 
	  double *greekb, double *greekc, int n)
{
	int i;
	double temp = 0.0;

	fa[0] = 0.0;
	fc[n-1] = 0.0;


	greekb[0] = fc[0]/fb[0];
	greekc[0] = f[0]/fb[0];
	for (i = 0; i < n-1; i++){
		temp = fb[i+1] - fa[i+1] * greekb[i];
		if (temp == 0.0)
			return 1;
		greekb[i+1] = fc[i+1]/temp;
		greekc[i+1] = (f[i+1] - fa[i+1] * greekc[i])/temp;
	}

	res[n-1] = greekc[n-1];
	for (i = n-2; i >= 0; i--){
		res[i] = greekc[i] - greekb[i] * res[i+1];
	}
	
	return 0;
}
#endif /*NHC*/
