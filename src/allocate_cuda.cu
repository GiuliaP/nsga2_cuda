#include "global_cuda.cuh"

void allocate_pop_gpu (population *pop, int size) {

	if (nreal != 0)
       checkCudaErrors(cudaMalloc((void**)&(pop->xreal), size*nreal*sizeof(float)));
 
    checkCudaErrors(cudaMalloc((void**)&(pop->obj), size*nobj*sizeof(float)));
    
	if (ncon != 0)
        checkCudaErrors(cudaMalloc((void**)&(pop->constr), size*ncon*sizeof(float)));

	checkCudaErrors(cudaMalloc((void**)&(pop->rank), size*sizeof(int))); 
	checkCudaErrors(cudaMalloc((void**)&(pop->crowd_dist), size*sizeof(float)));
    checkCudaErrors(cudaMalloc((void**)&(pop->constr_violation), size*sizeof(float)));
	checkCudaErrors(cudaMalloc((void**)&(pop->front_size), size*sizeof(int)));
	checkCudaErrors(cudaMalloc((void**)&(pop->front_offset), size*sizeof(int)));
	checkCudaErrors(cudaMalloc((void**)&(pop->front_elements), size*sizeof(int)));
}
void free_pop_gpu (population *pop) {

	if (nreal != 0)
       checkCudaErrors(cudaFree(pop->xreal));
 
    checkCudaErrors(cudaFree(pop->obj));
    
	if (ncon != 0)
        checkCudaErrors(cudaFree(pop->constr));

	checkCudaErrors(cudaFree(pop->rank)); 
	checkCudaErrors(cudaFree(pop->crowd_dist));
    checkCudaErrors(cudaFree(pop->constr_violation));
	checkCudaErrors(cudaFree(pop->front_size));
	checkCudaErrors(cudaFree(pop->front_offset));
	checkCudaErrors(cudaFree(pop->front_elements));
}

void upload_pop (population *pop, population *pop_host, int size) {

	if (nreal != 0)
       checkCudaErrors(cudaMemcpy((void*)pop->xreal, (void*)pop_host->xreal, size*nreal*sizeof(float), cudaMemcpyHostToDevice));
 
    checkCudaErrors(cudaMemcpy((void*)pop->obj, (void*)pop_host->obj, size*nobj*sizeof(float), cudaMemcpyHostToDevice));
    
	if (ncon != 0)
        checkCudaErrors(cudaMemcpy((void*)pop->constr, pop_host->constr, size*ncon*sizeof(float), cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMemcpy((void*)pop->rank, (void*)pop_host->rank, size*sizeof(int), cudaMemcpyHostToDevice)); 
	checkCudaErrors(cudaMemcpy((void*)pop->crowd_dist, (void*)pop_host->crowd_dist, size*sizeof(float), cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy((void*)pop->constr_violation, (void*)pop_host->constr_violation, size*sizeof(float), cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy((void*)pop->front_size, (void*)pop_host->front_size, size*sizeof(int), cudaMemcpyHostToDevice)); 
	checkCudaErrors(cudaMemcpy((void*)pop->front_offset, (void*)pop_host->front_offset, size*sizeof(int), cudaMemcpyHostToDevice)); 
	checkCudaErrors(cudaMemcpy((void*)pop->front_elements, (void*)pop_host->front_elements, size*sizeof(int), cudaMemcpyHostToDevice)); 
}
void download_pop (population *pop_host, population *pop, int size) {

	if (nreal != 0)
       checkCudaErrors(cudaMemcpy((void*)pop_host->xreal, (void*)pop->xreal, size*nreal*sizeof(float), cudaMemcpyDeviceToHost));
 
    checkCudaErrors(cudaMemcpy((void*)pop_host->obj, (void*)pop->obj, size*nobj*sizeof(float), cudaMemcpyDeviceToHost));
    
	if (ncon != 0)
        checkCudaErrors(cudaMemcpy((void*)pop_host->constr, pop->constr, size*ncon*sizeof(float), cudaMemcpyDeviceToHost));

	checkCudaErrors(cudaMemcpy((void*)pop_host->rank, (void*)pop->rank, size*sizeof(int), cudaMemcpyDeviceToHost)); 
	checkCudaErrors(cudaMemcpy((void*)pop_host->crowd_dist, (void*)pop->crowd_dist, size*sizeof(float), cudaMemcpyDeviceToHost));
    checkCudaErrors(cudaMemcpy((void*)pop_host->constr_violation, (void*)pop->constr_violation, size*sizeof(float), cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy((void*)pop_host->front_size, (void*)pop->front_size, size*sizeof(int), cudaMemcpyDeviceToHost)); 
	checkCudaErrors(cudaMemcpy((void*)pop_host->front_offset, (void*)pop->front_offset, size*sizeof(int), cudaMemcpyDeviceToHost)); 
	checkCudaErrors(cudaMemcpy((void*)pop_host->front_elements, (void*)pop->front_elements, size*sizeof(int), cudaMemcpyDeviceToHost)); 
}