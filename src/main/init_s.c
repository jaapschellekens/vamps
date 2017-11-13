/* $Header: /home/schj/src/vamps_0.99g/src/main/RCS/init_s.c,v 1.10 1999/01/06 12:13:01 schj Alpha $ 
 */

/*  $RCSfile: init_s.c,v $
 *  $Author: schj $
 *  $Date: 1999/01/06 12:13:01 $
 */

static char RCSid[] =
"$Id: init_s.c,v 1.10 1999/01/06 12:13:01 schj Alpha $";

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "vamps.h"
#include "deffile.h"
#include "swatsoil.h"
#include "marquard.h"
#include "met.h"
#include "ts.h"

#ifdef HAVE_LIBSLANG
#include "slang.h"

extern double VPD;
extern int Getout;
extern void gp_close();
extern void sl_init();
void setoutfilename(char *n);
void setinfilename(char *n);
char *at_start = "at_start";
char *at_end = "at_end";
char *each_step = "each_step";
int  noslang = FALSE; /*don't use any slang functions*/
int sreadline = 0;
extern char *statfile(char *file, char *sp);
extern int slash_evalfile(char *file);
extern void slash_autoload(char *func, char *file);
extern void slash_help(char *cmd);
extern void v_plot(char *pltcmd);
extern char *sl_getarg(void);
extern char sl_arg0[];
extern char *sl_prompt[2];
void getspar();
void setspar(int *nr, char *des,double *val);
void V_tstep_soil();
void Sinterpreter(void);
double V_ts_time(char *tmstr);
char *V_ts_fmt(double *inp);
int sl_add_set (char *name, int *points);
extern void prtopsys();

/* Here are two intrinsics that the S-lang code can call. */
/* An intrinsic function to set error */
static void c_error (char *s)
{
	SLang_Error = INTRINSIC_ERROR;
	fprintf (stderr, "Error: %s\n", s);
}

/* Function to quit */
static void c_quit (void)
{
	(void)fprintf(stderr,"Press enter to Leave interactive mode...\n");
	Getout = 1;
}

#define FLOAT_TYPE 3

/* This table provides some 'general' utilities */
SLang_Name_Type Vamps_Utils[] =
{
#ifdef unix
	MAKE_VARIABLE("._vampspid_",&VampsPid,INT_TYPE,0), 
#endif
	MAKE_VARIABLE("._tmp_prefix", tmp_prefix, STRING_TYPE,1), 
	MAKE_INTRINSIC(".vamps_help", slash_help, VOID_TYPE, 1),
	/*I:vamps_help
	 *Void vamps_help(String key)
	 * 
	 * gets help on @key@. See @'?'@ for further info.
	 */ 
	MAKE_INTRINSIC(".SLang_reset_tty", SLang_reset_tty, VOID_TYPE, 0),
	/*I:SLang_reset_tty
	 *@ Int SLang_reset_tty()
	 *
	 *  Resets the S-Lang tty. Needed is you want to use stdin and
	 *  stdout from interactive mode.  */
	MAKE_INTRINSIC(".SLang_init_tty", SSLang_init_tty, INT_TYPE, 0),
	/*I:SLang_init_tty
	 *@ Int SLang_init_tty()
	 *
	 *  Initialize the S-Lang tty. */
	MAKE_INTRINSIC(".cpu", cpusec, FLOAT_TYPE, 0),
	/*I:cpu
	 *@ Float cpu()
	 *
	 * Return the number of CPU seconds used since the start of
	 * the program. On msdos systems this will be equal to the
	 * total run-time. On multitasking systems the actually used cpu time
	 * will be returned.
	 */ 
	SLANG_END_TABLE
};

/*I:@
 *@ @scriptname
 * This operator passes it's argument to @evalfile@. It
 * can only be used when running interactively. It
 * can be used to run scripts in stead of an expression
 * like:
 * 
 *@ () = evalfile("scriptname");
 *  
 * Note the lack of a space between the operator and it's
 * argument!
 */

/*I:#
 *@ # [plotcommand]
 *
 * Operator to send a single command to the plotting system (gnuplot) when
 * followed by an argument or to enter plotting mode if no argument is given.
 * In plotting mode all commands should be gnuplot commands. To switch back to
 * normal mode give a single '#'. The @:@ and \@ operators also work within
 * plotting mode.
 *
 * Some special characters can be used to addres S-Lang variables from within
 * plotting mode. Words withing $'s are replaced by a string representation of
 * the S-Lang variable with that name. Example
 *@ variable h = "sin(x)";
 *@ # plot $h$
 *
 * To plot S-Lang matrix variables the \@ char must be used. Example
 *@ variable m = mread("mydata.file",-1);
 *@ # plot @m@ using 1:2
 *
 * Although S-Lang matrix variables are zero based gnuplot starts counting at
 * 1.
 *
 * Variables from vamps output files can also be plotted. In this case both
 * the name of the of the variable and the output filename are needed. 
 * They should be seperated by a colon and enclosed in \@ characters:
 * 
 *@ # plot @precipitation:example1.out@ using boxes
 *
 * See also @plt@.
 */ 

