#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>
#include <string.h>

#ifdef LARGE
    #define TARGET "SUSE and the University of Delaware have joined the OpenMP ARB, a group of leading hardware and software vendors and research organizations creating the standard for the most popular shared-memory parallel programming model in use today."
    #define NCHROMOSOMES 237+1
    #define NINDIVIDUALS 2000
#elif MEDIUM
    #define TARGET "Parallel programming is funny!"
    #define NCHROMOSOMES 30+1
    #define NINDIVIDUALS 1000
#else
    #define TARGET "~OpenMP~"
    #define NCHROMOSOMES 8+1
    #define NINDIVIDUALS 1000
#endif

#define NITERATIONS 1000
#define SELECTION_PERCENT 70
#define MUTATION_PERCENT 5

/**
 * Jedna jedinka u populaciji. Jedinka ima svoj identifikator koji se koristi samo pri
 * ispisu, hromozome koji su predstavljeni stringom i meru adaptacije - fitnes. Sto je
 * fitnes veci, to je jedinka manje adaptirana.
 */
struct Individual {
    int id;
    char chromosomes[NCHROMOSOMES];
    float fitness;
};
typedef struct Individual individual_t;

/**
 * Populaciju cini niz jedinki. Na nivou populacije se cuva fitnes kako bi se lakse
 * pratilo da li populacija evaluira ka resenju problema.
 */
static individual_t *population;
static unsigned long int total_fitness;


/** Funkcije za rad sa populacijom. */

/**
 * Inicijalizuje populaciju stvaranjem NINDIVIDUALS jedinki.
 * Hromozomi svake jedinke se nasumicno biraju medju vidljivim ascii
 * karakterima (od koda 32 do koda 126). Po zavrsetku ove funkcije
 * fitnes svake od jedinki ce biti 0.
 */
void initialize();

/**
 * Racuna fitnes za svaku od jedinki iz populacije. Vrednost fitnesa
 * odgovara broju korespodentnih karaktera koji se razlikuju u TARGET
 * stringu i hromozomu jedinke. Ukoliko nema razlike izmedju ova dva
 * stringa, fitnes jedinke ce biti 0.
 *
 * Svaki put kada se pozove ova metoda, staticka
 * promenljiva total_fitnes se postavi na vrednost 0, a zatim se u nju
 * akumulira sracunata vrednost fitnesa svake jedinke iz populacije.
 */
void calculate_fitness();

/**
 * Sortiranje populacije jedinki u rastucem redosledu prema fitnesu.
 * Za sortiranje se koristi bubble sort algoritam.
 */
void sort();

/**
 * Stvaranje nove populacije selektovanjem i ukrstanjem roditelja izabranih
 * iz postojece populacije.
 *
 * NINDIVIDUALS puta se biraju po dva roditelja iz prvih SELECTION_PERCENT
 * procenata postojece generacije. Zatim se formira jedinka potomak tako sto
 * se za svaki od njenih hromozoma nasumicno bira jedan od korespodentnih
 * hromozoma prvog ili drugog roditelja. Kada se odrede hromozomi nove jedinke,
 * funkcija sracuna i vrednost fitnesa za tu jedinku.
 *
 * Nakon sto se napravi citava nova populacija, pretodna populacija se brise, a
 * u narednoj iteraciji se iz nove populacije biraju roditelji za dalje ukrstanje.
 */
void crossover_select();

/**
 * Mutira nasumicno odabran hromozom nasumicno odabrane jedinke iz populacije.
 * Sa MUTATION_PERCENT je odredjen ukupan procenat jedinki koje ce biti mutirane.
 */
void mutate();

// helper functions

/**
 * Ispis jedne jedinke iz populacije.
 *
 * @param i     Pokazivac na jedinku cije podatke treba ispisati.
 */
void print_individual(individual_t *i);

/**
 * Ispis svih jedinki populacije.
 */
void print_population();

/**
 * Funkcija omotac koja poziva prosledjenu funkciju pri cemu meri vreme njenog
 * izvrsavanja i izmereno vreme dodaje na prosledjenu adresu.
 *
 * @param f     Funkcija koju treba izvrsiti.
 * @param time  Pokazivac na vrednost koja se uvecava izmerenim vremenom izvrsavanja
 *              funkcije f.
 */
void timer(void (*f)(void), double *time);

/* Promenljive za merenje vremena izvrsavanja razlicitih funkcija. */
static double init_time = 0;
static double fitness_time = 0;
static double sort_time = 0;
static double crossselect_time = 0;
static double mutation_time = 0;

