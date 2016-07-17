# include "global.h"

void assign_rank_and_crowding_distance_cpu (population_cpu *pop, int size) {
    
	/*
	pop->front_counter = 0;
    
	memset(n, 0, size*sizeof(float));
	memset(S, 0, size*size*sizeof(unsigned char));
	memset(flag, 0, size*size*sizeof(float));

	memset(front_size, 0, size*sizeof(int));
	memset(front_offset, 0, size*sizeof(int));

	for (int p=size-1; p>=0; p--) {
		for (int q=0; q<size; q++) {

			// flag = 1 if p dominates q
			//   flag = -1 if q dominates p
            //   flag = 0 if both p and q are non-dominated
			flag[q*size + p] = check_dominance (pop, size, p, q);
			if (flag[q*size + p]==1)
				S[q*size + p] = 1;
			if (flag[q*size + p]==-1)
				n[p]++;
		}
		if (n[p]==0) {
			pop->front_elements[front_size[pop->front_counter]] = p;
			front_size[pop->front_counter]++;
		}
	}

	while (front_size[pop->front_counter]>0) {

		assign_crowding_distance (pop, size, pop->front_counter);

		front_offset[pop->front_counter+1] = front_offset[pop->front_counter] + front_size[pop->front_counter];

		for (int p=0; p<front_size[pop->front_counter]; p++)
			for (int q=size-1; q>=0; q--)
				if (S[q*size + pop->front_elements[front_offset[pop->front_counter] + p]]) {
					n[q]--;
					if (n[q]==0) {
						pop->front_elements[front_offset[pop->front_counter+1] + front_size[pop->front_counter+1]] = q;
						front_size[pop->front_counter+1]++;
					}
				}

		pop->front_counter++;
	}
	*/
}