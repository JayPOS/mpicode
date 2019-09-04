#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>
int main (int argc, char **argv)

{ 
int n, meu_ranque, num_procs, rc;
double meu_pi, pi, h, x, raiz = 0, soma = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    
    if (meu_ranque == raiz){
            printf ("Entre com o número de intervalos: \n"); 
	    scanf("%d", &n);
    }
/* Faz a difusão do valor lido de n */	
    MPI_Bcast(&n, 1, MPI_INT, raiz, MPI_COMM_WORLD);
/* Calcula o valor local de pi */
    if (n != 0){
	h=1.0/ (double) n;
	for (int i=meu_ranque+1; i <= n; i+=num_procs)
	{ 
	    x = h * ((double)i - 0.5);
	    soma += (4.0/(1.0 + x*x));
	}
    meu_pi = h* soma;
/* Faz a redução para o processo raiz */
    MPI_Reduce(&meu_pi, &pi, 1, MPI_DOUBLE, MPI_SUM, raiz, MPI_COMM_WORLD);
/* O processo raiz imprime o resultado */
    if (meu_ranque == raiz) 
	printf ("Valor aproximado de pi: %.16f \n", pi);
    }
MPI_Finalize( );
}

