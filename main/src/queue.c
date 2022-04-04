#include <queue.h>
#include <stdio.h>
#include <stdlib.h>

int queue_size(queue_t *queue) {
    if (queue == NULL) return 0;
    int size = 1;

    queue_t *first = queue, *it;
    for (it = queue->next; it != first; it = it->next) {
        size++;
    }
    return size;
}

void queue_print(char *name, queue_t *queue, void print_elem(void *)) {
    queue_t *first = queue, *it;
    printf("%s: [", name);

    if (queue != NULL) {
        print_elem(first);
        for (it = queue->next; it != first; it = it->next) {
            printf(" ");
            print_elem(it);
        }
    }
    printf("]\n");
}

int queue_append(queue_t **queue, queue_t *elem) {
    if (queue == NULL) {
        fprintf(stderr, "%s", "A fila não existe!\n");
        return -1;
    }
    if (elem == NULL) {
        fprintf(stderr, "%s", "O elemento não existe!\n");
        return -2;
    }
    if (elem->prev != NULL || elem->next != NULL) {
        fprintf(stderr, "%s", "O elemento pertence a outra lista.\n");
        return -3;
    }
    if (*queue == NULL) {
        *queue = elem;
        elem->next = *queue;
        elem->prev = *queue;
    } else {
        elem->next = *queue;
        elem->prev = (*queue)->prev;
        (*queue)->prev->next = elem;
        (*queue)->prev = elem;
    }
    return 0;
}

void remove_elem(queue_t *elem) {
    if (elem->next != elem) {  // se a fila tem mais de 1 elemento
        elem->prev->next = elem->next;
        elem->next->prev = elem->prev;
    }
    elem->prev = NULL;
    elem->next = NULL;
}

int queue_remove(queue_t **queue, queue_t *elem) {
    if (queue == NULL) {
        fprintf(stderr, "%s", "A fila não existe!\n");
        return -1;
    }
    if ((*queue) == NULL) {
        fprintf(stderr, "%s", "A fila está vazia!\n");
        return -2;
    }
    if (elem == NULL) {
        fprintf(stderr, "%s", "O elemento não existe!\n");
        return -3;
    }

    queue_t *first = *queue, *it;
    if (elem == first) {
        if (elem->next == elem) {
            *queue = NULL;
        } else {
            *queue = elem->next;
        }
        remove_elem(elem);
        return 0;
    }

    for (it = first->next; it != first; it = it->next) {
        if (it == elem) {
            remove_elem(elem);
            return 0;
        }
    }

    fprintf(stderr, "%s", "O elemento não está na fila!\n");
    return -4;
}
