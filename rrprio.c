/*
*  rrprio.c - Implementacao do algoritmo Round Robin com Prioridades
*
*  Autores: André Ferreira Martins
*           Lohan Rodrigues Narcizo Ferreira
*           Lucas Carvalho Ribeiro
*  Projeto: Trabalho Pratico I - Sistemas Operacionais
*  Organizacao: Universidade Federal de Juiz de Fora
*  Departamento: Dep. Ciencia da Computacao
*
*
*/

#include "rrprio.h"
#include <stdio.h>
#include <string.h>

#define NUM_ITERATIONS_PER_PROCESS 25

//Nome unico do algoritmo. Deve ter 4 caracteres.
const char rrprioName[]="RRPr";
int rrprioSchedSlot = -1;

//=====Funcoes Auxiliares=====

typedef struct process_node
{
    Process *process;
    struct process_node *next;
} ProcessNode;

typedef struct process_queue
{
    ProcessNode *first;
    ProcessNode *last;
} ProcessQueue;

// Cada elemento do vetor representa uma fila de processos daquela prioridade
ProcessQueue priorityQueues[8];
int iterationsOnCPU = 0;
Process *currentProcess = NULL;

void enqueueProcessNode(ProcessQueue *processQueue, ProcessNode *processNode) {
    if (processQueue->first==NULL) {
        processQueue->first = processNode;
        processQueue->last = processNode;
    }
    else {
        processQueue->last->next = processNode;
        processQueue->last = processNode;
    }
}

void enqueueProcess(ProcessQueue *processQueue, Process *process) {
    ProcessNode *processNode = (ProcessNode*)malloc(sizeof(ProcessNode));
    processNode->process = process;
    processNode->next = NULL;

    enqueueProcessNode(processQueue, processNode);
}

ProcessNode* dequeueProcessNode(ProcessQueue *processQueue) {
    ProcessNode *processNode = processQueue->first;

    if (processNode!=NULL) {
        processQueue->first = processNode->next;
        processNode->next = NULL;

        if (processNode==processQueue->last) {
            processQueue->last = NULL;
        }
    }

    return processNode;
}

ProcessNode* requeueProcessNode(ProcessQueue *processQueue) {
    ProcessNode *processNode = dequeueProcessNode(processQueue);
    enqueueProcessNode(processQueue, processNode);

    return processNode;
}

// Esta operação quebra as regras de pilha
void removeProcess(ProcessQueue *processQueue, Process *process) {
    ProcessNode *processNodeIt = processQueue->first;
    ProcessNode *prevProcessNodeIt = NULL;

    while(processNodeIt!=NULL) {
        if (processNodeIt->process==process)
            break;

        prevProcessNodeIt = processNodeIt;
        processNodeIt = processNodeIt->next;
    }

    if (processNodeIt!=NULL) {
        if (processNodeIt==processQueue->first) {
            processQueue->first = processNodeIt->next;
        }
        else
        {
            prevProcessNodeIt->next = processNodeIt->next;
        }

        if (processNodeIt->next == NULL)
            processQueue->last = prevProcessNodeIt;

        if (processNodeIt->process == currentProcess)
            currentProcess = NULL;

        free(processNodeIt);
    }
}

void printQueue() {
    int i;
    for (i=0; i<8; ++i) {
        ProcessNode *processNodeIt = priorityQueues[i].first;
        printf("Prioridade %d: ", i);
        while (processNodeIt!=NULL) {
            printf("%d, ", processGetPid(processNodeIt->process));
            processNodeIt = processNodeIt->next;
        }
        printf("\n");
    }
}

//=====Funcoes da API=====

//Funcao chamada pela inicializacao do S.O. para a incializacao do escalonador
//conforme o algoritmo Round Robin com Prioridades
//Deve envolver a inicializacao de possiveis parametros gerais
//Deve envolver o registro do algoritmo junto ao escalonador
void rrpInitSchedInfo() {
    SchedInfo *sched = (SchedInfo*)malloc(sizeof(SchedInfo));
    strcpy(sched->name, rrprioName);
    sched->initParamsFn = rrpInitSchedParams;
    sched->scheduleFn = rrpSchedule;
    sched->releaseParamsFn = rrpReleaseParams;
    rrprioSchedSlot = schedRegisterScheduler(sched);
}

//Inicializa os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' associado ao slot de RRPrio
void rrpInitSchedParams(Process *p, void *params) {
    RRPrioParams *rrprioParams = (RRPrioParams*)params;
    processSetSchedParams(p, params);
    processSetSchedSlot(p, rrprioSchedSlot);
    enqueueProcess(priorityQueues + rrprioParams->prio, p);
}

//Retorna o proximo processo a obter a CPU, conforme o algortimo RRPrio
//Se o processo atual não tiver excedido o numero de iterações para cada processo, retorna o processo atual
//Do contrario, para cada nivel de prioridade reenfilera os processos até encontrar um processo pronto
//Se nao ha processos em uma prioridade ou todos estao bloqueados, inicia o ciclo em uma prioridade abaixo
Process* rrpSchedule(Process *plist) {
    if (currentProcess != NULL &&
        iterationsOnCPU < NUM_ITERATIONS_PER_PROCESS &&
        processGetStatus(currentProcess)==PROC_READY) {

        ++iterationsOnCPU;
        return currentProcess;
    }
    iterationsOnCPU = 1;

    int i;
    for(i=7; i>=0; --i) {
        if (priorityQueues[i].first==NULL) continue;

        ProcessNode *last = priorityQueues[i].last;

        ProcessNode *processNodeIt = requeueProcessNode(priorityQueues + i);

        int foundProcessReady = 1;
        while(processGetStatus(processNodeIt->process)!=PROC_READY) {
            if (processNodeIt==last) {
                foundProcessReady = 0;
                break;
            }
            processNodeIt = requeueProcessNode(priorityQueues + i);
        }

        if (foundProcessReady) {
            currentProcess = processNodeIt->process;
            return currentProcess;
        }
    }
    return NULL;
}

//Libera os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' desassociado do slot de RRPrio
//Remove o ProcessNode referente ao processo que esta sendo destruido
int rrpReleaseParams(Process *p) {
    RRPrioParams *rrprioParams = (RRPrioParams*)processGetSchedParams(p);
    removeProcess(priorityQueues + rrprioParams->prio, p);
    free(rrprioParams);
    return 0;
}

//Modifica a prioridade de um processo para um novo valor
//Retorna o valor da prioridade anteriormente atribuida ao processo
//Remove o processo da fila da prioridade antiga e insere na fila da nova prioridade
int rrpSetPrio(Process *p, int prio) {
    RRPrioParams *rrprioParams = (RRPrioParams*)processGetSchedParams(p);
    int oldprio = rrprioParams->prio;

    if (prio!=oldprio) {
        rrpReleaseParams(p);
        rrprioParams = (RRPrioParams*)malloc(sizeof(RRPrioParams));
        rrprioParams->prio = prio;
        rrpInitSchedParams(p,rrprioParams);
    }

    return oldprio;
}
