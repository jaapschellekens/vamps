/* $Header: /home/schj/src/vamps_0.99g/src/main/RCS/plot.c,v 1.8 1999/01/06 12:13:01 schj Alpha $ */
/* $RCSfile: plot.c,v $
*  $Author: schj $
*  $Date: 1999/01/06 12:13:01 $
*/

static char RCSid[] = "$Id: plot.c,v 1.8 1999/01/06 12:13:01 schj Alpha $";



#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include "vamps.h"
#include "swatsoil.h"
#include "soils.h"
#include "marquard.h"
#include "deffile.h"

char *scan_gp_cmd(char *cmd);
char tmp_prefix[128];
void gp_command(char *gnuplot);
FILE *gp_cmd_file;
extern int VampsPid;

#ifndef GNUPLOT
#define GNUPLOT "/usr/local/bin/gnuplot"
#endif

char graphcommand[256] = GNUPLOT;
/* char tmpfname[256]; */
char *tmpfname;

#ifdef HAVE_LIBSLANG
void gp_close();
extern char **sl_make_strv(char *,char *, int *);

void
v_plot (char *pltcmd)
{
	int i;

	if ((pltcmd = scan_gp_cmd(pltcmd)) != NULL){
		if (!gp_cmd_file){
			sprintf(tmp_prefix,"/tmp/%d.",VampsPid);
			gp_command(graphcommand);
			fprintf(gp_cmd_file,"%s\n",pltcmd);
			fflush (gp_cmd_file);
			fflush (gp_cmd_file);
		}else{
			fprintf(gp_cmd_file,"%s\n",pltcmd);
			fflush (gp_cmd_file);
			fflush (gp_cmd_file);
		}
	}
}


void gp_command(char *gnuplot)
{
#ifdef HAVE_POPEN
	if (gp_cmd_file) /* close first if already open */
		gp_close();
	gp_cmd_file = popen(gnuplot, "w");
#else
	Perror(progname,0,0,RCSid,"popen not present on this system","Check configure");
#endif
}



void
gp_close()
{
#ifdef HAVE_POPEN
	if (gp_cmd_file)
		pclose(gp_cmd_file);
	gp_cmd_file = NULL;
	del_tmplist(1);
#else
	Perror(progname,0,0,RCSid,"popen not present on this system","Check configure");
#endif
}

/*C:scan_gp_cmd
 *@ char *scan_gp_cmd(char *cmd)
 *
 * Scans the string cmd for special chars. Everything
 * between two \@ chars is treated as an S-Lang matrix variable
 * to be plotted. Names delimited by the $'s are replaced by
 * the string representation of that S-Lang variable.
 * The variable is dumped to a file named @tmp_prefix+variable_name._slmat@.
 *
 * Bugs: temporary files are not cleaned.
 */ 
char *
scan_gp_cmd(char *cmd)
{
	char **v_cmd,**v_name,**vars,*str;
	static char buf[256],buf2[256],buf3[256];
	int nr,i,nrr,nrvars,i1 = 0;

	vars = sl_make_strv(cmd,"$",&nrvars);

	if (nrvars > 1){
		if (vars[0])
		strcpy(buf,vars[0]);
		for (i=1; i < nrvars; i++){
			if (i % 2 == 0) /* This can go unaltered to gnuplot */
				strcat(buf,vars[i]);
			else{
				sprintf(buf2,"string(%s);\n",vars[i]);
				SLang_load_string(buf2); 
				if (SLang_Error){
					SLang_Error = 0;
					sl_free_strv(vars);
					return NULL;
				}
				SLang_pop_string(&str, &i1);
				strcat(buf,str);
			}
		}
		cmd = buf;
	}
	v_cmd = sl_make_strv(cmd,"@",&nr);
	sl_free_strv(vars);
	if (nr <= 1){ /* No S-Lang matrx variables present */
		sl_free_strv(v_cmd);
		return cmd;
	}else{ /* S-Lang variable(s) present, scan string...*/
		if (vars[0])
		strcpy(buf,v_cmd[0]);
		for (i = 1; i < nr; i++){
			if (i % 2 == 0) /* This can go unaltered to gnuplot */
				strcat(buf,v_cmd[i]);
			else{/* dump the var to a file and change the command */
				v_name = sl_make_strv(v_cmd[i],":",&nrr);
				if (nrr < 2){
					tmpfname = tempnam(NULL,NULL);
					sprintf(buf2,"mwrite(%s,\"%s\");\n", v_cmd[i],tmpfname);
					addtotmplist(tmpfname);
					(void)SLang_load_string(buf2);
					if (SLang_Error){
						SLang_Error = 0;
						sl_free_strv(v_cmd);
						sl_free_strv(v_name);
						return NULL;
					}
					sprintf(buf3,"\"%s\"",tmpfname);
					strcat(buf,buf3);
					free(tmpfname);
				}else{ /* assume varname:filename */
					tmpfname = tempnam(NULL,NULL);
					sprintf(buf2,"f_save(\"%s\",\"%s\",\"%s\");\n",v_name[0],v_name[1],tmpfname); 
					(void)SLang_load_string(buf2);
					if (SLang_Error){
						SLang_Error = 0;
						sl_free_strv(v_cmd);
						sl_free_strv(v_name);
						return NULL;
					}
					addtotmplist(tmpfname);

					sprintf(buf3,"\"%s\"",tmpfname);
					strcat(buf,buf3);
					free(tmpfname);
				}
				sl_free_strv(v_name);
			}
		}
		strcat(buf,"\n");
		sl_free_strv(v_cmd);
		return buf;
	}

}
#endif
