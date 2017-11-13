/* $Header: /home/schj/src/vamps_0.99g/src/main/RCS/tmplist.c,v 1.5 1999/01/06 12:13:01 schj Alpha $ 
 */
/* 
 *  $RCSfile: tmplist.c,v $
 *  $Author: schj $
 *  $Date: 1999/01/06 12:13:01 $
 */

static char RCSid[] =
"$Id: tmplist.c,v 1.5 1999/01/06 12:13:01 schj Alpha $";
#include "vamps.h"

#ifdef HAVE_LIBSLANG
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char **tmplist; /*S-Lang interactive mode history */

/* How to manage history list */
static int tmpl=0;

/*C:addtotmplist
 *@ int addtotmplist(char *item)
 *
 * adds @item@ to the history list
 **/
int
addtotmplist(char *item)
{
	if (tmpl == 0){
		tmplist = (char **) ts_memory (NULL, sizeof (char *), progname);	
		tmplist[tmpl]=strdup(item);
		tmpl++;
		return tmpl-1;
	}else {
		tmplist = (char **) ts_memory ((void *)tmplist, (tmpl+1)*sizeof (char *), progname);	
		tmplist[tmpl]=strdup(item);
		tmpl++;
		return tmpl-1;
	}
	/* NOTREACHED */
}

void
show_tmplist(void)
{
	int i;
	
	for (i=0;i<tmpl-1;i++){
		(void)fprintf(stderr,"%d: %s\n",i,tmplist[i]);
	}
}

int
save_tmplist(char *histfname)
{
	int i;
	FILE *hf;
	
	if ((hf = fopen(histfname,"w")) == NULL){
		(void)fprintf(stderr,"Error opening %s\n",histfname);
		return -1;
	}else{
		for (i=0;i<tmpl-1;i++){
			(void)fprintf(hf,"%s\n",tmplist[i]);
		}
		(void)fclose(hf);
		return 0;
	}
	/* NOTREACHED */
}

/*C:del_tmplist
 *@ void del_tmplist(int files)
 *
 * Deletes the files in the tmp list if @files@ > 0. Otherwise only
 * the list itself is freed */
void
del_tmplist(int files)
{
	int i;
  
	for (i = tmpl-1; i >=0; i--){
		if (files)
			if (remove(tmplist[i]) == -1)
				Perror(progname,0,1,RCSid,"Could not remove:",tmplist[i]);
		free((void *)tmplist[i]);
	}

	free((void *)tmplist);
	tmpl =0;
}
#endif
