SHELL 	= /bin/sh

NROBJ 	= nrutil.o nr_rw.o nr_mat.o nr_vec.o nr_stat.o nr_ut.o nr_free.o\
		log.o

NR_TRASH 	= *.o libnru.a *~ *.log *.idx *.ind nr_test test? *.aux ${TRASH}

NRDEFS 	= ${DEFS} -DNR_LIST #-DNR_DEBUG

CC	= gcc
CFLAGS	= -g -O2

SLDOC 	= vamps -I sldoc
NR_CC 	= $(CC)
AR 	= ar
RANLIB 	= ranlib
RM 	= rm -f

all: $(NROBJ) libnru.a


log.o: log.c 
	${CC} $(CFLAGS) $(INC) -c ${NRDEFS} -o $@ log.c 

nr_free.o: nr_free.c
	${NR_CC} ${CFLAGS} $(INC) -c ${NRDEFS} nr_free.c

nr_rw.o: nr_rw.c
	${NR_CC} ${CFLAGS} $(INC) -c ${NRDEFS} nr_rw.c

nr_ut.o: nr_ut.c
	${NR_CC} ${CFLAGS} $(INC) -c ${NRDEFS} nr_ut.c

nr_stat.o: nr_stat.c
	${NR_CC} ${CFLAGS} $(INC) -c ${NRDEFS} nr_stat.c

nr_mat.o: nr_mat.c
	${NR_CC} ${CFLAGS} $(INC) -c ${NRDEFS} nr_mat.c

nr_vec.o: nr_vec.c
	${NR_CC} ${CFLAGS} $(INC) -c ${NRDEFS} nr_vec.c

nrutil.o: nrutil.c
	${NR_CC} ${CFLAGS} $(INC) -c ${NRDEFS} nrutil.c

nr_test.o:	nr_test.c
	${NR_CC} ${CFLAGS}  -DNR_LIST -DNR_DEBUG -c nr_test.c

nr_test:	nr_test.o ${NROBJ}
	${NR_CC} ${CFLAGS}   *.o -o nr_test -lm #-lefence
	
clean:
	${RM} ${NR_TRASH}
	${RM} sldoc.idx doc 0nr_ut.???

libnru.a: $(NROBJ) dummy
	${AR} ru $@ $(NROBJ)
	${RANLIB} $@

doc: 0nr_ut.tex
0nr_ut.tex: *.c
	${SLDOC} -KC -c *.c > doc
	${SLDOC} -i doc | sort > sldoc.idx
	${SLDOC} doc > 0nr_ut.txt
	echo "\\documentclass{article}\\usepackage{makeidx}\\makeindex\\begin{document}\\printindex" > 0nr_ut.tex
	echo "\\section{libnru.a}" >> 0nr_ut.tex
	${SLDOC} -l doc >> 0nr_ut.tex
	echo "\\end{document}" >> 0nr_ut.tex
	${SLDOC} -t doc > 0nr_ut.trf
	rm doc sldoc.idx

dummy:
