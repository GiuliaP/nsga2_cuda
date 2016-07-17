# include "global.h"

void allocate_pop_cpu (population_cpu *pop, int size) {

    if (nreal != 0)
        pop->xreal = (float*) malloc(size*nreal*sizeof(float));
 
    pop->obj = (float*) malloc(size*nobj*sizeof(float));
    
	if (ncon != 0)
        pop->constr = (float*) malloc(size*ncon*sizeof(float));

	pop->crowd_dist = (float*) malloc(size*sizeof(float));
    pop->constr_violation = (float*) malloc(size*sizeof(float));
}
void free_pop_cpu (population_cpu *pop) {
    
   if (nreal != 0)
        free(pop->xreal);
 
    free(pop->obj);
    
	if (ncon != 0)
        free(pop->constr);

	free(pop->crowd_dist);
    free(pop->constr_violation);
}
