/**
 **********************************************
 * Espera Ocupada Alternancy implementation  **
 *                                           **
 * Autores: Ricky Lemes Habegger &           **
 *          Adriano José Paulichi            **
 *                                           **
 **********************************************
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Internal libs
#include <queue.h>

#define NUM_THREADS 2

int turn = 0;

void enter_task(int id) {
    while (turn != id) {
    } // busy waiting
}

void leave_task() {
    turn = (turn + 1) % NUM_THREADS;
}

typedef struct queue_val_t {
    struct queue_val_t *prev;
    struct queue_val_t *next;
    int val;
} queue_val_t;

typedef struct thread_args {
    int id;
    queue_val_t **queue;
} thread_args;

queue_val_t *create_random_elem() {
    queue_val_t *elem = malloc(sizeof(queue_val_t));
    elem->val = rand() % 100;
    elem->prev = NULL;
    elem->next = NULL;
    return elem;
}

void *threadBody(void *t_args) {

    thread_args *args = t_args;
    int thread_id = args->id;
    queue_val_t **queue = args->queue;

    while (1) {
        enter_task(thread_id);
        queue_val_t *old = *queue;
        queue_remove((queue_t **)queue, (queue_t *)old);

        queue_val_t *new = create_random_elem();
        queue_append((queue_t **)queue, (queue_t *)new);

        char out[100];
        sprintf(out, "thread %d: tira %2d,  põe: %2d,  fila: %2d", thread_id,
                old->val, new->val, (*queue)->val);
        queue_val_t *it;
        for (it = (*queue)->next; it != *queue; it = it->next) {
            char val[5];
            sprintf(val, " %2d", it->val);
            strcat(out, val);
        }

        fprintf(stdout, "%s\n", out);
        free(old);
        leave_task();
    }

    pthread_exit(NULL);
}

queue_val_t *create_random_queue() {
    queue_val_t *queue = NULL;
    for (int i = 0; i < 10; i++) {
        queue_append((queue_t **)&queue, (queue_t *)create_random_elem());
    }
    return queue;
}

void execute_alternancy() {
    pthread_t thread[NUM_THREADS];
    pthread_attr_t attr;
    int i, status;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    queue_val_t *queue = create_random_queue();

    // create threads
    for (i = 0; i < NUM_THREADS; i++) {

        thread_args *args = malloc(sizeof(thread_args));
        args->id = i;
        args->queue = &queue;

        status = pthread_create(&thread[i], &attr, threadBody, (void *)args);
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
    execute_alternancy();
}
