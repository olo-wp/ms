#include "inc.h"
#include "proc_tab.h"
#include <lib.h>
#include <minix/syslib.h>
#include <minix/rs.h>

endpoint_t who;
endpoint_t FPS;
endpoint_t pm;
endpoint_t SELF_E;

static int callnr;
static const int verbose = 1;

proc_tab intr_cur;
proc_tab intr_end;
proc_tab locks;

static void sef_local_startup(void);
static int sef_cb_init_fresh(int type, sef_init_info_t *info);
static void sef_cb_signal_handler(int signo);

static void free_locks(proc_tab *l, endpoint_t tar);
static void proc_tab_cleanup(proc_tab *l, endpoint_t tar);


int main(int argc, char** argv) {
    pt_init(&intr_cur);
    pt_init(&intr_end);
    pt_init(&locks);

    sef_local_startup();

    /* ===================================================
     * Sending PM our endpoint. PM cannot use _rs_lookup, so we will send it a mes.
     */

    message m;

    minix_rs_lookup("pm", &pm);
    m.m_type = FPS_ENDP;

    int r = sendrec(pm,&m);
    if(r < 0) printf("initial sendrec error");
    if(verbose) printf("succesful FPS registration\n");

    while (TRUE) {
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
        int r;
        if ((r = sef_receive(ANY, &m)) != OK)
            printf("FPS: sef_receive failed %d.\n", r);
        who = m.m_source;
        callnr = m.m_type;
        if(verbose) printf("got message type %d.\n", callnr);
        switch (callnr) {
            case WATCH: {
                if(verbose) printf("CASE watch start\n");
                add_pair(&intr_cur, who, m.m1_i1);
                if(verbose) printf("CASE watch tab op\n");
                _syscall(who, OK, &m);
                if(verbose) printf("CASE watch success\n");
                break;
            }
            case STOP_WATCH: {
                if(verbose) printf("CASE stopwatch start\n");
                del_pair(&intr_cur, who, m.m1_i1);
                if(verbose) printf("CASE SW tab op\n");
                _syscall(who, OK, &m);
                if(verbose) printf("CASE stopwatch success\n");
                break;
            }
            case LOCK: {
                /* we need to ask the pm, about the endpoint of a procces with a certain pid */
                m.m_type = EDP_ASK;
                int res = sendrec(pm, &m);
                if(res < 0) printf("LOCK sendrec error \n");
                endpoint_t target = m.m1_i1;
                if (target) {
                    add_pair(&locks, who, target);
                } else {
                    m.m_type = NO_PROC;
                    _syscall(who, NO_PROC, &m);
                }
                // do not reply until process "target" ends, unless pid does not exist;
                break;
            }
            case PROC_END: {
                endpoint_t end_proc = m.m1_i1;
                /* removing locks on the processes */
                free_locks(&locks, end_proc);
                /* cleaning intr_end from old entries */
                proc_tab_cleanup(&intr_end, end_proc);
                /* notifing processes */
                for (int i = 0; i < MAX_PROC; i++) {
                    if (intr_cur.t[i].target == end_proc) notify(intr_cur.t[i].source);
                }
                /* moving ended procceses to another table */
                rewrite_with_target(&intr_cur, &intr_end, end_proc);
                break;
            }
            case QUERY_EXIT: {
                if(verbose) printf("CASE query exit start\n");
                int cc = cur_count(&intr_end, who);
                m.m1_i1 = cc > 0 ? cc - 1 : cc;
                m.m1_i2 = find_and_del(&intr_end, who);
                if(verbose) printf("CASE QE tab op\n");
                _syscall(who, OK, &m);
                if(verbose) printf("CASE stopwatch succes\n");
                break;
            }
            case GOT_SIGNAL: {
                endpoint_t target = m.m1_i1;
                m.m_type = SIG_DIS;
                for (int i = 0; i < MAX_PROC; i++) {
                    if (locks.t[i].target == target) _syscall(locks.t[i].source, SIG_DIS, &m);
                }
                break;
            }
        }
    }
}


    /* removing locks on the processes and freeing locks array*/
    void free_locks(proc_tab *l, endpoint_t tar){
        message free;
        free.m_type = OK;
        for(int i = 0; i < MAX_PROC; i++){
            if(l -> t[i].target == tar){
                _syscall(l -> t[i].source, OK, &free);
                l -> t[i].source = -1;
                l -> t[i].target = -1;
            }
        }
    }
    /* cleaning intr_end from old entries */
    void proc_tab_cleanup(proc_tab *l, endpoint_t tar){
        endpoint_t tar_p = _ENDPOINT_P(tar);
        for(int i = 0; i < MAX_PROC; i++){
            if(_ENDPOINT_P(l -> t[i].target) == tar_p){
                l -> t[i].source = -1;
                l -> t[i].target = -1;
            }
        }
    }

    void sef_local_startup() {
        sef_setcb_init_fresh(sef_cb_init_fresh);
        sef_setcb_init_restart(sef_cb_init_fresh);

        sef_setcb_signal_handler(sef_cb_signal_handler);

        sef_startup();
    }

    static int sef_cb_init_fresh(int UNUSED(type), sef_init_info_t *UNUSED(info))
    {
    /* Initialize the fps server. */

        SELF_E = getprocnr();

        if(verbose)
            printf("FPS: I'm alive. Self: %d\n", SELF_E);

        return(OK);
    }

    static void sef_cb_signal_handler(int signo)
    {
        /* Only check for termination signal, ignore anything else. */
        if (signo != SIGTERM) return;

        if (verbose){
            printf("FPS: I quit.\n");
        }
    }



