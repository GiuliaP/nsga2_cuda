#include "global_cuda.cuh"

/* Routine for usual non-domination checking
   It will return the following values
   1 if p dominates q
   -1 if q dominates p
   0 if both p and q are non-dominated */

__device__ void check_dominance_device_masked (float *obj, float *constr_violation, const int size, const int nobj, unsigned p, unsigned q, float *S_loc, char *flag_loc) {

    unsigned char pmeno = (constr_violation[p]<0); 
	unsigned char qmeno = (constr_violation[q]<0);
	unsigned char pzero = (constr_violation[p]==0); 
	unsigned char qzero = (constr_violation[q]==0);

	char mask6 = pzero*qmeno;
	char mask8 = -(pmeno*qzero);
	char mask9 = pmeno*qmeno;
	char mask754321 = 1 - (mask6 - mask8 + mask9);

	float diff = constr_violation[p] - constr_violation[q];
	mask9 *= (char)(diff/fabs(diff));;

	unsigned char flag1 = 0;
    unsigned char flag2 = 0;
	for (int i=0; i<nobj; i++) {
		flag1 = flag1 || (obj[i*size + p] < obj[i*size + q]);
		flag2 = flag2 || (obj[i*size + p] > obj[i*size + q]);
	}
	mask754321 *= (char)(flag1 - flag2);

	// flag_loc contiene la dominance (-1,0,1)
	char mask_tot = mask6 + mask8 + mask9 + mask754321;
	*flag_loc = mask_tot;

	// diversamente dall'algoritmo originale!!!!
	*S_loc = (float)(mask_tot<0);
}

__global__ void check_dominance_kernel_masked (float *obj, float *constr_violation, const int size, const int nobj, float *S_loc, char *flag_loc) {

	unsigned int p = threadIdx.x + blockDim.x*(blockIdx.x/size);
	unsigned int q = blockIdx.x%size;
	unsigned idx = p + size*q;

	check_dominance_device_masked (obj, constr_violation, size, nobj, p, q, S_loc + idx, flag_loc + idx);

	//check_dominance_device_masked (obj, constr_violation, size, nobj, threadIdx.x, blockIdx.x, S_loc + threadIdx.x + blockIdx.x*blockDim.x, flag_loc + threadIdx.x + blockIdx.x*blockDim.x);
}