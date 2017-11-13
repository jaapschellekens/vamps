# Generated automatically from Makefile.in by configure.
SHELL = /bin/sh

CC	= gcc
CFLAGS	= -g -O2
MOBJ	= ${OBJDIR}/vamps.o ${OBJDIR}/sigs.o ${OBJDIR}/init_s.o ${OBJDIR}/scanf.o ${OBJDIR}/misc.o ${OBJDIR}/intrins.o ${OBJDIR}/help.o ${OBJDIR}/hist.o ${OBJDIR}/plot.o ${OBJDIR}/tmplist.o
top_srcdir 	= ..
srcdir 		= .
prefix 		= /usr/local
exec_prefix 	= ${prefix}
BINDIR 		= ${exec_prefix}/bin
LIBDIR 		= ${exec_prefix}/lib
OBJDIR 		= ../objsLinux_x86_64
RM 	= rm -f
VAMPSLIBDIR	= ${prefix}/share/vamps0.99i
INC	= -I$(top_srcdir)/include -I$(top_srcdir)
GNUPLOT	= /usr/bin/gnuplot
DEFS	= -DHAVE_CONFIG_H -DVAMPSLIB=\"${VAMPSLIBDIR}\" -DGNUPLOT=\"${GNUPLOT}\"
SLDOC	= vamps -I sldoc

all: ${MOBJ}

${OBJDIR}/vamps.o: vamps.c
	${CC} $(CFLAGS) $(INC) -c ${DEFS} -o $@ vamps.c 

${OBJDIR}/plot.o: plot.c
	${CC} $(CFLAGS) $(INC) -c ${DEFS} -o $@ plot.c 

${OBJDIR}/hist.o: hist.c
	${CC} $(CFLAGS) $(INC) -c ${DEFS} -o $@ hist.c 

${OBJDIR}/tmplist.o: tmplist.c
	${CC} $(CFLAGS) $(INC) -c ${DEFS} -o $@ tmplist.c 

${OBJDIR}/sigs.o: sigs.c
	${CC} $(CFLAGS) $(INC) -c ${DEFS} -o $@ sigs.c 

${OBJDIR}/scanf.o: scanf.c
	${CC} $(CFLAGS) $(INC) -c ${DEFS} -o $@ scanf.c 

${OBJDIR}/intrins.o: intrins.c
	${CC} $(CFLAGS) $(INC) -c ${DEFS} -o $@ intrins.c 

${OBJDIR}/help.o: help.c
	${CC} $(CFLAGS) $(INC) -c ${DEFS} -o $@ help.c 

${OBJDIR}/misc.o: misc.c
	${CC} $(CFLAGS) $(INC) -c ${DEFS} -o $@ misc.c 

${OBJDIR}/fit.o: fit.c
	${CC} $(CFLAGS) $(INC) -c ${DEFS} -o $@ fit.c 

${OBJDIR}/init_s.o: init_s.c
	${CC} $(CFLAGS) $(INC) -c ${DEFS} -o $@ init_s.c 

doc: 0main.tex

0main.tex: *.c
	${SLDOC}  -KC -c *.c > doc
	${SLDOC}  -i doc | sort > sldoc.idx
	${SLDOC}  doc > 0main.txt
	echo "\\documentclass{article}\\usepackage{makeidx}\\makeindex\\begin{document}\\printindex" > 0main.tex
	echo "\\section{main}" >> 0main.tex
	${SLDOC}  -l doc >> 0main.tex
	echo "\\end{document}" >> 0main.tex
	${SLDOC}  -t doc > 0main.trf
	rm sldoc.idx doc
