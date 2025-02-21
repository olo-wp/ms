# Makefile for Test STorage server (used for testing Exit Server)
PROG=	tst
SRCS=	main.c 

DPADD+=	${LIBSYS}
LDADD+=	-lsys -lc

MAN=

BINDIR?= /usr/sbin
FILES=tst.conf
FILESNAME=tst
FILESDIR= /etc/system.conf.d

.include <minix.service.mk>
