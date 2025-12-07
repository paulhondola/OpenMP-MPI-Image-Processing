CLUSTERS = 1
THREADS = 4
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
BIN = bin/main

build:
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

execute:
	$(RUN) -n $(CLUSTERS) $(BIN) $(THREADS)

run: setup build execute

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
	mkdir -p data/times
	mkdir -p data/times/serial
	mkdir -p data/times/parallel
	mkdir -p data/plots
	mkdir -p data/plots/serial
	mkdir -p data/plots/parallel
	@echo "Done."

.PHONY: setup run execute build clean
