// Root (0) postavi n, Bcast ga posalje svima.
#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int n = 0;
    if (rank == 0) n = 100;
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::cout << "Proces " << rank << ": n = " << n << std::endl;

    MPI_Finalize();
}
