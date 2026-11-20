// Suma rankova, rezultat dobiju svi procesi.
#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int suma = 0;
    MPI_Allreduce(&rank, &suma, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    std::cout << "Proces " << rank << ": suma = " << suma << std::endl;

    MPI_Finalize();
}
