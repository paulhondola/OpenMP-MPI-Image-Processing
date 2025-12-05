#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

void get_args(int argc, char **argv, int *threads) {
  if (argc < 2) {
    printf("Usage: mpirun -n <clusters> %s <threads>\n", argv[0]);
    exit(1);
  }

  *threads = atoi(argv[1]);
}

int main(int argc, char **argv) {

  int total_threads, thread_rank;
  int pid, np;
  get_args(argc, argv, &total_threads);
  omp_set_num_threads(total_threads);

  int provided = 4;

  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
  if (provided < MPI_THREAD_FUNNELED) {
    MPI_Abort(MPI_COMM_WORLD, 1);
    return 1;
  }

  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  printf("Sequental %d out of %d!\n", pid, np);
  MPI_Barrier(MPI_COMM_WORLD);

#pragma omp parallel default(none) private(thread_rank)                        \
    shared(total_threads, pid, np)
  {
    thread_rank = omp_get_thread_num();
    printf("parallel: %d out of %d from proc %d out of %d\n", thread_rank,
           total_threads, pid, np);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}
