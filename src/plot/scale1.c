#include <math.h>
#include "graph.h"

/*
 * Select step size to generate nice scale values for plotting
 * Reference:
 *	Lewart, C. R., "Algorithm 463:  Algorithms SCALE1, SCALE2, and
 *	SCALE3 for Determination of Scales on Computer Generated
 *	Plots[J6],"  Collected Algorithms from CACM.
 *
 */
double
scale1(vmin, vmax, ntics)
double	vmin, vmax;		/* data min/max */
int	ntics;			/* approx # tics wanted */
{
	int	i;
	double	vsign, a, b, nal;

/* set of valid interval spacings */
	static double vint[] = {
		1.0, 2.0, 5.0, 10.0
	};

/* break points for selecting interval spacing */
	static double sqr[] = {
		1.414214, 3.162278, 7.071068
	};

/* find approximate interval spacing a */
	vsign = vmax > vmin ? 1.0 : -1.0;
	a = (vmax - vmin) / ntics * vsign;
	if(a <= 0.0)		/* can't happen, can it ? */
		a = 1.0;	/* avoid log10 domain errors */

/* scale a between 1 and 10 and find the closest permissible value */
	nal = floor(log10(a));
	b = a * pow(10.0, -nal);
	for(i = 0; i < 3; i++) {
		if(b < sqr[i])
			break;
	}

/* interval spacing */
	return(vsign * vint[i] * pow(10.0, nal));
}
