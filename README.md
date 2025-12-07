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
├── data            # Output directory for benchmark content (times, plots)
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
    ├── kernel      # Kernel struct usage logic
    └── main.c      # Entry point
```

## Setup

Before running the project, you must create the necessary directory structure for artifacts and images.

```bash
make setup
```

This command initializes:
- `bin/`, `data/`, `docs/`
- `images/base` (Put your source BMP images here)
- Specific output directories for every kernel (Ridge, Edge, Sharpen, BoxBlur, Gaussian, Unsharp) and their serial/parallel subfolders.

## Build and Run

### 1. Build and Run Hybrid Benchmark

To compile and run the full benchmark suite on the images located in `images/base`:

```bash
make run
```

This will:
1.  **Compile** the source into `bin/main` using `mpicc`.
2.  **Execute** via `mpirun` with default settings (2 MPI Clusters, 4 OpenMP Threads).
3.  **Process** all images in `images/base`.
4.  **Save** results to the appropriate `images/[kernel]/[type]` folder.

### 2. Customizing Parallelism

You can override the number of MPI processes (`CLUSTERS`) and OpenMP threads (`THREADS`) directly from the command line:

```bash
# Example: 4 MPI processes, 8 OpenMP threads per process
make run CLUSTERS=4 THREADS=8
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

