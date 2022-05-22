/**
 **********************************************
 *    Filosofos - Sem controle de impasse    **
 *                                           **
 * Autores: Ricky Lemes Habegger &           **
 *          Adriano José Paulichi            **
 *                                           **
 **********************************************
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_FILO 5
#define TEMPO_MEDITAR 2000
#define TEMPO_COMER 2000

sem_t hashi[NUM_FILO];
pthread_mutex_t mut;
int numRefeicoes = 0;

typedef struct thread_args {
    int id;
} thread_args;

void meditar(int id);
void comer(int id);

int mssleep(long miliseconds) {
    struct timespec rem;
    struct timespec req = {
        (int)(miliseconds / 1000), /* secs (Must be Non-Negative) */
        (miliseconds % 1000) *
            1000000 /* nano (Must be in range of 0 to 999999999) */
    };

    return nanosleep(&req, &rem);
}

void *filosofo_body(void *t_args) {
    thread_args *args = t_args;
    int thread_id = args->id;

    int dir = thread_id;
    int esq = (thread_id + 1) % NUM_FILO;

    while (1) {
        meditar(thread_id);
        sem_wait(&hashi[dir]);
        sem_wait(&hashi[esq]);
        comer(thread_id);
        sem_post(&hashi[dir]);
        sem_post(&hashi[esq]);
    }
}

void execute() {
    pthread_t thread[NUM_FILO];
    pthread_attr_t attr;
    int i, status;

    for (i = 0; i < NUM_FILO; i++) {
        sem_init(&hashi[i], 0, 1);
    }
    pthread_mutex_init(&mut, NULL);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // create threads
    for (i = 0; i < NUM_FILO; i++) {

        thread_args *args = malloc(sizeof(thread_args));
        args->id = i;

        status = pthread_create(&thread[i], &attr, filosofo_body, (void *)args);
        if (status) {
            exit(1);
        }
    }

    while (1) {
        sleep(1);
        pthread_mutex_lock(&mut);
        printf("Refeições por segundo: %d\n", numRefeicoes);
        numRefeicoes = 0;
        pthread_mutex_unlock(&mut);
    }

    pthread_attr_destroy(&attr);
    pthread_exit(NULL);
}

void meditar(int id) {
    int time = rand() % TEMPO_MEDITAR;
    mssleep(time);
    printf("Filosofo %d meditou em %d ms\n", id, time);
}

void comer(int id) {
    int time = rand() % TEMPO_COMER;
    mssleep(time);

    pthread_mutex_lock(&mut);
    numRefeicoes++;
    pthread_mutex_unlock(&mut);

    printf("Filosofo %d comeu em %d ms\n", id, time);
}

int main() {
    srand(time(NULL));
    execute();
}
