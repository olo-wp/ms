#include <minix/ipc.h>
#include <minix/syslib.h>
#include <minix/rs.h>
#include <sys/types.h>
#include <lib.h>
#include <stdio.h>

#include "proc_watch.h"

int watch_exit(endpoint_t ep){
    //printf("------ WATCH EXIT START CALL -----\n");
    message mes;
    endpoint_t FPS;
    if (minix_rs_lookup("fps", &FPS) < 0) printf("fail rs lookup \n");
    mes.m1_i1 = ep;
    mes.m_type = WATCH;
    int res;
    int cnt = 0;
    while(1) {
        res = sendrec(FPS, &mes);
        if(res >= 0){
            //printf("------ WATCH EXIT ANS RECIEVED -----\n");
            break;
        }
        cnt++;
        if(cnt >= 20){
            printf("max retry exceeded \n");
            return -100;
        }
    }
    //printf("------ WATCH EXIT RETURN -----\n");
    return 0;
}

int cancel_watch_exit(endpoint_t ep){
    //printf("------ CANCEL WATCH EXIT START CALL -----\n");
    message mes;
    endpoint_t FPS;
    if (minix_rs_lookup("fps", &FPS) < 0) printf("fail rs lookup \n");
    mes.m1_i1 = ep;
    mes.m_type = STOP_WATCH;
    int res;
    int cnt = 0;
    while(1) {
        res = sendrec(FPS, &mes);
        if(res >= 0){
            //printf("------ CANCEL WATCH EXIT ANS RECIEVED -----\n");
            break;
        }
        cnt++;
        if(cnt >= 20){
            printf("max retry exceeded \n");
            return -100;
        }
    }
    //printf("------CANCEL WATCH EXIT RETURN -----\n");
    return 0;
}

int query_exit(endpoint_t *epp){
    //printf("------QUERY EXIT CALL -----\n");
    message mes;
    endpoint_t FPS;
    if (minix_rs_lookup("fps", &FPS) < 0) printf("fail rs lookup \n");
    mes.m_type = QUERY_EXIT;
    int res;
    int cnt = 0;
    while(1) {
        res = sendrec(FPS, &mes);
        if(res >= 0){
            //printf("------QUERY EXIT ANS RECIEVED -----\n");
            break;
        }
        cnt++;
        if(cnt >= 20){
            printf("max retry exceeded \n");
            return -100;
        }
    }
    *epp = mes.m1_i2;
    //printf("------QUERY EXIT RETURN -----\n");
    return mes.m1_i1;
}