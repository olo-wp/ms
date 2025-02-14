#ifndef MINIX_SERV_INC_H
#define MINIX_SERV_INC_H

#define _POSIX_SOURCE      1	/* tell headers to include POSIX stuff */
#define _MINIX             1	/* tell headers to include MINIX stuff */
#define _SYSTEM            1    /* get OK and negative error codes */

#define TRUE               1

#define WATCH              997
#define STOP_WATCH         998
#define LOCK               999
#define PROC_END           1000
#define QUERY_EXIT         1001
#define GOT_SIGNAL         1002

#define NO_PROC            2000
#define SIG_DIS            2001

#define FPS_ENDP           3000
#define EDP_ASK            3001

#include <minix/sysutil.h>
#include <minix/syslib.h>

#include <minix/es.h>
#include <minix/ipc.h>

#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/param.h>
#include <limits.h>
#include <errno.h>
#include <regex.h>

#include <minix/callnr.h>
#include <minix/config.h>
#include <minix/type.h>
#include <minix/const.h>
#include <minix/com.h>
#include <minix/ds.h>
#include <minix/syslib.h>
#include <minix/sysinfo.h>
#include <minix/sysutil.h>
#include <minix/keymap.h>
#include <minix/bitmap.h>
#include <minix/rs.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#endif //MINIX_SERV_INC_H
