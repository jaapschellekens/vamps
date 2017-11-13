/* ts.h */

#ifndef TS_H_
#define TS_H_

#include <stdio.h>
#include <time.h>

#define VERSION		"TS 2.1, Feb 1995"

typedef struct tm TM;

typedef struct {
	double	x;	/* x coordinate */
	double	y;	/* y coordinate */
} XY;

/* default parameters
*/
#define DIG_TTY		"/dev/ttyb"
#define DIG_DELTA	4
#define S5002M		2.0
#define LSF4M		1.0
#define TIME_OFMT	"%d/%m/%Y %H:%M:%S "
#define JDATE_OFMT	"%.8f"
#define VALUE_OFMT	" %g"

/* ts_getinput() return values
*/
#define INP_OTHER	0	/* none of the below */
#define INP_COMMENT	1	/* # */


/* common functions
*/
extern char	*ts_command  (int argc, char *argv[]);
extern char	*ts_getdefault  (char *name);
extern void	ts_jday  (TM *tm, double *jul);
extern void	ts_jdate  (TM *tm, double jul);
extern int	ts_time  (char *str, char **eptr, TM *tm);
extern int	ts_getinput  (FILE *fp, char *buf);
extern int	ts_opendefaults  (void);
extern XY	*ts_getxy  (char *buf, XY *xy,int xcol, int ycol);
extern void	ts_closedefaults  (void);
extern void	*ts_memory  (void *ptr, size_t size,const char *progname);
extern XY 	*ts_readf  (int *points,FILE *datfile, char *prgname,int xcol, int ycol, int verbose);
extern		double *ts_slopes(XY xy[], int np);
extern		int ts_meval(XY val[], XY tab[], double mtab[], int nt, int nv);
#endif /* TS_H_ */

