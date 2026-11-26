#pragma once

// Black box funkcije iz zadatka (implementacija u blackbox.cc).

// Broj mjerenja n za dati seed.
long long broj_mjerenja(int seed);

// i-to mjerenje (0 <= i < n), u desetinkama stepena.
int mjerenje(long long i, long long n, int seed);
