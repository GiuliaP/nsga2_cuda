#include "global_cuda.cuh"

void mutation_gpu (population_gpu *pop, int size) {

	if (nreal!=0)
		for (int i=0; i<size; i++)
			realmutate_gpu (pop, size, i);
}

void realmutate_gpu (population_gpu *pop, int size, int ind) {
   
    float rnd;
	float delta1, delta2;
	float mut_pow, deltaq;
    float y, yl, yu;
	float val, xy;
    
	for (int j=0; j<nreal; j++)
        if (randomperc() <= pmut_real) {
            y = pop->xreal[j*size + ind];
            yl = min_realvar[j];
            yu = max_realvar[j];
            delta1 = (y-yl)/(yu-yl);
            delta2 = (yu-y)/(yu-yl);
            rnd = randomperc();
            mut_pow = 1.0f/(eta_m+1.0f);
            if (rnd <= 0.5f) {
                xy = 1.0f-delta1;
                val = 2.0f*rnd + (1.0f-2.0f*rnd)*pow(xy,eta_m+1.0f);
                deltaq =  pow(val,mut_pow) - 1.0f;
            }
            else {
                xy = 1.0f-delta2;
                val = 2.0f*(1.0f-rnd)+2.0f*(rnd-0.5f)*pow(xy,eta_m+1.0f);
                deltaq = 1.0f - (pow(val,mut_pow));
            }
            y = y + deltaq*(yu-yl);
            if (y<yl)
                y = yl;
            if (y>yu)
                y = yu;
            pop->xreal[j*size + ind] = y;
            nrealmut+=1;
        }
}