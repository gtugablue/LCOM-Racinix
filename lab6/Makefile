# Makefile for the lab1 driver (based on that of hello).

COMPILER_TYPE= gnu

CC=gcc

PROG=	lab6
SRCS=	lab6.c rtc.c test6.c

CCFLAGS= -Wall -O3

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+=	-ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>