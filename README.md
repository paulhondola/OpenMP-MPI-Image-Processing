# OpenMP & MPI Image Processing

This project implements image processing kernels (convolution) using a serial and hybrid parallelization approach with OpenMP and MPI. It benchmarks various convolution kernels (Ridge, Edge, Sharpen, BoxBlur, Gaussian, Unsharp) on BMP images.

## Prerequisites

- **GCC/Clang** with OpenMP support.
- **MPI** (MPICH or OpenMPI).
- **Make**.

## Project Structure

```
├── Makefile        # Compilation and execution commands
├── bin             # Output directory for compiled binary
├── data            # Output directory for benchmark content
├── docs            # Documentation files
├── images          # Input and output directory for BMP images
│   ├── base        # Place input BMP files here
│   └── [kernel]    # Output directories for each kernel
└── src             # Source code
    ├── benchmark   # Benchmarking logic and kernel definitions
    ├── bmp         # BMP file I/O (read/write)
    ├── convolution # Convolution implementation (serial vs hybrid parallel)
    ├── errors      # Error handling (app_error enum)
    ├── file_utils  # File and directory utilities
    ├── kernel      # Kernel struct usage logic
    └── main.c      # Entry point
```

## Setup

Before running the project, create the necessary directory structure:

```bash
make setup
```

This will create `bin`, `data`, and the required `images` subdirectories.

## Build and Run

To compile and run the benchmark (with default 1 cluster, 10 threads):

```bash
make run
```

This command will:
1. Compile the code using `mpicc`.
2. Run the executable with `mpirun`.
3. Process images located in `images/base`.
4. Save processed images to their respective kernel folders (e.g., `images/ridge/serial/`).

### Configuration

You can override the number of MPI processes (CLUSTERS) and OpenMP threads (THREADS) in the `Makefile` or via command line:

```bash
make run CLUSTERS=2 THREADS=4
```

## Clean

To remove compiled binaries and generated image outputs (excluding `images/base`):

```bash
make clean
```
