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
static const int verbose = -1;

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
        int r;
        if ((r = sef_receive(ANY, &m)) != OK)
            printf("FPS: sef_receive failed %d.\n", r);
        who = m.m_source;
        callnr = m.m_type;
        switch (callnr) {
            case WATCH: {
                add_pair(&intr_cur, who, m.m1_i1);
                m.m_type = OK;
                send(who, &m);
                break;
            }
            case STOP_WATCH: {
                del_pair(&intr_cur, who, m.m1_i1);
                m.m_type = OK;
                send(who, &m);
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
                    send(who, &m);
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
                int cc = cur_count(&intr_end, who);
                if(verbose) printf("checking table for proc nr: %d\n", who);
                m.m1_i1 = cc - 1;
                if(verbose) printf("alive processes: %d\n", cc);
                m.m1_i2 = find_and_del(&intr_end, who);
                if(verbose) printf("returing process: %d\n", m.m1_i2);
                m.m_type = OK;
                send(who, &m);
                break;
            }
            case GOT_SIGNAL: {
                endpoint_t target = m.m1_i1;
                m.m_type = SIG_DIS;
                for (int i = 0; i < MAX_PROC; i++) {
                    if (locks.t[i].source == target) {
                        m.m_type = SIG_DIS;
                        send(locks.t[i].source, &m);
                        locks.t[i].source = -1;
                        locks.t[i].target = -1;
                    }
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
                send(l -> t[i].source, &free);
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
                printf("cleaning endpoint %d",l -> t[i].target);
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



