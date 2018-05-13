/*
*  main.c - Simulacao de carga de trabalho para escalonamento de processos
*
*  Autor: Marcelo Moreno
*  Projeto: Trabalho Pratico I - Sistemas Operacionais
*  Organizacao: Universidade Federal de Juiz de Fora
*  Departamento: Dep. Ciencia da Computacao
*
*  => NAO MODIFIQUE ESTE ARQUIVO <=
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "process.h"
#include "sched.h"
#include "rrprio.h"

#define SCHED_ITERATIONS 100
#define PROCESS_CREATION_PROBABILITY 0.3
#define PROCESS_DESTROY_PROBABILITY 0.05
#define PROCESS_BLOCK_PROBABILITY 0.6
#define PROCESS_UNBLOCK_PROBABILITY 0.4
#define PROCESS_SETPRIO_PROBABILITY 0.1

void _dumpSchedParams(Process *p) {
    RRPrioParams *rrp = processGetSchedParams(p);
    printf("Prio: %d; ",rrp->prio);
}

Process* _createProcess(Process *plist, int ppid, int prio) {
    RRPrioParams *rrp;
    printf("Criando processo... ");
    plist = processCreate(plist);
    rrp = malloc(sizeof (RRPrioParams));
    rrp->prio = prio;
    rrpInitSchedParams(plist, rrp);
    processSetStatus(plist, PROC_READY);
    processSetParentPid(plist, ppid);
    printf(" Criado PID %d!\n", processGetPid(plist));
    return plist;
}

Process* _destroyProcess(Process *plist, int pid) {
    printf("Destruindo processo... ");
    plist = processDestroy(plist, pid);
    printf(" Destruido PID %d!\n", pid);
    return plist;
}

int _countReady(Process *plist) {
    Process *p, *next;
    int count = 0;
    for (p=plist; p!=NULL; p=next) {
        next = processGetNext(p);
        if (processGetStatus(p)==PROC_READY)
            count++;
    }
    return count;
}

Process* _getNthReady(Process *plist, int n) {
    Process *p=NULL, *next;
    int count = 0;
    for (p=plist; p!=NULL; p=next) {
        next = processGetNext(p);
        if (processGetStatus(p)==PROC_READY)
            count++;
        if (count==n) return p;
    }
    return NULL;
}

Process* _doRandomThings(Process *plist) {
    Process *p, *next, *dst;
    int pid, n, newprio, oldprio;
    int ready;
    double r = rand() / (double)RAND_MAX;
    printf("===Acoes Aleatorias===\n");
    if (r < PROCESS_CREATION_PROBABILITY)
        plist = _createProcess(plist,1,rand()%8);
    for (p=plist;p!=NULL;p=next) {
        next = processGetNext(p);
        pid = processGetPid(p);
        if (pid==1) continue;
        r = rand() / (double)RAND_MAX;
        if (processGetStatus(p)==PROC_READY &&
            r < PROCESS_DESTROY_PROBABILITY) {
            plist = _destroyProcess(plist,processGetPid(p));
            continue;
        }
        r = rand() / (double)RAND_MAX;
        if (processGetStatus(p)==PROC_RUNNING &&
            r < PROCESS_BLOCK_PROBABILITY) {
            processSetStatus(p,PROC_WAITING);
            r = rand() / (double)RAND_MAX;
            if (r < PROCESS_SETPRIO_PROBABILITY) {
                ready = _countReady(plist) - 1;
                if (ready > 0) {
                    n = rand() % ready + 1;
                    newprio = rand()%8;
                    oldprio = rrpSetPrio(p,newprio);
                    printf("Processo %d, passa a ter prioridade %d, que anteriormente era %d\n",
                    pid,newprio,oldprio);
                }
            }
            printf("Bloqueado processo %d\n",pid);
        }
        else if (processGetStatus(p)==PROC_WAITING
                 && r < PROCESS_UNBLOCK_PROBABILITY) {
            processSetStatus(p,PROC_READY);
            printf("Desbloqueado processo %d\n",pid);
        }
    }
    printf("======================\n");
    return plist;
}

int main(void) {
    int i = 0, step = 0;
    char c = ' ';
    Process* plist = NULL, *p1 = NULL;

    srand(time(NULL));

    //Inicializar escalonadores de processos
    schedInitSchedInfo();
    rrpInitSchedInfo();

    //Criando primeiro processo...
    plist = _createProcess(plist, 1, 0);
    printf("\n");

    while (c != 'n') {
        switch (i) {
            case 0:
                printf("(Passo:%d)\n", step, i);
                plist = _doRandomThings(plist);
                processDump(plist,_dumpSchedParams);
                printf("\n");
                i++;
                break;
            case SCHED_ITERATIONS+1:
                printf("(Passo:%d/Iteracoes:%d)\n", step, i-1);
                processDump(plist,_dumpSchedParams);
                step++;
                i = 0;
                printf("\nContinuar (s/n)? ");
                fflush(stdout);
                c = getchar();
                printf("\n");
                break;
            default:
                p1 = schedSchedule(plist);
//              printf("(Passo:%d/Iteracao:%d) Processo escalonado PID %d.\n", step, i, processGetPid(p1));
//              processDump(plist,_dumpSchedParams);
//              printf("\n");
                i++;
        }
    }
    return 0;
}
