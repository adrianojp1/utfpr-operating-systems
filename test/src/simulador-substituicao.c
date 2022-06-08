/**
 **********************************************
 *   Simulador de substituição de memória    **
 *                                           **
 * Autores: Ricky Lemes Habegger &           **
 *          Adriano José Paulichi            **
 *                                           **
 **********************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PAGES 1e6

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

int readPages(int *pages) {
    int n = 0;
    while ((!feof(stdin)) && (n < MAX_PAGES)) {
        scanf("%d\n", &(pages[n++]));
    }
    return n;
}

int pageInRam(int *ram, int ram_size, int page) {
    for (int i = 0; i < ram_size; i++) {
        if (ram[i] == page) {
            return 1;
        }
    }
    return 0;
}

int loadPageFifo(int *ram, int ram_size, int page, Fila *fifo) {
    // Há espaço para carregar a pagina
    for (int i = 0; i < ram_size; i++) {
        if (ram[i] == -1) {
            ram[i] = page; // carregar a pagina
            // Inserir a pagina carregada na fila
            inserir_fila(fifo, page);
            return 0;
        }
    }
    // não ha espaço para carregar a pagina
    int remover = retirar_fila(fifo);
    for (int i = 0; i < ram_size; i++) {
        if (ram[i] == remover) {
            ram[i] = page; // substituir a pagina
            // Inserir a pagina carregada na fila
            inserir_fila(fifo, page);
            return 0;
        }
    }
    printf("Erro ao carregar página: Pagina não encontrada na RAM");
    return 1; // indica erro
}

int *initRam(int ram_size) {
    int *ram = (int *)malloc(sizeof(int) * ram_size);
    for (int i = 0; i < ram_size; i++) {
        ram[i] = -1; // indica que não possui paginas alocadas
    }
    return ram;
}

int simulateFIFO(int *pages, int pages_size, int ram_size) {
    // Vetor de quadros contendo as paginas
    int *ram = initRam(ram_size);

    // FIFO
    Fila *fifo = cria_fila(ram_size + 1);
    int falta_de_paginas = 0;

    // Para cada pagina verifica se está na ram, se não insere na ram
    for (int i = 0; i < pages_size; i++) {
        if (!pageInRam(ram, ram_size, pages[i])) {
            loadPageFifo(ram, ram_size, pages[i], fifo);
            falta_de_paginas += 1;
        }
    }
    desaloca_fila(fifo);
    free(ram);
    return falta_de_paginas;
}

int getPosElem(int *vector, int size, int elem) {
    for (int i = 0; i < size; i++) {
        if (vector[i] == elem) {
            return i;
        }
    }
    return -1;
}

int getPosElemReverse(int *vector, int size, int elem) {
    for (int i = size - 1; i >= 0; i--) {
        if (vector[i] == elem) {
            return i;
        }
    }
    return -1;
}

int getPosBestPageToRemoveRLU(int *ram, int ram_size, int *pages,
                              int pages_size) {
    int pos_best_page = pages_size - 1;
    int pos_best_page_ram = 0;
    int pos_elem;
    for (int i = 0; i < ram_size; i++) {
        pos_elem = getPosElemReverse(pages, pages_size, ram[i]);
        if (pos_elem < pos_best_page) {
            pos_best_page = pos_elem;
            pos_best_page_ram = i;
        }
    }
    return pos_best_page_ram;
}

int simulateRLU(int *pages, int pages_size, int ram_size) {
    // Vetor de quadros contendo as paginas
    int *ram = initRam(ram_size);

    int falta_de_paginas = 0;
    int best_page;
    int espaco_usado_ram = 0;

    // Para cada pagina verifica se está na ram, se não insere na ram
    for (int i = 0; i < pages_size; i++) {
        if (!pageInRam(ram, ram_size, pages[i])) {
            if (espaco_usado_ram >= ram_size) {
                best_page =
                    getPosBestPageToRemoveRLU(ram, ram_size, pages, i + 1);
                // printf("p%d substitui p%d\n", pages[i], ram[best_page]);

                // Substitui a pagina pela melhor pagina
                ram[best_page] = pages[i];
            } else { // Ainda há espaço vazio na ram
                // Adiciona a pagina na ram
                ram[espaco_usado_ram++] = pages[i];

                // printf("p%d carrega em q%d\n", pages[i], espaco_usado_ram -
                // 1);
            }
            falta_de_paginas += 1;
        }
    }

    return falta_de_paginas;
}

int getPosBestPageToRemoveOPT(int *ram, int ram_size, int *pages,
                              int pages_size) {
    int pos_best_page = 0;
    int pos_best_page_ram = 0;
    int pos_elem;
    for (int i = 0; i < ram_size; i++) {
        pos_elem = getPosElem(pages, pages_size, ram[i]);
        if (pos_elem == -1) {
            return i; // ram[i] não será mais utilizado
        }
        if (pos_elem > pos_best_page) {
            pos_best_page = pos_elem;
            pos_best_page_ram = i;
        }
    }
    return pos_best_page_ram;
}

int simulateOPT(int *pages, int pages_size, int ram_size) {
    // Vetor de quadros contendo as paginas
    int *ram = initRam(ram_size);

    int falta_de_paginas = 0;
    int best_page;
    int espaco_usado_ram = 0;

    // Para cada pagina verifica se está na ram, se não insere na ram
    for (int i = 0; i < pages_size; i++) {
        if (!pageInRam(ram, ram_size, pages[i])) {
            if (espaco_usado_ram >= ram_size) {
                best_page = getPosBestPageToRemoveOPT(ram, ram_size, &pages[i],
                                                      pages_size - i);

                // printf("p%d substitui p%d\n", pages[i], ram[best_page]);

                // Substitui a pagina pela melhor pagina
                ram[best_page] = pages[i];
            } else { // Ainda há espaço vazio na ram
                // Adiciona a pagina na ram
                ram[espaco_usado_ram++] = pages[i];

                // printf("p%d carrega em q%d\n", pages[i], espaco_usado_ram -
                // 1);
            }
            falta_de_paginas += 1;
        }
    }

    return falta_de_paginas;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Erro: digite o número de quadros da ram\n");
        exit(1);
    }
    int ram_size = atoi(argv[1]);

    int *pages = (int *)malloc(sizeof(int) * MAX_PAGES);
    int n = readPages(pages);

    // for (int i = 0; i < n; i++) {
    //     printf("%d, ", pages[i]);
    // }
    // printf("\n");

    int fifo = simulateFIFO(pages, n, ram_size);
    int rlu = simulateRLU(pages, n, ram_size);
    int opt = simulateOPT(pages, n, ram_size);

    printf("%5d quadros, %7d refs: FIFO: %5d PFs, LRU: %5d PFs, OPT: %5d PFs\n",
           ram_size, n, fifo, rlu, opt);
}
