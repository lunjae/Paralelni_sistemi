#include <stdio.h>
#include "omp.h"
#include <math.h>
#include <stdlib.h>

double dot_seq(double *a, double *b, int N){
    double sum = 0;
    for(int i = 0; i < N; i++){
        sum += a[i] * b[i];
    }
    return sum;
}

double par_dynamic(double *a, double *b, int N, int nt, int chunk){
    int i;
    double sum = 0;
    omp_set_num_threads(nt);
    #pragma omp parallel for schedule(dynamic, chunk) private(i) shared(a, b, sum)
    for(i = 0; i < N; i++){
        #pragma omp critical
        sum += a[i]*b[i];
    }
}


double par_static(double *a, double *b, int N, int nt, int chunk){
    int i;
    double sum = 0;
    omp_set_num_threads(nt);
    #pragma omp parallel for schedule(static, chunk) private(i) shared(a, b, sum)
    for(i = 0; i < N; i++){
        #pragma omp critical
        sum += a[i]*b[i];
    }
}


double par_guided(double *a, double *b, int N, int nt, int chunk){
    int i;
    double sum = 0;
    omp_set_num_threads(nt);
    #pragma omp parallel for schedule(guided, chunk) private(i) shared(a, b, sum)
    for(i = 0; i < N; i++){
        #pragma omp critical
        sum += a[i]*b[i];
    }
}

int main(){
    
}