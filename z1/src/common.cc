#include "common.h"

#include <cstring>
#include <iostream>
#include <iterator>

#include "blackbox.h"

// Test niz iz zadatka; očekivano: dužina 7, početak 9.
static const int TEST[23] = { 50, 12, 15, 21, 30, 31, 44, 40, 38, -10, -4,
                              0, 3, 9, 17, 26, 20, 22, 25, 27, 33, 36, 41 };

Data::Data(int argc, char* argv[])
    : test(argc > 1 && std::strcmp(argv[1], "test") == 0),
      seed(SEED),
      n(test ? (long long)std::size(TEST) : broj_mjerenja(seed))
{
}

int Data::operator[](long long i) const
{
    return test ? TEST[i] : mjerenje(i, n, seed);
}

Block Block::of(int rank, int size, long long n)
{
    // r*n/size ravnomjerno dijeli i kad n nije djeljiv sa size.
    return { rank * n / size, (rank + 1) * n / size };
}

bool Period::better_than(const Period& other) const
{
    return len > other.len || (len == other.len && start < other.start);
}

Period longest_starting_in(const Data& data, Block block)
{
    Period best;  // ostaje {0, LLONG_MAX} ako u bloku ne počne nijedan period
    long long i = block.lo;
    if (i >= block.hi) return best;  // prazan blok (više procesa nego mjerenja)

    int x = data[i];  // svaka vrijednost se računa samo jednom

    // 1) Ako blok počinje usred rastućeg perioda, taj period pripada lijevom
    //    susjedu: preskoči ga dok niz raste.
    if (i > 0) {
        int prev = data[i - 1];
        while (x > prev) {
            if (++i >= block.hi) return best;  // cijeli blok je tuđi rep
            prev = x;
            x = data[i];
        }
    }

    // 2) Obradi svaki period koji počinje u [i, hi). Ovdje uvijek vrijedi
    //    x == data[i]. Zadnji period smije nastaviti iza hi, sve do n.
    while (i < block.hi) {
        Period cur{ 1, i };
        int prev = x;
        while (++i < data.n && (x = data[i]) > prev) {
            prev = x;
            cur.len++;
        }
        // Strogo "bolji": kod iste dužine ostaje raniji period.
        if (cur.better_than(best)) best = cur;
    }
    return best;
}

void print_result(const Period& p, double seconds)
{
    std::cout << "duzina = " << p.len << ", pocetak = " << p.start << "\n"
              << "vrijeme = " << seconds << " s\n";
}
