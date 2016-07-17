#include "global_cuda.cuh"

float oldrand[DIM];
__device__ float oldrand_dev[DIM];
__device__ int jrand_dev;

__global__ void advance_rndgen_kernel () {
    
    unsigned int tid = blockDim.x*blockIdx.x + threadIdx.x;
	
	float new_random;

    if (tid<24) {
        new_random = oldrand_dev[tid]-oldrand_dev[tid+31];
        if (new_random<0.0f)
            new_random = new_random+1.0f;
        oldrand_dev[tid] = new_random;
    }
	__syncthreads();

    if (tid>23 && tid<48) {
        new_random = oldrand_dev[tid]-oldrand_dev[tid-24];
        if (new_random<0.0f)
            new_random = new_random+1.0f;
        oldrand_dev[tid] = new_random;
    }
	__syncthreads();

	if (tid>47 && tid<DIM) {
        new_random = oldrand_dev[tid]-oldrand_dev[tid-24];
        if (new_random<0.0f)
            new_random = new_random+1.0f;
        oldrand_dev[tid] = new_random;
    }
}
void initialize_rndgen_gpu (float seed) {
	
	memset(oldrand,0,DIM*sizeof(float));
	oldrand[DIM-1] = seed;

	float new_random = 0.000000001f;
	float prev_random = seed;
	
	int i; 
	for (int j=1; j<=DIM-1; j++) {
		i = (21*j)%(DIM-1);
		oldrand[i] = new_random;
		new_random = prev_random-new_random;
		if (new_random<0.0f)
			new_random += 1.0f;
		prev_random = oldrand[i];
	}

	checkCudaErrors(cudaMemcpyToSymbol(oldrand_dev, oldrand, DIM*sizeof(float), 0, cudaMemcpyHostToDevice));

	int jrand = 0;
	checkCudaErrors(cudaMemcpyToSymbol(jrand_dev, &jrand, sizeof(int), 0, cudaMemcpyHostToDevice));

	dim3 threadsPerBlock(DIM,1,1);
	dim3 blocksPerGrid(1,1,1);

	advance_rndgen_kernel<<<blocksPerGrid, threadsPerBlock>>> ();
    advance_rndgen_kernel<<<blocksPerGrid, threadsPerBlock>>> ();
    advance_rndgen_kernel<<<blocksPerGrid, threadsPerBlock>>> ();
}

