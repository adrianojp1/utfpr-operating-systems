/**
 **********************************************
 *      Simulador de alocação de memória     **
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

#define TOTAL_MEMORY_SIZE 10000
#define MAX_BLOCK_SIZE (TOTAL_MEMORY_SIZE / 10)
#define BLOCK_COUNT 20

int block_id = 0;

typedef struct mem_block_t {
    struct mem_block_t *next;
    struct mem_block_t *prev;
    int id;
    int start_idx;
    int size;
} mem_block_t;

typedef struct memory_t {
    mem_block_t *memory_table;
    int size;
} memory_t;

void append_next(mem_block_t **queue_block, mem_block_t *next) {
    if (*queue_block == NULL) {
        *queue_block = next;
        return;
    }

    mem_block_t *block = *queue_block;
    if (block->next == NULL) {
        block->next = next;
        next->prev = block;
        return;
    }

    mem_block_t *block_next = block->next;
    block->next = next;
    next->prev = block;
    next->next = block_next;
    block_next->prev = next;
}

mem_block_t *get_block(int size) {
    mem_block_t *block = malloc(sizeof(mem_block_t));
    block->next = NULL;
    block->prev = NULL;
    block->id = block_id++;
    block->start_idx = 0;
    block->size = size;
    return block;
}

int get_block_end(mem_block_t *block) {
    return block->start_idx + block->size;
}

void print_memory_table(memory_t *memory) {
    char out[TOTAL_MEMORY_SIZE * 100] = "[";

    int mem_idx = 0;
    mem_block_t *block_it = memory->memory_table;
    while (block_it != NULL) {
        for (int i = mem_idx; i < block_it->start_idx; i++, mem_idx++) {
            strcat(out, " _");
        }

        strcat(out, "(");
        for (int i = mem_idx; i < get_block_end(block_it); i++, mem_idx++) {
            char id[5];
            sprintf(id, " %d", block_it->id);
            strcat(out, id);
        }
        strcat(out, ")");
        block_it = block_it->next;
    }

    for (int i = mem_idx; i != memory->size; i++, mem_idx++) {
        strcat(out, " _");
    }

    strcat(out, "]");
    fprintf(stdout, "%s\n", out);
}

void simulate_first_fit(memory_t *memory, int *sequence_to_alloc) {
#ifdef DEBUG
    print_memory_table(memory);
#endif
    mem_block_t *memory_table = memory->memory_table;

    int all_blocks_fit_flag = 1;
    int i = 0;
    while (i < BLOCK_COUNT && all_blocks_fit_flag) {
        mem_block_t *block_to_alloc = get_block(sequence_to_alloc[i]);
        mem_block_t *first_fit_prev = NULL;

        mem_block_t *mem_it = memory_table;
        while (first_fit_prev == NULL && all_blocks_fit_flag) {

            int block_end_idx = get_block_end(mem_it);
            if (mem_it->next == NULL) {
                if (memory->size - block_end_idx >= block_to_alloc->size) {
                    first_fit_prev = mem_it;
                } else {
                    all_blocks_fit_flag = 0;
                }

            } else {
                if (mem_it->next->start_idx - block_end_idx >= block_to_alloc->size) {
                    first_fit_prev = mem_it;
                }
            }

            mem_it = mem_it->next;
        }

        if (first_fit_prev != NULL) {
            block_to_alloc->start_idx = get_block_end(first_fit_prev);
            append_next(&first_fit_prev, block_to_alloc);
#ifdef DEBUG
            print_memory_table(memory);
#endif
        }

        i++;
    }
}

void simulate_next_fit(memory_t *memory, int *sequence_to_alloc) {
#ifdef DEBUG
    print_memory_table(memory);
#endif
    mem_block_t *memory_table = memory->memory_table;

    int all_blocks_fit_flag = 1;
    int i = 0;
    mem_block_t *last_fit = memory_table;
    while (i < BLOCK_COUNT && all_blocks_fit_flag) {
        mem_block_t *block_to_alloc = get_block(sequence_to_alloc[i]);
        mem_block_t *first_fit_prev = NULL;

        mem_block_t *mem_it = last_fit;
        while (first_fit_prev == NULL && all_blocks_fit_flag) {

            int block_end_idx = get_block_end(mem_it);
            if (mem_it->next == NULL) {
                if (memory->size - block_end_idx >= block_to_alloc->size) {
                    first_fit_prev = mem_it;
                }
                mem_it = memory_table;
            } else {
                if (mem_it->next->start_idx - block_end_idx >= block_to_alloc->size) {
                    first_fit_prev = mem_it;
                } else if (last_fit == mem_it->next) {
                    all_blocks_fit_flag = 0;
                }
                mem_it = mem_it->next;
            }
        }

        if (first_fit_prev != NULL) {
            block_to_alloc->start_idx = get_block_end(first_fit_prev);
            append_next(&first_fit_prev, block_to_alloc);
            last_fit = block_to_alloc;
#ifdef DEBUG
            print_memory_table(memory);
#endif
        }

        i++;
    }
}

void simulate_best_fit(memory_t *memory, int *sequence_to_alloc) {
#ifdef DEBUG
    print_memory_table(memory);
#endif
    mem_block_t *memory_table = memory->memory_table;

    int all_blocks_fit_flag = 1;
    int i = 0;
    while (i < BLOCK_COUNT && all_blocks_fit_flag) {
        mem_block_t *block_to_alloc = get_block(sequence_to_alloc[i]);
        mem_block_t *best_fit_prev = NULL;
        int best_fit_size = memory->size;

        mem_block_t *mem_it = memory_table;
        while (mem_it != NULL && all_blocks_fit_flag) {
            int block_end_idx = get_block_end(mem_it);
            if (mem_it->next == NULL) {
                int free_frag_size = memory->size - block_end_idx;
                if (free_frag_size >= block_to_alloc->size) {
                    if (best_fit_prev == NULL || free_frag_size < best_fit_size) {
                        best_fit_prev = mem_it;
                        best_fit_size = free_frag_size;
                    }
                } else {
                    if (best_fit_prev == NULL) {
                        all_blocks_fit_flag = 0;
                    }
                }

            } else {
                int free_frag_size = mem_it->next->start_idx - block_end_idx;
                if (free_frag_size >= block_to_alloc->size) {
                    if (best_fit_prev == NULL || free_frag_size < best_fit_size) {
                        best_fit_prev = mem_it;
                        best_fit_size = free_frag_size;
                    }
                }
            }

            mem_it = mem_it->next;
        }

        if (best_fit_prev != NULL) {
            block_to_alloc->start_idx = get_block_end(best_fit_prev);
            append_next(&best_fit_prev, block_to_alloc);
#ifdef DEBUG
            print_memory_table(memory);
#endif
        }

        i++;
    }
}

void simulate_worst_fit(memory_t *memory, int *sequence_to_alloc) {
#ifdef DEBUG
    print_memory_table(memory);
#endif
    mem_block_t *memory_table = memory->memory_table;

    int all_blocks_fit_flag = 1;
    int i = 0;
    while (i < BLOCK_COUNT && all_blocks_fit_flag) {
        mem_block_t *block_to_alloc = get_block(sequence_to_alloc[i]);
        mem_block_t *worst_fit_prev = NULL;
        int worst_fit_size = 0;

        mem_block_t *mem_it = memory_table;
        while (mem_it != NULL && all_blocks_fit_flag) {
            int block_end_idx = get_block_end(mem_it);
            if (mem_it->next == NULL) {
                int free_frag_size = memory->size - block_end_idx;
                if (free_frag_size >= block_to_alloc->size) {
                    if (worst_fit_prev == NULL || free_frag_size > worst_fit_size) {
                        worst_fit_prev = mem_it;
                        worst_fit_size = free_frag_size;
                    }
                } else {
                    if (worst_fit_prev == NULL) {
                        all_blocks_fit_flag = 0;
                    }
                }

            } else {
                int free_frag_size = mem_it->next->start_idx - block_end_idx;
                if (free_frag_size >= block_to_alloc->size) {
                    if (worst_fit_prev == NULL || free_frag_size > worst_fit_size) {
                        worst_fit_prev = mem_it;
                        worst_fit_size = free_frag_size;
                    }
                }
            }

            mem_it = mem_it->next;
        }

        if (worst_fit_prev != NULL) {
            block_to_alloc->start_idx = get_block_end(worst_fit_prev);
            append_next(&worst_fit_prev, block_to_alloc);
#ifdef DEBUG
            print_memory_table(memory);
#endif
        }

        i++;
    }
}

void print_frag_stats(memory_t *memory) {
    int frag_count = 0;
    int smallest_frag_size = memory->size;
    int biggest_frag_size = 0;
    int total_frag_size = 0;

    int mem_idx = 0;
    mem_block_t *block_it = memory->memory_table;
    while (block_it != NULL) {

        if (block_it->start_idx > mem_idx) {
            int frag_size = block_it->start_idx - mem_idx;

            if (frag_size < smallest_frag_size) {
                smallest_frag_size = frag_size;
            }
            if (frag_size > biggest_frag_size) {
                biggest_frag_size = frag_size;
            }

            frag_count++;
            total_frag_size += frag_size;
        }
        mem_idx = get_block_end(block_it);

        if (block_it->next == NULL && memory->size > mem_idx) {
            int frag_size = memory->size - mem_idx;

            if (frag_size < smallest_frag_size) {
                smallest_frag_size = frag_size;
            }
            if (frag_size > biggest_frag_size) {
                biggest_frag_size = frag_size;
            }

            frag_count++;
            total_frag_size += frag_size;
        }
        block_it = block_it->next;
    }

    float avg_frag_size = (float)total_frag_size / frag_count;
    fprintf(stdout, "\nNúmero de fragmentos livres de memória: %d\n", frag_count);
    if (frag_count > 0) {
        fprintf(stdout, "Tamanho do menor fragmento: %d\n", smallest_frag_size);
        fprintf(stdout, "Tamanho do maior fragmento: %d\n", biggest_frag_size);
        fprintf(stdout, "Tamanho médio dos fragmentos: %.2f\n", avg_frag_size);
    }
}

memory_t *init_memory() {
    memory_t *memory = malloc(sizeof(memory_t));
    memory->size = TOTAL_MEMORY_SIZE;
    memory->memory_table = NULL;

    mem_block_t *block_1 = get_block(TOTAL_MEMORY_SIZE / 8);
    append_next(&(memory->memory_table), block_1);

    mem_block_t *block_2 = get_block(TOTAL_MEMORY_SIZE / 8);
    block_2->start_idx = get_block_end(block_1) + TOTAL_MEMORY_SIZE / 4;
    append_next(&block_1, block_2);

    mem_block_t *block_3 = get_block(TOTAL_MEMORY_SIZE / 8);
    block_3->start_idx = get_block_end(block_2) + TOTAL_MEMORY_SIZE / 4;
    append_next(&block_2, block_3);
    return memory;
}

int main() {
    srand(time(NULL));

    int random_sizes[BLOCK_COUNT];
    for (int i = 0; i < BLOCK_COUNT; i++) {
        random_sizes[i] = (rand() % (MAX_BLOCK_SIZE - 1)) + 1;
    }

    printf("\nRandom sizes: ");
    for (int i = 0; i < BLOCK_COUNT; i++) {
        fprintf(stdout, "%d ", random_sizes[i]);
    }
    printf("\n=========================================================\n\n");

    printf("First-fit\n");
    memory_t *first_fit_memory = init_memory();
    simulate_first_fit(first_fit_memory, random_sizes);
    print_frag_stats(first_fit_memory);
    printf("\n=========================================================\n\n");

    printf("Next-fit\n");
    memory_t *next_fit_memory = init_memory();
    simulate_next_fit(next_fit_memory, random_sizes);
    print_frag_stats(next_fit_memory);
    printf("\n=========================================================\n\n");

    printf("Best-fit\n");
    memory_t *best_fit_memory = init_memory();
    simulate_best_fit(best_fit_memory, random_sizes);
    print_frag_stats(best_fit_memory);
    printf("\n=========================================================\n\n");

    printf("Worst-fit\n");
    memory_t *worst_fit_memory = init_memory();
    simulate_worst_fit(worst_fit_memory, random_sizes);
    print_frag_stats(worst_fit_memory);
    printf("\n=========================================================\n\n");
}
