# Makefile for the lab1 driver (based on that of hello).

COMPILER_TYPE= gnu

CC=gcc

PROG=	proj
SRCS=	proj.c vbe.c video_gr.c vector2D.c queue.c vehicle.c kbc.c keyboard.c timer.c track.c mouse.c bitmap.c vector.c context_menu.c font.c

CCFLAGS= -Wall -O3

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+=	-llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
