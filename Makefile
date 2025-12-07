CLUSTERS = 1
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
BIN = bin/main

run:
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)
	$(RUN) -n $(CLUSTERS) $(BIN) $(THREADS)

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
	mkdir -p images/edge
	mkdir -p images/edge/serial
	mkdir -p images/edge/parallel
	mkdir -p images/ridge
	mkdir -p images/ridge/serial
	mkdir -p images/ridge/parallel
	mkdir -p images/sharpen
	mkdir -p images/sharpen/serial
	mkdir -p images/sharpen/parallel
	mkdir -p images/boxblur
	mkdir -p images/boxblur/serial
	mkdir -p images/boxblur/parallel
	mkdir -p images/gaussblur3
	mkdir -p images/gaussblur3/serial
	mkdir -p images/gaussblur3/parallel
	mkdir -p images/gaussblur5
	mkdir -p images/gaussblur5/serial
	mkdir -p images/gaussblur5/parallel
	mkdir -p images/unsharp5
	mkdir -p images/unsharp5/serial
	mkdir -p images/unsharp5/parallel
	mkdir -p images/test
	mkdir -p data/times
	mkdir -p data/times/serial
	mkdir -p data/times/parallel
	mkdir -p data/plots
	mkdir -p data/plots/serial
	mkdir -p data/plots/parallel
	@echo "Done."

.PHONY: setup run
