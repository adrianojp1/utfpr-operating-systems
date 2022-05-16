/**
 **********************************************
 *        Produtor/Consumidor POSIX          **
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
#include <string.h>
#include <time.h>
#include <unistd.h>

// Internal libs
#include <queue.h>
#define NUM_THREADS 5
#define QUEUE_SIZE 10
#define RANDOM_MAX 100
#define DEFAULT_OUTPUT stderr

int num_elem = 0;

sem_t s_buffer, s_item, s_vaga;
sem_t s_leitores_ativos, s_escrita;
int leitores_ativos = 0;

typedef struct queue_val_t {
    struct queue_val_t *prev;
    struct queue_val_t *next;
    int val;
} queue_val_t;

typedef struct thread_args {
    int id;
    queue_val_t **queue;
} thread_args;

void print_queue_med(int thread_id, queue_val_t **queue) {
    char out[100];
    sprintf(out, "Leitor %d: [", thread_id);
    int sum = (*queue)->val, num = 1;
    queue_val_t *it;

    char val[5];
    sprintf(val, " %2d", (*queue)->val);
    strcat(out, val);

    for (it = (*queue)->next; it != *queue; it = it->next) {
        char val[5];
        sprintf(val, " %2d", it->val);
        strcat(out, val);
        sum += it->val;
        num += 1;
    }
    char med[50];
    sprintf(med, "] -> med = %2f", (double)sum / num);
    strcat(out, med);
    fprintf(DEFAULT_OUTPUT, "%s\n", out);
    fflush(DEFAULT_OUTPUT);
}

queue_val_t *create_elem(int val) {
    queue_val_t *elem = malloc(sizeof(queue_val_t));
    elem->val = val;
    elem->prev = NULL;
    elem->next = NULL;
    return elem;
}

queue_val_t *create_random_elem(int range) {
    return create_elem(rand() % range);
}

queue_val_t *create_random_queue() {
    queue_val_t *queue = NULL;
    for (int i = 0; i < QUEUE_SIZE; i++) {
        queue_append((queue_t **)&queue,
                     (queue_t *)create_random_elem(RANDOM_MAX));
    }
    return queue;
}

void *escritor_body(void *t_args) {
    thread_args *args = t_args;
    int thread_id = args->id;
    queue_val_t **queue = args->queue;

    while (1) {
        sem_wait(&s_escrita);

        queue_val_t *old = *queue;
        queue_remove((queue_t **)queue, (queue_t *)old);

        queue_val_t *new = create_random_elem(RANDOM_MAX);
        queue_append((queue_t **)queue, (queue_t *)new);

        char str[100];
        sprintf(str, "e%d --> removeu: %d, escreveu %d\n", thread_id, old->val,
                new->val);
        fprintf(DEFAULT_OUTPUT, "%s", str);
        fflush(DEFAULT_OUTPUT);
        free(old);
        sem_post(&s_escrita);
    }
}

void *leitor_body(void *t_args) {
    thread_args *args = t_args;
    int thread_id = args->id;
    queue_val_t **queue = args->queue;

    while (1) {
        sem_wait(&s_leitores_ativos);
        leitores_ativos++;
        if (leitores_ativos == 1) {
            sem_wait(&s_escrita);
        }
        sem_post(&s_leitores_ativos);

        print_queue_med(thread_id, queue);

        sem_wait(&s_leitores_ativos);
        leitores_ativos--;
        if (leitores_ativos == 0) {
            sem_post(&s_escrita);
        }
        sem_post(&s_leitores_ativos);
    }
}

void execute() {
    pthread_t thread[NUM_THREADS];
    pthread_attr_t attr;
    int i, status;

    sem_init(&s_leitores_ativos, 0, 1);
    sem_init(&s_escrita, 0, 1);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    queue_val_t *queue = create_random_queue();
    // create threads
    for (i = 0; i < 2; i++) {

        thread_args *args = malloc(sizeof(thread_args));
        args->id = i;
        args->queue = &queue;

        status = pthread_create(&thread[i], &attr, escritor_body, (void *)args);
        if (status) {
            exit(1);
        }
    }

    for (i = 0; i < 3; i++) {

        thread_args *args = malloc(sizeof(thread_args));
        args->id = i;
        args->queue = &queue;

        status = pthread_create(&thread[i], &attr, leitor_body, (void *)args);
        if (status) {
            exit(1);
        }
    }

    // wait all threads to finish
    for (i = 0; i < NUM_THREADS; i++) {
        status = pthread_join(thread[i], NULL);
        if (status) {
            exit(1);
        }
    }

    pthread_attr_destroy(&attr);
    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));
    execute();
}
