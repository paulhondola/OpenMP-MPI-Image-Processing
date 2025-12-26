project('MpiOmpImageProcessing', 'c',
  version : '1.7.0',
  default_options : ['warning_level=3', 'c_std=c99'])

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

# Optimization flags
add_project_arguments('-O3', '-march=native', language : 'c')

# Configuration Variables
mpi_processes = '8'
omp_threads = '8'

# Dependencies
omp_dep = dependency('openmp')
mpi_dep = dependency('mpi', language : 'c')

# Source files
src_files = files(
  'src/benchmark/kernel_run.c',
  'src/benchmark/benchmark_run.c',
  'src/benchmark/benchmark_io.c',
  'src/config/kernel.c',
  'src/config/files.c',
  'src/main.c',
  'src/errors/errors.c',
  'src/bmp/bmp_io.c',
  'src/bmp/mpi_bmp_io.c',
  'src/convolution/convolution.c',
  'src/file_utils/file_utils.c'
)

# Include directories
inc_dir = include_directories('src')

# Executable
exe = executable('mpi_omp_convolution',
           src_files,
           include_directories : inc_dir,
           dependencies : [omp_dep, mpi_dep, m_dep],
           install : true)

# Run Targets
# Note: 'run_target' commands are executed in the build directory by default.
# We modify them to run from the project root using 'sh -c'.

# Serial
run_target('run_serial',
  command : ['sh', '-c', 'cd ' + meson.project_source_root() + ' && ' + exe.full_path() + ' -serial'],
  depends : exe
)

# Multithreaded
run_target('run_multithreaded',
  command : ['sh', '-c', 'cd ' + meson.project_source_root() + ' && ' + exe.full_path() + ' -threads ' + omp_threads + ' -multithreaded'],
   depends : exe
)

# Distributed
# We use 'mpirun' (or mpiexec) which should be available if MPI dependency is found.
# Meson doesn't have a built-in 'mpirun' variable exposed easily without finding program.
mpirun = find_program('mpirun', required : true)

run_target('run_distributed',
  command : ['sh', '-c', 'cd ' + meson.project_source_root() + ' && ' + mpirun.full_path() + ' -n ' + mpi_processes + ' ' + exe.full_path() + ' -threads ' + omp_threads + ' -distributed'],
   depends : exe
)

# Shared
run_target('run_shared',
  command : ['sh', '-c', 'cd ' + meson.project_source_root() + ' && ' + mpirun.full_path() + ' -n ' + mpi_processes + ' ' + exe.full_path() + ' -threads ' + omp_threads + ' -shared'],
   depends : exe
)

# Task Pool
run_target('run_task_pool',
  command : ['sh', '-c', 'cd ' + meson.project_source_root() + ' && ' + mpirun.full_path() + ' -n ' + mpi_processes + ' ' + exe.full_path() + ' -threads ' + omp_threads + ' -task_pool'],
   depends : exe
)

# All
run_target('run_all',
  command : ['sh', '-c', 'cd ' + meson.project_source_root() + ' && ' + mpirun.full_path() + ' -n ' + mpi_processes + ' ' + exe.full_path() + ' -threads ' + omp_threads + ' -all'],
   depends : exe
)
