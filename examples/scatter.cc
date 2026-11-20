// Root podijeli niz, svaki proces dobije 2 elementa.
#include <mpi.h>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<int> svi;
    if (rank == 0) {
        svi.resize(2 * size);
        for (int i = 0; i < 2 * size; i++) svi[i] = i + 1;   // 1, 2, 3, ...
    }

    int dio[2];
    MPI_Scatter(svi.data(), 2, MPI_INT, dio, 2, MPI_INT, 0, MPI_COMM_WORLD);

    std::cout << "Proces " << rank << ": {" << dio[0] << ", " << dio[1] << "}" << std::endl;

    MPI_Finalize();
}
