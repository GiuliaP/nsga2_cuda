#include "global_cuda.cuh"

void allocate_pop_gpu (population_gpu *pop, int size) {

	if (nreal != 0)
       checkCudaErrors(cudaMalloc((void**)&(pop->xreal), size*nreal*sizeof(float)));
 
    checkCudaErrors(cudaMalloc((void**)&(pop->obj), size*nobj*sizeof(float)));
    
	if (ncon != 0)
        checkCudaErrors(cudaMalloc((void**)&(pop->constr), size*ncon*sizeof(float)));

	checkCudaErrors(cudaMalloc((void**)&(pop->crowd_dist), size*sizeof(float)));
    checkCudaErrors(cudaMalloc((void**)&(pop->constr_violation), size*sizeof(float)));


}
void free_pop_gpu (population_gpu *pop) {

	if (nreal != 0)
       checkCudaErrors(cudaFree(pop->xreal));
 
    checkCudaErrors(cudaFree(pop->obj));
    
	if (ncon != 0)
        checkCudaErrors(cudaFree(pop->constr));

	checkCudaErrors(cudaFree(pop->crowd_dist));
    checkCudaErrors(cudaFree(pop->constr_violation));
}

void upload_pop (population_gpu *pop_dev, population_cpu *pop, int size) {

	if (nreal != 0)
       checkCudaErrors(cudaMemcpy((void*)pop_dev->xreal, (void*)pop->xreal, size*nreal*sizeof(float), cudaMemcpyHostToDevice));
 
    checkCudaErrors(cudaMemcpy((void*)pop_dev->obj, (void*)pop->obj, size*nobj*sizeof(float), cudaMemcpyHostToDevice));
    
	if (ncon != 0)
        checkCudaErrors(cudaMemcpy((void*)pop_dev->constr, pop->constr, size*ncon*sizeof(float), cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMemcpy((void*)pop_dev->crowd_dist, (void*)pop->crowd_dist, size*sizeof(float), cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy((void*)pop_dev->constr_violation, (void*)pop->constr_violation, size*sizeof(float), cudaMemcpyHostToDevice));
}
void download_pop (population_cpu *pop, population_gpu *pop_dev, int size) {

	if (nreal != 0)
       checkCudaErrors(cudaMemcpy((void*)pop->xreal, (void*)pop_dev->xreal, size*nreal*sizeof(float), cudaMemcpyDeviceToHost));
 
    checkCudaErrors(cudaMemcpy((void*)pop->obj, (void*)pop_dev->obj, size*nobj*sizeof(float), cudaMemcpyDeviceToHost));
    
	if (ncon != 0)
        checkCudaErrors(cudaMemcpy((void*)pop->constr, pop_dev->constr, size*ncon*sizeof(float), cudaMemcpyDeviceToHost));

	checkCudaErrors(cudaMemcpy((void*)pop->crowd_dist, (void*)pop_dev->crowd_dist, size*sizeof(float), cudaMemcpyDeviceToHost));
    checkCudaErrors(cudaMemcpy((void*)pop->constr_violation, (void*)pop_dev->constr_violation, size*sizeof(float), cudaMemcpyDeviceToHost));
}