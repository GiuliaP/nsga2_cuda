# include "global.h"
# include "rand.h"

/* Routine for tournament selection, it creates a new_pop from old_pop by performing tournament selection and crossover */
void selection (population *old_pop, population *new_pop, int size) {
    
	int *p1 = &(tmp_locs[0]);
	int *p2 = &(tmp_locs[size]);
	int *temp1 = &(tmp_locs[2*size]);
	int *temp2 = &(tmp_locs[3*size]);
    int *rand1 = &(tmp_locs[4*size]);
	int *rand2 = &(tmp_locs[5*size]);
	
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
        
		parent1[i] = tournament (old_pop, size, p1[i], p1[i+1]);
        parent2[i] = tournament (old_pop, size, p1[i+2], p1[i+3]);
        realcross (old_pop, new_pop, size, parent1[i], parent2[i], i, i+1);
        
		parent1[i] = tournament (old_pop, size, p2[i], p2[i+1]);
        parent2[i] = tournament (old_pop, size, p2[i+2], p2[i+3]);
        realcross (old_pop, new_pop, size, parent1[i], parent2[i], i+2, i+3);
    }
}

int tournament (population *pop, int size, int ind1, int ind2) {
    
    flag[ind2*size+ind1] = check_dominance (pop, size, ind1, ind2);
    
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
