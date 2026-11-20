# MPI cheat sheet

Primjeri (`mpicxx examples/X.cc -o X && mpirun -np 4 ./X`):
[Send/Recv](examples/send_recv.cc) ·
[Reduce](examples/reduce.cc) ·
[Bcast](examples/bcast.cc) ·
[Allreduce](examples/allreduce.cc) ·
[Scatter](examples/scatter.cc) ·
[Gather](examples/gather.cc) ·
[Allgather](examples/allgather.cc)

```cpp
MPI_Send(buf, count, datatype, dest,   tag, comm);
MPI_Recv(buf, count, datatype, source, tag, comm, &status);  // ili MPI_STATUS_IGNORE
```

- `buf` - podaci (pokazivač)
- `count` - broj elemenata (ne bajtova)
- `datatype` - `MPI_INT`, `MPI_CHAR`, `MPI_DOUBLE`...
- `dest` / `source` - rank procesa
- `tag` - oznaka poruke (`MPI_ANY_TAG` kod prijema)
- `comm` - obično `MPI_COMM_WORLD`

```bash
mpicxx p.cc -o p && mpirun -np 2 ./p
```

## MPI_Reduce

Primjer: [examples/reduce.cc](examples/reduce.cc)

Skuplja vrijednosti iz svih procesa i primjenjuje operaciju (suma, max...). Rezultat dobije samo `root`.

```cpp
MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
```

- `sendbuf` - lokalna vrijednost svakog procesa
- `recvbuf` - rezultat (bitan samo na `root`)
- `count` - broj elemenata (za niz se reducira element po element)
- `op` - `MPI_SUM`, `MPI_MAX`, `MPI_MIN`, `MPI_PROD`
- `root` - rank procesa koji dobije rezultat
- Zovu ga **svi** procesi, ne samo root

Primjer: suma rankova.

```cpp
int rank, size, suma = 0;
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Reduce(&rank, &suma, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
if (rank == 0)
    std::cout << "Suma: " << suma << std::endl;  // -np 4 -> 0+1+2+3 = 6
```

### count > 1

Reducira se **element po element** (po pozicijama), rezultat je niz duzine `count`.

```cpp
int lokalno[3] = {rank, rank * 10, 1};
int rezultat[3];
MPI_Reduce(lokalno, rezultat, 3, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
```

`-np 4`:

| proces | [0] | [1] | [2] |
|--------|-----|-----|-----|
| 0 | 0 | 0 | 1 |
| 1 | 1 | 10 | 1 |
| 2 | 2 | 20 | 1 |
| 3 | 3 | 30 | 1 |
| **root** | **6** | **60** | **4** |

- `count` mora biti isti na svim procesima
- Za jedan zbir cijelog niza: prvo lokalno sumiraj petljom, pa `count = 1`

`MPI_Allreduce` (bez `root`) isto, ali rezultat dobiju svi procesi.

## MPI_Bcast

Primjer: [examples/bcast.cc](examples/bcast.cc)

Root pošalje isti podatak **svim** procesima (jedan prema svima).

```cpp
MPI_Bcast(buf, count, datatype, root, comm);
```

- `buf` - na rootu podaci koje šalje, na ostalima bafer u koji se upisuje
- `count`, `datatype` - kao kod Send/Recv
- `root` - rank procesa koji šalje
- Zovu ga **svi** procesi, s istim `count`, `datatype` i `root`
- Nema taga i nema posebnog Recv

Primjer:

```cpp
int n;
if (rank == 0) n = 100;
MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
// sad svi procesi imaju n == 100
```

## MPI_Allreduce

Primjer: [examples/allreduce.cc](examples/allreduce.cc)

Isto kao `MPI_Reduce`, ali rezultat dobiju **svi** procesi (nema `root`).

```cpp
MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
```

- `sendbuf` - lokalna vrijednost (ili niz) svakog procesa
- `recvbuf` - rezultat; popunjen na **svim** procesima
- `count` - broj elemenata; za niz se reducira element po element
- `datatype` - tip elemenata: `MPI_INT`, `MPI_DOUBLE`...
- `op` - operacija: `MPI_SUM`, `MPI_MAX`, `MPI_MIN`, `MPI_PROD`
- `comm` - komunikator, obično `MPI_COMM_WORLD`
- Zovu ga **svi** procesi, s istim `count`, `datatype` i `op`

Primjer:

