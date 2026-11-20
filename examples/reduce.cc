// Suma rankova svih procesa, rezultat samo na rootu (0).
#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int suma = 0;
    MPI_Reduce(&rank, &suma, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
        std::cout << "Suma rankova: " << suma << std::endl;

    MPI_Finalize();
}
