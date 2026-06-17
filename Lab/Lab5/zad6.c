#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

void inicijalizacija(double *a, int n){
    for(int i = 0; i < n; i++){
        a[i]=i;
    }
}

double sekvencijalno(double *a, int n){
    double sum = 0.0;
    for(int i = 0; i < n; i++){
        sum+=a[i];
    }
    sum/=n;
    return sum;
}

double par_reduction(double *a, int n, int nt, int chunk, int sch){
    double sum=0.0;
    int i;
    omp_set_num_threads(nt);
    if(sch == 0){
        #pragma omp parallel for reduction(+:sum) private(i) schedule(static, chunk)
        for(i = 0; i < n; i++)
            sum += a[i];
    }else if(sch == 1){
        #pragma omp parallel for reduction(+:sum) private(i) schedule(dynamic, chunk)
        for(i = 0; i < n; i++)
            sum += a[i];
    }else{
        #pragma omp parallel for reduction(+:sum) private(i) schedule(guided, chunk)
        for(int i = 0; i < n; i++)
            sum += a[i];
    }
    return sum/n;
}

int main(){
    int dim[] = {1000000, 2000000, 3000000};
    int br_dim = 3;
    int niti[] = {2,4,6};
    int bt_niti = 3;
    int chunks[] = {100, 1000, 10000};

    for(int d = 0; d < br_dim; d++){
        int n = dim[d];
        double *a = (double*)malloc(n*sizeof(double));   
        inicijalizacija(a, n);

        printf("DImenzija vektora: %d\n", n);
        printf("----------------------------------------------------------\n");
        double t_start = omp_get_wtime();
        double sum = sekvencijalno(a, n);
        double t_end = omp_get_wtime();
        double t_sek = t_end - t_start;
        printf("Sekvencijalno:  prosek: %.4f, vreme izvresnja: %.6f\n", sum, t_sek);
    
        printf("----------------------------------------------------------\n");
        printf("\t\t\tReduction\n");
        printf("----------------------------------------------------------\n");
        printf("Schedule: static ");
        t_start = omp_get_wtime();
        sum = par_reduction(a, n, niti[d], chunks[d], 0);
        t_end = omp_get_wtime();
        t_sek = t_end - t_start;
        printf("prosek: %.4f, vreme izvresnja: %.6f\n", sum, t_sek);

        printf("Schedule: dynamic ");
        t_start = omp_get_wtime();
        sum = par_reduction(a, n, niti[d], chunks[d], 1);
        t_end = omp_get_wtime();
        t_sek = t_end - t_start;
        printf("prosek: %.4f, vreme izvresnja: %.6f\n", sum, t_sek);

        printf("Schedule: guided ");
        t_start = omp_get_wtime();
        sum = par_reduction(a, n, niti[d], chunks[d], 2);
        t_end = omp_get_wtime();
        t_sek = t_end - t_start;
        printf("prosek: %.4f, vreme izvresnja: %.6f\n", sum, t_sek);
        printf("----------------------------------------------------------\n");

    }


    return 0;
}