```cpp
int suma;
MPI_Allreduce(&rank, &suma, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
// -np 4 -> svi procesi imaju suma == 6
```

## MPI_Scatter

Primjer: [examples/scatter.cc](examples/scatter.cc)

Root podijeli niz na jednake dijelove i svakom procesu pošalje po jedan dio (po rankovima: prvi dio procesu 0, drugi procesu 1...).

```cpp
MPI_Scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
```

- `sendbuf` - niz koji se dijeli (bitan samo na `root`)
- `sendcount` - broj elemenata koji ide **svakom** procesu (ne ukupan broj)
- `sendtype` - tip elemenata koji se šalju: `MPI_INT`, `MPI_DOUBLE`...
- `recvbuf` - bafer u koji svaki proces (i root) primi svoj dio
- `recvcount` - broj elemenata koje svaki proces prima (obično isti kao `sendcount`)
- `recvtype` - tip elemenata koji se primaju (obično isti kao `sendtype`)
- `root` - rank procesa koji dijeli niz
- `comm` - komunikator, obično `MPI_COMM_WORLD`
- Zovu ga **svi** procesi
- `sendbuf` mora imati `sendcount * size` elemenata

Primjer (`-np 4`):

```cpp
int svi[8] = {1,2,3,4,5,6,7,8};   // samo root ga popuni
int dio[2];
MPI_Scatter(svi, 2, MPI_INT, dio, 2, MPI_INT, 0, MPI_COMM_WORLD);
// proces 0: {1,2}  proces 1: {3,4}  proces 2: {5,6}  proces 3: {7,8}
```

## MPI_Gather

Primjer: [examples/gather.cc](examples/gather.cc)

Suprotno od Scatter: svaki proces pošalje svoj dio, a root ih sve skupi u jedan niz (po rankovima: dio procesa 0 prvi, procesa 1 drugi...).

```cpp
MPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
```

- `sendbuf` - dio koji šalje svaki proces (i root šalje svoj)
- `sendcount` - broj elemenata koje šalje **svaki** proces
- `sendtype` - tip elemenata koji se šalju: `MPI_INT`, `MPI_DOUBLE`...
- `recvbuf` - bafer u koji root skuplja sve dijelove (bitan samo na `root`)
- `recvcount` - broj elemenata koje root prima **od svakog** procesa (ne ukupan broj, obično isti kao `sendcount`)
- `recvtype` - tip elemenata koji se primaju (obično isti kao `sendtype`)
- `root` - rank procesa koji skuplja podatke
- `comm` - komunikator, obično `MPI_COMM_WORLD`
- Zovu ga **svi** procesi
- `recvbuf` mora imati `recvcount * size` elemenata

Primjer (`-np 4`):

```cpp
int dio[2] = {rank * 10, rank * 10 + 1};
int svi[8];                        // bitan samo na rootu
MPI_Gather(dio, 2, MPI_INT, svi, 2, MPI_INT, 0, MPI_COMM_WORLD);
// root: svi = {0,1, 10,11, 20,21, 30,31}
```

`MPI_Allgather` (bez `root`) isto, ali skupljeni niz dobiju svi procesi.

## MPI_Allgather

Primjer: [examples/allgather.cc](examples/allgather.cc)

Svaki proces pošalje svoj dio, a skupljeni niz (poredan po rankovima) dobiju **svi** procesi (nema `root`).

```cpp
MPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
```

- `sendbuf` - dio koji šalje svaki proces
- `sendcount` - broj elemenata koje šalje **svaki** proces
- `sendtype` - tip elemenata koji se šalju: `MPI_INT`, `MPI_DOUBLE`...
- `recvbuf` - bafer u koji svaki proces prima skupljeni niz (popunjen na **svim** procesima)
- `recvcount` - broj elemenata koji se primaju **od svakog** procesa (ne ukupan broj, obično isti kao `sendcount`)
- `recvtype` - tip elemenata koji se primaju (obično isti kao `sendtype`)
- `comm` - komunikator, obično `MPI_COMM_WORLD`
- Zovu ga **svi** procesi
- `recvbuf` mora imati `recvcount * size` elemenata na svakom procesu

Primjer (`-np 4`):

```cpp
int dio[2] = {rank * 10, rank * 10 + 1};
int svi[8];
MPI_Allgather(dio, 2, MPI_INT, svi, 2, MPI_INT, MPI_COMM_WORLD);
// svi procesi: svi = {0,1, 10,11, 20,21, 30,31}
```
