// Svaki proces posalje 2 elementa, root ih skupi u jedan niz.
#include <mpi.h>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int dio[2] = {rank * 10, rank * 10 + 1};
    std::vector<int> svi(2 * size);   // bitan samo na rootu

    MPI_Gather(dio, 2, MPI_INT, svi.data(), 2, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Root skupio:";
        for (int x : svi) std::cout << " " << x;
        std::cout << std::endl;
    }

    MPI_Finalize();
}