/*---------------------------------------------------------------------------*/
/* Create the Table that S-Lang requires, here all the vamps
   specific functions usable from s-lang are defined */
SLang_Name_Type Vamps_Intrinsics[] =
{
	MAKE_VARIABLE(".v_steps",&steps,INT_TYPE,0), 
	/*I:v_steps
	 *@ Int v_steps
	 *
	 * Number of steps in the current simulation (Read,only) */
	MAKE_VARIABLE(".verbose", &verbose, INT_TYPE, 0),  
	/*I:verbose
	 *@ Int verbose
	 *
	 * The general verbose level in the current simulation. 0 is quiet. A
	 * higher the number makes Vamps more verbose. Verbose levels above 1
	 * are usefull for debugging only */
	MAKE_VARIABLE("._version_", &version, INT_TYPE, 1),  
	/*I:_version_
	 *@ Int _version_
	 *
	 * Devide by 10 to get version number of Vamps */
	MAKE_VARIABLE("._BUILDDATE_", BUILDDATE, STRING_TYPE, 1),  
	MAKE_VARIABLE("._VERSTRING_", PROVERSION, STRING_TYPE, 1),  
	MAKE_VARIABLE("._OS_", OS, STRING_TYPE, 1),  
	MAKE_VARIABLE(".sreadline", &sreadline, INT_TYPE, 0),  
	/*I:sreadline
	 *@ Int sreadline
	 *
	 * If this variable is 1 vamps runs in interactive mode (readline
	 * initialized.). If this is set to 0 pressing ^C causes Vamps to
	 * switch to interactive mode on unix systems. */
	MAKE_VARIABLE("._sets_", &sets, INT_TYPE, 1),
	/*I:_sets_
	 *@ Int _sets_
	 *
	 * Number of data sets in memory */
	MAKE_VARIABLE(".v_t", &t, FLOAT_TYPE, 0),
	/*I:v_t
	 *@ Float v_t
	 *
	 * Current time in simulation (in days) */
	MAKE_VARIABLE(".v_dt", &dt, FLOAT_TYPE, 0),
	/*I:v_dt
	 *@ Float v_dt
	 *
	 * Current timestep in interation process. The 'real' timeste can 
	 * be found in the @v_thiststep@ variable (in days) */
	MAKE_VARIABLE(".v_qtop", &qtop, FLOAT_TYPE, 1),
	/*I:v_qtop
	 *@ Float v_qtop
	 *
	 * Flow trough top of profile for current timestep (Read only) */
	MAKE_VARIABLE(".v_qbot", &qbot, FLOAT_TYPE, 1),
	/*I:v_qbot
	 *@ Float v_qbot
	 *
	 * Flow trough bottom of profile for current timestep (Read only)*/
	MAKE_VARIABLE(".v_masbal", &masbal, FLOAT_TYPE, 1),
	/*I:v_masbal
	 *@ Float v_masbal
	 *
	 * mass balance error in %. Read only.*/
	MAKE_VARIABLE(".v_cumbot", &cumbot, FLOAT_TYPE, 1),
	/*I:v_cumbot
	 *@ Float v_cumbot
	 *
	 * Cumulative flow trough bottom of profile calculated from start of
	 * simulation (Read only)*/
	MAKE_VARIABLE(".v_cumtop", &cumtop, FLOAT_TYPE, 1),
	/*I:v_cumtop
	 *@ Float v_cumtop
	 *
	 * Cumulative flow trough top of profile calculated from start of
	 * simulation (Read only)*/
	MAKE_VARIABLE(".v_rootextract", &rootextract, FLOAT_TYPE, 1),
	/*I:v_rootextract
	 *@ Float v_rootextract
	 * 
	 * Cumulative amount of water extracted by the roots (actual 
	 * transpiration) (Read only) */
	MAKE_VARIABLE(".v_cumeva", &cumeva, FLOAT_TYPE, 1),
	/*I:v_cumeva
	 *@ Float v_cumeva
	 * 
	 * Cumulative evaporation (from wet canopy) (Read only) */ 
	MAKE_VARIABLE(".v_cumdra", &cumdra, FLOAT_TYPE, 1),
	MAKE_VARIABLE(".v_cumtra", &cumtra, FLOAT_TYPE, 1),
	/*I:v_cumtra
	 *@ Float v_cumtra
	 * 
	 * Cumulative transpiration. This is equal to @v_rootextract@ if
	 * transpiration reduction is not calculated (Read only) */
	MAKE_VARIABLE(".v_cumprec", &cumprec, FLOAT_TYPE, 1),
	/*I:v_cumprec
	 *@ Float v_cumprec
	 * 
	 * Cumulative precipitation [cm] (Read only) */
	MAKE_VARIABLE(".v_cumintc", &cumintc, FLOAT_TYPE, 1),
	/*I:v_cumintc
	 *@ Float v_cumint
	 * 
	 * Cumulative interception [cm] (Read only) */
	MAKE_VARIABLE(".v_volact", &volact, FLOAT_TYPE, 1),
	/*I:v_volact
	 *@ Float v_volact
	 * 
	 * Actual water content of the total soil profile [cm] (Read only)
	 * */
	MAKE_VARIABLE(".v_layers", &layers, INT_TYPE, 1),
	MAKE_VARIABLE(".switchtoint", &switchtoint, INT_TYPE, 0),     
	MAKE_VARIABLE(".v_infilename", infilename, STRING_TYPE, 0),
	MAKE_VARIABLE(".v_outfilename", outfilename, STRING_TYPE, 0),
	MAKE_VARIABLE(".vampslib", vampslib, STRING_TYPE, 0),
	MAKE_VARIABLE(".v_volsat", &volsat, FLOAT_TYPE, 1),
	/*I:v_volsat
	 *@ Float v_volsat
	 * 
	 * Water content of the soil profile at saturation (read only)
	 * */
	MAKE_VARIABLE(".v_thiststep", &thiststep, FLOAT_TYPE, 0),
	MAKE_VARIABLE(".v_thisstep", &thisstep, INT_TYPE, 0),     
	MAKE_VARIABLE(".commentchars", commchar, STRING_TYPE, 0),     
	MAKE_VARIABLE(".v_surface_runoff", &surface_runoff, FLOAT_TYPE, 1),
	MAKE_VARIABLE(".v_ptra", &ptra, FLOAT_TYPE, 1),
	MAKE_VARIABLE(".v_rootts", &rootts, FLOAT_TYPE, 1),     
	MAKE_VARIABLE(".v_qdrtot", &qdrtot, FLOAT_TYPE, 0),
	MAKE_VARIABLE(".dumpini", &dumpini, INT_TYPE, 0),
	MAKE_VARIABLE(".soilverb", &soilverb, INT_TYPE, 0),
	MAKE_VARIABLE(".id.pre", &id.pre, INT_TYPE, 0),
	MAKE_VARIABLE(".id.ptr", &id.ptr, INT_TYPE, 0),
	MAKE_VARIABLE(".id.spe", &id.spe, INT_TYPE, 0),
	MAKE_VARIABLE(".id.spe", &id.spe, INT_TYPE, 0),
	MAKE_VARIABLE(".id.inr", &id.inr, INT_TYPE, 0),
	MAKE_VARIABLE(".id.win", &id.win, INT_TYPE, 0),
	MAKE_VARIABLE(".id.nra", &id.nra, INT_TYPE, 0),
	MAKE_VARIABLE(".id.tem", &id.tem, INT_TYPE, 0),
	MAKE_VARIABLE(".v_avgtheta", &avgtheta, FLOAT_TYPE, 1),
	MAKE_VARIABLE(".v_smd", &SMD, FLOAT_TYPE, 1),
	/*I:v_smd
	 *@ Float v_smd
	 *
	 * Soil moisture deficit for current timestep in cm */
	MAKE_VARIABLE(".v_vpd", &VPD, FLOAT_TYPE, 1),
	/*I:v_vpd
	 *@ Float v_vpd
	 * 
	 * Actual vapour pressure deficit in mbar (Read only)
	 * */
	MAKE_VARIABLE(".check", &check, INT_TYPE, 0),     
	MAKE_VARIABLE(".v_thetol", &thetol, FLOAT_TYPE, 0),     
	MAKE_VARIABLE(".v_maxitr", &maxitr, INT_TYPE, 0),     
	MAKE_INTRINSIC(".getdefname", getdefname, STRING_TYPE, 1),
	MAKE_INTRINSIC(".editmemitem", editmemitem, VOID_TYPE, 3),
	MAKE_INTRINSIC(".print", print, VOID_TYPE, 1),
	MAKE_INTRINSIC(".error", c_error, VOID_TYPE, 1),
	MAKE_INTRINSIC(".quit", quit_vamps, VOID_TYPE, 0),
	/*I:exit
	 *I:quit
	 *@exit()
	 *@quit()
	 * Quits the S-Lang interpreter and returns to the
	 * operating system */
	MAKE_INTRINSIC(".exit", quit_vamps, VOID_TYPE, 0),
	MAKE_INTRINSIC(".interpreter", Sinterpreter, VOID_TYPE, 0),
	/*I:interpreter
	 *@Void interpreter(int verb)
	 *
	 * Start the Vamps interactive mode. If @verb@ != 0
	 * the opening banner will be shown.
	 */ 
	MAKE_INTRINSIC(".cont", c_quit, VOID_TYPE, 0),
	/*I:cont
	 *@ cont()
	 * Continiue the current Vamps run */
	MAKE_INTRINSIC(".v_precanop", precanop, VOID_TYPE, 0),
	MAKE_INTRINSIC(".plt_close", gp_close, VOID_TYPE, 0),
	/*I:plt_close
	 *@ Void plt_close()
	 *
	 * Closes the pipe to the plotting program
	 */ 
	MAKE_INTRINSIC(".plt", v_plot, VOID_TYPE, 1),
	/*I:plt
	 *@ Void plt(String cmd)
	 * Sends the string @cmd@ to the plotting system. This has
	 * the same effect as using the @#@ operator in interactive mode.
	 * @plt@ can be used in scripts.
	 *
	 * See also @#@.
	 */ 
	MAKE_INTRINSIC(".v_inidump", inidump, VOID_TYPE, 0),
	MAKE_INTRINSIC(".v_presoil", presoil, VOID_TYPE, 0),
	/*I:v_presoil
	 *@ Void v_presoil()
	 * 
	 * Initialize the soil moisture module
	 * */
	MAKE_INTRINSIC(".v_postsoil", postsoil, VOID_TYPE, 0),
	/*I:v_postsoil
	 *@ Void v_postsoil
	 * 
	 * Cleans up after the soil module
	 * */
	MAKE_INTRINSIC(".v_postcanop", postcanop, VOID_TYPE, 0),
	MAKE_INTRINSIC(".v_prtopsys", prtopsys, VOID_TYPE, 0),
	MAKE_INTRINSIC(".v_closeout", closeout, VOID_TYPE, 0),
	MAKE_INTRINSIC(".v_xcloseout", xcloseout, VOID_TYPE, 0),
	MAKE_INTRINSIC(".disclaim", disclaim, VOID_TYPE, 0),
	/*I:disclaim
	 *I:copyright
	 *@Void disclaim
	 *@Void copyright
	 * Shows a short version of the GPL */
	MAKE_INTRINSIC(".copyright", disclaim, VOID_TYPE, 0),
	MAKE_INTRINSIC(".readset", readset, INT_TYPE, 2),
	/*I:readset
	 *@Int readset(String filename, String setname)
	 *
	 * Reads set @setname@ from file @fname@. Returns the
	 * id of the set or -1 on error. 
	 */
	MAKE_INTRINSIC(".addset", sl_add_set, INT_TYPE, 2),
	/*I:addset
	 *@Void addset(name, points)
	 *
	 * Adds an empty set @name@ with @points@ points
	 * to the list
	 * */
	MAKE_INTRINSIC(".at_start_f", at_start_f, VOID_TYPE, 0),
	MAKE_INTRINSIC(".v_tstep_soil", V_tstep_soil, VOID_TYPE, 0),
	MAKE_INTRINSIC(".v_setinfilename", setinfilename, VOID_TYPE, 1),
	MAKE_INTRINSIC(".v_setoutfilename", setoutfilename, VOID_TYPE, 1),
	MAKE_INTRINSIC(".v_utout", utout, VOID_TYPE, 1),
	MAKE_INTRINSIC(".v_xtraout", xtraout, VOID_TYPE, 1),
	MAKE_INTRINSIC(".getsetbyname", getsetbyname, INT_TYPE, 1),
	MAKE_INTRINSIC(".v_printcom", printcom, VOID_TYPE, 1),
	MAKE_INTRINSIC(".v_printstr", printstr, VOID_TYPE, 2),
	/*I:v_printstr
	 *@ Void v_printstr(name, str)
	 *
	 * Stores variable @str@ with name @name@ in the output-file.
	 * This function can be used in the @each_step@ function only.
	 * It's purpose is to store extra information in the output-file.
	 * */
	MAKE_INTRINSIC(".getset_x", getset_x, FLOAT_TYPE, 2),
	MAKE_INTRINSIC(".getset_y", getset_y, FLOAT_TYPE, 2),
	MAKE_INTRINSIC(".getset_points", getset_points, INT_TYPE, 1),
	MAKE_INTRINSIC(".getset_y_from_x", getset_y_from_x, FLOAT_TYPE, 2),
	MAKE_INTRINSIC(".getset_name", getset_name, STRING_TYPE, 1),
	MAKE_INTRINSIC(".getset_fname", getset_fname, STRING_TYPE, 1),
	MAKE_INTRINSIC(".getset_xmax", getset_xmax, FLOAT_TYPE, 1),
	MAKE_INTRINSIC(".getset_ymax", getset_ymax, FLOAT_TYPE, 1),
	MAKE_INTRINSIC(".getset_ymin", getset_ymin, FLOAT_TYPE, 1),
	MAKE_INTRINSIC(".getset_xmin", getset_xmin, FLOAT_TYPE, 1),
	MAKE_INTRINSIC(".getset_xsum", getset_xsum, FLOAT_TYPE, 1),
	MAKE_INTRINSIC(".getset_ysum", getset_ysum, FLOAT_TYPE, 1),
	MAKE_INTRINSIC(".v_del_all_sets",del_all_sets, VOID_TYPE, 0),
	MAKE_INTRINSIC(".v_prelim",prelim, VOID_TYPE, 0),
	MAKE_INTRINSIC(".v_dorun",dorun, VOID_TYPE, 0),
	MAKE_INTRINSIC(".v_dotail",dotail, VOID_TYPE, 0),
	MAKE_INTRINSIC(".history",show_history, VOID_TYPE, 0),
	MAKE_INTRINSIC(".addtohist",addtohist, INT_TYPE, 1),
	/*I:addtohist
	 *@ Int addtohist(string)
	 *
	 * Adds @string@ to the history list.
	 */ 
	MAKE_INTRINSIC(".hist",show_history, VOID_TYPE, 0),
	/*I:hist
	 *@ Void hist()
	 *
	 * Prints the history of command entered at the interactive
	 * prompt to the screen. 
	 *
	 * Note that the  help operator `?' is never added to the list.
	 * The excecute operator \@ is replaced by @() = evalfile("arg");@.
	 * */
	MAKE_INTRINSIC(".del_history",del_history, VOID_TYPE, 0),
	MAKE_INTRINSIC(".save_history",save_history, INT_TYPE, 1),
	/*I:save_history
	 *@Int save_history(fname)
	 *
	 * Save the command entered at the interactive prompt to the
	 * file @fname@. Returns -1 on error (this file could not be
	 * opened in write mode) and 0 on success.
	 * */
	MAKE_INTRINSIC(".v_printsum",s_printsum, VOID_TYPE, 0),
	/*I:v_printsum
	 *@Void v_printsum()
	 *
	 * Prints actual water balance summary at @stderr@ */
	MAKE_INTRINSIC(".v_outfiletrailer",outfiletrailer, VOID_TYPE, 0),
	MAKE_INTRINSIC(".v_endshow",endshow, VOID_TYPE, 0),
	MAKE_INTRINSIC(".v_dumpset",s_dumpset, VOID_TYPE, 2),
	MAKE_INTRINSIC(".v_getspar",getspar, VOID_TYPE,0),
	/*I:v_getspar
	 * 
	 *@ Float v_getspar(int nr, char *des)
	 * 
	 * Gets parameter from soil description structure
	 * */
	MAKE_INTRINSIC(".v_setspar",setspar, VOID_TYPE, 3),
	/*I:v_setspar
	 * 
	 *@ Void v_setspar(int nr, char *des, val double)
	 * 
	 * set parameter in soil description structure
	 * */
	MAKE_INTRINSIC(".ts_time",V_ts_time, STRING_TYPE, 1),
	MAKE_INTRINSIC(".ts_fmt",V_ts_fmt, FLOAT_TYPE, 1),
	MAKE_INTRINSIC(".sl_met_init",sl_met_init, VOID_TYPE, 0),
	/*I:sl_met_init
	 *@ Void sl_met_init
	 *
	 * Initializes the meteo functions:
	 * @eaes, vslope, e0b@
	 * */
	SLANG_END_TABLE
};

