Escalonamento de Processos
==========================

Descrição do problema
---------------------

Dado um simulador de carga de trabalho para o subsistema de gerenciamento de processos de um sistema operacional hipotético, implemente o algoritmo Round Robin com Prioridades como um dos algoritmos suportados pelo simulador. Seu “RRPrio” deve ser implementado com 8 níveis de prioridade (0 - 7, 7 é a maior prioridade) e com a possibilidade de atribuição dinâmica de prioridades a processos.

Descrição dos arquivos
----------------------

O simulador encontra-se escrito em linguagem C e é composto pelos seguintes arquivos:

- `process.h`: Arquivo de cabeçalho para a declaração da API de manutenção de processos
- `process.c`: Arquivo que implementa a API de manutenção de processos
- `sched.h`: Arquivo de cabeçalho para a declaração da API de escalonamento de processos
- `sched.c`: Arquivo com a implementação do escalonamento de processos
- `rrprio.h`: Arquivo de cabeçalho para a declaração da API baseada no RRPrio Scheduling
- `rrprio.c`: Arquivo com a implementação do escalonamento baseado em RRPrio Scheduling
- `main.c`: Arquivo do programa principal do simulador

Restrições do trabalho
----------------------

Todo o seu desenvolvimento deve estar contido nos arquivos `rrprio.h` e `rrprio.c`, destacados acima. Você somente pode alterar esses arquivos e fazer uso apenas das APIs disponibilizadas nos arquivos de cabeçalho, além é claro, da API padrão C.

Toda a descrição das APIs está contida nos respectivos arquivos de cabeçalho.

As estruturas e funções a serem implementadas possuem explicação no arquivo de cabeçalho `rrprio.h`. São elas:

```C
typedef struct rrprio_params RRPrioParams;
void rrpInitSchedInfo(void);
void rrpInitSchedParams(Process *p, void *params);
Process* rrpSchedule(Process *plist);
int rrpReleaseParams(Process *p);
int rrpSetPrio(Process *p, int prio);
```

Quaisquer funções auxiliares não podem ser externalizadas no arquivo de cabeçalho rrprio.h. Identificadores, tipos, assinaturas de função, enfim, linhas já escritas de programa, não podem ser modificados.

Quaisquer funções necessárias e não implementadas na API de processo ou na API de escalonador devem ser discutidas e solicitadas ao professor por e-mail. O mesmo para possíveis erros detectados.
