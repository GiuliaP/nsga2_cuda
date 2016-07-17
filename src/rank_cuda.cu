#include <thrust/iterator/counting_iterator.h>
#include <thrust/device_ptr.h>
#include <thrust/device_vector.h>
#include <thrust/copy.h>
#include <thrust/sort.h>

#include "global_cuda.cuh"

using namespace thrust;
using namespace thrust::placeholders;

template<typename T>
struct is_one : thrust::unary_function<T, bool>
{
    __host__ __device__
    bool operator()(const T &x)
    {
        return (x==1);
    }
};

template<typename T>
struct is_zero : thrust::unary_function<T, bool>
{
    __host__ __device__
    bool operator()(const T &x)
    {
        return (x==0);
    }
};

__global__ void calc_next_front (float *n, const float *S, const int pop_size, int *front_elements_loc, int *front_size_loc, int *front_offset_loc, const int front) {

	int tid = blockIdx.x*blockDim.x + threadIdx.x;
	
	int index;
	float S_loc;
	float n_loc = n[tid];
	int new_front_loc=0;

	for (int k=0; k<front_size_loc[front-1]; k++) {
		S_loc = S[front_elements_loc[front_offset_loc[front-1] + k]*pop_size + tid];
		n_loc -= S_loc;
		new_front_loc = new_front_loc || ( (n_loc==0) && (S_loc==1) );
	}

	if (new_front_loc) {
		index = atomicAdd(front_size_loc + front, 1);
		front_elements_loc[front_offset_loc[front] + index] = tid;
	}

	n[tid] = n_loc;
}

void qsort_front_obj_gpu (int *values, float *keys, int left, int right) {
    
	int index;
    int temp;
    int i, j;
    float pivot;
    
	if (left<right) {
        
		index = rnd (left, right);
        temp = values[right];
        values[right] = values[index];
        values[index] = temp;

        pivot = keys[values[right]];
		i = left-1;
        for (j=left; j<right; j++)
			if (keys[values[j]] <= pivot) {
                i++;
                temp = values[j];
                values[j] = values[i];
                values[i] = temp;
            }
       
        index=i+1;
        temp = values[index];
		values[index] = values[right];
		values[right] = temp;	
       
        qsort_front_obj_gpu (values, keys, left, index-1);
        qsort_front_obj_gpu (values, keys, index+1, right);
    }
}

void assign_crowd_dist_gpu (population_cpu *pop, int size, int front) {
   
	// 1) esplorare possibilità di usare la thrust per fare una sort_by_key che nel frattempo copi sull'host: obj_i_copy(k) = obj_i(front_elements(k)) for i=0, .., nobj-1 and k=0, .., size-1
	// 2) risolvere il problema dell'estrazione random nella sort di adesso (collaterale al punto 1)

	if (front_size_locked[front]==1) {
		pop->crowd_dist[front_elements_locked[0]] = INF;
		return;
	}
	if (front_size_locked[front]==2) {
		pop->crowd_dist[front_elements_locked[0]] = INF;
		pop->crowd_dist[front_elements_locked[1]] = INF;
		return;
	}
	
	for (int i=0; i<nobj; i++)
		 memcpy(tmp_locs_32s + i*size, front_elements_locked + front_offset_locked[front], front_size_locked[front]*sizeof(int));

	for (int i=0; i<nobj; i++) {
		 qsort_front_obj_gpu (tmp_locs_32s + i*size, pop->obj + i*size, 0, front_size_locked[front]-1);
		 pop->crowd_dist[tmp_locs_32s[i*size]] = INF;
	}

    for (int i=0; i<nobj; i++)
		for (int j=1; j<front_size_locked[front]-1; j++) {
			if (pop->crowd_dist[tmp_locs_32s[i*size + j]] != INF) {
				if (pop->obj[i*size + tmp_locs_32s[i*size + front_size_locked[front]-1 ]] == pop->obj[i*size + tmp_locs_32s[i*size]])
                    pop->crowd_dist[tmp_locs_32s[i*size + j]] += 0.0f;
                else
                    pop->crowd_dist[tmp_locs_32s[i*size + j]] += (pop->obj[size*i + tmp_locs_32s[i*size + j+1]] - pop->obj[size*i + tmp_locs_32s[i*size + j-1]]) / (pop->obj[size*i + tmp_locs_32s[i*size + front_size_locked[front]-1]] - pop->obj[size*i + tmp_locs_32s[i*size + 0]]);
			}
		}

	for (int j=0; j<front_size_locked[front]; j++)
		if (pop->crowd_dist[front_elements_locked[front_offset_locked[front] + j]] != INF)
			pop->crowd_dist[front_elements_locked[front_offset_locked[front] + j]] /= nobj;
    
}

