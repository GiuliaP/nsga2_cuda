# include "global_cuda.cuh"

/* Routine to merge two populations into one */
void merge_gpu (population_gpu *dst_pop, population_gpu *src_pop1, population_gpu *src_pop2, int size) {
    
    /*
	for (int i=0; i<size; i++) {
        copy_gpu (dst_pop, i, 2*size, src_pop1, i, size);
        copy_gpu (dst_pop, i+size, 2*size, src_pop2, i, size);
	}
	*/
}

void copy_gpu (population_gpu *dst_pop, int dst_ind, int dst_size, population_gpu *src_pop, int src_ind, int src_size) {
   
    dst_pop->constr_violation[dst_ind] = src_pop->constr_violation[src_ind];
    dst_pop->crowd_dist[dst_ind] = src_pop->crowd_dist[src_ind];
	
	if (nreal!=0)
        for (int i=0; i<nreal; i++) 
            dst_pop->xreal[i*dst_size + dst_ind] = src_pop->xreal[i*src_size + src_ind];

    for (int i=0; i<nobj; i++)
        dst_pop->obj[i*dst_size + dst_ind] = src_pop->obj[i*src_size + src_ind];

    if (ncon!=0)
        for (int i=0; i<ncon; i++)
            dst_pop->constr[i*dst_size + dst_ind] = src_pop->constr[i*src_size + src_ind];
}
