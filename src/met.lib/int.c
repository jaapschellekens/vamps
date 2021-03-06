/* $Header: /home/schj/src/vamps_0.99g/src/met.lib/RCS/int.c,v 1.2 1999/01/06 12:13:01 schj Alpha $ */
/*
 *  $RCSfile: int.c,v $
 *  $Author: schj $
 *  $Date: 1999/01/06 12:13:01 $
 */

/*F:int
 * Several methods to determine rainfall interception by plants/trees.
 *
 * (C) Jaap Schellekens 
 * */
static char RCSid[] =
"$Id: int.c,v 1.2 1999/01/06 12:13:01 schj Alpha $";

#include <math.h>
#include <string.h>

static double dmaxarg1,dmaxarg2;
#define DMAX(a,b) (dmaxarg1=(a),dmaxarg2=(b),(dmaxarg1) > (dmaxarg2) ?\
        (dmaxarg1) : (dmaxarg2))

static double dminarg1,dminarg2;
#define DMIN(a,b) (dminarg1=(a),dminarg2=(b),(dminarg1) < (dminarg2) ?\
        (dminarg1) : (dminarg2))


/*C:lai_int
 *@void lai_int (double P,double lai,double coeff,double *throughfall,
 *@		double *stemflow, double *interception)
 *
 * TOPOG type interception using a lai fraction
 */
void
lai_int (double P,double lai,double coeff,double *throughfall,double *stemflow,
	 double *interception)
{
	*interception = lai * coeff <= P? lai * coeff : P;
	*throughfall = P - (*interception);
	*stemflow =0.0;
}

/*C:gash
 *@ int gash (int method, double p,double p_tr,double S,
 *@           double E,double P,double T, double thefrac,
 *@           double *throughfall,double *stemflow, 
 *@           double *interception,double *SS,double LAI)
 *
 * calculate forest interception using Gash's model
 *  	see GASH79165
 *	If E_avg/R is defined in the input file, this value
 *	is used in stead of using calculated E_avg.
 *
 * 	Amount of P needed to fill the canopy:
 *
 *@	P' = -RS/E ln [1- E/R(1-p-p_Tr)^-1)] (mm/d)
 *@
 *@	p		free Throughfall coefficient
 *@	p_tr		stemflow coefficient
 *@	S		storage capacity of the canopy (mm)
 *@	E		average evaporation during storms (mm/hr)
 *@	R		average precipitation (mm/hr)
 *@     T               duration of storm
 *     method == 0 is old
 *
 *      Adapted version:
 *      If LAI > 0 then the following assumptions are made:
 *      	* S = S* LAI
 *      	* E = E* LAI
 *      	* Ss = Ss* LAI
 *      	* p_tr = p_tr* LAI
 *      	* p = exp(-p * LAI)
 */
int
gash (int method, double p,double p_tr,double S,double E,double P,double T,
      double thefrac,double *throughfall,double *stemflow,
      double *interception,double *SS,double LAI)
{
	double  P_sat;		/*amount of P needed to fill canopy*/
	double  R;			/*average P (mm/hr)*/
	double  E_avg;		/*average E (mm/hr)*/
	double  Iwet,Isat,Idry;
	double  Sorg;

	LAI = LAI <= 0.0? LAI = 1.0 : LAI;
	/* do LAI modifications */
	if (LAI != 1.0){
		S *=  LAI;
		thefrac *= LAI;
		E *= LAI;
		p_tr *= LAI;
		p = exp(-p * LAI);
	}

	/* recalculate to cm as vamps uses cm/day */
	Sorg = S;
	P = P*T;
	E = E*T;
	R = P;
	E_avg = E;
	if (thefrac == 0.0)
		thefrac = E_avg / R;

	if (method == 0){ /* old method */
		P_sat = (-S / thefrac) * log (1 - (LAI * thefrac/(1 - p - p_tr)));
		if (P_sat > P){	/* small storms */
			Iwet = (1 - p - p_tr)*P;
			Isat = 0.0;
			Idry = 0.0;
			*stemflow = p_tr * P;
			*interception = Iwet+Isat+Idry;
			*throughfall = P - (*interception) - (*stemflow);
			*interception /= T;
			*throughfall /= T;
			*stemflow /= T;
			return 0;
		}else{
			Iwet = ((1 - p -p_tr) * P_sat) - S;
			Isat = (thefrac) * (P - P_sat);
			Idry = S;
			*interception = Iwet + Isat +Idry;
			*stemflow = p_tr * P;
			*throughfall = P - *interception - *stemflow;
			*stemflow/=T;
			*throughfall/=T;
			*interception/=T;
			return 1;
		}
	}else{ /* Adapted gash for small timesteps */
		/* First let the canopy dry SS */
		*SS = *SS - E >= 0.0 ? *SS - E : 0.0; /* evaporation + drainage */
		S -= *SS; /* Substract amount in canopy from storage */
		S = S < 0.0 ? 0.0 : S;
		P_sat = (-S / thefrac) * log (1 - (thefrac/(1 - p - p_tr)));
		if (P_sat > P){/* small storms */
			Iwet = (1 - p - p_tr)*P;
			Isat = 0.0;
			Idry = 0.0;
			*stemflow = p_tr * P;
			*interception = Iwet+Isat+Idry;;
			*throughfall = P - (*interception) - (*stemflow);
			*interception /= T;
			*throughfall /= T;
			*stemflow /= T;
			*SS += (1-p-p_tr)*P;
			*SS = *SS > Sorg ? Sorg : *SS;
			return 0;
		}else{
			Iwet = ((1 - p -p_tr) * P_sat) - S;
			Isat = (thefrac) * (P - P_sat);
			Idry = S < E? S : E;
			*interception = Iwet + Isat +Idry;
			*stemflow = p_tr * P;
			*throughfall = P - *interception - *stemflow;
			*stemflow/=T;
			*throughfall/=T;
			*interception/=T;
			*SS = Sorg; /* Full canopy */
			return 1;
		}
	}
}

