/*numero de processos que teremos na nossa execução*/
#define NPROCESSOS 5

/*QUANTUM Período de tempo que os processos têm direito de executar
 na CPU cada vez que forem escalonados.*/
#define QUANTUM 1

/*tempo de requisição de IO*/
#define TIMEIO 1

/*tempo de bloqueio de cada IO*/
#define TIOFITA 3
#define TIODISCO 4
#define TIOIMPRESSORA 2

/*status dos processos para identificação de prioridade*/
#define NOVO 5
#define STATUSNOVO 1

#define PREENPTADO 3


#include <stdio.h>
#include <stdlib.h>
#include <time.h>


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

    int x, z, w;
    int probIO = 0;
    int probPPID = 0;


    typedef struct {
        int PID;
        int PPID;
        int status;
        int tempoChegada;
        int tserv;
        int tIOFita;
        int tIODisco;
        int tIOImpressora;
        int prioridade;
        int esperaIO;
    } ProcessoPCB;
    ProcessoPCB proce[NPROCESSOS];

    /*as filas do programa*/
    struct Queue Proc;
    struct Queue IO_impressora;
    struct Queue IO_fita;
    struct Queue IO_disco;
    struct Queue baixaPri;

    /*iniciando as filas*/
    init(&Proc);
    init(&IO_impressora);
    init(&IO_disco);
    init(&IO_fita);
    init(&baixaPri);

    /*criando os processos e seus PCBs*/
    for (int i = 0; i < NPROCESSOS; i++) {

        proce[i].PID = i;
        proce[i].PPID = 0;
        probPPID = (rand() % 100);
        if (i > 0 && probPPID >= 50) {
            proce[i].PPID = rand() % (i - 1);
        }
        if (i == 0) {
            proce[i].tempoChegada = 0;
        } else {
            proce[i].tempoChegada = ((proce[i - 1].tempoChegada) + (rand() % 10) + 1);
        }
        proce[i].status = STATUSNOVO;
        proce[i].prioridade = NOVO;
        proce[i].esperaIO = 0;
        proce[i].tserv = rand() % 10 + 5;
        probIO = (rand() % 12);

        if (probIO <= 2) {
            proce[i].tIOImpressora = 1 + (rand() % proce[i].tserv);
            proce[i].tIOFita = 0;
            proce[i].tIODisco = 0;

        }
        if (probIO > 2 && probIO <= 4) {
            proce[i].tIOImpressora = 0;
            proce[i].tIOFita = 1 + (rand() % proce[i].tserv);
            proce[i].tIODisco = 0;
        }
        if (probIO > 4 && probIO <= 6) {
            proce[i].tIOImpressora = 0;
            proce[i].tIOFita = 0;
            proce[i].tIODisco = 1 + (rand() % proce[i].tserv);
        }
        if (probIO > 6) {
            proce[i].tIOImpressora = 0;
            proce[i].tIOFita = 0;
            proce[i].tIODisco = 0;
        }
        enq(&Proc, proce[i].PID);
    }


    while (Proc.size || (IO_impressora.size) || (IO_disco.size) || (IO_fita.size) || (baixaPri.size)) {

        if (IO_impressora.size > 0) {
            int y;
            y = head(&IO_impressora);
            proce[y].esperaIO = proce[y].esperaIO - 1;
            if (proce[y].esperaIO == 0) {
                enq(&Proc, proce[y].PID);
                deq(&IO_impressora);
            }
        }
        if (IO_fita.size > 0) {
            int y;
            y = head(&IO_fita);
            proce[y].esperaIO = proce[y].esperaIO - 1;
            if (proce[y].esperaIO == 0) {
                enq(&Proc, proce[y].PID);
                deq(&IO_fita);
            }
        }
        if (IO_disco.size > 0) {
            int y;
            y = head(&IO_disco);
            proce[y].esperaIO = proce[y].esperaIO - 1;
            if (proce[y].esperaIO == 0) {
                enq(&baixaPri, proce[y].PID);
                deq(&IO_disco);
            }
        }

        if (Proc.size > 0) {

            x = head(&Proc);

            printf("PID %d\n", x);
            printf("PPID: %d\n", proce[x].PPID);
            printf("Tempo de chegada: %d\n", proce[x].tempoChegada);
            printf("Status: %d\n", proce[x].status);
            printf("Tempo de servico: %d\n", proce[x].tserv);
            printf("Tempo IO de fita: %d\n", proce[x].tIOFita);
            printf("Tempo IO de disco: %d\n", proce[x].tIODisco);
            printf("Tempo IO de impressora: %d\n", proce[x].tIOImpressora);
            printf("Prioridade: %d\n\n", proce[x].prioridade);

            if (proce[x].tserv == proce[x].tIOFita || proce[x].tserv == proce[x].tIOImpressora ||
                proce[x].tserv == proce[x].tIODisco) {

                if (proce[x].tserv == proce[x].tIOFita) {
                    proce[x].tserv = proce[x].tserv - (QUANTUM);
                    proce[x].esperaIO = TIOFITA;
                    enq(&IO_fita, proce[x].PID);
                    deq(&Proc);
                }
                if (proce[x].tserv == proce[x].tIOImpressora) {
                    proce[x].tserv = proce[x].tserv - (QUANTUM);
                    proce[x].esperaIO = TIOIMPRESSORA;
                    enq(&IO_impressora, proce[x].PID);
                    deq(&Proc);
                }
                if (proce[x].tserv == proce[x].tIODisco) {
                    proce[x].tserv = proce[x].tserv - (QUANTUM);
                    proce[x].esperaIO = TIODISCO;
                    enq(&IO_disco, proce[x].PID);
                    deq(&Proc);
                }
            }
            else {

                    proce[x].tserv = proce[x].tserv - (QUANTUM);
                    if(proce[x].tserv > 0) {
                        enq(&baixaPri,proce[x].PID);
                        deq(&Proc);
                    }
                    else {
                        deq(&Proc);
                    }
                }
            }
            if(Proc.size == 0 && &baixaPri>0) {
                int y = head(&baixaPri);
                enq(&Proc, proce[y].PID);
                deq(&baixaPri);
        }

    }
        return 0;

}