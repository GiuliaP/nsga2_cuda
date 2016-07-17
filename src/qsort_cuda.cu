#include "global_cuda.cuh"

void qsort_crowd_dist_gpu (population_gpu *pop, int size, int front, int left, int right) {
    
	/*
	int index;
    int temp;
    int i, j;
    float pivot;
    
	if (left<right) {

        index = rnd (left, right);
        temp = pop->front_elements[front_offset_locked[front] + right];
        pop->front_elements[front_offset_locked[front] + right] = pop->front_elements[front_offset_locked[front] + index];
        pop->front_elements[front_offset_locked[front] + index] = temp;

        pivot = pop->crowd_dist[pop->front_elements[front_offset_locked[front] + right]];
        i = left-1;
        for (j=left; j<right; j++)
            if (pop->crowd_dist[pop->front_elements[front_offset_locked[front] + j]] <= pivot) {
                i++;
                temp = pop->front_elements[front_offset_locked[front] + j];
                pop->front_elements[front_offset_locked[front] + j] = pop->front_elements[front_offset_locked[front] + i];
                pop->front_elements[front_offset_locked[front] + i] = temp;
            }

        index=i+1;
        temp = pop->front_elements[front_offset_locked[front] + index];
        pop->front_elements[front_offset_locked[front] + index] = pop->front_elements[front_offset_locked[front] + right];
        pop->front_elements[front_offset_locked[front] + right] = temp;

        qsort_crowd_dist_gpu (pop, size, front, left, index-1);
        qsort_crowd_dist_gpu (pop, size, front, index+1, right);
    }
	*/
}
