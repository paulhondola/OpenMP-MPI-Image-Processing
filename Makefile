CLUSTERS = 8
THREADS = 8
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

build:
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

run_serial: setup build
	$(BIN) -serial

run_multithreaded: setup build
	$(BIN) -threads $(THREADS) -multithreaded

run_distributed: setup build
	$(RUN) -n $(CLUSTERS) $(BIN) -threads $(THREADS) -distributed

run_shared: setup build
	$(RUN) -n $(CLUSTERS) $(BIN) -threads $(THREADS) -shared

run_task_pool: setup build
	$(RUN) -n $(CLUSTERS) $(BIN) -threads $(THREADS) -task_pool

run_all: setup build
	$(RUN) -n $(CLUSTERS) $(BIN) -threads $(THREADS) -all

CLUSTER_ARRAY = 2 4 8
THREAD_ARRAY = 2 4 8

sweep:
	for C in $(CLUSTER_ARRAY); do \
		for T in $(THREAD_ARRAY); do \
			echo "Running with CLUSTERS=$$C, THREADS=$$T"; \
			$(RUN) -n $$C $(BIN) -t $$T -a; \
		done; \
	done

plot:
	python3 data/plot_benchmark.py

clean:
	find images -mindepth 1 -maxdepth 1 -not -name 'base' -exec rm -rf {} +
	rm -rf data/chronos
	rm -rf data/speedups
	rm -rf data/plots
	rm -f $(BIN)

setup:
	@echo "Setting up the project..."
	mkdir -p bin
	mkdir -p src
	mkdir -p docs
	mkdir -p images
	mkdir -p images/base
	mkdir -p data
	mkdir -p data/chronos
	mkdir -p data/speedups
	@echo "Done."

.PHONY: setup execute build clean plot run_serial run_multithreaded run_distributed run_shared run_task_pool run_all
