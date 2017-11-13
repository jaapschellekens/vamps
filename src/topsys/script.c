/* $Header: /home/schj/src/vamps_0.99g/src/topsys/RCS/script.c,v 1.4 1999/01/06 12:13:01 schj Alpha $ */

/* $RCSfile: script.c,v $
 * $Author: schj $
 * $Date: 1999/01/06 12:13:01 $ */
#include "topsys.h"
#include "vamps.h"

#ifdef HAVE_LIBSLANG
#include "slang.h"


static SLang_Name_Type *tfp;

static char RCSid[] =
 "$Id: script.c,v 1.4 1999/01/06 12:13:01 schj Alpha $";

/*F:script.c
 * Topsystem based on three S-Lang scripts: @pre_top@, @tstep_top@ and
 * @post_top@. @pre_top@ is executed before the first calculations and can be
 * used to set op variables and time-series needed for the run. It should not
 * require arguments or return any items. @post_top@ can be used to clean up
 * or report after te last calculations have been made. It also should not
 * require arguments or return any items. @tstep_top@ should return 4 floating
 * point numbers: precipitation, interception, transpiration and
 * soilevaporation for that timestep. @tstep_top@ requires one argument, the
 * current timestep.
 * 
 * This topsystem can be very simple or very complex. The file topsys.sl in 
 * the S-Lang directory contains a simple example.
 */ 

/* External procedure in scripting language (S-Lang) */
void
tstep_top_script(int tstep, double *precipitation, double *interception,
		double *transpiration, double *soilevaporation)
{
	int ix,tt;
	double p,i,pp,ppp;

	SLang_push_integer(tstep);
	SLexecute_function(tfp);
	if (SLang_pop_float(&p,&ix,&tt) ||
			SLang_pop_float(&i,&ix,&tt) ||
			SLang_pop_float(&pp,&ix,&tt) ||
			SLang_pop_float(&ppp,&ix,&tt))
		Perror(progname,1,0,RCSid,"Pop_error:","tstep_top did not return the required number of items.");

	*precipitation = p;
	*interception = i;
	*transpiration = pp;
	*soilevaporation = ppp;
}


void
pre_top_script()
{
	/* This one should set how to determine stuff and get info from
	 * input file. Also sets should be added and mem allocated */

	if (SLang_execute_function("pre_top")){
		if (NULL == (tfp = SLang_get_function("tstep_top")))
			Perror(progname,1,0,RCSid,"Could not find S-Lang function:","tstep_top");
	}else{
		Perror(progname,1,0,RCSid,"Could not excecute S-Lang function:","pre_top");
	}
}

void
post_top_script()
{
	if (SLang_execute_function("post_top")){
	}else{
		Perror(progname,1,0,RCSid,"Could not excecute S-Lang function:","post_top");
	}
}
#endif
