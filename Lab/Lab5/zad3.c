#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "omp.h"

int prost(int n){
    if (n < 2) return 0;
    if(n == 2) return 1;
    if(n % 2 == 0) return 0;
    int sq = (int)sqrt(n);
    for(int i = 3; i <= sq; i += 2)
        if(n % i == 0) return 0;

    return 1;
}

long seq(int n){
    long br = 0;
    for(int i = 2; i <= n; i++){
        if(prost(i)) br++;
    }
    return br;
}

long par_static(int n, int nt, int chunk){
    omp_set_num_threads(nt);
    long br = 0;
    int i;

    #pragma omp parallel for schedule(static, chunk) private(i) shared(br)
    for(i = 1; i <= n; i++){
        if(prost(i)){
            #pragma omp critical
            br++;
        }
    }
    return br;
}

long par_dynamic(int n, int nt, int chunk){
    omp_set_num_threads(nt);
    long br = 0;
    int i;
    
    #pragma omp parallel for schedule(dynamic, chunk) private(i) shared(br)
    for(i = 1; i < n; i++){
        if(prost(i)){
            #pragma omp critical
            br++;
        }
    }

    return br;
}

long par_gid(int n, int nt, int chunk){
    omp_set_num_threads(nt);
    long br = 0;
    int i;

    #pragma omp parallel for schedule(guided, chunk) private(i) shared(br)
    for(i = 1; i < n; i++){
        if(prost(i)){
            #pragma omp critical
            br++;
        }
    }
    return br;
}

int main(){
    int brojevi[] = {400000, 520000, 1230412304};
    int niti[] = {2,4,6};
    int chunks[] = {1, 100, 500};
    
    for(int ni = 0; ni < 3; ni++){
        int n = brojevi[ni];
        double t_start = omp_get_wtime();
        double seq_cnt = seq(n);
        double t_end = omp_get_wtime();
        double t_seq = t_end - t_start;
        printf("Sekvencijalno: count=%ld  T=%.4fs\n\n", seq_cnt, t_seq);
        printf("----------------------------------------'n");
    }

    for (int ti = 0; ti < 3; ti++) {
          for (int ki = 0; ki < 3; ki++) {
                for (int ci = 0; ci < 3; ci++) {
                    int nt = niti[ti], chunk = chunks[ci];
                    double t_start = omp_get_wtime();
        
       
                }
          }
      }

}