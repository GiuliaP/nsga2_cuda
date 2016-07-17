# ifndef _GLOBAL_H_
# define _GLOBAL_H_

# define INF 1.0e14
# define EPS 1.0e-14
# define E  2.71828182845905
# define PI 3.14159265358979
# define GNUPLOT_COMMAND "gnuplot -persist"

#define ntmp 6

#define DIM 55

# define sch1 
/* # define sch2 */
/* # define fon */
/* # define kur */
/* # define pol */
/* # define vnt */
/* # define zdt1 */
/* # define zdt2 */
/* # define zdt3 */
/* # define zdt4 */
/* # define zdt5 */
/* # define zdt6 */
/* # define bnh */
/* # define osy */
/* # define srn */
/* # define tnk */
/* # define ctp1 */
/* # define ctp2 */
/* # define ctp3 */
/* # define ctp4 */
/* # define ctp5 */
/* # define ctp6 */
/* # define ctp7 */
/*# define ctp8*/

////////////////////////////////////////////////////////

// System includes.
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// STL includes.
#include <iostream>
#include <string>
#include <iomanip>

// Sample framework includes.
#include <npp.h>
#include <nppdefs.h>
#include <cublas_v2.h>
#include <cuda_runtime.h>
#include <helper_timer.h>
#include <helper_cuda.h>
#include <device_launch_parameters.h>

#include <windows.h>

typedef struct {
    
    float *constr_violation;
	float *constr; 
    float *xreal;
	float *obj;

	int front_counter;

	float *crowd_dist;

} population_cpu;

extern int *front_size;
extern int *front_offset;
extern int *front_elements;

extern int *tmp_locs_32s;
extern float *n;
extern unsigned char *S;
extern float *flag;

extern float *min_realvar;
extern float *max_realvar;

extern int nreal;
extern int nobj;
extern int ncon;
extern int popsize;

extern float pcross_real;
extern float pmut_real;

extern float eta_c;
extern float eta_m;

extern int ngen;
extern int nrealmut;
extern int nrealcross;

extern int choice;
extern int obj1;
extern int obj2;
extern int obj3;
extern int angle1;
extern int angle2;

void allocate_pop_cpu (population_cpu *pop, int size);
void free_pop_cpu (population_cpu *pop);

void initialize_pop_cpu (population_cpu *pop, int size);

void evaluate_cpu (population_cpu *pop, int size);
void test_problem_cpu (population_cpu *pop, int size, int ind);
void assign_rank_and_crowding_distance_cpu (population_cpu *pop, int size);
float check_dominance_cpu (population_cpu *pop, int size, int p, int q);
void assign_crowding_distance_cpu (population_cpu *pop, int size, int front);
void qsort_front_obj_cpu (population_cpu *pop, int size, int front, int obj, int left, int right);

void selection_cpu (population_cpu *old_pop, population_cpu *new_pop, int size);
int tournament_cpu (population_cpu *pop, int size, int ind1, int ind2);
void realcross_cpu (population_cpu *parent_pop, population_cpu *child_pop, int size, int parent1, int parent2, int child1, int child2);

void mutation_cpu (population_cpu *pop, int size);
void realmutate_cpu (population_cpu *pop, int size, int ind);

void merge_cpu (population_cpu *dst_pop, population_cpu *src_pop1, population_cpu *src_pop2, int size);
void copy_cpu (population_cpu *dst_pop, int dst_ind, int dst_size, population_cpu *src_pop, int src_ind, int src_size);

void fill_nondominated_sort_cpu (population_cpu *new_pop, population_cpu *mixed_pop, int size);
void fill_cpu (population_cpu *new_pop, population_cpu *mixed_pop, int size);
void qsort_crowd_dist_cpu (population_cpu *pop, int size, int front, int left, int right);

void report_pop (population_cpu *pop, int size, FILE *fpt);
void report_feasible (population_cpu *pop, int size, FILE *fpt);
void onthefly_display (population_cpu *pop, int size, FILE *gp, int ii);

/* Variable declarations for the random number generator */

extern float oldrand[DIM];
extern int jrand;

/* Function declarations for the random number generator */

void initialize_rndgen_cpu (float seed);
void warmup_random (float seed);

void advance_rndgen_cpu ();

float randomperc();
int rnd (int low, int high);
float rndreal (float low, float high);

# endif
