SHELL = /bin/sh

TSOBJ = ts_com.o ts_input.o ts_mem.o ts_readf.o ts_spl.o ts_time.o
SLDOC = vamps -I sldoc
CC	= gcc
CFLAGS	= -g -O2

AR = ar
RANLIB = ranlib
RM 	= rm -f

libts.a: ${TSOBJ} dummy
	${AR} ru $@ ${TSOBJ}
	${RANLIB} $@

ts_input.o: ts_input.c
	${CC} ${CFLAGS} $(INC) -c ${DEFS} ts_input.c

ts_mem.o: ts_mem.c
	${CC} ${CFLAGS} $(INC) -c ${DEFS} ts_mem.c

ts_time.o: ts_time.c
	${CC} ${CFLAGS} $(INC) -c ${DEFS} ts_time.c

ts_readf.o: ts_readf.c
	${CC} ${CFLAGS} $(INC) -c ${DEFS} ts_readf.c

ts_com.o: ts_com.c
	${CC} ${CFLAGS} $(INC) -c ${DEFS} ts_com.c

ts_spl.o: ts_spl.c
	${CC} ${CFLAGS} $(INC) -c ${DEFS} ts_spl.c

doc: 0ts.tex
0ts.tex: *.c
	${SLDOC} -KC -c *.c > doc
	${SLDOC} -i doc | sort > sldoc.idx
	${SLDOC} doc > 0ts.txt
	echo "\\documentclass{article}\\usepackage{makeidx}\\makeindex\\begin{document}\\printindex" > 0ts.tex
	echo "\\section{libts.a}" >> 0ts.tex
	${SLDOC} -l doc >> 0ts.tex
	echo "\\end{document}" >> 0ts.tex
	${SLDOC} -t doc > 0ts.trf
	rm doc sldoc.idx

clean:
	rm -f *.o *.a *~ 0ts.???

dummy:


