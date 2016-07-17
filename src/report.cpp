# include "global.h"

/* Function to print the information of a population in a file */
void report_pop (population_cpu *pop, int size, FILE *fpt) {
    
	for (int i=0; i<size; i++) {
        
		for (int j=0; j<nobj; j++)
            fprintf(fpt,"%e\t",pop->obj[j*size + i]);
      
        if (ncon!=0)
            for (int j=0; j<ncon; j++)
                fprintf(fpt,"%e\t",pop->constr[j*size + i]); 
        
		if (nreal!=0)
            for (int j=0; j<nreal; j++)
                fprintf(fpt,"%e\t",pop->xreal[j*size + i]);
        
        fprintf(fpt,"%e\n",pop->crowd_dist[i]);
    }
}

/* Function to print the information of feasible and non-dominated population in a file */
void report_feasible (population_cpu *pop, int size, FILE *fpt) {
    
    /*
	for (int i=0; i<front_size[0]; i++)
		if (pop->constr_violation[i]==0.0f) {
            
			for (int j=0; j<nobj; j++)
                fprintf(fpt,"%e\t",pop->obj[j*size + pop->front_elements[front_offset[0] + i]]);
            
			if (ncon!=0)
                for (int j=0; j<ncon; j++)
                    fprintf(fpt,"%e\t",pop->constr[j*size + pop->front_elements[front_offset[0] + i]]);

            if (nreal!=0)
                for (int j=0; j<nreal; j++)
                    fprintf(fpt,"%e\t",pop->xreal[j*size + pop->front_elements[front_offset[0] + i]]);

            fprintf(fpt,"%e\t",pop->constr_violation[i]);
            fprintf(fpt,"%e\n",pop->crowd_dist[i]);
        }
	*/
}
