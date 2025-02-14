//
// Created by Olgierd Zygmunt on 18/01/2025.
//
#include <stdio.h>

#ifndef MINIX_SERV_PROC_TAB_H
#define MINIX_SERV_PROC_TAB_H

#define MAX_PROC 512

typedef int endpoint_t;

typedef struct proc_pair{
    endpoint_t source;
    endpoint_t target;
}proc_pair;

void pp_init(proc_pair *t, endpoint_t f, endpoint_t s);

void print_pair(proc_pair t);

typedef struct proc_tab{
    proc_pair t[MAX_PROC];
} proc_tab;

void pt_init(proc_tab *tab);

int cur_count(proc_tab *tab, endpoint_t f);

int find_pair(proc_tab *tab, endpoint_t source, endpoint_t target);

proc_pair del_pair(proc_tab *tab, endpoint_t source, endpoint_t target);

int add_pair(proc_tab *tab, endpoint_t source, endpoint_t target);

endpoint_t find_and_del(proc_tab *tab, endpoint_t source);

void rewrite_with_target(proc_tab *tab_1, proc_tab *tab_2, endpoint_t target);

void print_tab(proc_tab *tab);


#endif //MINIX_SERV_PROC_TAB_H
