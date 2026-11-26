// Paralelno rješenje (pristup C): svaki proces sam računa svoja mjerenja i
// traži najduži period koji počinje u njegovom bloku. Podaci se ne šalju,
// samo dva broja po procesu.
#include <mpi.h>

#include "common.h"

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const Data data(argc, argv);  // svaki proces zna n i može računati mjerenja

    double t0 = MPI_Wtime();
    Period local = longest_starting_in(data, Block::of(rank, size, data.n));

    // 1) Najveća dužina, poznata svim procesima.
    long long len;
    MPI_Allreduce(&local.len, &len, 1, MPI_LONG_LONG, MPI_MAX, MPI_COMM_WORLD);

    // 2) Najmanji početak među periodima te dužine; ostali šalju LLONG_MAX.
    long long candidate = (local.len == len) ? local.start : LLONG_MAX;
    long long start;
    MPI_Reduce(&candidate, &start, 1, MPI_LONG_LONG, MPI_MIN, 0, MPI_COMM_WORLD);
    double t1 = MPI_Wtime();

    if (rank == 0) print_result({ len, start }, t1 - t0);

    MPI_Finalize();
}
