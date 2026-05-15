/*
Napisati MPI program koji realizuje mnozenje marice Anxk i matrice Bkxm, cime se dobija
rezultujuca matrica C. Mnozenje se obavlja tako sto master proces salje svakom procesu 
celu matricu A i po m/p kolona matrice B(p-broj procesa, m deljovo sa p). Elementi svih
kolona matrice B koji se salju svakom procesu, salju se odjednom. Svi procesi ucestvuju
u izracunavanjima potrebnim za generisanje rezultata programa. KOnacni rezultat mnozenja
se nalazi u procesu koji ga i prikazuje, a koji sadrzi maksimalnu vrednost elemenata
u matrici B, nakon raspodele kolona po procesima. Zadatak realizovati koriscenjem grupnih
operacija. Dati primer A i B za proizvoljno n, k i m i ilustrovati podelu podataka po 
procesima, kao i nacin generisanja rezultujuce matrice C.
*/

#include "mpi.h"
#include <stdio.h>
#include <float.h>

#define N 2
#define K 3
#define M 4

int main(int argc, char* argv[]){
    int rank, size;
    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    int m = M/size;

    if( M % size != 0){
        if(rank == 0) 
            printf("M mora biti deljivo sa brojem procesa");
            MPI_Finalize();
            return 0;
    }
    
    double A[N][K], B[K][M], C[N][M];
    double local_B[K][m], local_C[N][m];

    MPI_Datatype col_vector, col_type;
    MPI_Type_vector(K, m, M, MPI_DOUBLE, &col_vector);
    MPI_Type_create_resized(col_vector, 0, m*sizeof(double), &col_type);
    MPI_Type_commit(&col_type);

    if(rank == 0){
         for (int i = 0; i < N; i++)
            for (int j = 0; j < K; j++) A[i][j] = i + j + 1.0;
        for (int i = 0; i < K; i++)
            for (int j = 0; j < M; j++) B[i][j] = 1.0;
        B[K][M - 1] = 99.0; // Test max vrednost u poslednjem procesu
    }

    // Slanje podataka grupnim operacijama ostalim procesima
    MPI_Bcast(A, N*K, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, 1, col_type, local_B, K * m, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Lokalno mozenje, svako racuna svoj vertikalni isecak matrice C
    for(int i = 0; i < N; i++){
        for(int j = 0; j < m; j++){
            local_C[i][j] = 0;
            for(int r = 0; r < K; r++) local_C[i][j] += A[i][r] * local_B[r][j];
        }
    }



    MPI_Datatype rezultat_tip, rezultat_vektor;
    // Type_vector uzima N redova gde je svaki blok dugavak m elematana ali da preskovi M elemenata/mesat
    MPI_Type_vector(N, m, M, MPI_DOUBLE, &rezultat_vektor);
    // Type_resize - sledeci proces stavlja svoje izracunate elemente odmah na mestu gde je stao prethodni
    MPI_Type_create_resized(rezultat_vektor, 0, m*sizeof(double), &rezultat_tip);
    MPI_Type_commit(&rezultat_tip);

    MPI_Gather(local_C, N*m, MPI_DOUBLE, C, 1, rezultat_tip, 0, MPI_COMM_WORLD);

    if(rank == 0){
        printf("Rezultat: \n");
        for(int i = 0; i < N; i++){
            for(int j = 0; j < M; j++)
                printf("%.1f ", C[i][j]);
            printf("\n");
        }
    }
    MPI_Finalize();
}
