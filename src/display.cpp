# include "global.h"

void onthefly_display (population_cpu *pop, int size, FILE *gp, int ii) {
    
    int flag;
    FILE *fpt;
    errno_t err = fopen_s(&fpt, "plot.out","w");
    flag = 0;

    for (int i=0; i<size; i++)
        if (pop->constr_violation[i]==0) {
            if (choice!=3)
                fprintf(fpt,"%e\t%e\n",pop->obj[(obj1-1)*size + i],pop->obj[(obj2-1)*size + i]);
            else
                fprintf(fpt,"%e\t%e\t%e\n",pop->obj[(obj1-1)*size + i],pop->obj[(obj2-1)*size + i],pop->obj[(obj3-1)*size + i]);
            fflush(fpt);
            flag = 1;
        }

	fclose(fpt);
	Sleep(500);

    if (flag==0)
        printf("\n No feasible soln in this pop, hence no display");
    else {
        if (choice!=3)
            fprintf(gp,"set title 'Generation #%d'\n unset key\n plot 'plot.out' w points pointtype 6 pointsize 1\n",ii);
        else
            fprintf(gp,"set title 'Generation #%d'\n set view %d,%d\n unset key\n splot 'plot.out' w points pointtype 6 pointsize 1\n",ii,angle1,angle2);
        fflush(gp);
    }
}
