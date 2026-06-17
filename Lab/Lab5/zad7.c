#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <omp.h>

void inc(double *a, int n){
    for(int i = 0; i < n; i++)
        a[i] = i*n-26;
}

/* 1) Sekvencijalno */
double sekvencijalno(double* a, int n) {
    double maks = a[0];
    for (int i = 1; i < n; i++)
        if (a[i] > maks) maks = a[i];
    return maks;
}

double paralelno_critical(double *a, int n, int nt, int chunk, int sched){
    double max = a[0];
    int i;
    omp_set_num_threads(nt);
    if(sched == 0){
        #pragma omp parallel for shared(max) private(i) schedule(static, chunk)
        for(i = 0; i < n; i++){
            #pragma omp critical
            if(a[i]>max) max = a[i];
        }
    }else if(sched == 1){
        #pragma omp parallel for shared(max) private(i) schedule(dynamic, chunk)
        for(i = 0; i < n; i++){
            #pragma omp critical
            if(a[i]>max) max = a[i];
        }
    }else {
        #pragma omp parallel for shared(max) private(i) schedule(guided, chunk)
        for(i = 0; i < n; i++){
            #pragma omp critical
            if(a[i]>max) max = a[i];
        }
    }
    return max;
}

int main(){
    int dim[] = {100000, 200000, 300000};
    int dim_n = 3;
    int chunks[] = {100, 10000, 100000};
    int niti[] = {2,4,6};
    int nn = 3;
    double max;
    double t_start, t_end, t;

    for(int i = 0; i < dim_n; i++){
        int n = dim[i];
        double *a = (double*)malloc(n*sizeof(double));    
        inc(a, n);
        
        printf("DImenzija vektora: %d\n", n);

        printf("Sekvencijalno: \n");
        t_start = omp_get_wtime();
        max = sekvencijalno(a, n);
        t_end = omp_get_wtime();
        t = t_end - t_start;
        printf("Max el: %f, vreme izvrenja: %.8f\n", max, t);
        printf("------------------------------------------\n");
        printf("Critical: \n");
        printf("------------------------------------------\n");
        t_start = omp_get_wtime();
        max = paralelno_critical(a, n, niti[i], chunks[i], 0);
        t_end = omp_get_wtime();
        t = t_end - t_start;
        printf("Schedual: static Max el: %f, vreme izvrenja: %.8f\n", max, t);
        printf("------------------------------------------\n");
        t_start = omp_get_wtime();
        max = paralelno_critical(a, n, niti[i], chunks[i], 1);
        t_end = omp_get_wtime();
        t = t_end - t_start;
        printf("Schedual: dynamic Max el: %f, vreme izvrenja: %.8f\n", max, t);
        printf("------------------------------------------\n");
        t_start = omp_get_wtime();
        max = paralelno_critical(a, n, niti[i], chunks[i], 2);
        t_end = omp_get_wtime();
        t = t_end - t_start;
        printf("Schedual: guided Max el: %f, vreme izvrenja: %.8f\n", max, t);
        printf("------------------------------------------\n");

    }
    return 0;
}