/*C:init_s
 * @void init_s (char *vamps_sl)
 *
 * Initializes the S-Lang system and loads the startup file
 * @vamps_sl@.
 */
void
init_s (char *vamps_sl)
{
	double lai_to_s(double lai);

	/* Initialize the library.  This is always needed. */

	if (!init_SLang()		       /* basic interpreter functions */
			|| !init_SLmath() 	       /* sin, cos, etc... */
#ifdef unix
			|| !init_SLunix()	       /* unix system calls */
#endif
			|| !init_SLfiles()	       /* file i/o */

			/* Now add intrinsics for this application */
			|| !SLang_add_table(Vamps_Utils, "Vamps")  
			|| !SLang_add_table(Maq, "Marquardt")
			|| !SLang_add_table(Vamps_Intrinsics, "Vamps"))  {
		fprintf(stderr, "Unable to initialize S-Lang.\n");
		exit(-1);
	}
	if (sl_def_init() == -1)
		fprintf(stderr, "Unable to initialize S-Lang deffile functions.\n");
#ifdef HAVE_LIBAGL
	sl_AGL_init();
#endif
#ifdef HAVE_LIBPLOT
	sl_plot_init();
#endif
	

	sl_init();

	/* Turn on debugging */
	SLang_Traceback = 1;

	/* Add VAMPS as a define from S-Lang preprocessor */
	SLdefine_for_ifdef("VAMPS");
	/* Now load an initialization file and exit */
	if (vamps_sl)
		SLang_load_file (vamps_sl);
	else
		Perror(progname,1,1,RCSid,"Slang init file not found",vamps_sl);
}


