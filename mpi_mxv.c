#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int meu_ranque, num_procs;

void Get_dims(int argc, char* argv[], int* m_p, int* local_m_p, 
      int* n_p, int* local_n_p);
void Allocate_arrays(double** local_A_pp, double** local_x_pp, 
      double** local_y_pp, int local_m, int n, int local_n);
void Generate_matrix(double local_A[], int local_m, int n);
void Generate_vector(double local_x[], int local_n);
void Mat_vect_mult(double local_A[], double local_x[], 
      double local_y[], double x[], int m, int local_m, int n, 
      int local_n);

/*-------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   double* local_A;
   double* local_x;
   double* local_y;
   double* x;
   int m, local_m, n, local_n;
   int M =1000, N=1000;
   //  M e N devem ser positivos e divisíveis pelo número de prcoessos
   double start, finish, loc_elapsed, elapsed;

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
   MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);

   if (meu_ranque == 0) 
	{
		printf("Por favor entre com M e N: ");
   		scanf("%d %d",&m,&n);   // strtol(argv[1], NULL, 10);
   		printf("\n");
   		if (m % num_procs != 0  || n % num_procs != 0) 
			{
          			printf("Erro! \n M e N deve devem ser divisíveis pelo número de processor \n");
          			exit(1);
        		}
   }

   MPI_Bcast(m, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(n, 1, MPI_INT, 0, MPI_COMM_WORLD);

   local_m = m/num_procs;
   local_n = n/num_procs;

    if ( (local_A=(double *)malloc(local_m*local_n*sizeof(double))) == NULL )
      perror("Alocação de memória para a matriz A");
    if ( (local_x=(double *)malloc(local_n*sizeof(double))) == NULL )
      perror("Alocação de memória para o vetor de entrada x");
   if ( (local_y=(double *)malloc(local_m*sizeof(double))) == NULL )
      perror("Alocação de memória para o vetor de saída y");


/* 
#pragma omp parallel for default(shared) schedule(static) num_threads(4)
   for (j=0; j<n; j++)
      c[j] = 2.0;
#pragma omp parallel for default(shared) schedule(static) private(j) num_threads(4) collapse(2)
   for (i=0; i<m; i++)
//#pragma omp parallel for simd
      for (j=0; j<n; j++)
         b[i*n+j] = i;
*/


   x = malloc(n*sizeof(double));
   MPI_Barrier(MPI_COMM_WORLD);
   start = MPI_Wtime();
   Mat_vect_mult(local_A, local_x, local_y, x, m, local_m, n, local_n);
   finish = MPI_Wtime();
   loc_elapsed = finish-start;
   MPI_Reduce(&loc_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

   if (meu_ranque == 0) {
//    printf("Tempo total = %e\n", elapsed);
      printf("%e\n", elapsed);
//    fprintf(stderr, "From mat-vect: p = %d, n = %d, elapsed = %e\n",
//          num_procs, n, elapsed);
   }
   free(local_A);
   free(local_x);
   free(local_y);
   free(x);
   MPI_Finalize();
   return 0;
}  /* main */

/*-------------------------------------------------------------------*/
void Mat_vect_mult(
      double    local_A[]  /* in  */, 
      double    local_x[]  /* in  */, 
      double    local_y[]  /* out */,
      double    x[]        /* scratch */,
      int       m          /* in  */,
      int       local_m    /* in  */, 
      int       n          /* in  */,
      int       local_n    /* in  */) {
   int local_i, j;

   MPI_Allgather(local_x, local_n, MPI_DOUBLE,
         x, local_n, MPI_DOUBLE, MPI_COMM_WORLD);

   for (local_i = 0; local_i < local_m; local_i++) {
      local_y[local_i] = 0.0;
      for (j = 0; j < n; j++)
         local_y[local_i] += local_A[local_i*n+j]*x[j];
   }
}  /* Mat_vect_mult */
