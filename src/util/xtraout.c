/* $Header: /home/schj/src/vamps_0.99g/src/util/RCS/xtraout.c,v 1.9 1999/01/06 12:13:01 schj Alpha $ */
/* $RCSfile: xtraout.c,v $
 *  $Author: schj $
 *  $Date: 1999/01/06 12:13:01 $
 */

static char RCSid[] =
"$Id: xtraout.c,v 1.9 1999/01/06 12:13:01 schj Alpha $";

#include "vamps.h"
#include "swatsoil.h"
#define  VALFORM "%.7f "

extern FILE *xoutfile;
int xopen = 0;

/*- void xtraout(int tstep)
 * 	columnunar type output per timestep
 */
void
xtraout(int tstep)
{
#ifdef HAVE_SLANG
   if(!noslang){
      if (!SLang_execute_function("Sxtraout")){
      fprintf(stderr,"Error: Sxtraout not defined\n");
	 exit(88);
      }
   }else
#endif
     fprintf(xoutfile,"%g %g %g %g %g\n",t,volact,qtop,qbot,prec);
}

/*- void xopenout()
 *
 */
void
xopenout ()
{
#ifdef HAVE_SLANG
  if(!noslang){
     SLang_push_string(xtraoutfn);
     if (!SLang_execute_function("Sopenxtraout")){
	fprintf(stderr,"Error: Sopenxtraout not defined\n");
     }
  }else
#endif
  {
     if (!xoutfile)
       xoutfile = stdout;
     xopen = 1;
     fprintf(xoutfile,"#%s\n#t,volact,qtop,qbot,prec\n",RCSid);
  }
}

/*-
 * void xcloseout()
 *
 */
void
xcloseout ()
{
#ifdef HAVE_LIBSLANG
   if(!noslang){
      if (!SLang_execute_function("Sclosextraout"))
	fprintf(stderr,"Error: Sclosextraout not defined\n");
   }else
#endif
   {
     if (xoutfile != stdout && xopen == 1){
	(void) fclose (xoutfile);
	xopen = 0;
     }
   }
}