/* Here all the function definitions in s-lang follow. They can be used */
/* by all modules in vamps. Prototypes can be found in s_lang.h */
/* Three functions can exist one that is executed after the command
 * line options are parsed and the input file is read and before the 
 * modelling takes place. One is executed after each step in the precipitation
 * input array. One is excecuted after the program is finished, but before
 * the output files are closed.
 * they are stored in:
 *    at_start
 *    each_step
 *    at_end
 * by default the names of the S-Lang functions are the same, but they
 * can be changed by the user. Functions shoud return 0 on success and
 * something else on failure. Setting these vars to NULL disables checking
 * of these options.
 */

/*C:lai_to_s
 *@double lai_to_s (double lai)
 *
 * Converts lai to canopy storage by a user defined function in s_lang
 * called Slai_to_s (lai).
 */
/*I:Slai_to_s
 *@Float Slai_to_s(Float lai)
 *
 * Function called in the canopy module to convert lai to
 * canopy storage. If this S-Lang function is not defined the
 * lai value from the @canopy@ section in the input file is used.
 */
double
lai_to_s (double lai)
{
	double x;
	int ix,tt;

	SLang_push_float (lai);
	if (SLang_execute_function("Slai_to_s")) {
		SLang_pop_float(&x,&ix,&tt);
		return x;
	}else{
		fprintf(stderr,"Error: Slai_to_s not defined\n");
		exit(88);
	}
}


