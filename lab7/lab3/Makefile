COMPILER_TYPE= gnu

CC=gcc

PROG=	lab3
SRCS=	lab3.c test3.c timer.c kbd.c _kbd.S

CCFLAGS= -Wall

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+= -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
