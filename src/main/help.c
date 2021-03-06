/* $Header: /home/schj/src/vamps_0.99g/src/main/RCS/help.c,v 1.5 1999/01/06 12:13:01 schj Alpha $ */

/* $RCSfile: help.c,v $
 *  $Author: schj $
 *  $Date: 1999/01/06 12:13:01 $ */


/* help system functions */

#ifdef HAVE_CONFIG_H
#include "vconfig.h"
#endif

#ifdef HAVE_LIBSLANG
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <slang.h>

#define SLDOC_IDX	"sldoc.idx"

/* in vamps.c */

extern char	*sl_arg0;

/* in misc.c */
extern char	**sl_make_strv(char *, char *, int *);
extern char	*sl_statfile(char *, char *);
extern char	*sl_fgetl(FILE *);
extern void	sl_free_strv(char **);

/* forward declarations */
void sl_help_list(char *dir);
void sl_help_fmt_list(void);
void sl_help_name(char *dir, char *name);
void sl_page(void);
void sl_setidx(char *idxname);
void sl_help_fmt_name(char *dir, char *file, char *name);
void sl_help_fmt_verb(char *s);
void sl_help_fmt_text(char *s);
void sl_pageln(char *s, int len);
void sl_page_tty(char *txt[], int line, int row, int scroll, char *msg);

/* globals */
char	**lstp;
int	nlst, l_all, width;
static char sldoc_idx[512] = SLDOC_IDX;


void
sl_setidx(char *idxname)
{
	strcpy(sldoc_idx,idxname);
}

/* ? command entry point */
void 
slash_help(char *cmd)
{
	int	argc, dirc, i, j;
	char	**argv, **dirv;
	extern char	sl_path[];

	dirv = sl_make_strv(sl_path, ":", &dirc);
	argv = sl_make_strv(cmd, " \t\";", &argc);

	nlst = l_all = width = 0;

	if(!argc) {
		for(i = 0; i < dirc; i++)
			sl_help_list(dirv[i]);
		if(nlst) {
			sl_help_fmt_list();
			sl_free_strv(lstp);
		}
	}
	else for(j = 0; j < argc; j++) {
		for(i = 0; i < dirc; i++)
			sl_help_name(dirv[i], argv[j]);
	}
	sl_free_strv(argv);
	sl_free_strv(dirv);

	sl_page();

	return;
}

/* open idx in dir, if present */
FILE *
sl_help_open_idx(char *dir)
{
	FILE	*fp;
	char	*path;

	if((path = sl_statfile(sldoc_idx, dir)) == (char *)NULL)
		return((FILE *)NULL);

	fp = fopen(path, "r");
	if(fp == (FILE *)NULL)
		(void)fprintf(stderr, "%s: Can't read \"%s\"\n", sl_arg0, path);
	SLFREE(path);

	return(fp);
}

void
sl_help_list(char *dir)
{
	int	n;
	FILE	*fp;
	char	*sp, *rp;

	if((fp = sl_help_open_idx(dir)) == (FILE *)NULL)
		return;
	if(!l_all)
		lstp = (char **)SLMALLOC((l_all = 16) * sizeof(char *));
	while(sp = sl_fgetl(fp)) {
		while(*sp == ' ' || *sp == '\t')
			sp++;
		if(*sp == '%' || *sp == '\0')
			continue;
		for(n = 0; sp[n] && sp[n] != ' '; n++) ;
		if(nlst >= l_all - 1)
			lstp = (char **)SLREALLOC((char *)lstp,
					(l_all += 16) * sizeof(char *));
		lstp[nlst++] = SLmake_nstring(sp, n);
		if(n > width)
			width = n;
	}
	lstp[nlst] = (char *)NULL;
	(void)fclose(fp);

	return;
}

void
sl_help_fmt_list(void)
{
	int	i, n, r, rows, cols;
	char	*cp, buf[256];

	width += 3;
	if((cols = SLtt_Screen_Cols) > 256)
		cols = 256;
	cols = (cols + 2) / width;
	rows = (nlst + cols - 1) / cols;

	sl_pageln("", 0);
	for(r = 0; r < rows; r++) {
		i = 0;
		for(n = r; n < nlst; n += rows) {
			while(i % width)
				buf[i++] = ' ';
			for(cp = lstp[n]; *cp; cp++)
				buf[i++] = *cp;
		}
		buf[i] = '\0';
		sl_pageln(buf, strlen(buf));
	}
	sl_pageln("", 0);

	return;
}

/* Regular expression routines for strings */
static SLRegexp_Type regexp_reg;

static int string_match (char *str, char *pat, int *np)
{
   int n = *np;
   unsigned int len;
   unsigned char rbuf[512], *match;

   regexp_reg.case_sensitive = 1;
   regexp_reg.buf = rbuf;
   regexp_reg.pat = (unsigned char *) pat;
   regexp_reg.buf_len = sizeof (rbuf);

   if (SLang_regexp_compile (&regexp_reg))
     {
	SLang_doerror ("Unable to compile pattern.");
	return 0;
     }
   
   n--;
   len = strlen(str);
   if ((n < 0) || (n >= len))
     {
	SLang_Error = SL_INVALID_PARM;
	return 0;
     }
   str += n;
   len -= n;
   
   if (NULL == (match = SLang_regexp_match((unsigned char *) str, len, &regexp_reg))) return 0;
   
   /* adjust offsets */
   regexp_reg.offset = n;
   
   return (1 + (int) ((char *) match - str));
}

void
sl_help_name(char *dir, char *name)
{
	int	n,m;
	FILE	*fp;
	char	*sp, *rp;

	if((fp = sl_help_open_idx(dir)) == (FILE *)NULL)
		return;
	while(sp = sl_fgetl(fp)) {
		while(*sp == ' ' || *sp == '\t')
			sp++;
		if(*sp == '%' || *sp == '\0')
			continue;
		for(n = 0; sp[n] && sp[n] != ' '; n++) ;
		sp[n++] = '\0';
		m = 1;
		/*
		if(string_match (sp, name, &m))
			(void)sl_help_fmt_name(dir, &sp[n], name);
			*/
		if(!strcmp(sp, name))
			(void)sl_help_fmt_name(dir, &sp[n], name);
	}
	(void)fclose(fp);

	return;
}

void
sl_help_fmt_name(char *dir, char *file, char *name)
{
	FILE	*fp;
	char	*sp;
	int	fmt_cm, fmt_lm, fmt_pp;
	int	m = 1;

	if((sp = sl_statfile(file, dir)) == (char *)NULL) {
		(void)fprintf(stderr, "%s: \"%s/%s\" out of date\n",
			sl_arg0, dir, sldoc_idx);
		return;
	}

	fp = fopen(sp, "r");
	SLFREE(sp);
	if(fp == (FILE *)NULL) {
		(void)fprintf(stderr, "%s: Can't read \"%s/%s\"\n",
			sl_arg0, dir, name);
		return;
	}

	while((sp = sl_fgetl(fp)) != (char *)NULL) {
		if(!strncmp(sp, "%I:", 3) && !strcmp(&sp[3], name))
		/*if(!strncmp(sp, "%I:", 3) && string_match(&sp[3],
		 *name,&m))*/
			break;
	}
	if(sp == (char *)NULL) {
		(void)fprintf(stderr, "%s: \"%s/%s\" out of date\n",
			sl_arg0, dir, sldoc_idx);
		(void)fclose(fp);
		return;
	}

/* set up controls */
	fmt_cm = 0;		/* normal mode */
	fmt_lm = -1;		/* force vskip at first line */
	fmt_pp = 0;		/* no parinsert */

/* do all lines starting with '%' */
	while((sp = sl_fgetl(fp)) != (char *)NULL && *sp++ == '%') {
		if(sp[0] == 'I' && sp[1] == ':')
			continue;	/* other key, same entry */

	/* set mode to verbatim if first char == '@' */
		if(fmt_cm = *sp == '@')
			sp++;

	/* squeeze multiple blank lines */
		if(*sp == '\0')
			fmt_pp++;

	/* set statii and format */
		else {
			if(fmt_pp || fmt_cm != fmt_lm)
				sl_pageln("", 0);
			if(fmt_cm)
				(void)sl_help_fmt_verb(sp);
			else
				(void)sl_help_fmt_text(sp);
			fmt_lm = fmt_cm;
			fmt_pp = 0;
		}
	}
	sl_pageln("", 0);
	(void)fclose(fp);

	return;
}

