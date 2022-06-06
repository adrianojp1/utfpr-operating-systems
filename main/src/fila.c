#include <fila.h>
#include <stdio.h>

Fila *cria_fila(int tam) {
    Fila *f = (Fila *)malloc(sizeof(Fila));
    f->inicio = 0;
    f->fim = 0;
    f->tam = tam;
    f->vetor = (int *)malloc(sizeof(int) * tam);
    return (f);
}

void desaloca_fila(Fila *f) {
    free(f->vetor);
    free(f);
}

void inserir_fila(Fila *f, int elem) {
    if (cheia_fila(f)) {
        printf("ERRO: Fila Cheia");
        exit(1);
    }
    f->vetor[f->fim] = elem;
    f->fim = (f->fim + 1) % f->tam;
}

int retirar_fila(Fila *f) {
    int elem;
    if (vazia_fila(f)) {
        printf("ERRO: Fila Vazia");
        exit(1);
    }
    elem = f->vetor[f->inicio];
    f->inicio = (f->inicio + 1) % f->tam;
    return (elem);
}

int cheia_fila(Fila *f) {
    return (((f->fim + 1) % f->tam) == f->inicio);
}

int vazia_fila(Fila *f) {
    return (f->inicio == f->fim);
}

void imprimir_fila(Fila *f) {
    int i;
    for (i = f->inicio; i != f->fim; i = (i + 1) % f->tam) {
        printf("%d ", f->vetor[i]);
    }
    printf("\n");
}

int getProximoElem(Fila *f) {
    if (vazia_fila(f)) {
        printf("ERRO: Fila Vazia");
        exit(EXIT_FAILURE);
    }
    return (f->vetor[f->inicio]);
}

int getTamanho(Fila *f) {
    return (f->tam);
}
