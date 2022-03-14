#ifndef __TESTA_FILA__
#define __TESTA_FILA__

#define N 100

// A estrutura "filaint_t" será usada com as funções de queue.c usando um
// casting para o tipo "queue_t". Isso funciona bem, se os campos iniciais
// de ambas as estruturas forem os mesmos. De acordo com a seção 6.7.2.1 do
// padrão C99: "Within a structure object, the non-bit-ﬁeld members and the
// units in which bit-ﬁelds reside have addresses that increase in the order
// in which they are declared.".

typedef struct filaint_t
{
   struct filaint_t *prev ;  // ptr para usar cast com queue_t
   struct filaint_t *next ;  // ptr para usar cast com queue_t
   int id ;
   // outros campos podem ser acrescidos aqui
} filaint_t ;


//------------------------------------------------------------------------------

// imprime na tela um elemento da fila (chamada pela função queue_print)
void print_elem (void *ptr);

//------------------------------------------------------------------------------
#endif
