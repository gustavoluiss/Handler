/*numero de processos que teremos na nossa execução*/
#define NPROCESSOS 5

/*QUANTUM Período de tempo que os processos têm direito de executar
 na CPU cada vez que forem escalonados.*/
#define QUANTUM 1

/*tempo de troca - tempo necessário para trocar o processo executando
 na CPU pelo processo escolhido pelo algoritmo de escalonamento, ou seja, tempo de troca de contexto*/
#define TEMPOTROCA 1

/*tempo de requisição de IO*/
#define TIMEIO 1

/*tempo de bloqueio de cada IO*/
#define FITA 3
#define DISCO 4
#define IMPRESSORA 2

/*status dos processos para identificação de prioridade*/
#define NOVO 5
#define IO 2
#define PREENPTADO 3
#define FINALIZADO 4

#include <stdio.h>
#include <stdlib.h>


struct Node {
    int PID;
    struct Node *next;
};

struct Queue {
    struct Node *head;
    struct Node *tail;
    unsigned int size;
};

void init(struct Queue *q) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

int head(struct Queue *q) {
    return q->head->PID;
}

void deq(struct Queue *q) {
    q->size--;

    struct Node *tmp = q->head;
    q->head = q->head->next;
    free(tmp);
}

void enq(struct Queue *q, int PID) {
    q->size++;

    if (q->head == NULL) {
        q->head = (struct Node *) malloc(sizeof(struct Node));
        q->head->PID = PID;
        q->head->next = NULL;
        q->tail = q->head;
    } else {
        q->tail->next = (struct Node *) malloc(sizeof(struct Node));
        q->tail->next->PID = PID;
        q->tail->next->next = NULL;
        q->tail = q->tail->next;
    }
}

int main(void) {

    int x;
    int probIO = 0;
    int aconteceIO = 0;

    typedef struct {
        int PID;
        int PPID;
        int status;
        int start;
        int tserv;
        int tIOFita;
        int tIODisco;
        int tIOImpressora;
        int prioridade;
    }ProcessoPCB;
    ProcessoPCB proce[NPROCESSOS];

    /*as filas do programa*/
    struct Queue Proc;
    struct Queue IO_impressora;
    struct Queue IO_fita;
    struct Queue IO_disco;
    struct Queue altaPri;
    struct Queue baixaPri;

    /*iniciando as filas*/
    init(&Proc);
    init(&IO_impressora);
    init(&IO_disco);
    init(&IO_fita);
    init(&altaPri);
    init(&baixaPri);

    /*criando os processos e seus PCBs*/
    for(int i = 0; i < NPROCESSOS ; i++){
        proce[i].PID = i;
        /*switch (proce[i].PID){
            case(0):
                proce[i].PPID = 0;
                break;
            case(1):
                proce[i].PPID = 0;
                break;
            case(2):
                proce[i].PPID = 1;
                break;
            case(3):
                proce[i].PPID = 0;
                break;
        }*/
        proce[i].status = NOVO;
        proce[i].start = i;
        proce[i].tserv = rand() % 10 + 5;

        probIO = (rand() % 12);
        if (probIO <= 2) {
            proce[i].tIOImpressora = rand() % 5 + 1;;
            proce[i].tIOFita = 0;
            proce[i].tIODisco = 0;

        }
        if(probIO > 2 && probIO <=4) {
            proce[i].tIOImpressora = 0;
            proce[i].tIOFita = rand() % 5 + 1;;
            proce[i].tIODisco = 0;
        }
        if(probIO > 4 && probIO <=6) {
            proce[i].tIOImpressora = 0;
            proce[i].tIOFita = 0;
            proce[i].tIODisco = rand() % 5 + 1;;
        }
        if(probIO>6){
            proce[i].tIOImpressora = 0;
            proce[i].tIOFita = 0;
            proce[i].tIODisco = 0;
        }

        proce[i].prioridade = NOVO;
        enq(&Proc, proce[i].PID);
    }

    while(Proc.size) {
        x = head(&Proc);
        printf("PID %d\n", x);
        printf("PID %d\n",x);

        printf("PPID: %d\n", proce[x].PPID);
        printf("Status: %d\n", proce[x].status);
        printf("Start: %d\n", proce[x].start);
        printf("Tempo de servico: %d\n", proce[x].tserv);
        printf("Tempo IO de fita: %d\n", proce[x].tIOFita);
        printf("Tempo IO de disco: %d\n", proce[x].tIODisco);
        printf("Tempo IO de impressora: %d\n", proce[x].tIOImpressora);
        printf("Prioridade: %d\n\n", proce[x].prioridade);
            deq(&Proc);
    }

    return 0;
}