// Extract numel float random numbers between 0.0 and 1.0 -including the bounds-
__global__ void extract_rndvec_kernel (float *vout, int numel) {
    
    unsigned int tid = blockDim.x*blockIdx.x + threadIdx.x;
	
	float new_random;

    int nbatch = (numel+jrand_dev-1) / (DIM-1);
	int nrem = (numel+jrand_dev-1) % (DIM-1);
		
	if (tid>=jrand_dev && tid<jrand_dev+numel && tid<DIM)
		vout[tid-jrand_dev] = oldrand_dev[tid];
	
	if (nbatch==0) {
		jrand_dev += numel;
		return;
	} 
		
	for (int i=0; i<nbatch; i++) {

		if (tid<24) {
		new_random = oldrand_dev[tid]-oldrand_dev[tid+31];
		if (new_random<0.0f)
			new_random = new_random+1.0f;
		oldrand_dev[tid] = new_random;
		}
		__syncthreads();
		
		if (tid>23 && tid<48) {
			new_random = oldrand_dev[tid]-oldrand_dev[tid-24];
			if (new_random<0.0f)
				new_random = new_random+1.0f;
			oldrand_dev[tid] = new_random;
		}
		__syncthreads();
		
		if (tid>47 && tid<DIM) {
			new_random = oldrand_dev[tid]-oldrand_dev[tid-24];
			if (new_random<0.0f)
				new_random = new_random+1.0f;
			oldrand_dev[tid] = new_random;
		}
		__syncthreads();

		if (i<nbatch-1) {
			if (tid>=1 && tid<DIM)
				vout[(DIM-jrand_dev) + i*(DIM-1) + (tid-1)] = oldrand_dev[tid];
		} else {
			if (tid>=1 && tid<=nrem)
				vout[(DIM-jrand_dev) + i*(DIM-1) + (tid-1)] = oldrand_dev[tid];
			jrand_dev = nrem + 1;
		}
	}
}
// Extract numel int random numbers between low and high -including the bounds-
__global__ void extract_rndvec_kernel_lowhigh_32s (float *vout, int numel, int low, int high) {
    
    unsigned int tid = blockDim.x*blockIdx.x + threadIdx.x;
	
	float new_random;
	int vtmp;

    int nbatch = (numel+jrand_dev-1) / (DIM-1);
	int nrem = (numel+jrand_dev-1) % (DIM-1);
		
	if (tid>=jrand_dev && tid<jrand_dev+numel && tid<DIM) {
		vtmp = low + (oldrand_dev[tid]*(high-low+1.0f));
		if (vtmp>high) 
			vout[tid-jrand_dev] = (float)high;
		else vout[tid-jrand_dev] = (float)vtmp;
	}
	
	if (nbatch==0) {
		jrand_dev += numel;
		return;
	} 
		
	for (int i=0; i<nbatch; i++) {

		if (tid<24) {
		new_random = oldrand_dev[tid]-oldrand_dev[tid+31];
		if (new_random<0.0f)
			new_random = new_random+1.0f;
		oldrand_dev[tid] = new_random;
		}
		__syncthreads();
		
		if (tid>23 && tid<48) {
			new_random = oldrand_dev[tid]-oldrand_dev[tid-24];
			if (new_random<0.0f)
				new_random = new_random+1.0f;
			oldrand_dev[tid] = new_random;
		}
		__syncthreads();
		
		if (tid>47 && tid<DIM) {
			new_random = oldrand_dev[tid]-oldrand_dev[tid-24];
			if (new_random<0.0f)
				new_random = new_random+1.0f;
			oldrand_dev[tid] = new_random;
		}
		__syncthreads();

		if (i<nbatch-1) {
			if (tid>=1 && tid<DIM) {
				vtmp = low + (oldrand_dev[tid]*(high-low+1.0f));
				if (vtmp>high) 
					vout[(DIM-jrand_dev) + i*(DIM-1) + (tid-1)] = high;
				else vout[(DIM-jrand_dev) + i*(DIM-1) + (tid-1)] = vtmp;
			}
		} else {
			if (tid>=1 && tid<=nrem) {				
				vtmp = low + (oldrand_dev[tid]*(high-low+1.0f));
				if (vtmp>high) 
					vout[(DIM-jrand_dev) + i*(DIM-1) + (tid-1)] = (float)high;
				else vout[(DIM-jrand_dev) + i*(DIM-1) + (tid-1)] = (float)vtmp;
			}
			jrand_dev = nrem + 1;
		}
	}
}
// Extract numel float random numbers between low and high -including the bounds-
__global__ void extract_rndvec_kernel_lowhigh_32f (float *vout, int numel, float low, float high) {
    
    unsigned int tid = blockDim.x*blockIdx.x + threadIdx.x;
	
	float new_random;

    int nbatch = (numel+jrand_dev-1) / (DIM-1);
	int nrem = (numel+jrand_dev-1) % (DIM-1);
		
	if (tid>=jrand_dev && tid<jrand_dev+numel && tid<DIM)
		vout[tid-jrand_dev] = low + oldrand_dev[tid]*(high-low);
	
	if (nbatch==0) {
		jrand_dev += numel;
		return;
	} 
		
	for (int i=0; i<nbatch; i++) {

		if (tid<24) {
		new_random = oldrand_dev[tid]-oldrand_dev[tid+31];
		if (new_random<0.0f)
			new_random = new_random+1.0f;
		oldrand_dev[tid] = new_random;
		}
		__syncthreads();
		
		if (tid>23 && tid<48) {
			new_random = oldrand_dev[tid]-oldrand_dev[tid-24];
			if (new_random<0.0f)
				new_random = new_random+1.0f;
			oldrand_dev[tid] = new_random;
		}
		__syncthreads();
		
		if (tid>47 && tid<DIM) {
			new_random = oldrand_dev[tid]-oldrand_dev[tid-24];
			if (new_random<0.0f)
				new_random = new_random+1.0f;
			oldrand_dev[tid] = new_random;
		}
		__syncthreads();

		if (i<nbatch-1) {
			if (tid>=1 && tid<DIM)
				vout[(DIM-jrand_dev) + i*(DIM-1) + (tid-1)] = low + oldrand_dev[tid]*(high-low);
		} else {
			if (tid>=1 && tid<=nrem)			
				vout[(DIM-jrand_dev) + i*(DIM-1) + (tid-1)] = low + oldrand_dev[tid]*(high-low);
			jrand_dev = nrem + 1;
		}
	}
}

