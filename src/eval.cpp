# include "global.h"

void evaluate_cpu (population_cpu *pop, int size) {
 
    for (int i=0; i<size; i++) {
        test_problem_cpu (pop, size, i);
		
		if (ncon==0)
			pop->constr_violation[i] = 0.0f;
		else {
			pop->constr_violation[i] = 0.0f;
			for (int j=0; j<ncon; j++)
				if (pop->constr[j*size + i]<0.0f)
					pop->constr_violation[i] += pop->constr[j*size + i];
		}
	}
}

