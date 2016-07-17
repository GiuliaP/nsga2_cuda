# include "global.h"

void assign_crowding_distance_cpu (population_cpu *pop, int size, int front) {
   
    
	if (front_size[front]==1) {
        pop->crowd_dist[front_elements[front_offset[front]]] = INF;
		pop->crowd_dist[front_elements[front_offset[front]]] = INF;
        return;
    }
    if (front_size[front]==2) {
		pop->crowd_dist[front_elements[front_offset[front]]] = INF;
        pop->crowd_dist[front_elements[front_offset[front] + 1]] = INF;
        return;
    }
	
	for (int i=0; i<nobj; i++) {
		 memcpy(tmp_locs_32s + i*size, front_elements, size*sizeof(int));
		 qsort_front_obj_cpu (pop, size, front, i, 0, front_size[front]-1);
	}

	memset(pop->crowd_dist + front_elements[front_offset[front]], 0, front_size[front]*sizeof(float));

    for (int i=0; i<nobj; i++) {
        pop->crowd_dist[tmp_locs_32s[i*size + front_offset[front] + 0]] = INF;
		//pop->crowd_dist[tmp_locs[i*size + pop->front_offset[front] + pop->front_size[front]-1]] = INF;
	}

    for (int i=0; i<nobj; i++)
		for (int j=1; j<front_size[front]-1; j++) {
			if (pop->crowd_dist[tmp_locs_32s[i*size + front_offset[front] + j]] != INF) {
				if (pop->obj[i*size + tmp_locs_32s[i*size + front_offset[front] + front_size[front]-1 ]] == pop->obj[i*size + tmp_locs_32s[i*size + front_offset[front] + 0 ]])
                    pop->crowd_dist[tmp_locs_32s[i*size + front_offset[front] + j]] += 0.0f;
                else
                    pop->crowd_dist[tmp_locs_32s[i*size + front_offset[front] + j]] += (pop->obj[size*i + tmp_locs_32s[i*size + front_offset[front] + j+1]] - pop->obj[size*i + tmp_locs_32s[i*size + front_offset[front] + j-1]]) / (pop->obj[size*i + tmp_locs_32s[i*size + front_offset[front] + front_size[front]-1]] - pop->obj[size*i + tmp_locs_32s[i*size + front_offset[front] + 0]]);
			}
		}

	for (int j=0; j<front_size[front]; j++)
		if (pop->crowd_dist[front_elements[front_offset[front] + j]] != INF)
			pop->crowd_dist[front_elements[front_offset[front] + j]] /= nobj;
    

}