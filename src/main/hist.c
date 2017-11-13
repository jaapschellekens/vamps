/* $Header: /home/schj/src/vamps_0.99g/src/main/RCS/hist.c,v 1.4 1999/01/06 12:13:01 schj Alpha $ 
 */
/* 
 *  $RCSfile: hist.c,v $
 *  $Author: schj $
 *  $Date: 1999/01/06 12:13:01 $
 */

static char RCSid[] =
"$Id: hist.c,v 1.4 1999/01/06 12:13:01 schj Alpha $";
#include "vamps.h"

#ifdef HAVE_LIBSLANG
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char **history; /*S-Lang interactive mode history */

/* How to manage history list */
static int histl=0;

/*C:addtohist
 *@ int addtohist(char *item)
 *
 * adds @item@ to the history list
 **/
int
addtohist(char *item)
{
	if (histl == 0){
		history = (char **) ts_memory (NULL, sizeof (char *), progname);	
		/*history[histl] = (char *) ts_memory (NULL, (strlen(item)+1) * sizeof (char), progname);	
		 strcpy(history[histl],item);*/
		if((history[histl]=strdup(item)) == NULL)
			perror(RCSid);
		histl++;
		return histl-1;
	}else {
		history = (char **) ts_memory ((void *)history, (histl+1)*sizeof (char *), progname);	
		/* history[histl] = (char *) ts_memory (NULL, strlen(item)* sizeof (char), progname);
		   strcpy(history[histl],item); */
		if((history[histl]=strdup(item)) == NULL)
			perror(RCSid);
		histl++;
		return histl-1;
	}
	/* NOTREACHED */
}

void
show_history(void)
{
	int i;
	
	for (i=0;i<histl-1;i++){
		(void)fprintf(stderr,"%d: %s\n",i,history[i]);
	}
}

int
save_history(char *histfname)
{
	int i;
	FILE *hf;
	
	if ((hf = fopen(histfname,"w")) == NULL){
		(void)fprintf(stderr,"Error opening %s\n",histfname);
		return -1;
	}else{
		for (i=0;i<histl-1;i++){
			(void)fprintf(hf,"%s\n",history[i]);
		}
		(void)fclose(hf);
		return 0;
	}
	/* NOTREACHED */
}

void
del_history(void)
{
  int i;
  
  for (i = histl-2; i >=0; i--){
    free((void *)history[i]);
  }

  free((void *)history);
  histl =0;
}
#endif