void assign_rank_and_crowd_dist_gpu (population_gpu *pop_dev, population_cpu *pop, const int size) {
   
	int scale = 4;
	dim3 blocksPerGrid_dominance(size*scale, 1, 1);
	dim3 threadsPerBlock_dominance(size/scale, 1, 1);
	dim3 blocksPerGrid_front_elements(size/scale, 1, 1);
	dim3 threadsPerBlock_front_elements(scale, 1, 1);

	float alpha = 1.0f;
	float beta = 0.0f;

	NppiSize oneRowRoi;
	oneRowRoi.height = 1;
	oneRowRoi.width = size;
	int step = oneRowRoi.width*sizeof(float);

	device_ptr<int> tp_front_elements;
	device_ptr<int> tp_front_elements_end;
	device_ptr<float> tp_stencil;
	device_ptr<float> tp_idx_32f_dev = device_pointer_cast(idx_32f_dev);
	int front_size_0, front_size_1;
		
	// porta sull'host gli obj per usarli nel calcolo della crowd dist
	checkCudaErrors(cudaMemcpy(pop->obj, pop_dev->obj, nobj*popsize*sizeof(float), cudaMemcpyDeviceToHost));
	
	// S_dev:
	check_dominance_kernel_masked<<<blocksPerGrid_dominance, threadsPerBlock_dominance>>>(pop_dev->obj, pop_dev->constr_violation, size, nobj, S_dev, flag_dev);

	// n_dev:
	cublasSgemv(handle, CUBLAS_OP_N, size, size, &alpha, S_dev, size, ones_32f_dev, 1, &beta, n_dev, 1);

	// azzera crowd_dist --> sull'host (infatti usa pop e non pop_dev) <--
	memset(pop->crowd_dist, 0, size*sizeof(float));
	
	///////// PRIMO FRONTE ///////////

	pop->front_counter = 0;
	front_offset_locked[0] = 0;
	
	// 1) vettore binario degli elementi:
	nppiThreshold_LTValGTVal_32f_C1R(n_dev, step, tmp_locs_32f_dev + size, step, oneRowRoi, 1, 1, 0, 0);

	// 3) trova indici e determina numero elementi:
	tp_front_elements = device_pointer_cast(front_elements_dev);
	tp_stencil = device_pointer_cast(tmp_locs_32f_dev + size);
	
	tp_front_elements_end = copy_if(tp_idx_32f_dev, tp_idx_32f_dev + size, tp_stencil, tp_front_elements, is_one<int>());
	front_size_0 = tp_front_elements_end - tp_front_elements;
	front_size_locked[0] = front_size_0;

	// 4) assegna crowd_dist --> sull'host (infatti usa pop e non pop_dev) <--
	assign_crowd_dist_gpu (pop, size, pop->front_counter);

	///////// SECONDO FRONTE /////////

	pop->front_counter++;
	front_offset_locked[1] = front_size_0;
	
	// 1) vettore binario degli elementi:
	cublasSgemv(handle, CUBLAS_OP_N, size, size, &alpha, S_dev, size, tmp_locs_32f_dev + size, 1, &beta, tmp_locs_32f_dev, 1);
	nppsSub_32f_I(tmp_locs_32f_dev, n_dev, size);
	nppiThreshold_LTValGTVal_32f_C1R(n_dev, step, tmp_locs_32f_dev, step, oneRowRoi, 1, 1, 0, 0);
	nppsSub_32f_I(tmp_locs_32f_dev + size, tmp_locs_32f_dev, size);
	
	// 3) trova indici e determina numero elementi:
	tp_front_elements = device_pointer_cast(front_elements_dev + front_size_0);
	tp_stencil = device_pointer_cast(tmp_locs_32f_dev);
		
	tp_front_elements_end = copy_if(tp_idx_32f_dev, tp_idx_32f_dev + size, tp_stencil, tp_front_elements, is_one<int>());
	front_size_1 = tp_front_elements_end - tp_front_elements;
	front_size_locked[1] = front_size_1;

	///////// FRONTI SUCCESSIVI /////////
	
	while (front_size_locked[pop->front_counter]>0) {

		pop->front_counter++;
		front_size_locked[pop->front_counter] = 0;
		front_offset_locked[pop->front_counter] = front_offset_locked[pop->front_counter-1] + front_size_locked[pop->front_counter-1];

		calc_next_front<<<blocksPerGrid_front_elements,threadsPerBlock_front_elements>>>(n_dev, S_dev, size, 
						front_elements_dev, front_size_dev, front_offset_dev, pop->front_counter);

		assign_crowd_dist_gpu (pop, size, pop->front_counter-1);

		cudaDeviceSynchronize();
	}

	// copia sul device la crowd_dist calcolata
	checkCudaErrors(cudaMemcpy(pop_dev->crowd_dist, pop->crowd_dist, popsize*sizeof(float), cudaMemcpyHostToDevice));
}    
