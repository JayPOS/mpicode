/* M�todo do Trap�zio Paralelo
* 
* Estimativa da integral de a at� b de f(x)= exp(x)
* usando o m�todo do trap�zio e n trapez�ides.
*
* 1. Cada processo estima a integral de f(x)
* sobre seu conjunto aprox. de n/p trap�zios
* 2. Cada processo != 0 envia sua integral parcial para 0.
* 3. Processo 0 soma os valores recebidos dos processos
* individuais e imprime o resultado.
*
* Nota: f(x), a, b, e n s�o todos fixos.
*/

#include <stdio.h>
#include <math.h>
#include "mpi.h"             /* Fun��es e defini��es MPI.  */

double f(double x) {
        double return_val;
        return_val = exp(x);
        return return_val;
}

int main(int argc, char** argv) { 
    int meu_ranque;          /* O ranque do meu processo     */
    int p;                   /* O n�mero de processos        */
    double a = 0.0;          /* Limite esquerdo              */
    double b = 1.0;          /* Limite direito               */
    long int n = 100000000;  /* N�mero de trapez�ides        */
    double x,h;              /* x e h, a base do trapezoide  */
    double integral;         /* Integral de cada processo    */
    double total;            /* Integral total               */
    int origem;              /* Processo enviando a integral */
    int destino = 0;         /* Todas as mensagens v�o para 0*/
    int etiq = 3;            /* Uma etiqueta qualquer        */
    MPI_Status info;         /* Necess�rio para MPI_Recv     */
          
    /* Permite ao sistema iniciar o  MPI */
    MPI_Init(&argc, &argv);
    /* Pega o ranque do meu processo */
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    /* Encontra quantos processos est�o ativos */
    MPI_Comm_size(MPI_COMM_WORLD, &p);
       
    h = (b - a)/n;   /* h � o mesmo para todos os processos */

    /* O processo 0 calcula o valor inicial da sua integral */

    if (meu_ranque == 0) integral = (f(a) + f(b))/2.0;

    /* Cada processo calcula a integral aprox. sobre n/p trap�zios*/ 

    for (x =  a + h*meu_ranque; x < b ; x += p*h) {
         integral += f(x);
        }
    integral = integral*h;

    /* O processo 0 recebe e soma as integrais parciais calculadas por cada processo */
  
    if (meu_ranque == 0) { 
        total = integral; 
        for (origem = 1; origem < p; origem++) { 
             MPI_Recv(&integral, 1, MPI_DOUBLE, origem, etiq, MPI_COMM_WORLD, &info);
             total += integral; 
            }
    } else {
        MPI_Send(&integral, 1, MPI_DOUBLE, destino, etiq, MPI_COMM_WORLD);
          }

    /* Imprime o resultado */
    if (meu_ranque == 0){
        printf("Com n = %ld trapezoides, a estimativa \n", n);
        printf("da integral de %lf ate %lf = %lf \n", a, b, total);
        }

    /* Termina o MPI */
    MPI_Finalize();        
} /* main */