/*C:calder
 *@void calder (double gamma,double delta,double P,double *interception,
 *@		double *throughfall, double *stemflow)
 *
 *	P	Precipitation
 *	gamma	Max interception loss
 *	delta	Fitting parameter
 */
void
calder (double gamma,double delta,double P,double *interception,
		double *throughfall, double *stemflow)
{
	*interception = gamma * (1 - exp (-delta * P));
	*stemflow = 0;
	*throughfall = P - *interception;
}

/*C:c_drainage
 *@double c_drainge(double C, double S)
 *
 * canopy drainage calculated using actual canopy water content
 * and max */
double
c_drainage(double C, double S)
{
	double b = 3.7;
	double Do = 16.4160; /*cm/day = 0.0019mm/min*/ 

	return C < S? 0.0 : Do * exp (b*(C-S));
}

/*C:rutter
 *@double rutter (double p,double p_tr,double P,double E,double S,
 *@               double Cinit,	double *TF,double *SF,double *Ei,
 *@               double *I,double tstep)
 *
 * Calculates interception according to the rutter model. The equation is
 * solved direct implicitly:
 *@       Cj = Cj-1 + [(1-p-p_tr)P-Eic-Hc]delta_t
 *
 * At the moment evap from stem = 0.0!
 * 
 * Input variables:
 * 	E = evaporation from wet surface [cm]
 * 	P = precipitation [cm]
 * 	Cinit = actual canopy storage [cm]
 * 	S = max canopy storage [cm]
 * 	p = free throughfall coefficient
 * 	p_tr = fraction of rain diverted to trunks
 *
 * The following variables are set:
 * 	TF
 * 	SF
 * 	Ei
 * 	I
 *
 * Cinit is returned.
 */ 
double
rutter (double p,double p_tr,double P,double E,double S,double Cinit,
	double *TF,double *SF,double *Ei, double *I,double tstep)
{
	double delta_td = .04166666666666666667;	/* max 1.0 h (dry) */
	double delta_tw = .00416666666666666667;	/* max 0.1 h (wet) */
	double delta_t; /* actual delta_t */
	double t = 0.0;	/* time */
	double d=0.0,davg = 0.0; /* tstep drainage and average drainage */
	double ei = 0.0; /* tstep wet canopy evap */
	double dC=0.0; /* change in storage */
	int steps = 0; /* number of steps */

	*I=0.0;
	*TF=0.0;
	*SF=0.0;
	*Ei=0.0;

	while (t < tstep){
		steps++;
		/* determine max tstep... */
		if (Cinit <= 0.0)
			delta_t = DMIN(delta_td, tstep);
		else
			delta_t = DMIN(delta_tw, tstep);
		/* synchronize with external tstep... */
		delta_t = t + delta_t > tstep ? tstep - t: delta_t;

		ei = E * Cinit/S;/*..wet surface E as fraction of filled canopy*/

		/* change in storage and actual storage ... */
		dC = ((1 - p - p_tr) * P - ei - d) * delta_t;
		Cinit += dC;

		d = c_drainage(Cinit,S); 	/* Drainage in cm/day */
		
		/* add surplus in Cinit to drainage...*/
		d += Cinit > S ? Cinit - S: 0.0;
		Cinit = Cinit > S ? S : Cinit;

		/* calculate average drainage, wet canop E, trhougfall... */
		davg += d * delta_t;
		*Ei += ei * delta_t;
		*TF += (d + (p * P)) * delta_t;	/* Throughfall in cm */
		Cinit = DMAX(Cinit,0.0);	/* no negative C allowed */

		t += delta_t;			/* increase time */
	}
	/* now calculate stemflow and interception losses...*/
	*SF = P * p_tr;
	*I = P - (*TF) - *SF;

	return Cinit;
}
