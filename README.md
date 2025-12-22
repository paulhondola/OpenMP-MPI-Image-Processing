# OpenMP & MPI Image Processing

This project implements image processing kernels (convolution) using a serial and hybrid parallelization approach with OpenMP and MPI. It benchmarks various convolution kernels (Ridge, Edge, Sharpen, BoxBlur, Gaussian, Unsharp) on BMP images.

## Prerequisites

- **GCC/Clang**: The Makefile automatically handles OpenMP flags.
    - **macOS**: Detects and directs to `/opt/homebrew/opt/libomp`.
    - **Linux**: Uses standard `-fopenmp`.
- **MPI**: A message passing interface implementation (e.g., MPICH, OpenMPI).
- **Make**: For build automation.

## Project Structure

```
├── Makefile        # Compilation, setup, and execution commands
├── bin             # Output directory for compiled binary
├── data            # Output directory
│   ├── values      # CSV files (time_data.csv)
│   └── plots       # Generated plots
├── docs            # Documentation files
├── images          # Input and output directory for BMP images
│   ├── base        # Place input BMP files here
│   └── [kernel]    # Output directories for each kernel (e.g., ridge/serial|parallel)
└── src             # Source code
    ├── benchmark   # Benchmarking logic and kernel definitions
    ├── bmp         # BMP file I/O (read/write)
    ├── convolution # Convolution implementation (serial vs hybrid parallel)
    ├── errors      # Error handling (app_error enum)
    ├── file_utils  # File and directory utilities (mkdir -p execution)
    ├── constants   # Constant definitions (kernel weights and image file paths)
    └── main.c      # Entry point
```

## Setup

Before running the project, you must create the necessary directory structure for artifacts and images.

```bash
make setup
```

This command initializes:
- `bin/`, `data/`, `docs/`, `src/`
- `images/base` (Put your source BMP images here)
- Note: Specific output directories for every kernel are created automatically by the Make verification or can be created manually if needed.

## Build and Run

### 1. Build and Run Hybrid Benchmark

To compile and run the full benchmark suite on the images located in `images/base`:

```bash
make run
```

This will:
1.  **Compile** the source into `bin/main` using `mpicc`.
2.  **Execute** via `mpirun` with default settings (10 MPI Clusters, 10 OpenMP Threads).
3.  **Process** all images in `images/base`.
4.  **Save** results to the appropriate `images/[kernel]/[type]` folder.
5.  **Append** time data to the corresponding CSV file in `data/values/`:
    *   `serial_data.csv` (Serial)
    *   `multithreaded_data.csv` (Parallel Multithreaded)
    *   `distributed_data.csv` (Parallel Distributed FS)
    *   `shared_data.csv` (Parallel Shared FS)
    *   `task_pool_data.csv` (Parallel Task Pool)
    *   `time_data.csv` (Combined data when running ALL via `-a` or `make run_all`)

### 2. Available Make Targets

You can use specific Make targets to run individual benchmark modes:

```bash
# Run Serial benchmark
make run_serial

# Run Parallel Multithreaded benchmark
make run_multithreaded

# Run Parallel Distributed Filesystem benchmark
make run_distributed

# Run Parallel Shared Filesystem benchmark
make run_shared

# Run Parallel Task Pool benchmark
make run_task_pool

# Run All benchmarks
make run_all
```

Custom `CLUSTERS` and `THREADS` can still be passed:
```bash
make run_distributed CLUSTERS=4 THREADS=8
```

### 3. Manual Execution & Flags

You can run the benchmark binary directly to control which modes are executed. This uses the new flag-based configuration system.

```bash
# General syntax
bin/mpi_omp_convolution -t <threads> [mode flags]
```

# Flags:
# -t <n> : Set number of OpenMP threads
# -s     : proper Serial benchmark
# -m     : Parallel Multithreaded benchmark
# -d     : Parallel Distributed Filesystem benchmark
# -h     : Parallel Shared Filesystem benchmark
# -p     : Parallel Task Pool benchmark
# -a     : All benchmarks
# --help : Show usage

# Example: Run distributed with 4 threads
mpirun -n 10 bin/mpi_omp_convolution -t 4 -d
compares parallel outputs against serial outputs. You must run the Serial benchmark (`-s`) at least once to generate the reference images, otherwise verification will fail.

**Examples:**
```bash
# Run Serial benchmark (generates reference images)
bin/mpi_omp_convolution -s

# Run Distributed benchmark with 4 threads per rank
mpirun -n 4 bin/mpi_omp_convolution -t 4 -d

# Run All benchmarks
mpirun -n 4 bin/mpi_omp_convolution -t 4 -a
```

### 2. Customizing Parallelism

You can override the number of MPI processes (`CLUSTERS`) and OpenMP threads (`THREADS`) directly from the command line:

```bash
# Example: 4 MPI processes, 8 OpenMP threads per process
make run CLUSTERS=4 THREADS=8
```

### 3. Parameter Sweep

To run a parameter sweep across defined cluster and thread counts (defined in Makefile as `CLUSTER_ARRAY` and `THREAD_ARRAY`):

```bash
make sweep
```

### 3. Build only

If you only want to compile the binary without running:

```bash
make build
```

## Clean

To remove compiled binaries and all generated output images (preserving your source images in `images/base`):

```bash
make clean
```

