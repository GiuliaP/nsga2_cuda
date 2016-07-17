# ifndef _GLOBAL_H_CUDA_
# define _GLOBAL_H_CUDA_

#include "global.h"

typedef struct {
    
    float *constr_violation;
	float *constr; 
    float *xreal;
	float *obj;

	int front_counter;

	float *crowd_dist;

} population_gpu;

extern int *front_size_locked;
extern int *front_offset_locked;
extern int *front_elements_locked;
extern int *front_size_dev;
extern int *front_offset_dev;
extern int *front_elements_dev;

extern float *ones_32f_dev;
extern float *idx_32f_dev;
extern float *idx_32f;

extern float *tmp_locs_32f_dev;

extern char *flag_dev;
extern float *S_dev;
extern float *n_dev; 

extern cublasHandle_t handle;

#define cudaCheck(msg) \
    do { \
        cudaError_t __err = cudaGetLastError(); \
        if (__err != cudaSuccess) { \
            fprintf(stderr, "Fatal error: %s (%s at %s:%d)\n", \
                msg, cudaGetErrorString(__err), \
                __FILE__, __LINE__); \
            fprintf(stderr, "*** FAILED - ABORTING\n"); \
            exit(1); \
        } \
    } while (0)

void allocate_pop_gpu (population_gpu *pop, int size);
void free_pop_gpu (population_gpu *pop);

void copy_constants();
void upload_pop (population_gpu *pop, population_cpu *pop_host, int size);
void download_pop (population_cpu *pop_host, population_gpu *pop, int size);

void initialize_pop_gpu (population_gpu *pop, int size);

void evaluate_gpu (population_gpu *pop, int size);

void assign_rank_and_crowd_dist_gpu (population_gpu *pop_dev, population_cpu *pop, const int size);
void assign_rank_and_crowd_dist_gpu_prova (population_gpu *pop, const int size);

__global__ void calc_next_front (float *n, const float *S, const int pop_size, int *front_elements_loc, int *front_size_loc, int *front_offset_loc, const int front);

void check_dominance_gpu_masked (population_gpu *pop, const int size);
__global__ void check_dominance_kernel_masked (float *obj, float *constr_violation, const int size, const int nobj, float *S_loc, char *flag_loc);
__device__ void check_dominance_device_masked (float *obj, float *constr_violation, const int size, const int nobj, unsigned p, unsigned q, float *S_loc, char *flag_loc);

//void qsort_front_obj_gpu (population_gpu *pop_gpu, int size, int front, int obj, int left, int right);

void selection_gpu (population_gpu *old_pop, population_gpu *new_pop, int size);
__global__ void tournament_kernel (float* parent, float* p, float* crowd_dist_loc, char* flag, const int size, float *rand_vec);
__global__ void realcross_kernel (float* parent_x, float* child_x, int size, float* p, float *tmp1, float *tmp2a, float *tmp2b, float *tmp2c);

void mutation_gpu (population_gpu *pop, int size);
void realmutate_gpu (population_gpu *pop, int size, int ind);

//void merge_gpu (population_gpu *dst_pop, population_gpu *src_pop1, population_gpu *src_pop2, int size);
//void copy_gpu (population_gpu *dst_pop, int dst_ind, int dst_size, population_gpu *src_pop, int src_ind, int src_size);

//void fill_nondominated_sort_gpu (population_gpu *new_pop, population_gpu *mixed_pop, int size);
//void fill_gpu (population_gpu *new_pop, population_gpu *mixed_pop, int size);
//void qsort_crowd_dist_gpu (population_gpu *pop, int size, int front, int left, int right);

/* Function declarations for the random number generator */

void initialize_rndgen_gpu (float seed);
__global__ void advance_rndgen_kernel ();

__global__ void extract_rndvec_kernel (float *vout, int numel);
__global__ void extract_rndvec_kernel_lowhigh_32f (float *vout, int numel, float low, float high);
__global__ void extract_rndvec_kernel_lowhigh_32s (float *vout, int numel, int low, int high);

__device__ float extract_rndnum_device ();

# endif