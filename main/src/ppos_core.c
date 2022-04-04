#include "../include/ppos.h"

#define STACKSIZE 64*1024	/* tamanho de pilha das threads */

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
//#define DEBUG

task_t* currentTask, mainTask;
int currentId = 1;

void ppos_init () {
    /* desativa o buffer da saida padrao (stdout), usado pela função printf */
    setvbuf (stdout, 0, _IONBF, 0) ;
    mainTask.id = 0;
    currentTask = &mainTask;
}

int task_create (task_t *task, void (*start_func)(void *), void *arg) {
#ifdef DEBUG
    printf ("Iniciando a criacao da task %d\n", currentId);
#endif
    getcontext (&(task->context));
    task->stack = malloc (STACKSIZE);

    // Atualiza os parametros do contexto com a stack alocada
    if (task->stack) {
        task->context.uc_stack.ss_sp = task->stack ;
        task->context.uc_stack.ss_size = STACKSIZE ;
        task->context.uc_stack.ss_flags = 0 ;
        task->context.uc_link = 0 ;
        task->id = currentId++;
    }
    else {
        perror ("Erro na criação da pilha: ") ;
        return -1;
    }
    makecontext (&(task->context), (void*)start_func, 1, arg);
    
#ifdef DEBUG
    printf ("Criou task %d\n", currentId - 1);
#endif
    return 0;
}

void task_exit (int exit_code) {
    int id = task_id();
    task_switch(&mainTask);
    printf("PPOS: task %d exited", id);
}

int task_switch (task_t *task) {
#ifdef DEBUG
    printf ("Switch task %d -> %d\n", currentTask->id, task->id);
#endif
    ucontext_t* currentContext = &(currentTask->context);
    currentTask = task;
#ifdef DEBUG
    printf ("Current Task antes -> %d \n", currentTask->id);
#endif
    swapcontext (currentContext, &(task->context));
#ifdef DEBUG
    printf ("Current Task depois -> %d \n", currentTask->id);
#endif
    return 0;
    return -1;
}

int task_id () {
    return currentTask->id;
}
