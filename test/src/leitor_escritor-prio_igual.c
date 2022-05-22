/**
 **********************************************
 *       Leitor/Escritor - Prio Igual        **
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
#define NUM_READER 3
#define NUM_WRITER 2
#define NUM_THREADS (NUM_READER + NUM_WRITER)
#define QUEUE_SIZE 10
#define RANDOM_MAX 100
#define DEFAULT_OUTPUT stderr

// Struct que permite a concorrência entre threads do mesmo tipo
typedef struct lightswitch {
    sem_t *can_enter;
    int counter;
    sem_t *counter_lock;
} ls_t;

sem_t s_escritor_lock, s_thr_queue;
ls_t ls_leitores;

typedef struct queue_val_t {
    struct queue_val_t *prev;
    struct queue_val_t *next;
    int val;
} queue_val_t;

typedef struct thread_args {
    int id;
    queue_val_t **queue;
} thread_args;

void enter(ls_t *ls) {
    sem_wait(ls->counter_lock);
    /* First thread waits to be able to enter */
    if (++(ls->counter) == 1) {
        sem_wait(ls->can_enter);
    }
    sem_post(ls->counter_lock);
}

void leave(ls_t *ls) {
    sem_wait(ls->counter_lock);
    /* Last thread waits to be able to enter */
    if (--(ls->counter) == 0) {
        sem_post(ls->can_enter);
    }
    sem_post(ls->counter_lock);
}

void print_queue_med(int thread_id, queue_val_t **queue) {
    int sum = (*queue)->val, num = 1;
    queue_val_t *it;

    char queue_out[100];
    sprintf(queue_out, "%2d", (*queue)->val);

    for (it = (*queue)->next; it != *queue; it = it->next) {
        char val[5];
        sprintf(val, " %2d", it->val);
        strcat(queue_out, val);
        sum += it->val;
        num += 1;

#ifdef DEBUG
        fprintf(DEFAULT_OUTPUT, "Leitor %d somando %s\n", thread_id, val);
        fflush(DEFAULT_OUTPUT);
#endif
    }

    fprintf(DEFAULT_OUTPUT, "Leitor %d --> [ %s ] - med = %2f\n", thread_id, queue_out, (double)sum / num);
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

void queue_write(int thread_id, queue_val_t **queue) {
    queue_val_t *old = *queue;
    queue_remove((queue_t **)queue, (queue_t *)old);

    queue_val_t *new = create_random_elem(RANDOM_MAX);
    queue_append((queue_t **)queue, (queue_t *)new);

    fprintf(DEFAULT_OUTPUT, "Escritor %d --> removeu: %d, escreveu %d\n", thread_id, old->val, new->val);
    fflush(DEFAULT_OUTPUT);

    free(old);
}

void *escritor_body(void *t_args) {
    thread_args *args = t_args;
    int thread_id = args->id;
    queue_val_t **queue = args->queue;

    while (1) {
        sem_wait(&s_thr_queue);
        sem_wait(&s_escritor_lock);
        sem_post(&s_thr_queue);

        // Início seção crítica
        queue_write(thread_id, queue);
        // Fim seção crítica

        sem_post(&s_escritor_lock);
    }
}

void *leitor_body(void *t_args) {
    thread_args *args = t_args;
    int thread_id = args->id;
    queue_val_t **queue = args->queue;

    while (1) {
        sem_wait(&s_thr_queue);
        enter(&ls_leitores);
        sem_post(&s_thr_queue);

        // Início seção crítica
        print_queue_med(thread_id, queue);
        // Fim seção crítica

        leave(&ls_leitores);
    }
}

void init_lightswitch(ls_t *ls, sem_t *sem) {
    ls->can_enter = sem;
    ls->counter_lock = malloc(sizeof(sem_t));
    ls->counter = 0;

    sem_init(ls->can_enter, 0, 1);
    sem_init(ls->counter_lock, 0, 1);
}

void execute() {
    pthread_t thread[NUM_THREADS];
    pthread_attr_t attr;
    int i, status;

    init_lightswitch(&ls_leitores, &s_escritor_lock);
    sem_init(&s_thr_queue, 0, 1);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    queue_val_t *queue = create_random_queue();
    // create threads
    for (i = 0; i < NUM_WRITER; i++) {

        thread_args *args = malloc(sizeof(thread_args));
        args->id = i;
        args->queue = &queue;

        status = pthread_create(&thread[i], &attr, escritor_body, (void *)args);
        if (status) {
            exit(1);
        }
    }

    for (i = 0; i < NUM_READER; i++) {

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
