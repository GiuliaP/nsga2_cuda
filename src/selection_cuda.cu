#include <thrust/iterator/counting_iterator.h>
#include <thrust/device_ptr.h>
#include <thrust/device_vector.h>
#include <thrust/copy.h>
#include <thrust/sort.h>

#include "global_cuda.cuh"

using namespace thrust;
using namespace thrust::placeholders;

__constant__ float *min_realvar_dev;
__constant__ float *max_realvar_dev;

__constant__ float pcross_real_dev;
__constant__ int nrealcross_dev;
__constant__  float eta_c_dev;

__constant__ float pmut_real_dev;
__constant__ int nrealmut_dev;
__constant__ float eta_m_dev;

void copy_constants() {
	
	checkCudaErrors(cudaMemcpyToSymbol(min_realvar_dev, &min_realvar, nreal*sizeof(float)));
	checkCudaErrors(cudaMemcpyToSymbol(max_realvar_dev, &max_realvar, nreal*sizeof(float)));
	
	checkCudaErrors(cudaMemcpyToSymbol(&pcross_real_dev, &pcross_real, sizeof(float)));
	checkCudaErrors(cudaMemcpyToSymbol(&nrealcross_dev, &nrealcross, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(&eta_c_dev, &eta_c, sizeof(float)));
	
	checkCudaErrors(cudaMemcpyToSymbol(&pmut_real_dev, &pmut_real, sizeof(float)));
	checkCudaErrors(cudaMemcpyToSymbol(&nrealmut_dev, &nrealmut, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(&eta_m_dev, &eta_m, sizeof(float)));
}

void selection_gpu (population_gpu *old_pop, population_gpu *new_pop, int size) {
    
	float *parent12 = tmp_locs_32f_dev;
	
	float *p12 = tmp_locs_32f_dev + 2*size;
	
	float *rand = tmp_locs_32f_dev + 3*size; // per la dimensione di rand1 vedi sotto
	  
	dim3 threadsPerBlock_rndvec(DIM,1,1);
	dim3 blocksPerGrid_rndvec(1,1,1);

	dim3 threadsPerBlock_tournament(size,1,1);
	dim3 blocksPerGrid_tournament(1,1,1);

	dim3 threadsPerBlock_realcross(size,1,1);
	dim3 blocksPerGrid_realcross(nreal,1,1);

	extract_rndvec_kernel<<<blocksPerGrid_rndvec, threadsPerBlock_rndvec>>>(rand, 2*size);

	checkCudaErrors(cudaMemcpy(parent12, idx_32f_dev, size*sizeof(float), cudaMemcpyDeviceToDevice));
	checkCudaErrors(cudaMemcpy(parent12 + size, idx_32f_dev, size*sizeof(float), cudaMemcpyDeviceToDevice));
	
	sort_by_key(rand, rand + size, parent12);
	sort_by_key(rand + size, rand + 2*size, parent12 + size);

	// serbatoio di numeri random 
	extract_rndvec_kernel<<<blocksPerGrid_rndvec, threadsPerBlock_rndvec>>>(rand, size + // tournament
																	 size/2 + // tmp1 soglia pcross_real
																	 3*nreal*size/2); // tmp2a, tmp2b, tmp2c
	
	// mettere due stream e lanciare le due coppie di kernel in concorrenza
	tournament_kernel<<<blocksPerGrid_tournament,threadsPerBlock_tournament>>>(parent12, p12, old_pop->crowd_dist, flag_dev, size, rand);
	
	// 3) nreal_cross è la somma degli elementi pari a uno dentro a ...

	// 3) soglia 0.5 sui numeri rand1(size + size/2, size + size/2 + 2*nreal*size/2) e mette il risultato in tmp2
	// 4) il primo kernel usera tmp2(0, 2*nreal*size/4) e il secondo tmp2(2*nreal*size/4, 2*nreal*size/2)
		
	realcross_kernel<<<blocksPerGrid_realcross,threadsPerBlock_realcross>>>(old_pop->xreal, new_pop->xreal, size,
																				p12,
																				rand + size,
																				rand + size + size/2,
																				rand + size + size/2 + nreal*size/2,
																				rand + size + size/2 + 2*nreal*size/2);
}

__global__ void tournament_kernel (float* parent, float* p, float* crowd_dist_loc, char* flag, const int size, float *rand_vec) {

	unsigned tid = threadIdx.x + blockIdx.x*blockDim.x; // da 0 a size-1
	unsigned idx = tid + (tid%2) + (size-2)*((tid%4)>1);
	int ind1 = (int)parent[idx];
	int ind2 = (int)parent[idx + 1];

	float crowd_dist_loc1 = crowd_dist_loc[ind1];
	float crowd_dist_loc2 = crowd_dist_loc[ind2];

	int flag_loc = (int)flag[ind2*size + ind1];

	float rand_num = rand_vec[tid];

	p[tid] = (flag_loc==-1)*ind2 + 
		      (flag_loc==1)*ind1 + 
		      (flag_loc==0)*( (crowd_dist_loc1>crowd_dist_loc2)*ind1 + 
			                (crowd_dist_loc2>crowd_dist_loc1)*ind2 + 
							(crowd_dist_loc1==crowd_dist_loc2)*( (rand_num<=0.5f)*ind1 + (rand_num>0.5f)*ind2 ) );
}

__global__ void realcross_kernel (float* parent_x, float* child_x, int size, float* p, float *tmp1, float *tmp2a, float *tmp2b, float *tmp2c) {

	unsigned tid = threadIdx.x + blockIdx.x*blockDim.x; // da 0 a nreal*size-1
	unsigned c = tid/size;
	unsigned offc = c*size;
	unsigned pidx = tid - offc; // da 0 a size-1
	unsigned rem = (pidx%2);
	pidx -= rem; 
	int p1 = (int)p[pidx];
	int p2 = (int)p[pidx + 1];
	
	float xp1 = parent_x[offc + p1];
	float xp2 = parent_x[offc + p2];

	// 1) ogni thread soglia pcross_real su tmp1(0,size/2)
	bool thresh_ind = tmp1[pidx/2]<=pcross_real_dev;
	// 2) ogni thread soglia 0.5 su tmp2(0, nreal*size/2) fa la and e la mette in thresh_x1
	bool thresh_x1 = thresh_ind && ( tmp2a[tid/2]<=0.5f );
	// 3) ogni thread soglia EPS su fabs(xp1-xp2) fa la and e la mette in thresh_x2
	bool thresh_x2 = thresh_x1 && (fabs(xp1-xp2)>EPS);
	
	float y1 = min(xp1, xp2);
	float y2 = max(xp1, xp2);
	
	float yl = min_realvar_dev[c];
	float yu = max_realvar_dev[c];

	float beta1 = 1.0f + (2.0f*fabs(y1-yl)/(y2-y1));
	float beta2 = 1.0f + (2.0f*fabs(y2-yu)/(y2-y1));

	float alpha1 = 2.0f - powf(beta1,-(eta_c_dev+1.0f));
	float alpha2 = 2.0f - powf(beta2,-(eta_c_dev+1.0f));

	float rand = tmp2b[tid/2];
	bool thresh_rand1 = (rand <= (1.0f/alpha1));
	bool thresh_rand2 = (rand <= (1.0f/alpha2));

	float betaq1 = powf (rand*alpha1, 1.0f/(eta_c_dev+1.0f))*thresh_rand1 + powf (1.0f/(2.0f - rand*alpha1), 1.0f/(eta_c_dev+1.0f))*(1-thresh_rand1);
	float betaq2 = powf (rand*alpha2, 1.0f/(eta_c_dev+1.0f))*thresh_rand2 + powf (1.0f/(2.0f - rand*alpha2), 1.0f/(eta_c_dev+1.0f))*(1-thresh_rand2);
					
	float c1 = 0.5f * ((y1+y2) - betaq1*(y2-y1));
	float c2 = 0.5f * ((y1+y2) + betaq2*(y2-y1));

    c1 = (c1<yl)? yl : c1; 
	c1 = (c1>yu)? yu : c1; 
	c2 = (c2<yl)? yl : c2; 
	c2 = (c2>yu)? yu : c2; 

	rand = tmp2c[tid/2];
	bool thresh_rand = (rand<=0.5f);
	
	// 4a) adesso i punti per cui thresh_x2 vale 1 sono da modificare nel modo seguente:
	float child_x_changed = c1 * ( (1-rem)*(1-thresh_rand) + rem*thresh_rand ) + c2 * ( (1-rem)*thresh_rand + rem*(1-thresh_rand) );
	
	// 4b) gli altri nel modo seguente:
	float child_x_unchanged = xp1*(1-rem) + xp2*rem;
   
	child_x[tid] = child_x_changed * thresh_x2 + child_x_unchanged * (1-thresh_x2);
}