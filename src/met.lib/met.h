/* $Header: /home/schj/src/vamps_0.99g/src/met.lib/RCS/met.h,v 1.2 1999/01/06 12:13:01 schj Alpha $ */

/* $RCSfile: met.h,v $
 * $Author: schj $
 * $Date: 1999/01/06 12:13:01 $ */
/*F:met.h
 *
 * The met.h file provides an interface to libmet.a. This library
 * contains functions used to estimate evapo[trans]piration and 
 * interception losses of forests and crops. Some functions were
 * translated from Pascal source by A. Frumau.
 *
 * (c) J. Schellekens 
 *@ $Id: met.h,v 1.2 1999/01/06 12:13:01 schj Alpha $ 
 */
#ifndef _METUT_H
#define _METUT_H
extern void sl_met_init (void);

extern void eaes(double td,double rh,double *ea, double *es);

extern double vslope(double Td,double es);

extern double mgamma(double td, double lambda);

extern double lambda(double Td);

extern double earo(double ea, double es, double u);

extern double rnet_open(double Rs, double Rsout, double Rnet, double lambda);
extern double rnet_open_nN(double Rs, double nN, double td,
		double ea,  double Lambda);


extern double e0(double Rnetopen, double Slope, double Gamma, double earo);

extern  double ra (double z, double z0, double d, double u);

extern  double makkink(double rad,double Slope,double Gamma, double Lambda);

extern void lai_int (double P,double lai,double coeff,double *throughfall,
		double *stemflow, double *interception);

extern int gash (int method, double p,double p_tr,double S,double E,
		double P,double T, double thefrac,double *throughfall,
		double *stemflow, double *interception,double *SS,double LAI);

extern void calder (double gamma,double delta,double P,double *interception,
		double *throughfall, double *stemflow);

extern double c_drainage(double C, double S);

extern double rutter (double p,double p_tr,double P,double E,double S,
		double Cinit, double *TF,double *SF,double *Ei, double *I,
		double tstep);
extern double penmon (double A,double delta,double gamma,double rho,
		double c_p, double e_sat,double e_act,double r_a,double r_s);

extern double penmon_soil (double A,double delta,double gamma);
#endif /*_METUT_H*/