int main() {
    srand(time(NULL));
    
    double start = omp_get_wtime();
    
    // alokacija prostora za inicijalnu populaciju
    population = (individual_t *) calloc(NINDIVIDUALS, sizeof(individual_t));
    
    // inicijalizacija populacije
    timer( initialize, &init_time);
    
    // racunanje fitnesa za svaku jedinku
    timer( calculate_fitness, &fitness_time);
    
    // sortiranje
    timer( sort, &sort_time);
    
    int niterations = NITERATIONS;
    while (niterations--) {
        printf("\n===== ITERACIJA %d =====\nNajbolja jedinka: ", niterations);
        print_individual(&population[0]);
        printf("Fitnes populacije: %ld\n", total_fitness);
        
        // selekcija i ukrstanje
        timer( crossover_select, &crossselect_time );
    
        // mutacija
        timer( mutate, &mutation_time);
    
        // sortiranje
        timer( sort, &sort_time);
        
        // Ukoliko su hromozomi najbolje jedinke iz populacije jednaki ciljnom stringu, algoritam
        // se zavrsava
        if (memcmp(population[0].chromosomes, TARGET, NCHROMOSOMES * sizeof(char)) == 0)
            break;
    }
    
    double total_time = omp_get_wtime() - start;
    
    printf("\n===== Statistika izvrsavanja =====\n");
    printf("- Najbolja jedinka: ");
    print_individual(&population[0]);
    printf("- Ukupno vreme izvrsavanja:   %lf\n", total_time);
    printf("- Inicijalizacija populacije: %lf%%\n", (init_time / total_time) * 100);
    printf("- Racunanje fitnesa:          %lf%%\n", (fitness_time / total_time) * 100);
    printf("- Sortiranje populacije:      %lf%%\n", (sort_time / total_time) * 100);
    printf("- Inicijalizacija populacije: %lf%%\n", (init_time / total_time) * 100);
    printf("- Selekcija i ukrstanje:      %lf%%\n", (crossselect_time / total_time) * 100);
    printf("- Mutacija:                   %lf%%\n", (mutation_time / total_time) * 100);
    
    /* Unistavanje populacije */
    free(population);
    
    return 0;
}

void initialize() {
    total_fitness = 0;
    for (int i = 0; i < NINDIVIDUALS; i++) {
        for (int j = 0; j < NCHROMOSOMES-1; j++) {
            population[i].id = i;
            population[i].chromosomes[j] = (char) (32 + rand() % 95);
        }
    }
}

int calculate_fitness_individual(individual_t *i) {
    int difference = 0;
    for (int j = 0; j < NCHROMOSOMES - 1; j++) {
        // sto se vise karaktera razlikuje u ciljnom i trenutnom hromozomu, to je veca vrednost razlike
        if (TARGET[j] != i->chromosomes[j]) difference++;
    }
    total_fitness += difference;
    return difference;
}

void calculate_fitness() {
    total_fitness = 0;
    for (int i = 0; i < NINDIVIDUALS; i++) {
        population[i].fitness = calculate_fitness_individual(&population[i]);
    }
}

void sort() {
    int i, j;
    individual_t tmp, *i1, *i2;
    
    for (i = 0; i < NINDIVIDUALS - 1; i++)
        for (j = 0; j < NINDIVIDUALS - i - 1; j++) {
            i1 = &population[j];
            i2 = &population[j + 1];
            if (i1->fitness > i2->fitness) {
                tmp = *i2;
                *i2 = *i1;
                *i1 = tmp;
            }
        }
}

void crossover_select() {
    
    /* Prostor za novu generaciju */
    individual_t *new_population = (individual_t *) calloc(NINDIVIDUALS, sizeof(individual_t));
    total_fitness = 0;
    
    individual_t *i1, *i2;          // pokazivaci na roditelje nove jedinke
    individual_t *new_individual;   // pokazivac na novu jedinku
    int crossidx = 0;               // redni broj gena na kojem se vrsi promena
    int lb = (int) (NINDIVIDUALS * (SELECTION_PERCENT / 100.0));    // top 70% za ukrstanje
    
    for (int i = 0; i < NINDIVIDUALS; i++) {
        new_individual = &new_population[i];
        
        /* Izaberi dve jedinke */
        i1 = &population[rand() % lb];
        i2 = &population[rand() % lb];
        
        /* Ukrsti ih i napravi novu jedinku. */
        for (int j = 0; j < NCHROMOSOMES - 1; j++) {
            if (rand() % 2 == 0) {
                new_individual->chromosomes[j] = i1->chromosomes[j];
            } else {
                new_individual->chromosomes[j] = i2->chromosomes[j];
            }
        }
        new_individual->id = i;
        new_individual->fitness = calculate_fitness_individual(new_individual);
    }
    
    /* Stara populacija se menja novom */
    free(population);
    population = new_population;
}

void mutate() {
    int nindividuals = (int) (NINDIVIDUALS * (MUTATION_PERCENT / 100.0));
    while (nindividuals--) {
        int idx = rand() % NCHROMOSOMES;
        char value = (char) (32 + rand() % 94);
        //printf("%d %d %c\n", rand() % NINDIVIDUALS, idx, value);
        population[rand() % NINDIVIDUALS].chromosomes[idx] = value;
    }
}

void print_individual(individual_t *i) {
    printf("id: %d, hromozomi: %s, fitnes: %f\n", i->id, i->chromosomes, i->fitness);
}

void print_population() {
    for (int i = 0; i < NINDIVIDUALS; i++) {
        print_individual(&population[i]);
    }
}

void timer(void (*f)(void), double *time) {
    double start = omp_get_wtime();
    (*f)();
    double stop = omp_get_wtime();
    *time += (stop - start);
}

