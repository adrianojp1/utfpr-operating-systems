/**
 **********************************************
 *   Simulador de substituição de memória    **
 *                                           **
 * Autores: Ricky Lemes Habegger &           **
 *          Adriano José Paulichi            **
 *                                           **
 **********************************************
 */

#include <fila.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PAGES 51

int ram_quadros = 4;

int readPages(int *pages) {
    int n = 0;
    while ((!feof(stdin)) && (n < MAX_PAGES)) {
        scanf("%d\n", &(pages[n++]));
    }
    return n;
}

int pageInRam(int *quadros, int page) {
    for (int i = 0; i < ram_quadros; i++) {
        if (quadros[i] == page) {
            return 1;
        }
    }
    return 0;
}

// Retorna o número de faltas de paginas
int loadPageFifo(int *quadros, int page, Fila *fifo) {
    for (int i = 0; i < ram_quadros; i++) {
        if (quadros[i] == page) {
            // pagina ja esta na RAM
            return 0;
        }
    }
    // Falta de pagina
    for (int i = 0; i < ram_quadros; i++) {
        // Há espaço para carregar a pagina
        if (quadros[i] == -1) {
            quadros[i] = page; // carregar a pagina
            // Inserir a pagina carregada na fila
            inserir_fila(fifo, page);
            return 1; // 1 falta de pagina
        }
    }
    // não ha espaço para carregar a pagina
    int remover = retirar_fila(fifo);
    for (int i = 0; i < ram_quadros; i++) {
        if (quadros[i] == remover) {
            quadros[i] = page; // substituir a pagina
            // Inserir a pagina carregada na fila
            inserir_fila(fifo, page);
            return 1; // 1 falta de pagina
        }
    }
    printf("Erro");
    return -1;
}

int simulateFIFO(int *pages, int size) {
    // Vetor de quadros contendo as paginas
    int quadros[ram_quadros];
    for (int i = 0; i < ram_quadros; i++) {
        quadros[i] = -1; // indica que não possui paginas alocadas
    }

    // FIFO
    Fila *fifo = cria_fila(ram_quadros + 1);
    int falta_de_paginas = 0;

    for (int i = 0; i < size; i++) {
        if (!pageInRam(quadros, pages[i])) {
            falta_de_paginas += loadPageFifo(quadros, pages[i], fifo);
        }
    }
    return falta_de_paginas;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Erro: digite o número de quadros");
    }

    int pages[MAX_PAGES];
    int n = readPages(pages);

    for (int i = 0; i < n; i++) {
        printf("%d, ", pages[i]);
    }
    printf("\n");

    int fifo = simulateFIFO(pages, n);
    int rlu = -1;
    int opt = -1;
    printf("%5d quadros, %7d refs: FIFO: %5d PFs, LRU: %5d PFs, OPT: %5d PFs\n",
           ram_quadros, n, fifo, rlu, opt);
}
