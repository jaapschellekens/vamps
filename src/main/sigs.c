/* $Header: /home/schj/src/vamps_0.99g/src/main/RCS/sigs.c,v 1.8 1999/01/06 12:13:01 schj Alpha $ 
 */
/*  $RCSfile: sigs.c,v $
 *  $Author: schj $
 *  $Date: 1999/01/06 12:13:01 $ */

#ifdef HAVE_CONFG_H
#include "vconfig.h"
#endif

static char RCSid[] =
"$Id: sigs.c,v 1.8 1999/01/06 12:13:01 schj Alpha $";

#include <stdio.h>
#include <signal.h>
#include "vamps.h"
#ifdef HAVE_LIBSLANG
#include "slang.h"
#endif
#include "deffile.h"

int    setsig (void);
int    unsetsig (void);
int    switchtoint=0;

/* Number of signals to be ignored */
#define MAXIG 1
#ifdef unix
#define CATCHSIGS SIGINT, SIGABRT, SIGTERM, SIGFPE, SIGHUP, SIGWINCH, SIGPIPE, SIGSEGV, SIGALRM
#else
#define CATCHSIGS SIGINT, SIGABRT, SIGTERM, SIGFPE, SIGHUP
#endif

/* Signals to be caught or ignored */
const int catch[] =
{CATCHSIGS, 0};

/* Needed in djgpp */
#ifndef SIG_ERR
#define SIG_ERR 1
#endif

void
exitfunc()
{
	/* delete all datasets and free memory  */
	del_all_sets ();
	/* deletes the ini memory list (if present)  */
	delmemlist ();
#ifdef HAVE_LIBSLANG	
	gp_close();
	SLang_reset_tty();
#endif
}

/*C:onsig
 *@void onsig (int sig)
 *
 * New signal handler. Setup is run again after handling the signal as
 * Linux resets them to default behaviour. I suppose this is no
 * problem on other systems ;-).
 * Under MSDOS (djgpp) the stuff does not seem to work (what's new). */
#define EMG 2
extern FILE *gp_cmd_file;
void 
onsig (int sig)
{
	char s[512];
	static int emg=0;
	static int ign = 0; /* How many sigs may be ignored */
	static int steps1=0;

	switch (sig){
#ifdef HAVE_LIBSLANG	
	case SIGINT:
		if (!sreadline && steps){
			(void) fprintf(stderr,"                                                                                \n\n");
			(void) fprintf(stderr,"                                                                                \n\n");
			(void) showit ("sig",MESG,"Switching to interactive mode at end of timestep, please wait....",0,0);
			(void) fprintf (stderr,"\n\n\nType 'cont;' and a blank line to continue run while in interactive mode...      ");
			setsig ();
			switchtoint=1;
			if (steps1 == thisstep)
				emg++;
			steps1 = thisstep;
			if (emg >= EMG){
					emg =0;
					(void) showit ("sig",WARN,"Emergency!! Switching to interactive mode now! Timestep not ended.",0,0);
					interpreter(0);
					steps1 =0;
					switchtoint = 0;
			}
		}else{
			ign++;
			(void) sprintf (s, "Ignoring signal %d (%d/%d)", sig, ign, MAXIG);
			(void) showit("sig",WARN,s,0,0);
			if (ign < MAXIG)
				setsig ();
			else
				unsetsig (); 
		}
		break;
#ifdef unix       
	case SIGWINCH:
		SLtt_get_screen_size();
			setsig ();
			break;
	case SIGSEGV:
			fprintf(stderr,"Oops... Vamps tried to use memory that does not belong to vamps\n");
			fprintf(stderr,"Congratulations, you have found a bug in Vamps!\n");
			fprintf(stderr,"Switching to interactive mode....\n");
			setsig ();
			interpreter(0);
			break;
#endif       
	case SIGHUP:
			fprintf(stderr,"Vamps recieved a SIGHUP signal.\n");
			fprintf(stderr,"Switching to interactive mode....\n");
			setsig ();
			interpreter(0);
			break;
	case SIGALRM:
			fprintf(stderr,"Vamps recieved a SIGALRM signal.\n");
			fprintf(stderr,"Switching to interactive mode....\n");
			fprintf(stderr,"Type 'cont;' to continue run\n");
			setsig ();
			interpreter(0);
			break;
	case SIGPIPE:
			fprintf(stderr,"Trying to restart plotting program (%s)...\n",graphcommand);
			gp_cmd_file = NULL;
			gp_command(graphcommand);
			setsig ();
			break;
#endif /* HAVE_LIBSLANG */
	default:
		(void)sprintf(s,"\nExiting on signal %d", sig);
		(void) showit ("sig",WARN,s,0,0);
		/* perror("vamps ");*/
		exit (sig);
		break;
	}
}

/*C:setsig
 *@void setsig()
 *
 * set up custom signal handling. Works on UNIX systems, rather broken
 * on other systems */
int 
setsig (void)
{
	int     err = 0;
	int     i;

#ifdef HAVE_LIBSLANG
	/* set up signal handler */
	for (i = 0; catch[i] && !err; i++)
		if (signal (catch[i], onsig) == SIG_ERR){
			showit("sig",WARN,"Can't set up signal handler",0,0);
			Perror (progname, 0,1, RCSid, "Can't set up signal handler", "");
			err++;
		}
#endif
	return err;
}

/*C:unsetsig
 *@int unsetsig()
 *
 * resets signal handling
 */
int
unsetsig ()
{
	int     err = 0;
	int     i;

#ifdef HAVE_LIBSLANG
	/* resets signal handler */
	for (i = 0; catch[i] && !err; i++)
		if (signal (catch[i], SIG_DFL) == SIG_ERR) {
			showit("sig",WARN,"Can't reset signal handler",0,0);
			Perror (progname, 0,1, RCSid, "Can't reset signal handler", "");
			err++;
		}
#endif
	return err;
}
