# include "global.h"

void selection_cpu (population_cpu *old_pop, population_cpu *new_pop, int size) {
    
	int *p1 = &(tmp_locs_32s[0]);
	int *p2 = &(tmp_locs_32s[size]);
	int *temp1 = &(tmp_locs_32s[2*size]);
	int *temp2 = &(tmp_locs_32s[3*size]);
    int *rand1 = &(tmp_locs_32s[4*size]);
	int *rand2 = &(tmp_locs_32s[5*size]);
	
	for (int i=0; i<size; i++)
        p1[i] = p2[i] = i;

    for (int i=0; i<size; i++) {
        
		rand1[i] = rnd (i, size-1);
		rand2[i] = rnd (i, size-1);

		temp1[i] = p1[rand1[i]];
		temp2[i] = p2[rand2[i]];

        p1[rand1[i]] = p1[i];
		p2[rand2[i]] = p2[i]; 
		
		p1[i] = temp1[i];
        p2[i] = temp2[i];
    }

    int *parent1 = temp1;
	int *parent2 = temp2;

	for (int i=0; i<size; i+=4) {
        
		parent1[i] = tournament_cpu (old_pop, size, p1[i], p1[i+1]);
        parent2[i] = tournament_cpu (old_pop, size, p1[i+2], p1[i+3]);
        realcross_cpu (old_pop, new_pop, size, parent1[i], parent2[i], i, i+1);
        
		parent1[i] = tournament_cpu (old_pop, size, p2[i], p2[i+1]);
        parent2[i] = tournament_cpu (old_pop, size, p2[i+2], p2[i+3]);
        realcross_cpu (old_pop, new_pop, size, parent1[i], parent2[i], i+2, i+3);
    }
}

int tournament_cpu (population_cpu *pop, int size, int ind1, int ind2) {
    
    flag[ind2*size+ind1] = check_dominance_cpu (pop, size, ind1, ind2);
    
	if ( flag[ind2*size+ind1]==1)
        return ind1;

    if ( flag[ind2*size+ind1]==-1)
        return ind2;

    if (pop->crowd_dist[ind1] > pop->crowd_dist[ind2])
        return ind1;

    if (pop->crowd_dist[ind2] > pop->crowd_dist[ind1])
        return ind2;

    if (randomperc() <= 0.5f)
        return ind1;
    else
        return ind2;
}

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
