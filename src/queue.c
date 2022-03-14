#include <queue.h>
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <testafila.h>

int queue_size (queue_t *queue) {
    if (queue == NULL) return 0;
    int size = 1;

    queue_t* first = queue, *it;
    for (it = queue->next; it != first; it = it->next) {
        size++;
    }
    return size;
}


void queue_print (char *name, queue_t *queue, void print_elem (void*)) {
    queue_t* first = queue, *it;
    if (queue == NULL) return;

    print_elem(first);
    for (it = queue->next; it != first; it = it->next) {
        print_elem(it);
    }
    printf("end print\n");
}

int queue_append (queue_t **queue, queue_t *elem) {
    if(queue == NULL) {
        fprintf(stderr, "%s", "A fila não existe!\n");
        return -1;
    }
    if(elem == NULL) {
        fprintf(stderr, "%s", "O elemento não existe!\n");
        return -2;
    }
    if(elem->prev != NULL || elem->next != NULL) {
        fprintf(stderr, "%s", "O elemento pertence a outra lista.\n");
        return -3;
    }
    if(*queue == NULL) {
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

void remove_itself (queue_t** queue, queue_t* elem) {
    if(queue_size(*queue) == 1){
        *queue = NULL;
    } else {
        elem->prev->next = elem->next;
        elem->next->prev = elem->prev;
    }

    elem->prev = NULL;
    elem->next = NULL;
}

int queue_remove (queue_t **queue, queue_t *elem) {

    filaint_t * ptr = (void*) elem;

    printf("Removendo elemento: %d\n", ptr->id);
            
    queue_print("Olá", *queue, print_elem);
    if (queue == NULL) {
        return -1;
    }

    if ((*queue) == NULL) {
        return -2;
    }
    
    if (elem == NULL) {
        return -3;
    }

    queue_t* first = *queue, *it;
    if (elem == first) {
        remove_itself(queue, elem);
        queue_print("Olá", *queue, print_elem);
        return 0;
    }

    for (it = first->next; it != first; it = it->next) {
        if (it == elem) {
            remove_itself(queue, elem);
            queue_print("Olá", *queue, print_elem);
            return 0;
        }
    }

    return -4;
}
