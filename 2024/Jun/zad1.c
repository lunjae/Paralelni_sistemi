#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

struct{
    double vr;
    int rank;
} lok_max, glob_max;

int main(int argc, char* argv[]){
    int rank, size;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = 100;
    int lok_n = N /size;
    double *A = NULL;
    double lok_A[lok_n];

    double C, lok_sum = 0, glob_sum, As, lok_M = 0, final_M;

    if(rank == 0){
        A = (double*)malloc(N*sizeof(double));
        for(int i = 0; i < N; i++) A[i] = i + 1;
        C = 10;
    }

    MPI_Bcast(&C, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(A, lok_n, MPI_DOUBLE, lok_A, lok_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for(int i = 0; i < lok_n; i++)
        lok_sum += lok_A[i];

    MPI_Reduce(&lok_sum, &glob_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank == 0) As = glob_sum / N;
    MPI_Bcast(&As, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    lok_max.vr = lok_A[0];
    lok_max.rank = rank;

    for(int i = 1; i < lok_n; i++){
        if(lok_A[i] > lok_max.vr) lok_max.vr = lok_A[i];
    }

    MPI_Reduce(&lok_max, &glob_max, 1, MPI_DOUBLE_INT, MPI_MAXLOC, 0, MPI_COMM_WORLD);

    int ciljani_rank;
    if(rank == 0) ciljani_rank = glob_max.rank;
    MPI_Bcast(&ciljani_rank, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for(int i = 0; i < lok_n; i++){
        lok_M += (lok_A[i]+As)/C;
    }

    MPI_Reduce(&lok_M, &final_M, 1, MPI_DOUBLE, MPI_SUM, ciljani_rank, MPI_COMM_WORLD);

     if (rank == ciljani_rank) {
        printf("Proces %d je primio konacni rezultat M = %.2f\n", rank, final_M);
    }

    if (rank == 0) free(A);
    MPI_Finalize();
    return 0;
}
