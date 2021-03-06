/* $Header: /home/schj/src/vamps_0.99g/src/maq.lib/RCS/func.c,v 1.6 1999/01/06 12:13:01 schj Alpha $ */
/*
 *  $RCSfile: func.c,v $
 *  $Author: schj $
 *  $Date: 1999/01/06 12:13:01 $
 */
/*
 * The function should set rv[1..mm] according to:
 *	rv[n] = Ycol-(how to get Ycol)
 *	examp:  rv[i] = data[1]-(data[0]*par[1]+par[2])
 *	which is a straight line. with x values in col0 and y
 *	values in col1.
 *	The parameters (par[1]..par[n]) are optimized in the fitting procedure
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "marquard.h"


int  (*allfunc) (double par[],int mmm, int nnn) = NULL;	/* This should point to your own function */
int func_error = 0;

int
Funct (int m,int  n,double  par[],double  *rv)
{
  /* This is default, here the user supplies the *rv array */
  if (allfunc != NULL)
	  return  allfunc(par, m, n);
  else
	  return -1;
}

#ifdef HAVE_LIBSLANG
int sluser(double *npar, int mmm, int nnn)
{
  int ret = -99;

  SLang_push_user_object((SLuser_Object_Type *)SLcreate_array((char *)npar, 1, nnn + 1, 0, 0, 'f', 0));
  SLang_push_integer(mmm);
  SLang_push_integer(nnn);
  if (!SLang_execute_function("fitf")){
    fprintf(stderr,"Error: Function fitf not defined\n");
    return -1;
  }
  
  if (SLang_pop_integer(&ret) == 0)
	  return ret;
  else{
    	  fprintf(stderr,"Error: empty stack: %d\n",ret);
	  return -1;
  }
}
#endif
