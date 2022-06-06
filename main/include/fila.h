#ifndef _FILA_H
#define _FILA_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int inicio;
    int fim;
    int tam;
    int *vetor;
} Fila;

Fila *cria_fila(int tam);
void desaloca_fila(Fila *f);
void inserir_fila(Fila *f, int e);
int retirar_fila(Fila *f);
int cheia_fila(Fila *f);
int vazia_fila(Fila *f);
void imprimir_fila(Fila *f);
int getProximoElem(Fila *f);
int getTamanho();

#endif
