# MPI vježbe

- [cs.md](cs.md) - cheat sheet za MPI funkcije
- [examples/](examples/) - po jedan jednostavan primjer za svaku funkciju

## Kompajliranje i pokretanje

```bash
mpicxx <fajl>.cc -o <program>
mpirun -np <broj_procesa> ./<program>
```

Ako ima više procesa nego jezgri, dodaj `--oversubscribe` uz `mpirun`.
