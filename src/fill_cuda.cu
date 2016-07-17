# include "global_cuda.cuh"

void fill_gpu (population_gpu *new_pop, population_gpu *mixed_pop, int* front_size, int* front_offset, int size) {
   
	/*
	int new_pop_counter = 0;
	int front_counter = 0;
	
	while (new_pop_counter + front_size[front_counter] <= size) {
	
			for (int k=0; k<front_size[front_counter]; k++, new_pop_counter++)
				copy_gpu (new_pop, new_pop_counter, size, mixed_pop, front_elements_locked[front_offset[front_counter] + k], 2*size);

			assign_crowding_distance (new_pop, size, front_counter);

			front_counter++;
		}

	if (new_pop_counter<size) {
		
		qsort_crowd_dist (mixed_pop, 2*size, front_counter, 0, front_size[front_counter]-1);
		
		for (int k=front_size[front_counter]-1; k>=size-new_pop_counter; k--)
			copy_gpu (new_pop, new_pop_counter, size, mixed_pop, front_elements_locked[front_offset[front_counter] + k], 2*size);	

		assign_crowding_distance (new_pop, size, front_counter);

	}
	*/
}