int
at_start_f()
{
	int ret;

	if (at_start)
		if((ret = SLang_execute_function(at_start)))
			return 0;
		else
			return ret;

	return 1;
}

int
at_end_f()
{
	int ret;

	if (at_end)
		if((ret = SLang_execute_function(at_end)))
			return 0;
		else
			return ret;

	return 1;
}

int
each_step_f()
{
	int ret;

	if (each_step)
		if((ret = SLang_execute_function(each_step)))
			return 0;
		else
			return ret;

	return 1;
}


double 
getset_x (int *set,int *pos)
{
	return data[*set].xy[*pos].x;
}

double 
getset_y (int *set,int *pos)
{
	return data[*set].xy[*pos].y;
}

double
getset_xmax(int *set)
{
	return data[*set].xmax;
}
double
getset_ymax(int *set)
{
	return data[*set].ymax;
}
double
getset_ymin(int *set)
{
	return data[*set].ymin;
}
double
getset_xmin(int *set)
{
	return data[*set].xmin;
}
double
getset_ysum(int *set)
{
	return data[*set].ysum;
}
double
getset_xsum(int *set)
{
	return data[*set].xsum;
}

double
getset_y_from_x (int *set, double *x)
{
	int low=0,mid=0,high=0;

	high = data[*set].points;

	while (high > low) {
		mid = (high + low) / 2;
		if (*x > data[*set].xy[mid].x) {
			low = mid;
		} else if (data[*set].xy[mid].x > *x) {
			high = mid;
		} else
		high = -1;
	}

	return data[*set].xy[mid].y;
}

