#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
 
#define M 512
#define N 512
 
double a_seq[M][N];
double a_par_j[M][N];

void init(double a[M][N]) {
    /* Inicijalizuj samo nulti red, ostalo ce se popuniti u petlji */
    for (int j = 0; j < N; j++)
        a[0][j] = (double)(j + 1);
    for (int i = 1; i < M; i++)
        for (int j = 0; j < N; j++)
            a[i][j] = 0.0;
}

/* Sekvencijalno */
void seq(double a[M][N], int m, int n) {
    for (int i = 1; i < m; i++)
        for (int j = 0; j < n; j++)
            a[i][j] = 2.0 * a[i-1][j];
}
 
/* Paralelno - paralelizacija unutrasnje petlje po j
 * Ispravno jer nema LC zavisnosti po j.
 * Mana: m-1 fork/join operacija. */
void par_j(double a[M][N], int m, int n, int nt) {
    omp_set_num_threads(nt);
    int i, j;
    for (i = 1; i < m; i++) {
        #pragma omp parallel for private(j) shared(a, i)
        for (j = 0; j < n; j++)
            a[i][j] = 2.0 * a[i-1][j];
    }
}

void par_i_wrong(double a[M][N], int m, int n, int nt) {
    omp_set_num_threads(nt);
    int i, j;
    #pragma omp parallel for private(i, j) shared(a)
    for (i = 1; i < m; i++)
        for (j = 0; j < n; j++)
            a[i][j] = 2.0 * a[i-1][j];
}

int main() {
    int threads[] = {2, 4, 8};

    double a_wrong[M][N];
    init(a_seq); seq(a_seq, M, N);
    init(a_wrong); par_i_wrong(a_wrong, M, N, 4);

     for (int ti = 0; ti < 3; ti++) {
        int nt = threads[ti];
 
        init(a_seq);     seq(a_seq, M, N);
        init(a_par_j);   par_j(a_par_j, M, N, nt);  
 
        init(a_seq);
        double t0 = omp_get_wtime();
        seq(a_seq, M, N);
        double t_seq = omp_get_wtime() - t0;
 
        init(a_par_j);
        t0 = omp_get_wtime();
        par_j(a_par_j, M, N, nt);
        double t_par = omp_get_wtime() - t0;
 
    }

    return 0;
}