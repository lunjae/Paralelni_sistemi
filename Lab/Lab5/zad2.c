#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omp.h"

#define M 256
#define N 256

double A_seq[M][N];    
double A_par[M][N];
double A_par_par[M][N];

void inicijalizacija(double a[M][N]) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            a[i][j] = (double)(rand() % 100);
        }
    }
}

void kopiraj(double a[M][N], double b[M][N]) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            b[i][j] = a[i][j];
        }
    }
}

void seq(double a[M][N]) {
    for (int i = 0; i < M; i++) {
        for (int j = 2; j < N; j++) {
            a[i][j] = 2*a[i][j - 2];
        }
    }
}

void par(int th,double a[M][N]){
    omp_set_num_threads(th);
    int i, j;
    #pragma omp parallel for private(i, j) shared(a)
    for(int i = 0; i < M; i++){
        for(int j = 2; j < N; j++){
            a[i][j] = 2*a[i][j - 2];
        }
    }
}

void par_par(int th, double a[M][N]){
    omp_set_num_threads(th);
    int i, j;
    for( j = 2; j < N; j++){
        #pragma omp parralel for private(i) shared(a)
        for( i = 0; i < M; i++){
                a[i][j] = 2*a[i][j - 2];
        }
    }    
}

int main(){
    int threads[] = {2, 4, 6};
    
    for(int i = 0; i < 3; i++){
        printf("----------------------------------------\n");
        inicijalizacija(A_seq);
        inicijalizacija(A_par);
        inicijalizacija(A_par_par);
        printf("Izvrsavanje sa %d niti\n", threads[i]);

        double t_start = omp_get_wtime();
        seq(A_seq);
        double t_end = omp_get_wtime();
        double t_seq = t_end - t_start;
        printf("Sekvencijalno vreme: %f\n", t_seq);

         t_start = omp_get_wtime();
        seq(A_seq);
         t_end = omp_get_wtime();
        double t_par_i = t_end - t_start;
        printf("Vreme paralelizacije po i: %f\n", t_par_i);

         t_start = omp_get_wtime();
        seq(A_seq);
         t_end = omp_get_wtime();
        double t_par_j = t_end - t_start;
        printf("Vreme paralelizacije po j: %f\n", t_par_j);
    
        printf("Vreme izvrsavanje sa uporedjivanjem: \n");
        printf("Sekvencijalno: %f\n", t_seq);
        printf("Paralelizacija po i: %f\n", t_par_i);
        printf("Paralelizacija po j: %f\n", t_par_j);
    }

    return 0;
}