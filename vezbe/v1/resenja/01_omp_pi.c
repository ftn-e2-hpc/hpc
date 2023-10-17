/**
 * Program koji racuna vrednost integrala funkcije 4/(1+x^2). Numericki,
 * ova vrednost je jednaka broju pi.
 *
 * Originalni materijal za ovaj primer je preuzet iz niza prezentacija
 * "Introduction to OpenMP" ciji je autor Tim Mattson
 */

#include <stdio.h>
#include <omp.h>

#define NUM_THREADS 2
#define PADDING 8

#define PRINT_SEQ_RES 1
#define PRINT_PRL_RES_BASIC 1
#define PRINT_PRL_RES_NO_FALSE_SHARING 1
#define PRINT_PRL_RES_SYNCHRONIZATION 1
#define PRINT_PRL_RES_FOR_CONSTRUCT 1

static long num_steps = 100000000;

double step;
void serial_code();
void parallel_code_incorrect();
void parallel_code();
void parallel_code_no_false_sharing();
void parallel_code_synchronization();
void parallel_code_for_construct();

int main() {

#if PRINT_SEQ_RES
    printf("*************** Sekvencijalna implementacija ***************\n");
    serial_code();
    printf("************************************************************\n");
#endif
#if PRINT_SEQ_RES
    printf("************ Neispravna paralelna implementacija ************\n");
    parallel_code_incorrect();
    printf("************************************************************\n");
#endif
#if PRINT_PRL_RES_BASIC
    printf("***************** Paralelna implementacija *****************\n");
    parallel_code();
    printf("************************************************************\n");
#endif
#if PRINT_PRL_RES_NO_FALSE_SHARING
    printf("***** Paralelna implementacija (nema laznog deljenja) ******\n");
    parallel_code_no_false_sharing();
    printf("************************************************************\n");
#endif
#if PRINT_PRL_RES_SYNCHRONIZATION
    printf("******** Paralelna implementacija (sinhronizacija) *********\n");
    parallel_code_synchronization();
    printf("************************************************************\n");
#endif
#if PRINT_PRL_RES_FOR_CONSTRUCT
    printf("******** Paralelna implementacija (for konstrukcija) ********\n");
    parallel_code_for_construct();
    printf("************************************************************\n");
#endif

    return 0;
}

/*
 * Sekvencijalna implementacija programa.
 */
void serial_code() {
    double start = omp_get_wtime();     // trenutak pocetka merenja vremena
    int i;
    double x, pi, sum = 0.0;

    step = 1.0 / (double) num_steps;

    for (i = 0; i < num_steps; i++) { 
        double x = (i + 0.5) * step;
        sum = sum + 4.0 / (1.0 + x * x);
    }

    pi = step * sum;
    double end = omp_get_wtime();       // trenutak zavrsetka merenja vremena

    printf("pi = %lf\n", pi);
    printf("Time elapsed: %lf\n", end - start);
}

/*
 * OpenMP paralelizovana verzija sekvencijalnog programa. Rezultat izvrsavanja programa
 * je netacan, jer u kodu postoji stetno preplitanje (parallel konstrukcija).
 */ 
void parallel_code_incorrect() {
    double start = omp_get_wtime();
    int i;
    double x, pi, sum = 0.0;

    step = 1.0 / (double) num_steps;

    #pragma omp parallel
    {
        for (i = 0; i < num_steps; i++) { 
            double x = (i + 0.5) * step;
            sum = sum + 4.0 / (1.0 + x * x);
        }
    }

    pi = step * sum;
    double end = omp_get_wtime();

    printf("pi = %lf\n", pi);
    printf("Time elapsed: %lf\n", end - start);
}

/**
 * OpenMP paralelizovana verzija sekvencijalnog programa sa uklonjenim stetnim preplitanjem
 * (parallel konstrukcija + lokalni podaci)
 */
