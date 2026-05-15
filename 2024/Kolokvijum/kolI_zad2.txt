/*Isti zadatak kao u zadatku jedan samo je potrebno distribuciju kolona matrice B realizovati koriscenjem PtP operacija*/
#include <mpi.h>
#include <stdio.h>

#define N 2 // Redovi matrice A
#define K 3 // Kolone A / Redovi B
#define M 4 // Kolone matrice B

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status status;

    int m = M / size; // Broj kolona B po procesu

    if (M % size != 0) {
        if (rank == 0) printf("Greska: M mora biti deljivo sa brojem procesa.\n");
        MPI_Finalize(); return 0;
    }

    double A[N][K], B[K][M], C[N][M];
    double local_B[K][m], local_C[N][m];

    // 1. Izvedeni tip za kolone matrice B (neophodan za mastera kod slanja)
    MPI_Datatype col_vector, col_type;
    MPI_Type_vector(K, m, M, MPI_DOUBLE, &col_vector);
    MPI_Type_create_resized(col_vector, 0, m * sizeof(double), &col_type);
    MPI_Type_commit(&col_type); // OBAVEZAN COMMIT pre upotrebe [1]

    if (rank == 0) {
        // Inicijalizacija matrica
        for (int i = 0; i < N; i++)
            for (int j = 0; j < K; j++) A[i][j] = i + j + 1.0;
        for (int i = 0; i < K; i++)
            for (int j = 0; j < M; j++) B[i][j] = 1.0;
        B[K - 1][M - 1] = 99.0; // Test vrednost na kraju matrice
    }

    // 2. Distribucija podataka
    MPI_Bcast(A, N * K, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Master kopira svoj deo kolona (nulta grupa kolona)
        for (int i = 0; i < K; i++)
            for (int j = 0; j < m; j++) local_B[i][j] = B[i][j];

        // Master šalje ostalima (PtP) koristeći izvedeni tip [2]
        for (int p = 1; p < size; p++) {
            MPI_Send(&B[p * m], 1, col_type, p, 0, MPI_COMM_WORLD);
        }
    } else {
        // Radnici primaju podatke u kontinualni lokalni bafer [2]
        MPI_Recv(local_B, K * m, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
    }

    // 3. Lokalno množenje (svaki proces računa svoj vertikalni isečak matrice C)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < m; j++) {
            local_C[i][j] = 0;
            for (int r = 0; r < K; r++) {
                local_C[i][j] += A[i][r] * local_B[r][j];
            }
        }
    }

    // 4. Sakupljanje rezultata u proces 0
    MPI_Datatype res_vector, res_type;
    MPI_Type_vector(N, m, M, MPI_DOUBLE, &res_vector);
    MPI_Type_create_resized(res_vector, 0, m * sizeof(double), &res_type);
    MPI_Type_commit(&res_type);

    MPI_Gather(local_C, N * m, MPI_DOUBLE, C, 1, res_type, 0, MPI_COMM_WORLD); 

    if (rank == 0) {
        printf("Finalna matrica C (sakupljena u masteru):\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) printf("%6.1f ", C[i][j]);
            printf("\n");
        }
    }

    MPI_Type_free(&col_type);
    MPI_Type_free(&res_type);
    MPI_Finalize();
    return 0;
}

