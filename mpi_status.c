#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char *argv[]) {

    const int MAX_NUMEROS = 100;
    int numeros[MAX_NUMEROS];
    int meu_ranque, total_numeros;
    MPI_Status info;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
     
    if (meu_ranque == 0) {
    /* Escolhe uma quantidade aleatória de inteiros para enviar 
       para o processo 1 */
        srand(MPI_Wtime());
        total_numeros = (rand() / (float)RAND_MAX) * MAX_NUMEROS;

    /* Envia a quantidade de inteiros para o processo 1  */
        MPI_Send(numeros, total_numeros, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("Processo 0 enviou %d numeros para 1\n", total_numeros);
    } 
    else if (meu_ranque == 1) {
        MPI_Status info;
    /* Recebe no máximo MAX_NUMEROS do processo 0  */
        MPI_Recv(numeros, MAX_NUMEROS, MPI_INT, 0, 0, MPI_COMM_WORLD, &info);

    /* Depois de receber a mensagem, verifica o status para 
       determinar quantos números foram realmente recebidos */

        MPI_Get_count(&info, MPI_INT, &total_numeros);

    /* Imprime a quantidade de números e também imprime informação 
       adicional que está no manipulador "info"  */

        printf("Processo 1 recebeu %d numeros. Origem da mensagem = %d, etiqueta = %d\n", total_numeros, info.MPI_SOURCE, info.MPI_TAG);
    }
    MPI_Finalize();           
    return 0;
}
