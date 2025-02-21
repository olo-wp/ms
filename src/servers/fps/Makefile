# Makefile for Finished Processes Server
PROG=	fps
SRCS=	proc_tab.c main.c

DPADD+=	${LIBSYS}
LDADD+=	-lsys -lc


MAN=

BINDIR?= /usr/sbin
FILES=fps.conf
FILESNAME=fps
FILESDIR= /etc/system.conf.d

.include <minix.service.mk>
