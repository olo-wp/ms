//
// Created by Olgierd Zygmunt on 29/01/2025.
//

#ifndef MINIX_SERV_PROC_LOCK_H
#define MINIX_SERV_PROC_LOCK_H

#define LOCK               999
#define NO_PROC            2000
#define SIG_DIS            2001

int wait_exit(pid_t p);

#endif //MINIX_SERV_PROC_LOCK_H
