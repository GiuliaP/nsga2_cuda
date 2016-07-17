# include "global.h"

/* Routine for usual non-domination checking
   It will return the following values
   1 if p dominates q
   -1 if q dominates p
   0 if both p and q are non-dominated */

float check_dominance_cpu (population_cpu *pop, int size, int p, int q) {
   
    int flag1 = 0;
    int flag2 = 0;
  
    if (pop->constr_violation[p]<0 && pop->constr_violation[q]<0) { // case 9
        
		 if (pop->constr_violation[p] > pop->constr_violation[q])
			 return (1);
		 if (pop->constr_violation[p] < pop->constr_violation[q])
			 return (-1);
		 return (0);  
	 }
	 
	 if (pop->constr_violation[p]<0 && pop->constr_violation[q]==0) // case 8
		 return (-1);
	 
	 if (pop->constr_violation[p] == 0 && pop->constr_violation[q] <0) // case 6
		 return (1);
	 
	 // case 1 2 3 4 5 7 
	 for (int i=0; i<nobj; i++) { 
		 if (pop->obj[i*size + p] < pop->obj[i*size + q])
			 flag1 = 1;
		 if (pop->obj[i*size + p] > pop->obj[i*size + q])
			 flag2 = 1; 
	 }
	 
	 if (flag1==1 && flag2==0)
		 return (1);
	 if (flag1==0 && flag2==1)
		 return (-1);
	 return (0);
   
}


