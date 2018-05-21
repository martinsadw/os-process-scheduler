/*
*  rrprio.c - Implementacao do algoritmo Round Robin com Prioridades
*
*  Autores: André Ferreira Martins
            Lohan Rodrigues Narcizo Ferreira
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

//Nome unico do algoritmo. Deve ter 4 caracteres.
const char rrprioName[]="RRPr";
int sched_slot = -1;

typedef struct process_node
{
    Process *p;
    struct process_node *next;
    int time_on;

}PNode;

//Cada elemento do vetor representa uma lista encadeada de processos daquela prioridade
PNode *perprio[8];
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
    RRPrioParams *prms = params;
    processSetSchedParams(p,params);
    processSetSchedSlot(p,sched_slot);

    //Cria primeiro node caso lista encadeada vazia
    if(perprio[prms->prio]==NULL)
        {
            perprio[prms->prio] = malloc(sizeof(PNode));
            perprio[prms->prio]->p = p;
            perprio[prms->prio]->next = NULL;
            perprio[prms->prio]->time_on = 0;
        }
    //Percorre lista encadeada ate o final e insere node
    else
        {
            PNode *aux = perprio[prms->prio];
            while(aux->next!=NULL)
            {
                aux = aux->next;
            }
            aux->next = malloc(sizeof(PNode));
            aux->next->p = p;
            aux->next->next = NULL;
            aux->next->time_on = 0;
        }

}

//Retorna o proximo processo a obter a CPU, conforme o algortimo RRPrio
//Para cada posi��o do vetor percorre todos os nodes verificando disponibilidade dos processos e tempo corrido
//Se n�o h� processos em uma posi��o ou todos est�o bloqueados, inicia ciclo numa prioridade abaixo
//Time_on verifica a quanto tempo o processo est� na CPU, sendo 25 o tempo limite
//Esse limite � zerado quando todos os elementos da lista receberem uma oportunidade de ir para CPU
Process* rrpSchedule(Process *plist) {
    int i,j;
    for(i=7; i>=0;i--)
        {
            PNode *aux = perprio[i];
            while(aux != NULL)
                {
                    if(processGetStatus(aux->p)== PROC_READY && aux->time_on < 25)
                      {
                         aux->time_on++;
                         return aux->p;
                      }
                    else if(aux->next==NULL)
                      {
                        PNode *aux2 = perprio[i];
                        aux = NULL;
                        while(aux2!=NULL)
                        {
                            aux2->time_on = 0;
                            if(processGetStatus(aux2->p)== PROC_READY)
                                aux = perprio[i];
                            aux2 = aux2->next;
                        }
                      }

                    else aux = aux->next;
                }
        }
    return NULL;
}

//Libera os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' desassociado do slot de RRPrio
//Remove PNode referente ao processo que est� sendo destruido
int rrpReleaseParams(Process *p) {

    RRPrioParams *prm = processGetSchedParams(p);
    PNode *aux = perprio[prm->prio];
    PNode *aux2;
    while(aux!= NULL)
    {
        if(aux->p == p)
        {
            aux2 = aux;
            perprio[prm->prio] = aux->next;
            break;
        }
        else if(aux->next->p == p)
        {
            aux2 = aux->next;
            aux->next = aux2->next;
            break;
        }

        aux = aux->next;
    }
    aux2->p = NULL;
    aux2->next = NULL;
    free(aux2);
    free(prm);
    return 0;
}

//Modifica a prioridade de um processo para um novo valor
//Retorna o valor da prioridade anteriormente atribuida ao processo
//Remove PNode referente ao processo com antiga prioridade e cria outro PNode com nova prioridade
int rrpSetPrio(Process *p, int prio) {
    RRPrioParams *params = processGetSchedParams(p);
    int oldprio = params->prio;
    rrpReleaseParams(p);
    params = malloc(sizeof(RRPrioParams));
    params->prio = prio;
    rrpInitSchedParams(p,params);
    return oldprio;
}
