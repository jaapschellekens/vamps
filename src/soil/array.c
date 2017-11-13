/* $Header: /home/schj/src/vamps_0.99g/src/soil/RCS/array.c,v 1.6 1999/01/06 12:13:01 schj Alpha $ */
/*  $RCSfile: array.c,v $
 *  $Author: schj $
 *  $Date: 1999/01/06 12:13:01 $
 */

static char RCSid[] =
"$Id: array.c,v 1.6 1999/01/06 12:13:01 schj Alpha $";

#include <math.h>
#include <stdlib.h>
#include "vamps.h"
#include "deffile.h"
#include "swatsoil.h"
#include "marquard.h"
#include "soils.h"

#ifdef HAVE_LIBSLANG
static int done=0;

/*+
  Name:make_Slang_soil_arrays
  Prototype: void make_Slang_soil_arrays(void)
  Description: Make soil arrays visable within S-Lang
  Returns: nothing
+*/
void
make_Slang_soil_arrays(void)
{

	if (!done && !noslang){
		if ((NULL == SLang_add_array ("k",   /* slang name */
					      (long *)k,   /* location of the array */
					      1,      /* number of dimensions */
					      layers,    /* number of elements in X direction */
					      0, 0,   /* Number in Y and Z directions */
					      'f',    /* FLoating point array */
					      SLANG_IVARIABLE)))   /* Read/Write array */
			Perror(progname,1,0,RCSid,"Failed to add array","k");
		if ((NULL == SLang_add_array ("h",   /* slang name */
					      (long *)h,   /* location of the array */
					      1,      /* number of dimensions */
					      layers,    /* number of elements in X direction */
					      0, 0,   /* Number in Y and Z directions */
					      'f',    /* FLoating point array */
					      SLANG_IVARIABLE)))   /* Read/Write array */
			Perror(progname,1,0,RCSid,"Failed to add array","h");
		if ((NULL == SLang_add_array ("theta",   /* slang name */
					      (long *)theta,   /* location of the array */
					      1,      /* number of dimensions */
					      layers,    /* number of elements in X direction */
					      0, 0,   /* Number in Y and Z directions */
					      'f',    /* FLoating point array */
					      SLANG_IVARIABLE)))   /* Read/Write array */
			Perror(progname,1,0,RCSid,"Failed to add array","theta");
		if ((NULL == SLang_add_array ("diffmoist",   /* slang name */
					      (long *)diffmoist,   /* location of the array */
					      1,      /* number of dimensions */
					      layers,    /* number of elements in X direction */
					      0, 0,   /* Number in Y and Z directions */
					      'f',    /* FLoating point array */
					      SLANG_IVARIABLE)))   /* Read/Write array */
			Perror(progname,1,0,RCSid,"Failed to add array","diffmoist");
		if ((NULL == SLang_add_array ("depth",   /* slang name */
					      (long *)z,   /* location of the array */
					      1,      /* number of dimensions */
					      layers,    /* number of elements in X direction */
					      0, 0,   /* Number in Y and Z directions */
					      'f',    /* FLoating point array */
					      SLANG_IVARIABLE)))   /* Read/Write array */
			Perror(progname,1,0,RCSid,"Failed to add array","depth");
	}
	done++;
}
#endif
