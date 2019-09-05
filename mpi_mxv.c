#include <stdio.h>
#include <stdlib.h>
#include "mpi.h" 

void mxv(int n, double* A, double* b, double* c);

int main(int argc, char *argv[])
{
double *A,*Aloc, *b,*c;
int i, j, m, n;
int meu_ranque, num_procs, raiz=0;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
   MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
   
   if (meu_ranque == 0) {
      printf("Por favor entre com n: \n");
      scanf("%d",&n);
      printf("\n");
    }
   
    m = num_procs;
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, raiz, MPI_COMM_WORLD);

   
    if (meu_ranque == 0) {
        printf("Valor de m: %d e  n: %d \n", m,n);
       if ( (A=(double *)malloc(m*n*sizeof(double))) == NULL )
           perror("Erro de alocação de memória para a matriz A");
        if ( (Aloc=(double *)malloc(m*n*sizeof(double))) == NULL )
           perror("Erro de alocação de memória para a matriz local A");
       if ( (b=(double *)malloc(n*sizeof(double))) == NULL )
           perror("Erro de alocação de memória para o vetor b");
       if ( (c=(double *)malloc(m*sizeof(double))) == NULL )
           perror("Erro de alocação de memória para o vetor c");
     } else {
        if ( (Aloc=(double *)malloc(n*sizeof(double))) == NULL )
           perror("Erro de alocação de memória para a matriz local A");
       if ( (b=(double *)malloc(n*sizeof(double))) == NULL )
           perror("Erro de alocação de memória para o vetor b");
       if ( (c=(double *)malloc(sizeof(double))) == NULL )
           perror("Erro de alocação de memória para o vetor c");
     }    

     if (meu_ranque == 0) {
         printf("Atribuindo valor inicial à matriz A e ao vetor b\n");
         for (j=0; j<n; j++)
              b[j] = 2.0;
         for (i=0; i<m; i++)
              for (j=0; j<n; j++)
                  A[i*n + j] = (double) i;
     }

     MPI_Bcast(&b[0],n,MPI_DOUBLE,raiz, MPI_COMM_WORLD);
     MPI_Scatter(A, n, MPI_DOUBLE, Aloc, n, MPI_DOUBLE,raiz, MPI_COMM_WORLD);
     
//     for (i = 0; i < n; i++) 
//         printf("%f ",Aloc[i]);
//     printf("\n");

     (void) mxv(n, Aloc, b, c);

     MPI_Barrier(MPI_COMM_WORLD);  // ?

     MPI_Gather(c, 1, MPI_DOUBLE, c, 1, MPI_DOUBLE, raiz, MPI_COMM_WORLD);

     if (meu_ranque == 0){
        printf("Imprimindo o resultado\n");
	for (i = 0; i < m; i++) 
             printf("%f ",c[i]);
        printf("\n");
     }
     MPI_Finalize();
     free(A);
     free(b);
     free(c);
     return(0);
}

void mxv(int n, double* A, double* b, double* c) {
int j;
      c[0] = 0.0;
      for (j=0; j<n; j++)
           c[0] += A[j]*b[j];
}
