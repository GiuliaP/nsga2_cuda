# include "global.h"

/* Routine for real variable SBX crossover */
void realcross_cpu (population_cpu *parent_pop, population_cpu *child_pop, int size, int parent1, int parent2, int child1, int child2) {
    
    float rand;
    float y1, y2, yl, yu;
    float c1, c2;
    float alpha, beta, betaq;
    
	if (randomperc() <= pcross_real) {
        
		nrealcross++;

        for (int i=0; i<nreal; i++) {

            if ((randomperc()<=0.5f)) {

				if (fabs(parent_pop->xreal[i*size + parent1]-parent_pop->xreal[i*size + parent2]) > EPS) {
					
					if (parent_pop->xreal[i*size + parent1] < parent_pop->xreal[i*size + parent2]) {
                        y1 = parent_pop->xreal[i*size + parent1];
                        y2 = parent_pop->xreal[i*size + parent2];
                    } else {
                        y1 = parent_pop->xreal[i*size + parent2];
                        y2 = parent_pop->xreal[i*size + parent1];
					}

					yl = min_realvar[i];
					yu = max_realvar[i];
					rand = randomperc();
                    
					beta = 1.0f + (2.0f*(y1-yl)/(y2-y1));
					alpha = 2.0f - pow(beta,-(eta_c+1.0f));
					if (rand <= (1.0f/alpha))
						betaq = pow (rand*alpha,1.0f/(eta_c+1.0f));
					else
						betaq = pow (1.0f/(2.0f - rand*alpha),1.0f/(eta_c+1.0f));
					c1 = 0.5f*((y1+y2)-betaq*(y2-y1));
                    
					beta = 1.0f + (2.0f*(yu-y2)/(y2-y1));
					alpha = 2.0f - pow(beta,-(eta_c+1.0f));
					if (rand <= (1.0f/alpha))
						betaq = pow ((rand*alpha),(1.0f/(eta_c+1.0f)));
					else
						betaq = pow ((1.0f/(2.0f - rand*alpha)),(1.0f/(eta_c+1.0f)));
					c2 = 0.5f*((y1+y2)+betaq*(y2-y1));
                    
				if (c1<yl) c1=yl;
                if (c2<yl) c2=yl;
                if (c1>yu) c1=yu;
                if (c2>yu) c2=yu;

                if (randomperc()<=0.5f) {
                    child_pop->xreal[i*size + child1] = c2;
                    child_pop->xreal[i*size + child2] = c1;
                } else {
                    child_pop->xreal[i*size + child1] = c1;
                    child_pop->xreal[i*size + child2] = c2;
                }

			} else {
                child_pop->xreal[i*size + child1] = parent_pop->xreal[i*size + parent1];
                child_pop->xreal[i*size + child2] = parent_pop->xreal[i*size + parent2];
				}
			
			} else {
				child_pop->xreal[i*size + child1] = parent_pop->xreal[i*size + parent1];
                child_pop->xreal[i*size + child2] = parent_pop->xreal[i*size + parent2];
			}
		}

	} else {
        for (int i=0; i<nreal; i++) {
            child_pop->xreal[i*size + child1] = parent_pop->xreal[i*size + parent1];
            child_pop->xreal[i*size + child2] = parent_pop->xreal[i*size + parent2];
        }
    }
}
