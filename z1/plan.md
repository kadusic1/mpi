# Plan: najduži period rasta (pristup C)

Pregled svih razmotrenih pristupa: [approaches.html](https://claude.ai/artifact/DVmv4p5uZUjERPPoooXyyC)

## Cilj

Za niz mjerenja ispisati **dužinu** i **početak** najdužeg strogo rastućeg perioda.
Kod jednakih dužina ispisuje se manji početak.

- `test`: TEST niz od 23 elementa, očekivano **7 / 9** za 1–8 procesa
- bez argumenta: `n = broj_mjerenja(SEED)`, `mjerenje(i, n, SEED)`

Pristup **C**: period pripada procesu u čijem bloku počinje. Na kraju se rezultati spoje sa `MPI_Allreduce(MAX)` i `MPI_Reduce(MIN)`.
Koriste se samo funkcije iz `examples/` i `MPI_Wtime`.

## Struktura

```
z1/
├── inc/
│   ├── blackbox.h    deklaracije broj_mjerenja, mjerenje
│   └── common.h      structovi, SEED
├── src/
│   ├── blackbox.cc   black box iz zadatka, bez izmjena
│   ├── common.cc     TEST, implementacije
│   ├── serial.cc     main: serijsko rješenje
│   └── parallel.cc   main: MPI rješenje
├── build/            build output (.o, .d, serial, parallel), ignorisan u gitu
└── Makefile
```

## blackbox.h / blackbox.cc

`mix`, `broj_mjerenja` i `mjerenje` su kopirani iz zadatka bez izmjena. `mix` je `static inline`, pa je vidljiv samo u `blackbox.cc`.
Header deklariše samo `broj_mjerenja` i `mjerenje`, a koristi ga jedino `common.cc`.

## common.h / common.cc

Ovdje je sav zajednički kod, da serial i parallel ništa ne dupliraju.

| Element | Opis |
|---|---|
| `SEED` | `constexpr int`, zadnje 4 cifre indeksa (**TODO: upisati**) |
| `TEST[23]` | test niz |
| `struct Data` | `test`, `n`, `seed`; konstruktor `Data(argc, argv)` čita argument `test`; `int operator[](long long i) const` vraća `TEST[i]` ili `mjerenje(i, n, seed)` |
| `struct Block` | `lo`, `hi`; `static Block of(rank, size, n)` → `[rank·n/size, (rank+1)·n/size)` |
| `struct Period` | `len`, `start`; `bool better_than(const Period&)`: duži, ili iste dužine s manjim početkom |
| `Period longest_starting_in(const Data&, Block)` | skenira periode koji **počinju** u bloku (detaljno ispod) |
| `void print_result(const Period&, double sec)` | ispis rezultata u istom formatu za oba programa |

**DRY:** serijsko rješenje je `longest_starting_in(data, {0, n})`, isti algoritam kao paralelni sa jednim blokom.

### `longest_starting_in` (jedan prolaz, O(1) memorije)

1. `i = lo`. Ako je `lo > 0`, uporedi `data[lo]` sa `data[lo-1]`. Dok niz raste, preskači: taj rep pripada lijevom susjedu.
2. Za svaki period koji počne na `i < hi`: prati ga dok raste, i preko `hi` ako treba (do `n`).
3. Pamti najbolji period sa `better_than`. Prazan ili potpuno preskočen blok vraća `{0, LLONG_MAX}`.
4. Svaku vrijednost čitaj samo jednom: čuvaj `prev` i `x` i ne pozivaj `data[i]` dva puta.

## serial.cc

```
Data d(argc, argv)
t0 = steady_clock::now()
Period p = longest_starting_in(d, {0, d.n})
print_result(p, t1 - t0)
```

Bez MPI. Vrijeme se mjeri sa `std::chrono`.

## parallel.cc

```
MPI_Init, MPI_Comm_rank, MPI_Comm_size
Data d(argc, argv)                       // svaki proces sam računa n i mjerenja
t0 = MPI_Wtime()
Period local = longest_starting_in(d, Block::of(rank, size, d.n))
MPI_Allreduce(&local.len, &L, MPI_LONG_LONG, MPI_MAX)            // svi znaju L
cand = (local.len == L) ? local.start : LLONG_MAX
MPI_Reduce(&cand, &start, MPI_LONG_LONG, MPI_MIN, root 0)        // najmanji početak
t1 = MPI_Wtime()
rank 0: print_result({L, start}, t1 - t0)
MPI_Finalize
```

Podaci se ne šalju. Šalju se samo 2 broja po procesu.

## Makefile

| Target | Šta radi |
|---|---|
| `all` | gradi `serial` i `parallel` |
| `serial` | `g++` ← `serial.o common.o blackbox.o` |
| `parallel` | `mpicxx` ← `parallel.o common.o blackbox.o` |
| `run-serial` | `./serial` |
| `run-parallel` | `mpirun -n $(NP) ./parallel` (`NP ?= 4`) |
| `test` | `./serial test` + `mpirun --oversubscribe -n p ./parallel test` za p = 1…8 |
| `compare` | prava mjerenja: `parallel` za p = 1…8 mora dati isto što i `serial` |
| `bench` | `./serial` + `parallel` za p = 1, 2, 4, 8 |
| `clean` | briše `build/` |

- Flagovi: `-O2 -std=c++17 -Wall -Wextra`
- `-Iinc`; zavisnosti od headera automatski preko `-MMD -MP` (`build/*.d`).
- `common.o` i `blackbox.o` se grade jednom i linkuju u oba programa; sav output ide u `build/`.
- `build/` je u `.gitignore`.

## Provjera

- [ ] `make test`: svi redovi daju `duzina = 7, pocetak = 9`
- [ ] `make compare`: na pravim podacima `serial` i `parallel` daju isto za p = 1…8
- [ ] rubni slučajevi (approaches.html → „Primjer korak po korak“): jednake vrijednosti, nerijeseno, rekord na kraju niza, niz koji samo opada, jedan element, period preko više blokova, p > n
- [ ] `make bench`: zapisati vremena i ubrzanje

## Redoslijed rada

1. `common.h` / `common.cc` + `serial.cc`, pa provjera `./serial test` (7 / 9)
2. `parallel.cc`, pa `make test` za 1–8 procesa
3. Upisati pravi `SEED`, pa uporediti serial i parallel na pravim podacima
4. `make bench`, zapisati rezultate
