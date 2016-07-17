# include "global.h"

void fill_nondominated_sort_cpu (population_cpu *new_pop, population_cpu *mixed_pop, int size) {
    
	int new_pop_counter = 0;

	memset(n, 0, 2*size*sizeof(int));
	memset(S, 0, 2*size*2*size*sizeof(unsigned char));
	memset(flag, 0, 2*size*2*size*sizeof(short));

	memset(front_size, 0, 2*size*sizeof(int));
	memset(front_offset, 0, 2*size*sizeof(int));
	mixed_pop->front_counter = 0;
	
    for (int p=0; p<2*size; p++) {
		for (int q=2*size-1; q>=0; q--) {

			/* flag = 1 if p dominates q
			   flag = -1 if q dominates p
               flag = 0 if both p and q are non-dominated */
			flag[q*size+p] = check_dominance_cpu (mixed_pop, 2*size, p, q);
			if (flag[q*size+p]==1)
				S[q*size + p] = 1;
			if (flag[q*size+p]==-1)
				n[p]++;
		}
		if (n[p]==0) {
			front_elements[front_size[mixed_pop->front_counter]] = p;
			front_size[mixed_pop->front_counter]++;
		}
	}
	
	while (new_pop_counter + front_size[mixed_pop->front_counter] <= size) {
		
		if (front_size[mixed_pop->front_counter]!=0) {

			assign_crowding_distance_cpu (mixed_pop, 2*size, mixed_pop->front_counter);
			
			for (int k=0; k<front_size[mixed_pop->front_counter]; k++, new_pop_counter++)
				copy_cpu (new_pop, new_pop_counter, size, mixed_pop, front_elements[front_offset[mixed_pop->front_counter] + k], 2*size);

			front_offset[mixed_pop->front_counter+1] = front_offset[mixed_pop->front_counter] + front_size[mixed_pop->front_counter];
			
			for (int p=0; p<front_size[mixed_pop->front_counter]; p++) {
				for (int q=2*size-1; q>=0; q--) {
					if (S[q*size + front_elements[front_offset[mixed_pop->front_counter] + p]]) {
						n[q]--;
						if (n[q]==0) {
							front_elements[front_offset[mixed_pop->front_counter+1] + front_size[mixed_pop->front_counter+1]] = q;
							front_size[mixed_pop->front_counter+1]++;
						}
					}
				}
			}

			mixed_pop->front_counter++;
		}
	}

	if (new_pop_counter<size) {
		
		assign_crowding_distance_cpu (mixed_pop, 2*size, mixed_pop->front_counter);
		qsort_crowd_dist_cpu (mixed_pop, 2*size, mixed_pop->front_counter, 0, front_size[mixed_pop->front_counter]-1);
		for (int k=front_size[mixed_pop->front_counter]-1; k>=size-new_pop_counter; k--)
			copy_cpu (new_pop, new_pop_counter, size, mixed_pop, front_elements[front_offset[mixed_pop->front_counter] + k], 2*size);	
		assign_crowding_distance_cpu (new_pop, size, mixed_pop->front_counter);

	}
}

void fill (population_cpu *new_pop, population_cpu *mixed_pop, int *front_size, int *front_offset, int size) {
   
	int new_pop_counter = 0;
	int front_counter = 0;
	
	while (new_pop_counter + front_size[front_counter] <= size) {
	
			for (int k=0; k<front_size[front_counter]; k++, new_pop_counter++)
				copy_cpu (new_pop, new_pop_counter, size, mixed_pop, front_elements[front_offset[front_counter] + k], 2*size);

			//assign_crowding_distance (new_pop, size, front_counter);

			front_counter++;
		}

	if (new_pop_counter<size) {
		
		qsort_crowd_dist_cpu (mixed_pop, 2*size, front_counter, 0, front_size[front_counter]-1);
		
		for (int k=front_size[front_counter]-1; k>=size-new_pop_counter; k--)
			copy_cpu (new_pop, new_pop_counter, size, mixed_pop, front_elements[front_offset[front_counter] + k], 2*size);	

		assign_crowding_distance_cpu (new_pop, size, front_counter);

	}
}