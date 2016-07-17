# ifndef _RAND_H_
# define _RAND_H_

/* Variable declarations for the random number generator */
extern float seed;
extern float oldrand[55];
extern int jrand;

/* Function declarations for the random number generator */
void advance_random ();

void initialize_rndgen_cpu ();
void warmup_random (float seed);

float randomperc();
int rnd (int low, int high);
float rndreal (float low, float high);

# endif
