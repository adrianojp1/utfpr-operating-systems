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
int num_elem = 0;

sem_t s_buffer, s_item, s_vaga;

typedef struct queue_val_t {
    struct queue_val_t *prev;
    struct queue_val_t *next;
    int val;
} queue_val_t;

typedef struct thread_args {
    int id;
    queue_val_t **queue;
} thread_args;

queue_val_t *create_elem(int val) {
    queue_val_t *elem = malloc(sizeof(queue_val_t));
    elem->val = val;
    elem->prev = NULL;
    elem->next = NULL;
    return elem;
}

void *produtor_body(void *t_args) {
    thread_args *args = t_args;
    int thread_id = args->id;
    queue_val_t **buffer = args->queue;

    while (1) {
        // sleep(rand() % 4);
        sem_wait(&s_vaga);
        sem_wait(&s_buffer);
        int item = rand() % 1000;
        queue_val_t *elem = create_elem(item);
        queue_append((queue_t **)buffer, (queue_t *)elem);
        num_elem += 1;
        printf("p%d produziu %d\n", thread_id, item);
        printf("Numero de elementos: %d\n", num_elem);
        sem_post(&s_buffer);
        sem_post(&s_item);
    }
}

void *consumidor_body(void *t_args) {
    thread_args *args = t_args;
    int thread_id = args->id;
    queue_val_t **buffer = args->queue;

    while (1) {
        sem_wait(&s_item);
        sem_wait(&s_buffer);
        queue_val_t *old = *buffer;
        queue_remove((queue_t **)buffer, (queue_t *)old);
        num_elem -= 1;
        int item = old->val;
        free(old);
        printf("                   c%d consumiu %d\n", thread_id, item);
        printf("Numero de elementos: %d\n", num_elem);
        sem_post(&s_buffer);
        sem_post(&s_vaga);
        // sleep(rand() % 4);
    }
}

void execute() {
    pthread_t thread[NUM_THREADS];
    pthread_attr_t attr;
    int i, status;

    sem_init(&s_buffer, 0, 1);
    sem_init(&s_vaga, 0, 5);
    sem_init(&s_item, 0, 0);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    queue_val_t *buffer = NULL;

    // create threads
    for (i = 0; i < 3; i++) {

        thread_args *args = malloc(sizeof(thread_args));
        args->id = i;
        args->queue = &buffer;

        status = pthread_create(&thread[i], &attr, produtor_body, (void *)args);
        if (status) {
            exit(1);
        }
    }

    for (i = 0; i < 2; i++) {

        thread_args *args = malloc(sizeof(thread_args));
        args->id = i;
        args->queue = &buffer;

        status =
            pthread_create(&thread[i], &attr, consumidor_body, (void *)args);
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
