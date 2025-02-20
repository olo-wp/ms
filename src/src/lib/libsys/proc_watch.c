#include <minix/ipc.h>
#include <minix/syslib.h>
#include <minix/rs.h>
#include <sys/types.h>
#include <lib.h>

#include "proc_watch.h"

int watch_exit(endpoint_t ep){
    message mes;
    endpoint_t FPS;
    minix_rs_lookup("fps", &FPS);
    mes.m1_i1 = ep;
    int res;
    while(1) {
        res = _syscall(FPS, WATCH, &mes);
        if(res >= 0) break;
    }
    return 0;
}

int cancel_watch_exit(endpoint_t ep){
    message mes;
    endpoint_t FPS;
    minix_rs_lookup("fps", &FPS);
    mes.m1_i1 = ep;
    int res;
    while(1) {
        res = _syscall(FPS, STOP_WATCH, &mes);
        if(res >= 0) break;
    }
    return 0;
}

int query_exit(endpoint_t *epp){
    message mes;
    endpoint_t FPS;
    minix_rs_lookup("fps", &FPS);
    mes.m1_i1 = *epp;
    mes.m_type = QUERY_EXIT;
    int res;
    while(1) {
        res = sendrec(FPS, &mes);
        if(res >= 0) break;
    }
    *epp = mes.m1_i1;
    return mes.m1_i2;
}