int 
getset_points (int *set)
{
	return data[*set].points;
}

char 
*getset_name (int *set)
{
	return data[*set].name;
}

char 
*getset_fname (int *set)
{
	return data[*set].fname;
}


void
setoutfilename(char *n)
{
	strcpy(outfilename,n);
}

void
setinfilename(char *n)
{
	strcpy(infilename,n);
}

#endif /* HAVE_LIBSLANG*/

double
estrs(void)
{
#ifdef HAVE_LIBSLANG
	double x;
	int ix,tt;

	if (SLang_execute_function("estrs")) {
		SLang_pop_float(&x,&ix,&tt);
		return x;
	}else{
		fprintf(stderr,"Error: estrs not defined\n");
		exit(88);
	}
#endif
	Perror(progname,1,0,RCSid,"Need S-Lang for estrs","");
	/* NOTREACHED */
}


double
Rs_to_Rn(double Rs)
{
#ifdef HAVE_LIBSLANG
	double x;
	int ix,tt;

	SLang_push_float (Rs);
	if (SLang_execute_function("Rs_to_Rn")) {
		SLang_pop_float(&x,&ix,&tt);
		return x;
	}else{
		fprintf(stderr,"Error: Rs_to_Rn not defined\n");
		exit(88);
	}
#else
	return (Rs *0.858) - 35.0;
#endif
}



#ifdef HAVE_LIBSLANG

void
s_printsum(void)
{
	printsum(stderr);
}


void
s_dumpset(int *setn,char *fname)
{
	FILE *tt;

	if((tt = fopen(fname,"w"))){
		dumpset(*setn,tt);
		fclose(tt);
	}else{
		(void)fprintf(stderr,"File open failed: %s\n",fname);
	}
}
#endif


/* Below follows all needed for the interactive vamps mode. Most
   is taken from the S-Lang calc.c example */
   
 
#ifdef HAVE_LIBSLANG
int Getout =0;
/* This is calc.c S-Lang example stuff */
void quit_vamps (void)
{
	/*   SLsmg_reset_smg(); */
	SLang_reset_tty ();
#ifdef MALLOC_DEBUG
	SLmalloc_dump_statistics ();
#endif
	exit (SLang_Error);
}

void print (char *s)
{
	(void)fputs(s, stdout); 
	(void)fflush (stdout);
}

/* forward declarations */
static int take_input (void);
static int vamps_open_readline (SLang_Load_Type *);
static int vamps_close_readline (SLang_Load_Type *);

void Sinterpreter(void)
{
	int verb;
	if (SLang_pop_integer(&verb)) verb = 1;

	(void)interpreter(verb);
}

