# Author: Giulia Pasquale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.8.8)

set(PROJECTNAME nsga2_cuda)
project(${PROJECTNAME})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_package(CUDA REQUIRED)

list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_20,code=sm_20)
list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_30,code=sm_30)
list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_35,code=sm_35)
list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_50,code=sm_50)

include_directories(${PROJECT_SOURCE_DIR})

# no need to use the following instruction because this is automatically done by CUDA_ADD_EXECUTABLE
include_directories(${CUDA_INCLUDE_DIRS})

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

set(folder_source 
src/allocate.cpp 
src/dominance.cpp
src/initialize.cpp
src/qsort.cpp
src/report.cpp
src/crossover.cpp
src/eval.cpp
src/list.cpp
src/mutation.cpp
src/selection.cpp
src/crowd_dist.cpp
src/mem_manag.cpp
src/rand.cpp
src/crowddist.cpp
src/sort.cpp
src/display.cpp
src/fillnds.cpp
src/merge.cpp
src/problemdef.cpp
src/rank.cpp
src/tourselect.cpp
src/nsgaII.cpp
src/merge_cuda.cu
src/problemdef_cuda.cu
src/rank_cuda.cu
src/allocate_cuda.cu
src/dominance_cuda.cu
src/initialize_cuda.cu
src/merge_gpu.cu
src/qsort_cuda.cu
src/eval_cuda.cu
src/mutation_cuda.cu
src/selection_cuda.cu
src/fill_cuda.cu
src/mem_manag_cuda.cu
src/rand_cuda.cu)

set(folder_header
include/global.h
include/global_cuda.cuh
include/rand.h
include/rand_cuda.cuh)

source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

cuda_add_executable(${PROJECTNAME} ${folder_source} ${folder_header})

target_link_libraries(${PROJECTNAME} ${CUDA_LIBRARIES})

install(TARGETS ${PROJECTNAME} DESTINATION bin)
