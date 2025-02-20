#include <minix/ipc.h>
#include <minix/syslib.h>
#include <minix/rs.h>
#include <sys/types.h>
#include <errno.h>

#include "proc_lock.h"

int wait_exit(pid_t p){
    message mes;
    endpoint_t FPS;
    minix_rs_lookup("fps", &FPS);
    mes.m1_i1 = p;
    mes.m_type = LOCK;
    int res;
    while(1) {
        res = sendrec(FPS, &mes);
        if(res >= 0) break;
    }
    if(mes.m_type == NO_PROC){
        errno = ESRCH;
        return -1;
    }
    if(mes.m_type == SIG_DIS){
        errno = EINTR;
        return -1;
    }
    return 0;
}