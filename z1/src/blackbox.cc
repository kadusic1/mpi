// Black box iz zadatka, kopirano bez izmjena.
#include "blackbox.h"

#include <cstdint>

static inline uint32_t mix(uint32_t x)
{
    x ^= x >> 16; x *= 0x7FEB352Du;
    x ^= x >> 15; x *= 0x846CA68Bu;
    x ^= x >> 16;
    return x;
}

long long broj_mjerenja(int seed)
{
    return 50000001LL + 2 * (long long)(mix((uint32_t)seed ^ 0x5BD1E995u) % 50);
}

int mjerenje(long long i, long long n, int seed)
{
    static const uint32_t Q[2] = { 0x9E3779B9u, 0x85EBCA6Bu };
    static const uint32_t R[2] = { 0x1E3779B9u, 0x82EBCA6Bu };
    uint32_t s = mix((uint32_t)seed * 0x9E3779B1u + 0x7F4A7C15u);
    uint32_t v = mix((uint32_t)i ^ s);
    for (int e = 0; e < 2; e++) {
        uint32_t h = mix(s ^ Q[e]);
        long long d = 30 + s % 17 - e * (2 + (s >> 8) % 4);
        long long k = i - (long long)(((unsigned long long)n *
            (Q[e] ^ R[e])) >> 32) + d / 4 + (long long)(h % (d / 2));
        if (k >= 0 && k < d) return -250 + 7 * (int)k + (int)(v % 7);
        if (k == d) return -300;
    }
    return (int)(v % 550) - 200;
}
