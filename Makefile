setup:
	@echo "Setting up the project..."
	mkdir -p docs
	mkdir -p src
	mkdir -p bin
	mkdir -p data
	mkdir -p images
	mkdir -p images/base
	mkdir -p images/edge
	mkdir -p images/ridge
	mkdir -p images/sharpen
	mkdir -p images/boxblur
	mkdir -p images/gaussblur3
	mkdir -p images/gaussblur5
	mkdir -p images/unsharp5
	mkdir -p images/test
	@echo "Done."

CLUSTERS = 2
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

run:
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)
	$(RUN) -n $(CLUSTERS) $(BIN) $(THREADS) images/base/Large.bmp images/boxblur/serial/Large.bmp
	$(RUN) -n $(CLUSTERS) $(BIN) $(THREADS) images/base/XL.bmp images/boxblur/serial/XL.bmp
	$(RUN) -n $(CLUSTERS) $(BIN) $(THREADS) images/base/XXL.bmp images/boxblur/serial/XXL.bmp

.PHONY: setup run