// Serijsko rješenje: jedan blok preko cijelog niza.
#include <chrono>

#include "common.h"

int main(int argc, char* argv[])
{
    const Data data(argc, argv);

    auto t0 = std::chrono::steady_clock::now();
    Period best = longest_starting_in(data, { 0, data.n });
    std::chrono::duration<double> t = std::chrono::steady_clock::now() - t0;

    print_result(best, t.count());
}
