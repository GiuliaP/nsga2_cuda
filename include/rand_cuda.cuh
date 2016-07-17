# ifndef _RAND_H_CUDA_
# define _RAND_H_CUDA_

#define DIM 55

extern float oldrand[DIM];

__global__ void advance_random_device ();
__global__ void advance_random_device (float *vout, int nrem);

__global__ void initialize_rndgen_kernel (float seed);
void initialize_rndgen_gpu (float seed);

int rnd_device (int low, int high);

# endif