void
sl_help_fmt_verb(char *s)
{
	int	ch, n;
	char	*bp, buf[256];

/* strip leading spaces */
	while(*s == ' ')
		s++;
	bp = buf;
	*bp++ = '\037';
	for(n = 1; *s && *s != '\n' && n < 256; n++) {
		switch(ch = *s++) {
		case '\t':
			while(n % 5 && n++ < 256)
				*bp++ = ' ';
			break;
		default:
			*bp++ = ch;
			break;
		}
	}
	*bp++ = '\036';
	*bp = '\0';
	sl_pageln(buf, n);

	return;
}

void
sl_help_fmt_text(char *s)
{
	int	ch, cm, n;
	char	*bp, buf[256];

/* strip leading spaces and TABS */
	while(*s == ' ' || *s == '\t')
		s++;
	bp = buf;
	cm = '\036';
	for(n = 1; *s && *s != '\n' && n < 256; n++) {
		switch(ch = *s++) {
		case '\\':
			if(*s == '@')
				*bp++ = *s++;
			else
				*bp++ = '\\';
			break;
		case ' ':
		case '\t':
			*bp++ = ' ';
			while(*s == ' ' || *s == '\t')
				s++;
			break;
		case '@':
			*bp++ = cm = cm == '\036' ? '\037' : '\036';
			break;
		default:
			*bp++ = ch;
			break;
		}
	}
	*bp = '\0';
	sl_pageln(buf, n);

	return;
}

/* pager stuff */

static int	Rows, Cols;
static int	Pall, Npln;
static char	**Ptxt;

void
sl_pageln(char *s, int len)
{
	if(!Pall) {
		Rows = SLtt_Screen_Rows;
		Cols = SLtt_Screen_Cols;
		Ptxt = (char **)SLMALLOC((Pall = 24) * sizeof(char *));
	}
	if(Npln >= Pall - 1) {
		Pall += 24;
		Ptxt = (char **)SLREALLOC((char *)Ptxt, Pall * sizeof(char *));
	}

	/* should wrap long lines */
	if(len > Cols)
		len = Cols;
	Ptxt[Npln++] = SLmake_nstring(s, len);
	Ptxt[Npln] = (char *)NULL;

	return;
}

/* pager help and message texts */
#define H_LINES 13
char *sl_page_help[] = {
	"-----------------------------------------",
	"Keys: Kp = keypad, ^F denotes Ctrl-F etc.",
	"",
	"ArrDn, +, RET, j       Forward one line",
	"ArrUp, -, k            Backward one line",
	"PgDn, SPC, ^F, KpLR    Forward one page",
	"PgUp, b, ^B, KpUR      Backward one page",
	"Home, g, KpUL          To start of text",
	"End, G, KpLL           To end of text",
	"q, Q                   Quit text paging",
	"r, ^L                  Repaint screen",
	"h                      Display this help",
	"-----------------------------------------",
	NULL };

#ifndef __GO32__
/* pager control */
void
sl_page(void)
{
	int	key, line, last, hrow;
	char	*msg1 = "[Pager: PgUp/PgDn; q to quit]";
	char	*msg2 = "[Press any key to continue]";
	char	*msg3 = "[Press h for list of keys]";

	if(!Npln)
		return;

	SLtt_init_video();
	if(Npln < SLtt_Screen_Rows - 1) {
		sl_page_tty(Ptxt, 0, SLtt_Screen_Rows - Npln - 1, Npln, NULL);
		line = Npln;
	}
	else {
		sl_page_tty(Ptxt, 0, 0, SLtt_Screen_Rows - 1, msg1);
		line = 0;
	}
	while(line < Npln) {
		key = SLkp_getkey();
		if((hrow = SLtt_Screen_Rows - H_LINES - 1) < 0)
			hrow = 0;	/* bad luck if you resize too small */
		last = Npln - SLtt_Screen_Rows + 1;

	/* I need to scan for keypad keys A1/A3,C1/C3 because these
	 * send the same codes as Pg[Up/Dn] and Home/End on my NetBSD
	 * pc3 console driver and get evaluated first by SLang */
		switch(key) {
		case ' ':
		case SL_KEY_NPAGE:
		case SL_KEY_C3:
		case 6:		/* ^F */
			if(line >= last)
				break;
			if((line += SLtt_Screen_Rows - 2) > last)
				line = last;
			sl_page_tty(Ptxt, line, 0, 0, msg1);
			break;
		case 'b':
		case SL_KEY_PPAGE:
		case SL_KEY_A3:
		case 2:		/* ^B */
			if(line <= 0)
				break;
			if((line -= SLtt_Screen_Rows - 2) < 0)
				line = 0;
			sl_page_tty(Ptxt, line, 0, 0, msg1);
			break;
		case '+':
		case 'j':	/* ex/vi nostalgia */
		case '\r':
		case SL_KEY_DOWN:
			if(line >= last)
				SLtt_beep();
			else
				sl_page_tty(Ptxt, ++line + SLtt_Screen_Rows - 2,
					SLtt_Screen_Rows - 2, 1, msg1);
			break;
		case '-':
		case 'k':	/* ex/vi nostalgia */
		case SL_KEY_UP:
			if(line <= 0)
				SLtt_beep();
			else
				sl_page_tty(Ptxt, --line, 0, 0, msg1);
			break;
		case 'g':
		case SL_KEY_HOME:
		case SL_KEY_A1:
			if(line > 0)
				sl_page_tty(Ptxt, line = 0, 0, 0, msg1);
			break;
		case 'G':
		case SL_KEY_END:
		case SL_KEY_C1:
			if(line < last)
				sl_page_tty(Ptxt, line = last, 0, 0, msg1);
			break;
		case 'h':
			sl_page_tty(sl_page_help, 0, hrow, 0, msg2);
			SLkp_getkey();
			sl_page_tty(Ptxt, line + hrow, hrow, 0, msg1);
			break;
		case 'r':
		case 12:	/* ^L */
			sl_page_tty(Ptxt, line, 0, 0, msg1);
			break;
		case 'q':
		case 'Q':
			line = Npln;
			sl_page_tty(NULL, 0, SLtt_Screen_Rows, 0, NULL);
			break;
		default:
			sl_page_tty(NULL, 0, SLtt_Screen_Rows, 0, msg3);
			break;
		}
	}
/* do not reset, some "te" caps clear the screen */
/*	SLtt_reset_video(); */
	SLtt_erase_line();
	SLtt_flush_output();
	Pall = Npln = 0;
	sl_free_strv(Ptxt);

	return;
}
#else
void
sl_page(void)
{
	int i;

	for (i = 0; i < Npln ; i++)
		fprintf(stderr,"%s\n",Ptxt[i]);

	Pall = Npln = 0;
}
#endif

/* pager output on tty */
void
sl_page_tty(char *txt[], int line, int row, int scroll, char *msg)
{
	int	ch, n, ncols, nrows;
	char	*cp, *bp, buf[256];

	ncols = SLtt_Screen_Cols;
	nrows = SLtt_Screen_Rows - 1;

#ifndef __GO32__
	SLsig_block_signals();
#endif

	SLtt_erase_line();

/* stupid but device independent scroll up */
	if(scroll > 0) {
		while(scroll--){
			putc('\n', stderr);
		}
	}

	while(row < nrows) {
		SLtt_goto_rc(row, 0);
		row++;
		bp = buf;
		cp = txt[line++];
		for(n = 1; n <= ncols; n++) {
			switch(ch = *cp++) {
			case '\036':
				if(bp != buf) {
					*bp = '\0';
					SLtt_write_string(buf);
					bp = buf;
				}
				SLtt_normal_video();
				n--;
				break;
			case '\037':
				if(bp != buf) {
					*bp = '\0';
					SLtt_write_string(buf);
					bp = buf;
				}
				SLtt_reverse_video(1);
				n--;
				break;
			default:
				*bp++ = ch;
				break;
			}
			if(ch == '\0')
				break;
		}
		*bp = '\0';
		if(*buf)
			SLtt_write_string(buf);
		SLtt_del_eol();
	}
	SLtt_goto_rc(nrows, 0);
	if(msg != (char *)NULL) {
		SLtt_reverse_video(1);
		SLtt_write_string(msg);
	}
	SLtt_normal_video();
	SLtt_flush_output();
#ifndef __GO32__
	SLsig_unblock_signals();
#endif

	return;
}
#endif /* HAVE_LIBSLANG*/
