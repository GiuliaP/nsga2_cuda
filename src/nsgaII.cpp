/* NSGA-II routine (implementation of the 'main' function) */

#include "global_cuda.cuh"

int nreal;
int nobj;
int ncon;
int popsize;
int ngen;

/////////////////////

float *min_realvar;
float *max_realvar;

float pcross_real;
int nrealcross;
float eta_c;

float pmut_real;
float eta_m;
int nrealmut;

/////////////////////

int *tmp_locs_32s;
float *tmp_locs_32f_dev;

float *flag;
char *flag_dev;
unsigned char *S;
float *S_dev;
float *n;
float *n_dev;

float *ones_32f_dev;
float *idx_32f_dev;
float *idx_32f;

int *front_size;
int *front_offset;
int *front_elements;
int *front_size_locked;
int *front_offset_locked;
int *front_elements_locked;
int *front_offset_dev;
int *front_size_dev;
int *front_elements_dev;

int choice;
int obj1;
int obj2;
int obj3;
int angle1;
int angle2;

cublasHandle_t handle;

int main (int argc, char **argv) {

	int device = 0;
	cudaDeviceProp deviceProperties;
	checkCudaErrors(cudaGetDeviceProperties(&deviceProperties, device)); 
	
	// MAPPED memory
	if (!deviceProperties.canMapHostMemory)
		printf("\n Mapped memory not supported on this device!\n");
	cudaSetDeviceFlags(cudaDeviceMapHost);

	// CUBLAS library
	checkCudaErrors(cublasCreate(&handle));
 
    FILE *fpt1, *fpt2, *fpt3, *fpt4, *fpt5, *gp;
	fopen_s(&fpt1, "data/initial_pop.out","w");
    fopen_s(&fpt2, "data/final_pop.out","w");
    fopen_s(&fpt3, "data/best_pop.out","w");
    fopen_s(&fpt4, "data/all_pop.out","w");
    fopen_s(&fpt5, "data/params.out","w");
    fprintf(fpt1,"# This file contains the data of initial population\n");
    fprintf(fpt2,"# This file contains the data of final population\n");
    fprintf(fpt3,"# This file contains the data of final feasible population (if found)\n");
    fprintf(fpt4,"# This file contains the data of all generations\n");
    fprintf(fpt5,"# This file contains information about inputs as read by the program\n");

    float seed = 0.6f;
    if (seed<=0.0f || seed>=1.0f) {
        printf("\n Seed value is wrong, must be in (0,1) \n");
        exit(1);
    }
	popsize = 1024;
	ngen = 1;
	nobj = 2;
	ncon = 0;
	nreal = 1;

    if (nreal != 0) {
        
		min_realvar = (float*) malloc(nreal*sizeof(float));
        max_realvar = (float*) malloc(nreal*sizeof(float));
        for (int i=0; i<nreal; i++) {
            min_realvar[i] = -1000;
			max_realvar[i] = 1000;
			/*
			printf ("\n Enter the lower limit of real variable %d : ",i+1);
            scanf_s ("%f",&min_realvar[i]);
            printf ("\n Enter the upper limit of real variable %d : ",i+1);
            scanf_s ("%f",&max_realvar[i]);
			*/
            if (max_realvar[i] <= min_realvar[i]) {
                printf("\n Wrong limits entered for the min and max bounds of real variable, hence exiting \n");
                exit(1);
            }
        }

		pcross_real = 0.9f;
		pmut_real = 1.0f/nreal;
		eta_c = 20;
		eta_m = 20;
    }


	printf("\n Do you want to use gnuplot to display the results realtime (0 for NO) (1 for YES) : ");
    scanf_s("%d",&choice);
    if (choice!=0 && choice!=1) {
        printf("\n Entered the wrong choice, hence exiting, choice entered was %d\n",choice);
        exit(1);
    }

    if (choice==1) {
        gp = _popen(GNUPLOT_COMMAND,"w");
        if (gp==NULL) {
            printf("\n Could not open a pipe to gnuplot, check the definition of GNUPLOT_COMMAND in file global.h\n");
            printf("\n Edit the string to suit your system configuration and rerun the program\n");
            exit(1);
        }
        if (nobj==2) {
			obj1=1;
			obj2=2;
            /*printf("\n Enter the objective for X axis display : ");
            scanf_s("%d",&obj1);
            if (obj1<1 || obj1>nobj) {
                printf("\n Wrong value of X objective entered, value entered was %d\n",obj1);
                exit(1);
            }
            printf("\n Enter the objective for Y axis display : ");
            scanf_s("%d",&obj2);
            if (obj2<1 || obj2>nobj) {
                printf("\n Wrong value of Y objective entered, value entered was %d\n",obj2);
                exit(1);
            }*/
            obj3 = -1;
        }
        else {
            printf("\n #obj > 2, 2D display or a 3D display ?, enter 2 for 2D and 3 for 3D :");
            scanf_s("%d",&choice);
            if (choice!=2 && choice!=3) {
                printf("\n Entered the wrong choice, hence exiting, choice entered was %d\n",choice);
                exit(1);
            }
            if (choice==2) {
                printf("\n Enter the objective for X axis display : ");
                scanf_s("%d",&obj1);
                if (obj1<1 || obj1>nobj) {
                    printf("\n Wrong value of X objective entered, value entered was %d\n",obj1);
                    exit(1);
                }
                printf("\n Enter the objective for Y axis display : ");
                scanf_s("%d",&obj2);
                if (obj2<1 || obj2>nobj) {
                    printf("\n Wrong value of Y objective entered, value entered was %d\n",obj2);
                    exit(1);
                }
                obj3 = -1;
            }
            else {
                printf("\n Enter the objective for X axis display : ");
                scanf_s("%d",&obj1);
                if (obj1<1 || obj1>nobj) {
                    printf("\n Wrong value of X objective entered, value entered was %d\n",obj1);
                    exit(1);
                }
                printf("\n Enter the objective for Y axis display : ");
                scanf_s("%d",&obj2);
                if (obj2<1 || obj2>nobj) {
                    printf("\n Wrong value of Y objective entered, value entered was %d\n",obj2);
                    exit(1);
                }
                printf("\n Enter the objective for Z axis display : ");
                scanf_s("%d",&obj3);
                if (obj3<1 || obj3>nobj) {
                    printf("\n Wrong value of Z objective entered, value entered was %d\n",obj3);
                    exit(1);
                }
                printf("\n You have chosen 3D display, hence location of eye required \n");
                printf("\n Enter the first angle (an integer in the range 0-180) (if not known, enter 60) :");
                scanf_s("%d",&angle1);
                if (angle1<0 || angle1>180) {
                    printf("\n Wrong value for first angle entered, hence exiting \n");
                    exit(1);
                }
                printf("\n Enter the second angle (an integer in the range 0-360) (if not known, enter 30) :");
                scanf_s("%d",&angle2);
                if (angle2<0 || angle2>360) {
                    printf("\n Wrong value for second angle entered, hence exiting \n");
                    exit(1);
                }
            }
        }
    }

    fprintf(fpt5,"\n Seed for random number generator = %e",seed);
	fprintf(fpt5,"\n\n Population size = %d",popsize);
    fprintf(fpt5,"\n Number of generations = %d",ngen);
    fprintf(fpt5,"\n Number of objective functions = %d",nobj);
    fprintf(fpt5,"\n Number of constraints = %d",ncon);
    
	fprintf(fpt5,"\n Number of real variables = %d",nreal);
	if (nreal!=0) {
        for (int i=0; i<nreal; i++) {
            fprintf(fpt5,"\n Lower limit of real variable %d = %e",i+1,min_realvar[i]);
            fprintf(fpt5,"\n Upper limit of real variable %d = %e",i+1,max_realvar[i]);
        }
        fprintf(fpt5,"\n Probability of crossover of real variable = %e",pcross_real);
        fprintf(fpt5,"\n Probability of mutation of real variable = %e",pmut_real);
        fprintf(fpt5,"\n Distribution index for crossover = %e",eta_c);
        fprintf(fpt5,"\n Distribution index for mutation = %e",eta_m);
    }

    fprintf(fpt1,"# of objectives = %d, # of constraints = %d, # of real_var = %d, constr_violation, crowding_distance\n",nobj,ncon,nreal);
    fprintf(fpt2,"# of objectives = %d, # of constraints = %d, # of real_var = %d, constr_violation, crowding_distance\n",nobj,ncon,nreal);
    fprintf(fpt3,"# of objectives = %d, # of constraints = %d, # of real_var = %d, constr_violation, crowding_distance\n",nobj,ncon,nreal);
    fprintf(fpt4,"# of objectives = %d, # of constraints = %d, # of real_var = %d, constr_violation, crowding_distance\n",nobj,ncon,nreal);
   
    nrealmut = 0;
    nrealcross = 0;
	
	population_cpu parent_pop;
	population_gpu parent_pop_dev;
	population_cpu child_pop;
	population_gpu child_pop_dev;
	population_cpu mixed_pop;
	population_gpu mixed_pop_dev;
    
	allocate_pop_cpu (&parent_pop, popsize);
    allocate_pop_cpu (&child_pop, popsize);
    allocate_pop_cpu (&mixed_pop, 2*popsize);

	allocate_pop_gpu (&parent_pop_dev, popsize);
    allocate_pop_gpu (&child_pop_dev, popsize);
    allocate_pop_gpu (&mixed_pop_dev, 2*popsize);

	int size_tmp_locs_32f_dev = std::max(std::max(ncon, 2),ntmp)*popsize*sizeof(float);
	checkCudaErrors(cudaMalloc((void**)&tmp_locs_32f_dev, size_tmp_locs_32f_dev));
	
	tmp_locs_32s = (int*) malloc(nobj*popsize*sizeof(int));

	checkCudaErrors(cudaMalloc((void**)&S_dev, popsize*popsize*sizeof(float)));
	checkCudaErrors(cudaMalloc((void**)&flag_dev, popsize*popsize*sizeof(char)));
	checkCudaErrors(cudaMalloc((void**)&n_dev, popsize*sizeof(float)));

	checkCudaErrors(cudaMalloc((void**)&ones_32f_dev, popsize*sizeof(float)));
	checkCudaErrors(nppsSet_32f(1.0f, ones_32f_dev, popsize));
	NppiSize oneRowRoi;
	oneRowRoi.height = 1;
	oneRowRoi.width = popsize;
	int step = oneRowRoi.width*sizeof(float);
	checkCudaErrors(nppiSet_32f_C1R(1.0f, ones_32f_dev, step, oneRowRoi));

	idx_32f = (float*) malloc(popsize*sizeof(float));
	checkCudaErrors(cudaMalloc((void**)&idx_32f_dev, popsize*sizeof(float)));
	for (int i=0; i<popsize; i++)
		idx_32f[i]=i;
    checkCudaErrors(cudaMemcpy(idx_32f_dev, idx_32f, popsize*sizeof(float), cudaMemcpyHostToDevice));

	front_size = (int*) malloc(popsize*sizeof(int));
	front_offset = (int*) malloc(popsize*sizeof(int));
	front_elements = (int*) malloc(popsize*sizeof(int));
	
	checkCudaErrors(cudaHostAlloc((void**)&front_size_locked, popsize*sizeof(int), cudaHostAllocMapped));
	checkCudaErrors(cudaHostAlloc((void**)&front_offset_locked, popsize*sizeof(int), cudaHostAllocMapped));
	checkCudaErrors(cudaHostAlloc((void**)&front_elements_locked, popsize*sizeof(int), cudaHostAllocMapped));
	//checkCudaErrors(cudaMalloc((void**)&front_elements_dev, popsize*sizeof(int)));
	
	cudaHostGetDevicePointer(&front_size_dev, front_size_locked, 0);
	cudaHostGetDevicePointer(&front_offset_dev, front_offset_locked, 0);
	cudaHostGetDevicePointer(&front_elements_dev, front_elements_locked, 0);

	upload_pop (&parent_pop_dev, &parent_pop, popsize);
    upload_pop (&child_pop_dev, &child_pop, popsize);
    upload_pop (&mixed_pop_dev, &mixed_pop, 2*popsize);
	copy_constants();

	initialize_rndgen_cpu (seed); 

	initialize_rndgen_gpu (seed);
	initialize_pop_gpu (&parent_pop_dev, popsize); // OK
	
	evaluate_gpu (&parent_pop_dev, popsize); // OK
    
	assign_rank_and_crowd_dist_gpu (&parent_pop_dev, &parent_pop, popsize); // OK
	
	/*
    fprintf(fpt4,"# gen = 1\n");
	report_pop(&parent_pop, popsize, fpt4);

    printf("\n gen = 1");
    fflush(stdout);
    
	if (choice!=0) 
		onthefly_display (&parent_pop, popsize, gp, 1);
   
	fflush(fpt1);
    fflush(fpt2);
    fflush(fpt3);
    fflush(fpt4);
    fflush(fpt5);
    */

	//Sleep(1);
    
	for (int i=2; i<=ngen; i++) {
        
		StopWatchInterface *timer = NULL;
		sdkCreateTimer(&timer);
		sdkResetTimer(&timer); 
		sdkStartTimer(&timer);

		selection_gpu (&parent_pop_dev, &child_pop_dev, popsize);

		sdkStopTimer(&timer);
		float elapsed = sdkGetTimerValue(&timer);
		printf("\n\n%f\n\n",elapsed);
		sdkDeleteTimer(&timer);

        //mutation_gpu (&child_pop_dev, popsize);

        //evaluate_gpu (&child_pop_dev, popsize); // OK

        //merge_gpu (&mixed_pop_dev, &parent_pop_dev, &child_pop_dev, popsize);
	
        //assign_rank_and_crowd_dist_gpu (&mixed_pop_dev, &mixed_pop, 2*popsize); // OK

		//fill_gpu (&parent_pop, &mixed_pop, popsize);

		/*
		download_pop (&parent_pop, &parent_pop_dev, popsize);
		
		if (choice!=0) 
			onthefly_display (&parent_pop, popsize, gp, i);

        fprintf(fpt4,"# gen = %d\n",i);
        report_pop(&parent_pop, popsize, fpt4); 
		fflush(fpt4);
        
		printf("\n gen = %d",i);
		*/
    }

	/*
	download_pop (&parent_pop, &parent_pop_dev, popsize);
	
	report_pop (&parent_pop, popsize, fpt2);
    report_feasible (&parent_pop, popsize, fpt3);

	if (choice!=0) 
		onthefly_display (&parent_pop, popsize, gp, ngen);
	*/	

    if (nreal!=0) {
        fprintf(fpt5,"\n Number of crossover of real variable = %d",nrealcross);
        fprintf(fpt5,"\n Number of mutation of real variable = %d",nrealmut);
    }
   
	fflush(stdout);
    fflush(fpt1);
    fflush(fpt2);
    fflush(fpt3);
    fflush(fpt4);
    fflush(fpt5);
    fclose(fpt1);
    fclose(fpt2);
    fclose(fpt3);
    fclose(fpt4);
    fclose(fpt5);

    if (choice!=0) {
        _pclose(gp);
    }

    if (nreal!=0) {
        free (min_realvar);
        free (max_realvar);
    }
	
	free(tmp_locs_32s);
	checkCudaErrors(cudaFree(tmp_locs_32f_dev));

	checkCudaErrors(cudaFree(S_dev));
	checkCudaErrors(cudaFree(flag_dev));
	checkCudaErrors(cudaFree(n_dev));

	checkCudaErrors(cudaFree(ones_32f_dev));
	free(idx_32f);
	checkCudaErrors(cudaFree(idx_32f_dev));

	free(front_size);
	free(front_offset);
	free(front_elements);
	checkCudaErrors(cudaFreeHost(front_size_locked));
	checkCudaErrors(cudaFreeHost(front_offset_locked));
	checkCudaErrors(cudaFreeHost(front_elements_locked));
	//checkCudaErrors(cudaFree(front_elements_dev));

	free_pop_gpu (&parent_pop_dev);
    free_pop_gpu (&child_pop_dev);
    free_pop_gpu (&mixed_pop_dev);
	
	free_pop_cpu (&parent_pop);
    free_pop_cpu (&child_pop);
    free_pop_cpu (&mixed_pop);

	checkCudaErrors(cublasDestroy(handle));

    cudaDeviceReset();

	printf("\n Routine successfully exited \n");
	system("PAUSE");
    exit(EXIT_SUCCESS);
	
}
