# include "global.h"

extern float oldrand[55];
int jrand;

void initialize_rndgen_cpu (float seed) {
   
    for (int j1=0; j1<=54; j1++)
		oldrand[j1] = 0.0;
    jrand=0;
    warmup_random(seed);
}
void warmup_random (float seed) {
    
    int ii; 
	float new_random, prev_random;
    oldrand[54] = seed;
    new_random = 0.000000001f;
    prev_random = seed;
    for (int j1=1; j1<=54; j1++) {
        ii = (21*j1)%54;
        oldrand[ii] = new_random;
        new_random = prev_random-new_random;
        if (new_random<0.0)
			new_random += 1.0;
        prev_random = oldrand[ii];
    }
    advance_rndgen_cpu ();
    advance_rndgen_cpu ();
    advance_rndgen_cpu ();
    jrand = 0;
}

/* Create next batch of 55 random numbers */
void advance_rndgen_cpu () {
    
    float new_random;
    for (int j1=0; j1<24; j1++) {
        new_random = oldrand[j1]-oldrand[j1+31];
        if (new_random<0.0)
            new_random = new_random+1.0f;
        oldrand[j1] = new_random;
    }
    for (int j1=24; j1<55; j1++) {
        new_random = oldrand[j1]-oldrand[j1-24];
        if (new_random<0.0)
            new_random = new_random+1.0f;
        oldrand[j1] = new_random;
    }
}
/* Fetch a single random number between 0.0 and 1.0 */
float randomperc() {
    
	jrand++;
    if (jrand>=55) {
        jrand = 1;
        advance_rndgen_cpu ();
    }
    return((float)oldrand[jrand]);
}
/* Fetch a single random integer between low and high including the bounds */
int rnd (int low, int high) {
    
	int res;
    if (low >= high)
        res = low;
    else {
        res = low + (randomperc()*(high-low+1.0f));
        if (res > high)
            res = high;
    }
    return (res);
}
/* Fetch a single random real number between low and high including the bounds */
float rndreal (float low, float high) {
	
	return (low + (high-low)*randomperc());
}
