#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifndef DISABLE_HDF5
#include "utils/h5_matrix_utils.h"
#endif

const char PRINT_MATRICES = 1;
const char SAVE_RESULT = 1;

/**
 * Sekvencijalni program za transponovanje matrice.
 *
 * @param matrix    Pokazivac na blok memorije koji sadrzi matricu. Elementi matrice su tipa float.
 * @param rows      Broj vrsta ulazne matrice.
 * @param cols      Broj kolona ulazne matrice.
 * @return          Pokazivac na blok memorije u kojem se nalazi transponovana matrica. Pozivalac programa
 *                  je duzan da prihvati vrednost ovog pokazivaca i oslobodi zauzetu memoriju kada mu
 *                  transponovana matrica vise ne treba.
 */
float *transpose(float *matrix, unsigned long long rows, unsigned long long cols) {

    float *tmatrix = (float *) malloc(cols * rows * sizeof(float));

    int i, j;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            tmatrix[i * rows + j] = matrix[j * rows + i];
        }
    }
    return tmatrix;
}

float *transpose_openmp(float *matrix, unsigned long long rows, unsigned long long cols) {

    // TODO Implementirati OpenMP varijantu transponovanja matrice
    
    // return NULL;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stdout, "Program pozvan sa neispravnim brojem argumenata.\nPrimer poziva: ./transponovanje m3x3.h5");
        exit(1);
    }

    double start = omp_get_wtime();

    const char *input_file = argv[1];
    unsigned long long rows, cols;

    float *matrix = NULL, *tmatrix = NULL;
#ifndef DISABLE_HDF5
    matrix = h5_load_matrix(input_file, &rows, &cols);
#else
    printf("HDF5 podrska onemogucena!");
#endif

    assert(matrix != NULL);
    if (PRINT_MATRICES) {
        printf("\n===================================\n");
        printf("===== Netransponovana matrica =====\n");
        printf("===================================\n");
        print_float_matrix(matrix, rows, cols);   // otkomentarisati za ispis netransponovane matrice
    }

    printf("Transposing matrix %s.", input_file);
    tmatrix = transpose(matrix, rows, cols);
    // tmatrix = transpose_openmp(matrix, rows, cols);
    assert(tmatrix != NULL);
    if (PRINT_MATRICES) {
        printf("\n===================================\n");
        printf("====== Transponovana matrica ======\n");
        printf("===================================\n");
        print_float_matrix(tmatrix, rows, cols);  // otkomentarisati za ispis transponovane matrice
    }

    if (SAVE_RESULT) {
        char *output_dir = ".";
        char filename[301];
        sprintf(filename, "%s/%lldx%lld.h5", output_dir, rows, cols);
        h5_save_float_matrix(filename, tmatrix, rows, cols);
    }

    free(matrix);   // dealociraj originalnu matricu
    free(tmatrix);  // dealociraj transponovanu matricu

    printf("\nVreme izvrsavanja: %lf\n", omp_get_wtime() - start);

    return 0;
}