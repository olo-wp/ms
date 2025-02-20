/* This is the master header for PM.  It includes some other files
 * and defines the principal constants.
 */
#define _POSIX_SOURCE      1	/* tell headers to include POSIX stuff */
#define _MINIX             1	/* tell headers to include MINIX stuff */
#define _SYSTEM            1	/* tell headers that this is the kernel */


#define PROC_END           1000
#define GOT_SIGNAL         1002

#define FPS_ENDP           3000 /* we will use this so that pm knows fps's endpoint */
#define EDP_ASK            3001 /* used when fps asks us for an endpoint */

/* The following are so basic, all the *.c files get them automatically. */
#include <minix/config.h>	/* MUST be first */
#include <sys/types.h>
#include <minix/const.h>
#include <minix/type.h>

#include <fcntl.h>
#include <unistd.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/timers.h>
#include <minix/param.h>

#include <limits.h>
#include <errno.h>
#include <sys/param.h>

#include "const.h"
#include "type.h"
#include "proto.h"
#include "glo.h"
