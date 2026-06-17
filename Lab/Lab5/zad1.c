#include "omp.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define N 512


double A[N][N], B[N][N];
double C_seq[N][N];
double C_par[N][N];

void inicijalizacija_matrice(int n){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            A[i][j] = i+j;
            B[i][j] = i*j;
            C_seq[i][j] = 0.0;
            C_par[i][j] = 0.0;
        }
    }
}

void selkvencijalno_mnozenje(int n){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            for(int k = 0; k < n; k++){
                C_seq[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void paralelno_mnozenje(int n, int num_threads, int chunk_size){
    omp_set_num_threads(num_threads);
    int i, j, k;

    #pragma omp parallel for schedule(static, chunk_size) private(i, j, k) shared(A, B, C_par)
    for(i = 0; i < n; i++){
        for(j = 0; j < n; j++){
            for(k = 0; k < n; k++)
                C_par[i][j] += A[i][k] * B[k][j];
        }
    }
}

int main(){
    int sizes[] = {64, 128, 256, 512};
    int niti[] = {2,4,6};
    int chunks[] = {1, 4, 16};

    for(int s = 0; s < 4; s++){
        int n = sizes[s];
        inicijalizacija_matrice(n);

        double t_par;
        double t_start_seq = omp_get_wtime();
        selkvencijalno_mnozenje(n);
        double t_end_seq = omp_get_wtime();
        double t_seq = t_end_seq - t_start_seq;
        printf("Sekvencijalno mnozenje matrica %dx%d: %.6f sekundi\n", n, n, t_seq);


        for(int t = 0; t < 3; t++){
            for(int c = 0; c < 3; c++){
                int num_threads = niti[t];
                int chunk_size = chunks[c];

                double t_start_par = omp_get_wtime();
                paralelno_mnozenje(n, num_threads, chunk_size);
                double t_end_par = omp_get_wtime();
                t_par = t_end_par - t_start_par;
                printf("Paralelno mnozenje matrica %dx%d sa %d niti i chunk size %d: %.6f sekundi\n", n, n, num_threads, chunk_size, t_par);
            }
        }
        printf("Za velicinu matrica %dx%d, sekvencijalno vreme iznosi: %.6f sekundi a paralelno vreme izvrenja iznosi: %.6f sekundi\n", n, n, t_seq, t_par);
    }
}