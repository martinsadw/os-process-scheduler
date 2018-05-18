/*
*  rrprio.c - Implementacao do algoritmo Round Robin com Prioridades
*
*  Autores: SUPER_PROGRAMADORES_C
*  Projeto: Trabalho Pratico I - Sistemas Operacionais
*  Organizacao: Universidade Federal de Juiz de Fora
*  Departamento: Dep. Ciencia da Computacao
*
*
*/

#include "rrprio.h"
#include <stdio.h>
#include <string.h>

//Nome unico do algoritmo. Deve ter 4 caracteres.
const char rrprioName[]="RRPr";
int sched_slot = -1;
//=====Funcoes Auxiliares=====






//=====Funcoes da API=====

//Funcao chamada pela inicializacao do S.O. para a incializacao do escalonador
//conforme o algoritmo Round Robin com Prioridades
//Deve envolver a inicializacao de possiveis parametros gerais
//Deve envolver o registro do algoritmo junto ao escalonador
void rrpInitSchedInfo() {
    SchedInfo *sched = malloc(sizeof(SchedInfo));
    strcpy(sched->name,rrprioName);
	sched->initParamsFn = rrpInitSchedParams;
	sched->scheduleFn = rrpSchedule;
	sched->releaseParamsFn = rrpReleaseParams;
	sched_slot = schedRegisterScheduler(sched);
}

//Inicializa os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' associado ao slot de RRPrio
void rrpInitSchedParams(Process *p, void *params) {
    processSetSchedParams(p,params);
    processSetSchedSlot(p,sched_slot);
}

//Retorna o proximo processo a obter a CPU, conforme o algortimo RRPrio
Process* rrpSchedule(Process *plist) {
    //...
    return NULL;
}

//Libera os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' desassociado do slot de RRPrio
int rrpReleaseParams(Process *p) {
    free(processGetSchedParams(p));
    return 0;
}

//Modifica a prioridade de um processo para um novo valor
//Retorna o valor da prioridade anteriormente atribuida ao processo
int rrpSetPrio(Process *p, int prio) {
    RRPrioParams *params = processGetSchedParams(p);
    int oldprio = params->prio;
    params->prio = prio;
    return oldprio;
}
