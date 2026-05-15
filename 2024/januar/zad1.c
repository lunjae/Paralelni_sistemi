/*
Napisati MPI program koji realizuje mnozenje matrica Akxn i vektora bn.
Matrica i vektor se inicijalizuju u master procesu. Matrica je podeljena
u blokove po vrstama tako da ce proces Pi dobiti prvih 2nai vrsta, proces
Pi+1 dobice sledecih 2nai+1 vrsta, itd. Vektor b se u celosti salje svim
procesima. Predvideti da se slanje blokova matrica svakom procesu
salje jednim MPI_Send pozivom koji se salju svi neophodni elementi matrice
dok se slanje vektora b obavlja grupnim operacijama. Svaki proces obavlja
odgovarajuca izracunavanja i ucestvuje u generisanju rezultata. Rezultujuci
vektor d treba se naci u procesu koji je ucitao najvise vrsta matrice A.
Dati primer matrice A i vektora b i ilustrovati podelu podataka po procesima,
kao i iygled rezultata za izabran broj procesa. Napisaiti na koj nacin se 
iz komandne linije vrsi napisane MPI aplikacije.

Razmisljanje resavanja zadatka: Da bi se zahtev ispunio da svaki proces dobije 2^i vrsti matrice A, potrebno je da ima 2^p - 1 vrsti.
*/

#include "mpi.h"
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char* argv[]){
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);  

    MPI_Status status; 

    int n = 4, k=(1<<size)-1;
    int t = (1 << rank); // Broj tekucih vrsti u trenutnom procesu, tim se kreira lokalna matrica A koja je one velicine koja joj je potrebna da primi 2^i elemnata
    int offset; // Pamti gde je stao u matrici A kada salje vrste
    int bre = t * n; // Broj elemenata za slanje
    
    int A[k][n], b[n], d[k];
    int lok_A[t][n], lok_d[t];

   if(rank == 0){
        for (int i = 0; i < k; i++)
            for (int j = 0; j < n; j++) A[i][j] = i + j + 1;
        for (int i = 0; i < n; i++) b[i] = 10;

        memcpy(lok_A, A, t*n*sizeof(int));

        int offset = 1; // Slanje pocinje od druge vrste
        for(int p = 1; p < size; p++){
            int brv = (1 << p);
            MPI_Send(&A[offset], brv*n, MPI_INT, p, 0, MPI_COMM_WORLD);
            offset += brv;
        }
   }else{
        MPI_Recv(lok_A, bre, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
   }

    MPI_Bcast(b, n, MPI_INT, 0, MPI_COMM_WORLD);

    for(int i = 0; i < t; i++){
        lok_d[i] = 0;
        for(int j = 0; j < n; j++)
            lok_d[i] += lok_A[i][j]*b[j];
    }

    int master = size-1;

    if(rank != master){
        MPI_Send(lok_d, t, MPI_INT, master, 1, MPI_COMM_WORLD);
    }else{
        int moj_offset = (1 << rank) - 1;
        for(int i = 0; i < t; i++) d[moj_offset+i] = lok_d[i];
        
        for(int i = 0; i < size-1; i++){
            int tudje_t = (1 << i);
            int tudji_offset = (1 << i)-1; // 2^i - 1
            MPI_Recv(&d[tudji_offset], tudje_t, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
        }
        
        printf("Vektor d (u procesu %d):\n", rank);
        for (int i = 0; i < k; i++) printf("%d ", d[i]);
        printf("\n");
    }

    MPI_Finalize();
}
