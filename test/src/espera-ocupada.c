/**
 **********************************************
 *         PPOS Core implementation          **
 *                                           **
 * Autores: Ricky Lemes Habegger &           **
 *          Adriano José Paulichi            **
 *                                           **
 **********************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <queue.h>
#include <time.h>

#define NUM_THREADS 2

typedef struct queue_val_t {
    struct queue_val_t *prev;
    struct queue_val_t *next;
    int val;
} queue_val_t;

typedef struct thread_args {
    int id;
    queue_val_t *queue;
} thread_args;

queue_val_t *get_random_elem() {
    queue_val_t *elem = malloc(sizeof elem);
    elem->val = rand() % 100;;
    elem->prev = NULL;
    elem->next = NULL;
    return elem;
}

void print_elem(void *p_elem) {
    queue_val_t *elem = p_elem;
    if (!elem)
        return;

    if (elem->val < 10) {
        printf(" ");
    }
    printf("%d", elem->val);
}

void *threadBody(void *t_args) {
    thread_args *args = t_args;
    int thread_id = args->id;
    queue_val_t *queue = args->queue;

    while (1) {
        queue_val_t *old = queue;
        queue_remove((queue_t **)&queue, (queue_t *)queue);

        queue_val_t *new = get_random_elem();
        queue_append((queue_t **)&queue, (queue_t *)new);

        // thread 0: tira 34,  põe 81,  fila: 47  2 19 66 32 60  9 11 38 81
        printf("thread %d: tira %d,  põe: %d,  fila: \n", thread_id, old->val,
               new->val);
        queue_print("", (queue_t *)queue, print_elem);

        free(old);
    }

    pthread_exit(NULL);
}

queue_val_t *create_random_queue(int size) {
    queue_val_t *queue;
    for (int i = 0; i < size; i++) {
        queue_append((queue_t **)&queue, (queue_t *)get_random_elem());
    }
    return queue;
}

void destroy_queue(queue_val_t *queue) {
    while (queue) {
        queue_val_t *removed = queue;
        queue_remove((queue_t **)&queue, (queue_t *)removed);
        free(removed);
    }
}

void execute_no_sync() {
    pthread_t thread[NUM_THREADS];
    pthread_attr_t attr;
    int i, status;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    queue_val_t *queue = create_random_queue(10);

    // create threads
    for (i = 0; i < NUM_THREADS; i++) {

        thread_args args;
        args.id = i;
        args.queue = queue;

        status = pthread_create(&thread[i], &attr, threadBody, (void *)&args);
        if (status) {
            fprintf(stderr, "pthread_create on thread %d", i);
            exit(1);
        }
    }

    // wait all threads to finish
    for (i = 0; i < NUM_THREADS; i++) {
        status = pthread_join(thread[i], NULL);
        if (status) {
            fprintf(stderr, "pthread_join on thread %d", i);
            exit(1);
        }
    }

    destroy_queue(queue);
    
    pthread_attr_destroy(&attr);
    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));
    execute_no_sync();
}