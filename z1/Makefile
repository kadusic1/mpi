# Kompajleri: serijski bez MPI-ja, paralelni preko mpicxx.
CXX      := g++
MPICXX   := mpicxx
CXXFLAGS := -O2 -std=c++17 -Wall -Wextra
# Gasi zastarjele MPI C++ bindinge (Open MPI) koji bacaju upozorenja.
MPIFLAGS := -DOMPI_SKIP_MPICXX
NP       ?= 4
ARGS     ?=
EXPECT   := duzina = 7, pocetak = 9

# Izvori u src/, headeri u inc/, sav build output (.o, .d, programi) u build/.
SRC      := src
INC      := inc
BUILD    := build
SERIAL   := $(BUILD)/serial
PARALLEL := $(BUILD)/parallel

# -MMD -MP: kompajler sam zapiše od kojih headera zavisi svaki .o (build/*.d).
CPPFLAGS := -I$(INC) -MMD -MP

.PHONY: all run-serial run-parallel test compare bench clean

all: $(SERIAL) $(PARALLEL)

$(BUILD):
	mkdir -p $@

# Zajednički objekti se grade jednom i linkuju u oba programa.
COMMON   := $(BUILD)/common.o $(BUILD)/blackbox.o

# Svi .o osim parallel.o se grade običnim g++.
$(BUILD)/%.o: $(SRC)/%.cc | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# parallel.o treba mpi.h, pa ide preko mpicxx.
$(BUILD)/parallel.o: $(SRC)/parallel.cc | $(BUILD)
	$(MPICXX) $(CPPFLAGS) $(CXXFLAGS) $(MPIFLAGS) -c $< -o $@

$(SERIAL): $(BUILD)/serial.o $(COMMON)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(PARALLEL): $(BUILD)/parallel.o $(COMMON)
	$(MPICXX) $(CXXFLAGS) $^ -o $@

# Primjer: make run-parallel NP=8 ARGS=test
run-serial: $(SERIAL)
	./$(SERIAL) $(ARGS)

run-parallel: $(PARALLEL)
	mpirun -n $(NP) ./$(PARALLEL) $(ARGS)

# Test niz mora dati "duzina = 7, pocetak = 9" za 1..8 procesa.
test: $(SERIAL) $(PARALLEL)
	@./$(SERIAL) test | grep -q "$(EXPECT)" && echo "serial          OK" || { echo "serial          GRESKA"; exit 1; }
	@for p in 1 2 3 4 5 6 7 8; do \
	  mpirun --oversubscribe -n $$p ./$(PARALLEL) test | grep -q "$(EXPECT)" \
	    && echo "parallel p=$$p    OK" || { echo "parallel p=$$p    GRESKA"; exit 1; }; \
	done

# Prava mjerenja: paralelni rezultat mora biti isti kao serijski za 1..8 procesa.
compare: $(SERIAL) $(PARALLEL)
	@ref=$$(./$(SERIAL) | head -1); echo "serial: $$ref"; \
	for p in 1 2 3 4 5 6 7 8; do \
	  got=$$(mpirun --oversubscribe -n $$p ./$(PARALLEL) | head -1); \
	  [ "$$got" = "$$ref" ] && echo "parallel p=$$p    OK" || { echo "parallel p=$$p    GRESKA: $$got"; exit 1; }; \
	done

# Vremena za poređenje i ubrzanje.
bench: $(SERIAL) $(PARALLEL)
	@echo "== serial"; ./$(SERIAL)
	@for p in 1 2 4 8; do echo "== parallel p=$$p"; mpirun -n $$p ./$(PARALLEL); done

clean:
	rm -rf $(BUILD)

# Zavisnosti od headera koje generiše -MMD.
-include $(wildcard $(BUILD)/*.d)
