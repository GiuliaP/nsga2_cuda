# include "global.h"

void allocate_pop_cpu (population *pop, int size) {

    if (nreal != 0)
        pop->xreal = (float*) malloc(size*nreal*sizeof(float));
 
/*	if (nbin != 0) {
        ind->xbin = (float *)malloc(nbin*sizeof(float));
        ind->gene = (int **)malloc(nbin*sizeof(int *));
        for (int j=0; j<nbin; j++)
            ind->gene[j] = (int *)malloc(nbits[j]*sizeof(int));
    }*/

    pop->obj = (float*) malloc(size*nobj*sizeof(float));
    
	if (ncon != 0)
        pop->constr = (float*) malloc(size*ncon*sizeof(float));

	pop->rank = (int*) malloc(size*sizeof(int));
	pop->crowd_dist = (float*) malloc(size*sizeof(float));
    pop->constr_violation = (float*) malloc(size*sizeof(float));
	pop->front_size = (int*) malloc(size*sizeof(int));
	pop->front_offset = (int*) malloc(size*sizeof(int));
	pop->front_elements = (int*) malloc(size*sizeof(int));
}
void free_pop_cpu (population *pop) {
    
   if (nreal != 0)
        free(pop->xreal);
 
/*	if (nbin != 0) {
        ind->xbin = (float *)malloc(nbin*sizeof(float));
        ind->gene = (int **)malloc(nbin*sizeof(int *));
        for (int j=0; j<nbin; j++)
            ind->gene[j] = (int *)malloc(nbits[j]*sizeof(int));
    }*/

    free(pop->obj);
    
	if (ncon != 0)
        free(pop->constr);

	free(pop->rank);
	free(pop->crowd_dist);
    free(pop->constr_violation);
	free(pop->front_size);
	free(pop->front_offset);
	free(pop->front_elements);
}
