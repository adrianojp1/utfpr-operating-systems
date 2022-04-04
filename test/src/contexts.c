// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DINF UFPR
// Versão 1.4 -- Janeiro de 2022

// Demonstração das funções POSIX de troca de contexto (ucontext.h).

// operating system check
#if defined(_WIN32) || (!defined(__unix__) && !defined(__unix) && (!defined(__APPLE__) || !defined(__MACH__)))
#warning Este codigo foi planejado para ambientes UNIX (LInux, *BSD, MacOS). A compilacao e execucao em outros ambientes e responsabilidade do usuario.
#endif

#define _XOPEN_SOURCE 600	/* para compilar no MacOS */

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACKSIZE 64*1024	/* tamanho de pilha das threads */

// Instancia os contextos
ucontext_t ContextPing, ContextPong, ContextMain ;

/*****************************************************/

void BodyPing (void * arg)
{
   int i ;

   printf ("%s: inicio\n", (char *) arg) ;

   for (i=0; i<4; i++)
   {
      printf ("%s: %d\n", (char *) arg, i) ;
      // Salva o contexto atual em ContextPing e ativa o contexto ContextPong
      swapcontext (&ContextPing, &ContextPong) ;
   }
   printf ("%s: fim\n", (char *) arg) ;
  // Salva o contexto atual em ContextPing e ativa o contexto ContextMain
   swapcontext (&ContextPing, &ContextMain) ;
}

/*****************************************************/

void BodyPong (void * arg)
{
   int i ;

   printf ("%s: inicio\n", (char *) arg) ;

   for (i=0; i<4; i++)
   {
      printf ("%s: %d\n", (char *) arg, i) ;
      // Salva o contexto atual em ContextPong e ativa o contexto ContextPing
      swapcontext (&ContextPong, &ContextPing) ;
   }
   printf ("%s: fim\n", (char *) arg) ;

  // Salva o contexto atual em ContextPong e ativa o contexto ContextMain
   swapcontext (&ContextPong, &ContextMain) ;
}

/*****************************************************/

int main (int argc, char *argv[])
{
   char *stack ;

   printf ("main: inicio\n") ;

   // Salva o contexto atual em ContextPing
   getcontext (&ContextPing) ;

   // aloca espaço para stack do contexto
   stack = malloc (STACKSIZE) ;
   // Atualiza os parametros do contexto com a stack alocada
   if (stack)
   {
      ContextPing.uc_stack.ss_sp = stack ;
      ContextPing.uc_stack.ss_size = STACKSIZE ;
      ContextPing.uc_stack.ss_flags = 0 ;
      ContextPing.uc_link = 0 ;
   }
   else
   {
      perror ("Erro na criação da pilha: ") ;
      exit (1) ;
   }
    
   // Define que a função BodyPing será chamada ao ativar o ContextPing
   makecontext (&ContextPing, (void*)(*BodyPing), 1, "    Ping") ;


   // Salva o contexto atual em ContextPong
   getcontext (&ContextPong) ;

   // aloca espaço para stack do contexto
   stack = malloc (STACKSIZE) ;
   // Atualiza os parametros do contexto com a stack alocada
   if (stack)
   {
      ContextPong.uc_stack.ss_sp = stack ;
      ContextPong.uc_stack.ss_size = STACKSIZE ;
      ContextPong.uc_stack.ss_flags = 0 ;
      ContextPong.uc_link = 0 ;
   }
   else
   {
      perror ("Erro na criação da pilha: ") ;
      exit (1) ;
   }

   // Define que a função BodyPing será chamada ao ativar o ContextPong
   makecontext (&ContextPong, (void*)(*BodyPong), 1, "        Pong") ;

   // Salva o contexto atual em ContextMain e ativa o contexto ContextPing
   swapcontext (&ContextMain, &ContextPing) ;
   // Salva o contexto atual em ContextMain e ativa o contexto ContextPongk
   swapcontext (&ContextMain, &ContextPong) ;

   printf ("main: fim\n") ;

   exit (0) ;
}
