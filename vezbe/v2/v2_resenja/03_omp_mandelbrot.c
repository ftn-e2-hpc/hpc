#include <stdio.h>
#include <omp.h>

#define NPOINTS 1000
#define MXITR 1000

struct d_complex {
    double r; double i;
};
struct d_complex c;
int numoutside = 0;

void testpoint(struct d_complex);

int main(){
    int i, j;
    double area, error, eps = 1.0e-5;

    // Ispravka linije 33 iz mandelbrot.c datoteke
    // Prethodna verzija ove linije: #pragma omp parallel for default(shared) private(c, eps)
    //  1.  private(eps) ce kao posledicu imati kreiranje privatne promenljive eps za svaku
    //      nit, ali ta nova promenljiva ce ostati neinicijalizovana. 
    //      Resenje: zameniti sa firstprivate(eps)
    //  2.  nedostaje private(j) - openmp for konstrukt ce brojacku promenljivu i automatski
    //      proglasiti privatnom. Medjutim, to ne vazi za promenljivu j koja je deklarisana
    //      izvan paralelnog regiona i koja ce biti deljena izmedju niti.
    //      Resenje: Dodati j u private klauzu.
    
    #pragma omp parallel for default(none) private(j, c)\
            firstprivate(eps) 
    for (i = 0; i < NPOINTS; i++) {
        for (j = 0; j < NPOINTS; j++) {
            c.r = -2.0 + 2.5 * (double)(i) / (double)(NPOINTS) + eps;
            c.i = 1.125 * (double)(j) / (double)(NPOINTS) + eps;

            // Ispravka linije 38 iz mandelbrot.c datoteke
            // Prethodna verzija ove linije: testpoint();
            // Ako se ne prosledi privatna kopija verzija promenljive c, funkcija
            // testpoint ce raditi na statickoj, tj. globalno vidljivoj verziji 
            // promenljive c.
            testpoint(c);
        }
    }
    
    printf("Number outside: %d\n", numoutside);
    
    return 0;
}

void testpoint(struct d_complex c){
    struct d_complex z;
    int iter;
    double temp;
    z = c;
    for (iter = 0; iter < MXITR; iter++){
        temp = (z.r * z.r) - (z.i * z.i) + c.r;
        z.i = z.r * z.i * 2 + c.i;
        z.r = temp;
        if ((z.r * z.r + z.i * z.i) > 4.0) {
            {
            	// Dodato atomic da bi se obezbedio pristup promenljivoj numoutside
                #pragma omp atomic
                numoutside++;
            }
            break;
        }
    }
}
