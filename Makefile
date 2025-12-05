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
CFLAGS = -openmp -O3 -Wall -Wextra

SRC = src/bmp/bmp_io.c
BIN = bin/bmp_io

test_bmp:
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)
	$(RUN) -n $(CLUSTERS) $(BIN) images/base/Large.bmp images/test/Large.bmp
	$(RUN) -n $(CLUSTERS) $(BIN) images/base/XL.bmp images/test/XL.bmp
	$(RUN) -n $(CLUSTERS) $(BIN) images/base/XXL.bmp images/test/XXL.bmp

.PHONY: setup