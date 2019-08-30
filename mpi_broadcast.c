#include <stdio.h>
#include "mpi.h"
#define TAM 10

int main (int argc, char *argv[]) {
int  i, valor[TAM], meu_ranque, raiz = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    /* Cada processo tem um valor inicial diferente */
    valor = meu_ranque;
    /* O processo raiz faz a leitura do valor a ser enviado */
    if (meu_ranque == raiz) {
        printf("Entre um valor: \n");
        scanf("%d", &valor);    
    }
    /* A operação de difusão é chamada por todos os processos */
    /* Apenas o processo raiz envia, os demais recebem  */
    MPI_Bcast(&valor, TAM, MPI_INT, raiz, MPI_COMM_WORLD);
    /* O valor agora é o mesmo em todos os processos */
    printf("O processo com ranque %d recebeu o valor: %d\n",meu_ranque, valor);
    /* Termina a execução */
    MPI_Finalize();
    return 0;
}
