# Makefile for Finished Processes Server
PROG=	fps
SRCS=	main.c proctab.c

DPADD+=	${LIBSYS}
LDADD+=	-lsys

MAN=

BINDIR?= /usr/sbin
FILES=fps.conf
FILESNAME=fps
FILESDIR= /etc/system.conf.d

.include <minix.service.mk>
