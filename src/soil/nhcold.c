/*$Header: /home/schj/src/vamps_0.99g/src/soil/RCS/nhcold.c,v 1.2 1999/01/06 12:13:01 schj Alpha $ 
 */
/*  $RCSfile: nhcold.c,v $
 *  $Author: schj $
 *  $Date: 1999/01/06 12:13:01 $
 */

/*LAATSTE NODE VERANDERD NIET????????*/
static char RCSid[] =
"$Id: nhcold.c,v 1.2 1999/01/06 12:13:01 schj Alpha $";

#include "nrutil.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* campbell examples from ROss & Bristow (see table 1, page 1521) */
double ha = -40.48; /* [cm] */
double ksat = 11.09; /* cm/day] */
double tsat = 0.482;
double b = 11.05;
double nn;


double h2t (double h)
{
	if (h < ha)
		return tsat * pow(h/ha,-1/b);
	else
		return tsat;
}

double h2k (double h)
{
	if (h < ha)
		return ksat * pow(h/ha,-nn);
	else
		return ksat;
}

double h2u (double h)
{
	if (h < ha)
		return -(h2k(h)  * h) /( nn - 1);
	else
		return ksat * ( - ha/(nn -1 ) + h - ha);
}



double h2dtdp (double h)
{
	if (h < ha)
		return -tsat/(b*ha) * pow(h/ha,(-1/b) - 1);
	else
		return 0.0;
}

double h2dkdp (double h)
{
	if (h < ha)
		return -(nn * ksat)/ha * pow(h/ha,-nn-1);
	else
		return 0.0;
}

/*f:nhc.c
 *
 * This will be the _complete_ headcalc module. Shouls be based on
 * the TOpOG solution. Have fun!
 */ 


/*
 * Nodig:
 * k'	= partial diff of k with respect to h
 * dz	= thickness of each layer
 * k	= k_unsat
 * theta= moisture content
 * dt	= timstep
 * */

double *k,*theta,*dkdp,*dkdp1,*dzc,*dzf,dt;
double *dfa,*dfb,*dfc;
double *h,*h1,*psit,*greekc,*greekb;
double *f,*e,*ec,*qb,*U,*k1,*U1,*qb1,*dk1,*dt1,*theta1,*res;
int n = 30;
double drain,ddrain;
int day;

main()
{
	int i;
	char s[128];

	k = dvector(0, n -1);
	greekc = dvector(0, n -1);
	greekb = dvector(0, n -1);
	k1 = dvector(0, n -1);
	psit = dvector(0, n -1);
	h = dvector(0, n -1);
	h1 = dvector(0, n -1);
	theta = dvector(0, n -1);
	theta1 = dvector(0, n -1);
	dkdp = dvector(0, n -1);
	dkdp1 = dvector(0, n -1);
	dzf = dvector(0, n -1);
	dzc = dvector(0, n -1);
	dfb = dvector(0, n -1);
	dfa = dvector(0, n -1);
	dfc = dvector(0, n -1);
	U = dvector(0, n -1);
	dk1 = dvector(0, n -1);
	dt1 = dvector(0, n -1);
	U1 = dvector(0, n -1);
	qb = dvector(0, n -1);
	qb1 = dvector(0, n -1);
	e = dvector(0, n -1);
	f = dvector(0, n -1);
	ec = dvector(0, n -1);
	res = dvector(0, n -1);

	dt = 0.005; /* 0.1 day */
	nn = 2.0 + 3.0/b;
	/* now fill the initial H array with something usefull..... */

	for (i=0;i < n;i++){
		dzf[i] = dzc[i] = 0.1; /* 1 cm */
		h[i] = (n-i) * -dzc[i]*40 -40.0;
	}
	for (i = 0; i < n; i++){
		h1[i] = h[i]; /* store old values */
	}

	for (i = 0; i < n; i++){
		psit[i] = h[i];
		k[i] = h2k(psit[i]);
	}

	for (day=0; day < 40; day++){
		est(n);
		sprintf(s,"%d.h",day);
		nr_genw("theta",(void *)theta1);
		nr_genw(s,(void *)psit);
		nr_genw("kend",(void *)k1);
		nr_genw("greekc",(void *)greekc);
		nr_genw("f",(void *)f);
		nr_genw("res",(void *)res);
		nr_genw("qb1",(void *)qb1);
		nr_genw("e",(void *)e);
	}
}

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
	if (day >100 ){
	f[5] += -9.9;
	dfb[5] +=  h2dkdp(psit[5]);
	}
}


/*
 * ec = deel van e (met vorige theta)
 * */
