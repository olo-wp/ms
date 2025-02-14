#include "inc.h"
#include "proc_tab.h"

static endpoint_t who;
static endpoint_t FPS;
static endpoint_t PM;
static int callnr;

static proc_tab intr_cur;
static proc_tab intr_end;
static proc_tab locks;

static void sef_local_startup();
static void get_mes(message *m);
static void send_reply();
static void free_locks(proc_tab *l, endpoint_t tar);
static void proc_tab_cleanup(proc_tab *l, endpoint_t tar)


int main(int argc, char** argv){
    pt_init(&intr_cur);
    pt_init(&intr_end);
    pt_init(&locks);

    sef_local_startup(void);

    /* ===================================================
     * Sending PM our endpoint. PM cannot use _rs_lookup, so we will send it a mes.
     */

    message m;
    int ans;

    if (sys_whoami(&FPS, NULL, NULL) != OK) {
        panic("sys_whoami failed");
    }
    minix_rs_lookup("pm", &pm);
    m.m_type = FPS_ENDP;
    int r = sendrec(pm, &m);    //It might be useful to wait for the reply here, as it is vital for our server

    while(TRUE){
        /* Skeleton:
         * 1. we ge a message, depending on its type we either:
         * 2. switch(type):
         * case(WATCH) we add (sender, target) to intr_cur
         * case(STOP_WATCH) we remove (sender, target) from intr_cur;
         * case(LOCK) we lock sender onto target:
         *      basically we will need to add sender, target to locks and continue,
         *      when we know that target end we will reply all senders of that target.
         * case(PROC_END) pm tells us that a process ends, what we need to do:
         *      a) we move all the entries associated with (*,ended_proc) from intr_cur to intr_end
         *      b) we remove all the locks (*, ended_proc) and send corresponding replies
         *      c) if a process ends and there already are processes with the same endpoint in intr_end
         *      we need to remove all such processes from intr_end
         * case(QUERY_EXIT) we remove any entry corresponding with a given sender from intr_end,
         * then we cout number of remaining processes connected with sender in intr_end
         * 3. this is the main part, but we will also need to transfer signals onto locked processes
         */
        get_mes(&m);
        switch (callnr) {
            case WATCH:
                add_pair(&intr_cur, who, m.m1_i1);
                _syscall(who,OK,&m);
                break;
            case STOP_WATCH:
                del_pair(&intr_cur,who,m.m1_i1);
                _syscall(who,OK,&m);
                break;
            case LOCK:
                int pd = m.m1_i1;
                /* we need to ask the pm, about the endpoint of a procces with a certain pid */
                while(TRUE) {
                    int res = sendrec(pm, EDP_ASK, &m);
                    if(res >= 0) break;
                }
                endpoint_t target = m.m1_i1;
                if(target) {
                    add_pair(&locks, who, target);
                } else {
                    m.m_type = NO_PROC;
                    _syscall(who,NO_PROC,&m);
                }
                // do not reply until process "target" ends, unless pid does not exist;
                break;
            case PROC_END:
                endpoint_t end_proc = m.m1_i1;
                /* removing locks on the processes */
                free_locks(&locks, end_proc);
                /* cleaning intr_end from old entries */
                proc_tab_cleanup(&intr_end);
                /* notifing processes */
                for(int i = 0; i < MAX_PROC; i++){
                    if(intr_cur[i].target == end_proc) notify(intr_cur[i].source);
                }
                /* moving ended procceses to another table */
                rewrite_with_target(&intr_cur, &intr_end, end_proc);
                break;
            case QUERY_EXIT:
                int cc = cur_count(intr_end, who);
                m.m1_i1 = cc > 0 ? cc-1 : cc;
                m.m1_i2 = find_and_del(intr_end, who);
                _syscall(who,OK,&m);
                break;
            case GOT_SIGNAL:
                endpoint_t target = m.m1_i1;
                m.m_type = SIG_DIS;
                for(int i = 0; i < MAX_PROC; i++){
                    if(locks[i].target == target) _syscall(locks[i].source, SIG_DIS, &m);
                }
                break;
        }
    }


    /* removing locks on the processes and freeing locks array*/
    static void free_locks(proc_tab *l, endpoint_t tar){
        message free;
        free.m_type = OK;
        for(int i = 0; i < MAX_PROC; i++){
            if(l[i] -> target == end_proc){
                _syscall(l[i] -> source, OK, &free);
                l[i] -> source = -1;
                l[i] -> target = -1;
            }
        }
    }
    /* cleaning intr_end from old entries */
    static void proc_tab_cleanup(proc_tab *l, endpoint_t tar){
        endpoint_t tar_p = _ENDPOINT_P(tar);
        for(int i = 0; i < MAX_PROC; i++){
            if(_ENDPOINT_P(l[i] -> target) == tar_p){
                l[i] -> source = -1;
                l[i] -> target = -1;
            }
        }
    }


    static void sef_local_startup() {
        sef_setcb_init_fresh(sef_cb_init_fresh);
        sef_setcb_init_restart(sef_cb_init_fail);

        sef_setcb_signal_handler(sef_cb_signal_handler);

        sef_startup();
    }
    static void get_mes(message *m){
        sef_receive(ANY, &m);
        who = m->m_source;
        callnr = m->m_type;
    }
}

