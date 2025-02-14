//
// Created by Olgierd Zygmunt on 29/01/2025.
//

#ifndef MINIX_SERV_PROC_WATCH_H
#define MINIX_SERV_PROC_WATCH_H

#define WATCH              997
#define STOP_WATCH         998
#define QUERY_EXIT         1001

int watch_exit(endpoint_t ep);
int cancel_watch_exit(endpoint_t ep);
int query_exit(endpoint *epp);

#endif //MINIX_SERV_PROC_WATCH_H
