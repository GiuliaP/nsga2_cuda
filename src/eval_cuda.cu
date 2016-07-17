#include "global_cuda.cuh"

void evaluate_gpu (population_gpu *pop, int size) {
 
	#ifdef sch1
	
	checkCudaErrors(nppsMul_32f(pop->xreal, pop->xreal, pop->obj, size));
		
	checkCudaErrors(nppsSubC_32f(pop->xreal, 2.0f, pop->obj + size, size));	
	checkCudaErrors(nppsMul_32f_I(pop->obj + size, pop->obj + size, size));

	checkCudaErrors(nppsSet_32f(0, pop->constr_violation, size));

	if (ncon!=0) {
		checkCudaErrors(nppsThreshold_GT_32f(pop->constr, tmp_locs_32f_dev, ncon*size, 0));
		
		// se i vincoli sono pochi, okay così
		// altrimenti si potrebbe chiamare cublasSgemv facendo: transpose(tmp_locs_32f_dev)*ones(ncon,1) -dove ones è già disponibile-
		for (int i=0; i<ncon; i++)
			checkCudaErrors(nppsAdd_32f_I(tmp_locs_32f_dev + i*size, pop->constr_violation, size));
	}

	#endif
			
}