void est(int n)
{
	int kk,i = 0;



	/* is eigelijk nog extra code voor laag-grenzen nodig !! */
	i = 0;
	k[i] = h2k(psit[i]);
	theta[i] = h2t(psit[i]);
	U[i] = h2u(psit[i]);
	/*qb[i] = sqrt(k[i] * k[i+1]) - (U[i+1] - U[i])/dzf[i];*/
	ec[i] = theta[i] * dzf[i] /(2.0 *dt);
	for (i = 1; i < n-1; i++){
		k[i] = h2k(psit[i]);
		theta[i] = h2t(psit[i]);
		U[i] = h2u(psit[i]);
		qb[i-1] = sqrt(k[i] * k[i-1]) - (U[i] - U[i-1])/dzf[i-1];
		ec[i] = theta[i] * dzc[i]/(2.0*dt);
	}
	i = n-1; /* overbodig */
	k[i] = h2k(psit[i]);
	theta[i] = h2t(psit[i]);
	U[i] = h2u(psit[i]);
	ec[i] = theta[i] * dzf[i-1] /(2.0 *dt);
	qb[i-1] = sqrt(k[i] * k[i-1]) - (U[i] - U[i-1])/dzf[i-1];


	/* start of main iteration loop */
	for(kk = 0; kk < 50; kk++){
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
			/*qb1[i] = -.5;*/

		/* Gravity drainage .expand later...*/
		drain = h2k(psit[i]); /* does NOT work, why ?? */
		ddrain = h2dkdp(psit[i]);
	        drain = 0.0;ddrain = 0.0;
		

		mkmat(theta1, k1, dk1,dzc,dzf, dt, n);
		ntridiag(dfa,dfb,dfc,f,res);
		for (i = 0; i < n; i++)
			psit[i] = psit[i] - res[i];
	
	}
}


/*C:ntridiag
 *@ int ntridiag(double *fa,double *fb,double *fc)
 * 
 * Description: Solves the matrix by the simple and fast algorithm for
 * a tridiagonal matrix. If a zero pivot occurs it exits and returns 1
 * and the more general method for solving band-diagonal matrixes
 * should be used. 0 is returned on success
 *
 * Returns: 0 on sucess, 1 on failure*/

int
ntridiag (double *fa, double *fb, double *fc, double *f,double *res)
{
	int i;
	double temp;

	fa[0] = 0.0;
	fc[n-1] = 0.0;


	greekb[0] = fc[0]/fb[0];
	greekc[0] = f[0]/fb[0];
	for (i = 0; i < n-2; i++){
		temp = fb[i+1] - fa[i+1] * greekb[i];
		if (temp == 0.0){
			fprintf(stderr,"singular");
			exit(1);
		}
		greekb[i+1] = fc[i+1]/temp;
		greekc[i+1] = (f[i+1] - fa[i+1] * greekc[i])/temp;
	}

	res[n-1] = greekc[n-1];
	for (i = n-2; i >=0; i--){
		res[i] = greekc[i] - greekb[i] * res[i+1];
	}
	

	/*
	if (bet == 0.0)
		return 1;


	greekc[0] = thomf[0] / bet;
	for (i = 1; i < n; i++){
		greekb[i] = fc[i - 1] / bet;
		bet = fb[i] - fa[i] * greekb[i];
		if (bet == 0.0)
			return 1;

		greekc[i] = (thomf[i] - fa[i] * fc[i - 1]) / bet;
	}

	res[n-1] = greekc[n-1];
 	for (i = n - 2; i >= 0; i--){
		res[i] = greekc[i] - greekb[i + 1] * res[i + 1];
	}*/
	return 0;
}


/*C:fillmat
 *@ void fillmat(double **mat,double a[], double b[], double c[])
 *  Description: Fills the matrix mat in the compact 45oC rotated form
 *  Returns: nothing
 */ 
/*
void fillmat(double **mat,double a[],double b[],double c[])
{
	int i;
  
	mat[1][2] = b[0];
	mat[1][3] = c[0];
	mat[1][1] = 0.0;
	for (i=1;i<layers-1;i++){
		mat[i+1][1]=a[i];
		mat[i+1][2]=b[i];
		mat[i+1][3]=c[i];
	}
	mat[layers][1] = a[layers-1];
	mat[layers][2] = b[layers-1];
	mat[layers][3] = 0.0;
}

	fillmat (mat,thoma, thomb, thomc);
	bandec (mat, layers, a1, indx);
	banks (mat, layers, a1, indx, (&thomf[-1]));
	*/
