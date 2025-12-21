CLUSTERS = 10
THREADS = 10
CC = mpicc
RUN = mpirun
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	OMP_FLAGS = -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include -L/opt/homebrew/opt/libomp/lib -lomp
else
	OMP_FLAGS = -fopenmp
endif

ERROR_FLAGS = -Wall -Wextra
PERFORMANCE_FLAGS = -O3 -ffast-math -flto
CFLAGS = $(OMP_FLAGS) $(ERROR_FLAGS) $(PERFORMANCE_FLAGS)

SRC = $(shell find src -name "*.c")
BIN = bin/mpi_omp_convolution

run: setup build execute

build:
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

execute:
	$(RUN) -n $(CLUSTERS) $(BIN) $(THREADS)

CLUSTER_ARRAY = (2 4 8)
THREAD_ARRAY = (2 4 8 10)

sweep:
	for C in $(CLUSTER_ARRAY); do
		for T in $(THREAD_ARRAY); do
			echo "Running with CLUSTERS=$C, THREADS=$T"
			$(RUN) -n $C $(BIN) $T
		done
	done

plot:
	python3 data/plot_benchmark.py

clean:
	find images -mindepth 1 -maxdepth 1 -not -name 'base' -exec rm -rf {} +
	rm -f $(BIN)

setup:
	@echo "Setting up the project..."
	mkdir -p docs
	mkdir -p src
	mkdir -p bin
	mkdir -p data
	mkdir -p images
	mkdir -p images/base
	@echo "Done."

.PHONY: setup run execute build clean plot
