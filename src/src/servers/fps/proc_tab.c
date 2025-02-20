#include "proc_tab.h"

void pp_init(proc_pair *t, endpoint_t f, endpoint_t s){
    t->source = f;
    t->target = s;
}

void print_pair(proc_pair p){
    printf("%i -> %i\n", p.source, p.target);
}

void pt_init(proc_tab *tab){
    for(int i = 0; i < MAX_PROC; i++){
        tab->t[i].source = -1;
        tab->t[i].target = -1;
    }
}

void print_tab(proc_tab *tab){
    for(int i = 0; i < MAX_PROC; i++){
        if(tab->t[i].source != -1 && tab->t[i].target != -1){
            printf("i=%i ", i);
            print_pair(tab->t[i]);
        }
    }
}

int cur_count(proc_tab *tab, endpoint_t f){
    if(f == -1) return -1;
    int count = 0;
    for(int i = 0; i < MAX_PROC; i++){
        if(tab->t[i].source == f) count++;
    }
    return count;
}

int find_pair(proc_tab *tab, endpoint_t source, endpoint_t target){
    for(int i = 0; i < MAX_PROC; i++){
        if(tab->t[i].source == source && tab->t[i].target == target) return i;
    }
    return -1;
}

proc_pair del_pair(proc_tab *tab, endpoint_t source, endpoint_t target){
    int idx = find_pair(tab,source,target);
    proc_pair res;
    pp_init(&res,-1,-1);
    if(idx == -1) return res;
    endpoint_t temp1 = tab->t[idx].target;
    endpoint_t temp2 = tab->t[idx].source;
    tab->t[idx].target = -1;
    tab->t[idx].source = -1;
    pp_init(&res, temp1, temp2);
    return res;
}

int add_pair(proc_tab *tab, endpoint_t source, endpoint_t target){
    for(int i = 0; i < MAX_PROC; i++){
        if(tab->t[i].source == -1 && tab->t[i].target == -1){
            tab->t[i].source = source;
            tab->t[i].target = target;
            return 1;
        }
    }
    return 0;
}


endpoint_t find_and_del(proc_tab *tab, endpoint_t source){
    endpoint_t res = -1;
    for(int i = 0 ; i < MAX_PROC; i++){
        if(tab->t[i].source == source){
            res = tab->t[i].target;
            tab->t[i].source = -1;
            tab->t[i].target = -1;
            break;
        }
    }
    return res;
}

void rewrite_with_target(proc_tab *tab_1, proc_tab *tab_2, endpoint_t target){
    int k = 0;
    for(int i = 0; i < MAX_PROC; i++){
        endpoint_t t = tab_1->t[i].target;
        endpoint_t s = tab_1->t[i].source;
        if(t == target){
            tab_1->t[i].target = -1;
            tab_1->t[i].source = -1;
            while(k < MAX_PROC){
                if(tab_2->t[k].target == -1 && tab_2->t[k].source == -1){
                    tab_2->t[k].target = t;
                    tab_2->t[k].source = s;
                    break;
                }
                ++k;
            }
        }
    }
};