# include "global.h"
# include "rand.h"

void assign_crowding_distance (population *pop, int size, int front) {
   
    if (pop->front_size[front]==1) {
        //pop->crowd_dist[pop->front_elements[pop->front_offset[front]]] = INF;
		pop->crowd_dist[pop->front_elements[pop->front_offset[front]]] = INF;
        return;
    }
    if (pop->front_size[front]==2) {
		pop->crowd_dist[pop->front_elements[pop->front_offset[front]]] = INF;
        pop->crowd_dist[pop->front_elements[pop->front_offset[front] + 1]] = INF;
        return;
    }
	
	for (int i=0; i<nobj; i++) {
		 memcpy(tmp_locs + i*size, pop->front_elements, size*sizeof(int));
		 qsort_front_obj (pop, size, front, i, 0, pop->front_size[front]-1);
	}

	for (int j=0; j<pop->front_size[front]; j++)
        pop->crowd_dist[pop->front_elements[pop->front_offset[front] + j]] = 0.0f;

    for (int i=0; i<nobj; i++) {
        pop->crowd_dist[tmp_locs[i*size + pop->front_offset[front] + 0]] = INF;
		//pop->crowd_dist[tmp_locs[i*size + pop->front_offset[front] + pop->front_size[front]-1]] = INF;
	}

    for (int i=0; i<nobj; i++)
		for (int j=1; j<pop->front_size[front]-1; j++) {
			if (pop->crowd_dist[tmp_locs[i*size + pop->front_offset[front] + j]] != INF) {
				if (pop->obj[i*size + tmp_locs[i*size + pop->front_offset[front] + pop->front_size[front] -1 ]] == pop->obj[i*size + tmp_locs[i*size + pop->front_offset[front] + 0 ]])
                    pop->crowd_dist[tmp_locs[i*size + pop->front_offset[front] + j]] += 0.0f;
                else
                    pop->crowd_dist[tmp_locs[i*size + pop->front_offset[front] + j]] += (pop->obj[size*i + tmp_locs[i*size + pop->front_offset[front] + j+1]] - pop->obj[size*i + tmp_locs[i*size + pop->front_offset[front] + j-1]]) / (pop->obj[size*i + tmp_locs[i*size + pop->front_offset[front] + pop->front_size[front]-1]] - pop->obj[size*i + tmp_locs[i*size + pop->front_offset[front] + 0]]);
			}
		}

	for (int j=0; j<pop->front_size[front]; j++)
		if (pop->crowd_dist[pop->front_elements[pop->front_offset[front] + j]] != INF)
			pop->crowd_dist[pop->front_elements[pop->front_offset[front] + j]] /= nobj;
    
}