int interpreter (int verb)
{
	sreadline = 1; 
	SLang_set_abort_signal (onsig);
	SLang_Ignore_User_Abort = 0;
	SLang_Traceback = 1;
	alarm(0);
	if (verb){
		(void) fprintf (stderr, "\n%s - %s\n\t%s (%s) %s\n\t%s (c) - %s\n", progname, DESCRIPTION, IDENTIFY, STATUS, PROVERSION, AUTHOR, DATE);
		(void) fprintf (stderr, "\tBuild on %s at %s by %s (%s)\n", WHERE, BUILDDATE, WHO, OS);
		(void) fprintf (stderr, "\t(%s)\n", RCSid);
		(void)fputs(" \n\n This is  V A M P S  in interactive mode.\n", stderr);
		(void)fputs(" V A M P S  comes with ABSOLUTELY NO WARRANTY; for details type `disclaim;'.\n",stderr);
		(void)fputs(" This is free software, and you are welcome to redistribute it\n",stderr);
		(void)fputs(" under certain conditions; type `copyright;' for details.\n\n",stderr);
		(void)fputs(" Type `?' for a list of available functions. `??' explains how\n", stderr);
		(void)fputs(" to use help.\n", stderr);
		(void)fputs(" Note also that statements end in a `;'\n", stderr);
		(void)fputs("\n It is also important to note that most binary operators +, -, *, /,\n", stderr);
		(void) fputs(" as well as the `=' sign must be surrounded by spaces!\n", stderr);
		(void)fputs("\n Type `quit;' or `exit;' to exit this program.\n", stderr);
		(void)fputs("\n Type `cont;' to continue a run.\n", stderr);
	}

	SLang_User_Open_Slang_Object = vamps_open_readline;
	SLang_User_Close_Slang_Object = vamps_close_readline;

	while(1){
		if (SLang_Error) SLang_restart(1);
		SLKeyBoard_Quit = SLang_Error = 0;
		take_input();
		if (Getout == 1)
			break;
	}

	Getout = 0;
	sreadline =0;
	setsig();
	return (SLang_Error);
}

/* For a detailed explanation of all of this, see slang/demo/useropen.c */

static char *read_using_readline (SLang_Load_Type *x)
{
	int n;
	static int pmode =0;
	static int lineno = 0;
	char *cp,*rp,buf[256],s[256];
	SLang_RLine_Info_Type *rli;
	char *pmode_prompt = "plotting> ";

	rli = (SLang_RLine_Info_Type *) x->ptr;
	*rli->buf = 0;

	/* set prompt. Taken from slash */
	if (x->top_level){
		lineno++;
		cp = sl_prompt[0];
	}else cp = sl_prompt[1];

	rp = buf;

	while(*rp = *cp++) {		/* scan prompt for %[#$] */
		rp[1] = '\0';
		if(*rp == '%') {
			if(*cp == '#') {
				cp++;
				sprintf(rp, "%d", lineno);
			}
			else if(*cp == '$') {
				cp++;
				sprintf(rp, "%d", SLstack_depth());
			}
		}
		while(*rp)
			rp++;
	}

	/* do the readline */
	if (pmode)
		rli->prompt = pmode_prompt;
	else
		rli->prompt = buf;
	n = SLang_read_line (rli);

	putc ('\n', stdout);  fflush (stdout);
	if (n <= 0) return NULL;
	if ((n == 1) && ((int)*rli->buf == (int)SLang_RL_EOF_Char)) return "quit;";

	(void)SLang_rline_save_line (rli);
	for(cp = rli->buf; *cp == ' ' || *cp == '\t'; cp++) ;
	if(*cp == '?') {
		slash_help(++cp);
		return(NULL);
	}
	if(*cp == '@') {
		++cp;
		sprintf(s,"() = evalfile(\"%s\");",cp); 
		(void)addtohist(s); 
		slash_evalfile(cp);
		return(NULL);
	}
	if(*cp == ':'){
		++cp;
		while(*cp == ' ' || *cp == '\t' || *cp == ';')
			cp++;

		/* using >> 8 is dangerous, should test with macros in sys/wait.h */
		if(*cp != '\0' && (n = system(cp) >> 8)){
			(void)fprintf(stderr, "%s: exited with status %d\n", cp, n);
		}else{
			sprintf(s,"() = system(\"%s\");",cp); 
			(void)addtohist(s); 
		}
		return (NULL);
	}
	if(*cp == '#' || pmode) {
		if (pmode){
			if (*cp == '#'){
				pmode = 0;
				fprintf(stderr,"Leaving plotting mode...\nGive # as first char to switch back\n");
				return NULL;
			}
			v_plot(cp);
			return NULL;
		}
		++cp;
		v_plot(cp);
		if (strlen(cp) == 0){
			fprintf(stderr,"Entering plotting mode...\nGive # as first char to switch back\n");
			pmode = 1;
		}
		return(NULL);
	}
	(void)addtohist((char *)rli->buf); /* make a history */
	return((char *)rli->buf);


	return(NULL);
}

