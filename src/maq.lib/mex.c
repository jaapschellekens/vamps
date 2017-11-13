/* $Header: /home/schj/src/vamps_0.99g/src/maq.lib/RCS/mex.c,v 1.7 1999/01/06 12:13:01 schj Alpha $ */
/*  $RCSfile: mex.c,v $
 *  $Author: schj $
 *  $Date: 1999/01/06 12:13:01 $
 */
#include <stdlib.h>
#include <stdio.h>
#include <values.h>
#include "marquard.h"

#ifdef HAVE_LIBSLANG
#include <slang.h>
#endif

double **mydat;
int
ttf(double *npar,int mmm, int nnn)
{
 int i;

  for (i = 1; i <= mmm; i++)
   rv[i] = (mydat[i][2]- ((mydat[i][1]* (npar[1]))+(npar[2])));

 return 0; 
}



int
main(argc, argv)
int argc;
char *argv[];
{
  int i;
  int mmm = 2000;
  int nnn =2;
  double slope;
  double offset;


#ifdef HAVE_LIBSLANG
  if (!init_SLang()		       /* basic interpreter functions */
      || !init_SLmath() 	       /* sin, cos, etc... */
#ifdef unix
      || !init_SLunix()	       /* unix system calls */
#endif
      || !init_SLfiles())	       /* file i/o */
      
    {
      fprintf(stderr, "Unable to initialize S-Lang.\n");
      exit(-1);
    }

  if (!SLang_add_table(Maq, "Marquardt")) 
      
    {
      fprintf(stderr, "Unable to initialize S-Lang.\n");
      exit(-1);
    }
#endif

  maqverb=1; /* Be Verbose */
  
#ifdef HAVE_LIBSLANG
 fprintf(stderr,"Fitting 2000 points using S-lang function\n");
 SLang_load_file ("fit.sl");
 SLang_load_file ("fitf.sl");
#endif

  mydat=m_matr(mmm+1,nnn+1);

  for (i=1;i<=mmm;i++){
   mydat[i][1] = (double)i;
   mydat[i][2] = (double)i;
  }
  slope = 4.0;
  offset = 4000.5;

  /* assing our function to func, which will be evaluated in func.c*/
  /* now run Marquardt */
  if (maqinit(&mmm,&nnn))
	fprintf(stderr,"To few points for fitting"); 
  allfunc=ttf;
  par[1] = slope;
  par[2] = offset;
  inp[3] = 0.00001 ;
  inp[4] =  0.00001;
  fprintf(stderr,"Fitting 2000 points using C function\n");
  prhead();
  /*Marquardt((long)mmm, (long)nnn, par,rv, jjinv, inp, out);*/
  maqrun();
  prtail();

  /*  The fitting is now done and the results are stored in par[].
      It is wise to check the out[] array to check how the fit went */

  maqend(&mmm); /* this will destroy par[], so store your results first!*/
  m_free_matr(mydat,mmm+1);

  return 0;
}