void parallel_code() {
    double start = omp_get_wtime();

    int i, nthreads;
    double x, pi, sum[NUM_THREADS];

    step = 1.0 / (double) num_steps;

    // Funkcijom omp_set_num_threads programer zapravo zahteva odredjeni broj niti,
    // sto ne znaci nuzno da ce zahtevani broj niti stvarno biti i pokrenut. Okruzenje
    // moze da odluci da napravi manji broj niti od zahtevanog.
    omp_set_num_threads(NUM_THREADS);
    #pragma omp parallel
    {
        int i, id, nthrds;
        id = omp_get_thread_num();
        // Kako zahtevani broj niti i broj napravljenih niti mogu biti razliciti, 
        // potrebno je unutar pralelnog regiona proveriti koliko je niti stvarno
        // napravljeno i aktivirano.
        nthrds = omp_get_num_threads();


        // Kako je neophodno sabrati sve parcijalne rezultate izvan paralelog regiona
        // (dovoljno je da) jedna od niti sacuva informaciju o broju aktivnih niti
        // unutar paralelnog regiona. Kada se paralelni region zavrsi, sve niti koje 
        // su vrsile racun osim master niti ce biti unistene i ova informacija vise
        // nece biti dostupna.
        if (id == 0) nthreads = nthrds;

        // Round-robin raspodela intervala. Stetno preplitanje uklonjeno
        // tako sto svaka nit ima svoju promenljivu u koju upisuje rezultat
        // racunanja.
        for (i = id, sum[id] = 0; i < num_steps; i += nthrds) { 
            double x = (i + 0.5) * step;
            sum[id] = sum[id] + 4.0 / (1.0 + x * x);    // ova linija dovodi do laznog deljenja (false sharing)
        }
    }
    for (i = 0; i < nthreads; i++) pi += sum[i];  
    pi *= step;  

    double end = omp_get_wtime();

    printf("pi = %lf\n", pi);
    printf("Time elapsed: %lf\n", end - start);
}

/**
 * Paralelni OpenMP program u kojem nema laznog deljenja rezultata (rezultat promenljive
 * sum). Lazno deljenje je eliminisano uvodjenjem PADDING parametra.
 */
void parallel_code_no_false_sharing() {
    double start = omp_get_wtime();

    int i, nthreads;
    double x, pi, sum[NUM_THREADS][PADDING];

    step = 1.0 / (double) num_steps;

    omp_set_num_threads(NUM_THREADS);
    #pragma omp parallel
    {
        int i, id, nthrds;
        id = omp_get_thread_num();
        nthrds = omp_get_num_threads();

        if (id == 0) nthreads = nthrds;

        for (i = id, sum[id][0] = 0; i < num_steps; i += nthrds) { 
            double x = (i + 0.5) * step;
            sum[id][0] = sum[id][0] + 4.0 / (1.0 + x * x);
        }
    }
    for (i = 0; i < nthreads; i++) pi += sum[i][0];
    pi *= step;    

    double end = omp_get_wtime(); 

    printf("pi = %lf\n", pi);
    printf("Time elapsed: %lf\n", end - start);
}

/**
 * Paralelni OpenMP program u kojem nema laznog deljenja rezultata (rezultat promenljive
 * sum). Lazno deljenje je eliminisano uvodjenjem sinhronizacije izmedju niti.
 */
void parallel_code_synchronization() {
    double start = omp_get_wtime();
    int i, nthreads;
    double x, pi = 0.0, sum = 0.0;

    step = 1.0 / (double) num_steps;

    omp_set_num_threads(NUM_THREADS);
    #pragma omp parallel
    {
        int i, id, nthrds;
        double x, sum = 0.0;

        id = omp_get_thread_num();
        nthrds = omp_get_num_threads();

        if (id == 0) nthreads = nthrds;

        for (i = id; i < num_steps; i += nthrds) { 
            x = (i + 0.5) * step;
            sum += 4.0 / (1.0 + x * x);
        }
        #pragma omp critical // moze i atomic
        pi += sum;
    }
    pi *= step;

    double end = omp_get_wtime();

    printf("pi = %lf\n", pi);
    printf("Time elapsed: %lf\n", end - start);
}

/** 
 * Verzija programa paralelizovana OpenMP for konstrukcijom i redukcionom klauzom.
 */
void parallel_code_for_construct() {
    double start = omp_get_wtime();
    int i;
    double x, pi, sum = 0.0;

    step = 1.0 / (double) num_steps;

    {
        #pragma omp parallel for reduction(+:sum)
        for (i = 0; i < num_steps; i++) { 
            double x = (i + 0.5) * step;
            sum = sum + 4.0 / (1.0 + x * x);
        }
    }

    pi = step * sum;
    double end = omp_get_wtime();

    printf("pi = %lf\n", pi);
    printf("Time elapsed: %lf\n", end - start);
}