#include "global_cuda.cuh"

void initialize_pop_gpu (population_gpu *pop, int size) {
	
	if (nreal!=0) {
		
		/*
		int nbatch = (int)floorf((float)(nreal*size)/(DIM-1));
		int nrem = (nreal*size)%(DIM-1);

		dim3 threadsPerBlock(DIM,1,1);
		dim3 blocksPerGrid(1,1,1);
	
		for (int i=0; i<nbatch; i++)
			advance_random_device_copyvector<<<blocksPerGrid, threadsPerBlock>>>(pop->xreal + i*(DIM-1), DIM-1);

		if (nrem!=0)
			advance_random_device_copyvector<<<blocksPerGrid, threadsPerBlock>>>(pop->xreal + nbatch*(DIM-1), nrem);
		*/

		dim3 threadsPerBlock(DIM,1,1);
		dim3 blocksPerGrid(1,1,1);
		
		for (int i=0; i<nreal; i++) 
			extract_rndvec_kernel_lowhigh_32f<<<blocksPerGrid, threadsPerBlock>>>(pop->xreal, nreal*size, min_realvar[i], max_realvar[i]);

		/*
		for (int i=0; i<nreal; i++) 
			checkCudaErrors(nppsMulC_32f_I(max_realvar[i]-min_realvar[i],pop->xreal + i*size,size));
		for (int i=0; i<nreal; i++) 
			checkCudaErrors(nppsAddC_32f_I(min_realvar[i],pop->xreal + i*size,size));
		*/
	}
}