// Svaki proces posalje 2 elementa, svi dobiju skupljeni niz.
#include <mpi.h>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int dio[2] = {rank * 10, rank * 10 + 1};
    std::vector<int> svi(2 * size);

    MPI_Allgather(dio, 2, MPI_INT, svi.data(), 2, MPI_INT, MPI_COMM_WORLD);

    std::cout << "Proces " << rank << ":";
    for (int x : svi) std::cout << " " << x;
    std::cout << std::endl;

    MPI_Finalize();
}