static SLang_RLine_Info_Type Vamps_RLI;
static unsigned char Vamps_RLI_Buf[256];

static int 
vamps_open_readline (SLang_Load_Type *x)
{
	switch (x->type) {
		case 'r':
			x->ptr = (char *) &Vamps_RLI;
			x->read = read_using_readline;

			if (Vamps_RLI.buf != NULL){
				/* everything else already initialized.
				 * Just return. */
				return 0;
			}
			Vamps_RLI.buf = Vamps_RLI_Buf;
			Vamps_RLI.buf_len = 255;
			Vamps_RLI.tab = 8;
			Vamps_RLI.edit_width = 79;
			Vamps_RLI.dhscroll = 20;
			Vamps_RLI.prompt = "Vamps> ";
			Vamps_RLI.getkey = SLang_getkey;
#ifdef USE_ANSI_COLOR
			Vamps_RLI.flags = SL_RLINE_USE_ANSI;
#endif
			SLang_init_readline (&Vamps_RLI);
			return 0;
		default:
			/* unrecognized-- let S-lang handle it */
			return SL_OBJ_UNKNOWN;
	}
}



static int 
vamps_close_readline (SLang_Load_Type *x)
{
	switch (x->type){
		case 'r':
			/* Nothing to do */
			return 0;

		default:
			/* unrecognized-- let S-lang handle it */
			return SL_OBJ_UNKNOWN;
	}
}

static int take_input ()
{
	SLang_Load_Type x;

	x.type = 'r';		       /* readline type */
	x.read = read_using_readline;       /* function to call to perform the read */

	return SLang_load_object (&x);
}

int
SSLang_init_tty()
{
#ifdef __MSDOS__
	return SLang_init_tty (34, 0, 1);
#else	
	return SLang_init_tty (7, 0, 1);
#endif
}

void
getspar()
{
	int nr,d = 0;
	char *des;

	if (SLang_pop_string(&des, &d)) return;
	if (SLang_pop_integer(&nr)) return;

	if (strcmp(des,"thetas") == 0)
		SLang_push_float(sp[nr].thetas);
	else if (strcmp(des,"ksat") == 0)
		SLang_push_float(sp[nr].ksat);
	else if (strcmp(des,"residual_water") == 0)
		SLang_push_float(sp[nr].residual_water);
	else if (strcmp(des,"n") == 0)
		SLang_push_float(sp[nr].n);
	else if (strcmp(des,"alpha") == 0)
		SLang_push_float(sp[nr].alpha);
	else if (strcmp(des,"l") == 0)
		SLang_push_float(sp[nr].l);
	else if (strcmp(des,"psisat") == 0)
		SLang_push_float(sp[nr].psisat);
	else if (strcmp(des,"b") == 0)
		SLang_push_float(sp[nr].b);
	else SLang_push_float(MISSVAL);
}

void
setspar(int *nr, char *des,double *val)
{
	if (strcmp(des,"thetas") == 0)
		sp[*nr].thetas = *val;
	else if (strcmp(des,"ksat") == 0)
		sp[*nr].ksat = *val;
	else if (strcmp(des,"residual_water") == 0)
		sp[*nr].residual_water = *val;
	else if (strcmp(des,"n") == 0)
		sp[*nr].n = *val;
	else if (strcmp(des,"alpha") == 0)
		sp[*nr].alpha = *val;
	else if (strcmp(des,"l") == 0)
		sp[*nr].l = *val;
	else if (strcmp(des,"psisat") == 0)
		sp[*nr].psisat = *val;
	else if (strcmp(des,"b") == 0)
		sp[*nr].b = *val;
	else fprintf(stderr,"Don't know: %s\n",des);
}

int
sl_add_set (char *name, int *points)
{
	return add_set(NULL,name,name,*points,0,0);
}

int readset(char *fname,char *name)
{
	(void) get_data(fname,name,-1);
	return getsetbyname(name);
}


void
V_tstep_soil()
{
	fprintf(stderr,"under construction\n");
	exit(1);
}

double
V_ts_time(char *tmstr)
{
	TM tm;
	double dumm;

	ts_time  (tmstr, NULL, &tm);
	ts_jday  (&tm, &dumm);
	return dumm;
}

char *
V_ts_fmt(double *inp)
{
	static char buf[512];
	TM tm;
	
	ts_jdate(&tm, *inp);
	(void)strftime(buf, 512, TIME_OFMT, &tm);
	return buf;
}

#endif /* HAVE_LIBSLANG */

static int ft = 0;
static int bclk = 0;
double cpusec()
{
	if (ft == 0){
		bclk = (int)clock();
		ft = 1;
	}
#ifdef CLOCKS_PER_SEC
#undef CLK_TCK	
#define CLK_TCK CLOCKS_PER_SEC
#endif
#ifdef  CLK_TCK
	return (double)((int)clock() - bclk)/(double)CLK_TCK;
#else
	return -1.0;
#endif
}
