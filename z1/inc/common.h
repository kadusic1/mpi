#pragma once

#include <climits>

// Zadnje 4 cifre broja indeksa. TODO: upisati svoje.
constexpr int SEED = 1234;

// Izvor mjerenja: TEST niz (argument "test") ili mjerenje(i, n, seed).
struct Data {
    bool test;
    int seed;
    long long n;

    Data(int argc, char* argv[]);
    int operator[](long long i) const;
};

// Dio niza [lo, hi) koji obrađuje jedan proces.
struct Block {
    long long lo, hi;

    // Blok procesa `rank` kad se n mjerenja dijeli na `size` procesa.
    static Block of(int rank, int size, long long n);
};

// Rastući period: dužina i indeks prvog mjerenja.
struct Period {
    long long len = 0;
    long long start = LLONG_MAX;  // {0, LLONG_MAX} znači "nema perioda"

    // Duži period je bolji; kod iste dužine bolji je raniji početak.
    bool better_than(const Period& other) const;
};

// Najduži period koji POČINJE u bloku. Period smije završiti iza block.hi,
// a rep perioda koji je počeo lijevo od bloka se preskače.
Period longest_starting_in(const Data& data, Block block);

// Isti ispis za serijski i paralelni program.
void print_result(const Period& p, double seconds);
