/**
 * OpenMP paralelizovan Mandelbrotov algoritam. Program ne daje ispravne rezultate
 * zbog stetnog preplitanja koje se pojavljuje u njemu. Pronaci i otkloniti greske koje
 * su uzrok stetnom preplitanju. Ocekivani izlaz ispravnog programa za numoutside
 * iznosi
 *	731179 za NPOINTS=1000
 *	182743 za NPOINTS=500
 *	7296 za NPOINTS=100
 *	70 za NPOINTS=10 	
 */

#include <stdio.h>
#include <omp.h>

#define NPOINTS 1000
#define MXITR 1000

void testpoint(void);
struct d_complex {
    double r; double i;
};
struct d_complex c;
int numoutside = 0;

int main(){
    int i, j;
    double area, error, eps = 1.0e-5;
    /**
     * default(shared) navodi da je podrazumevana vidljivost promenljivih deljena
     * nije previše važno za zadatak, ne obraćati pažnju na ovo
     */
    #pragma omp parallel for default(shared) private(c, eps)
    for (i = 0; i < NPOINTS; i++) {
        for (j = 0; j < NPOINTS; j++) {
            c.r = -2.0 + 2.5 * (double)(i) / (double)(NPOINTS) + eps;
            c.i = 1.125 * (double)(j) / (double)(NPOINTS) + eps;
            
            testpoint();
        }
    }
    
    printf("Number outside: %d\n", numoutside);

    return 0;
}

void testpoint(void){
    struct d_complex z;
    int iter;
    double temp;
    z = c;
    for (iter = 0; iter < MXITR; iter++){
        temp = (z.r * z.r) - (z.i * z.i) + c.r;
        z.i = z.r * z.i * 2 + c.i;
        z.r = temp;
        if ((z.r * z.r + z.i * z.i) > 4.0) {
            numoutside++;
            break;
        }
    }
}
