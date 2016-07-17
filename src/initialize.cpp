# include "global.h"

void initialize_pop_cpu (population_cpu *pop, int size) {

    if (nreal!=0)
		for (int j=0; j<nreal; j++)
			for (int i=0; i<size; i++)
				pop->xreal[j*size + i] = rndreal (min_realvar[j], max_realvar[j]);
}
