# include "global.h"

void qsort_front_obj_cpu (population_cpu *pop, int size, int front, int object, int left, int right) {
    
	int index;
    int temp;
    int i, j;
    float pivot;
    
	if (left<right) {
        
		index = rnd (left, right);
        temp = tmp_locs_32s[object*size + front_offset[front] + right];
        tmp_locs_32s[object*size + front_offset[front] + right] = tmp_locs_32s[object*size + front_offset[front] + index];
        tmp_locs_32s[object*size + front_offset[front] + index] = temp;

        pivot = pop->obj[object*size + tmp_locs_32s[object*size + front_offset[front] + right]];
		i = left-1;
        for (j=left; j<right; j++)
			if (pop->obj[object*size + tmp_locs_32s[object*size + front_offset[front] + j]] <= pivot) {
                i++;
                temp = tmp_locs_32s[object*size + front_offset[front] + j];
                tmp_locs_32s[object*size + front_offset[front] + j] = tmp_locs_32s[object*size + front_offset[front] + i];
                tmp_locs_32s[object*size + front_offset[front] + i] = temp;
            }
       
        index=i+1;
        temp = tmp_locs_32s[object*size + front_offset[front] + index];
		tmp_locs_32s[object*size + front_offset[front] + index] = tmp_locs_32s[object*size + front_offset[front] + right];
		tmp_locs_32s[object*size + front_offset[front] + right] = temp;	
       
        qsort_front_obj_cpu (pop, size, front, object, left, index-1);
        qsort_front_obj_cpu (pop, size, front, object, index+1, right);
    }
}

void qsort_crowd_dist_cpu (population_cpu *pop, int size, int front, int left, int right) {
    
	int index;
    int temp;
    int i, j;
    float pivot;
    
	if (left<right) {

        index = rnd (left, right);
        temp = front_elements[front_offset[front] + right];
        front_elements[front_offset[front] + right] = front_elements[front_offset[front] + index];
        front_elements[front_offset[front] + index] = temp;

        pivot = pop->crowd_dist[front_elements[front_offset[front] + right]];
        i = left-1;
        for (j=left; j<right; j++)
            if (pop->crowd_dist[front_elements[front_offset[front] + j]] <= pivot) {
                i++;
                temp = front_elements[front_offset[front] + j];
                front_elements[front_offset[front] + j] = front_elements[front_offset[front] + i];
                 front_elements[front_offset[front] + i] = temp;
            }

        index=i+1;
        temp = front_elements[front_offset[front] + index];
        front_elements[front_offset[front] + index] = front_elements[front_offset[front] + right];
        front_elements[front_offset[front] + right] = temp;

        qsort_crowd_dist_cpu (pop, size, front, left, index-1);
        qsort_crowd_dist_cpu (pop, size, front, index+1, right);
    }

}
