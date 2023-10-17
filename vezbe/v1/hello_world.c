#include <stdio.h>

#include "omp.h"

int main() {

    // Ukoliko se ova linija stavi izvan parallel konstrukta, id je uvek 0.
    // Izgleda da je to id jedine niti koja je aktivna, a koja ce kasnije
    // postati master nit.
    // int id = omp_get_thread_num();

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        printf("Hello(%d)", id);
        printf(" world!(%d)\n", id);
    }

    return 0;
}