/* 
  Este programa faz o cálculo do produto de uma matriz m x n por um vetor de dimensão n. 
  É assumido que cada linha da matriz será processada por um processo distinto.
  O número de colunas da matriz e do vetor b é solicitado como parâmetro de entrada.
  A matriz e o vetor tem os valores iniciais gerados pelo processo 0 e então distribuídos
  para os demais processos.
  O vetor da saída tem dimensão igual a m, ou seja, igual ao número de processos em 
  execução.
*/  

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h" 

void mxv(int n, double* A, double* b, double* c);

int main(int argc, char *argv[])
{
double *A,*Aloc, *b,*c;
int i, j, m, n;
int meu_ranque, num_procs, raiz=0;
double start, finish, loc_elapsed, elapsed;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
   MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
   
   if (meu_ranque == 0) {
      printf("Por favor entre com n: \n");
      scanf("%d",&n);
      printf("\n");
    }
   
    m = num_procs;

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
     /* Difunde o vetor b para todos os processos */
     MPI_Bcast(&b[0],n,MPI_DOUBLE,raiz, MPI_COMM_WORLD);

     /* Distribui as linhas da matriz A entre todos os processos */
     MPI_Scatter(A, n, MPI_DOUBLE, Aloc, n, MPI_DOUBLE,raiz, MPI_COMM_WORLD);
     
//     for (i = 0; i < n; i++) 
//         printf("%f ",Aloc[i]);
//     printf("\n");
     start = MPI_Wtime();
     (void) mxv(n, Aloc, b, c);

     MPI_Gather(c, 1, MPI_DOUBLE, c, 1, MPI_DOUBLE, raiz, MPI_COMM_WORLD);
     finish = MPI_Wtime();
     loc_elapsed = finish-start;
     MPI_Reduce(&loc_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);  

     if (meu_ranque == 0){
        printf("Tempo total = %e\n", elapsed);
//        printf("Imprimindo o resultado\n");
//	for (i = 0; i < m; i++) 
//             printf("%f ",c[i]);
//        printf("\n");
     }
     free(A);
     free(Aloc);
     free(b);
     free(c);
     
     MPI_Finalize();
  
     return(0);
}

void mxv(int n, double* A, double* b, double* c) {
int j;
      c[0] = 0.0;
      for (j=0; j<n; j++)
           c[0] += A[j]*b[j];
}
