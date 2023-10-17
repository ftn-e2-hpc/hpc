#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <omp.h>

#ifndef DISABLE_HDF5
#include "utils/h5_matrix_utils.h"
#endif


void matrix_multiply(const float *matrix1, const float *matrix2,
                     unsigned long long rows1, unsigned long long cols1, unsigned long long cols2,
                     float *result) {
    assert(result != NULL);
    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            result[i * cols2 + j] = 0;
            for (int k = 0; k < cols1; k++) {
                result[i * cols2 + j] += matrix1[i * cols1 + k] * matrix2[k * cols2 + j];
            }
        }
    }
}

void matrix_multiply_openmp(const float *matrix1, const float *matrix2,
                     unsigned long long rows1, unsigned long long cols1, unsigned long long cols2,
                     float *result) {

    #pragma omp parallel for
    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            result[i * cols2 + j] = 0;
            for (int k = 0; k < cols1; k++) {
                result[i * cols2 + j] += matrix1[i * cols1 + k] * matrix2[k * cols2 + j];
            }
        }
    }
}


int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stdout, "Program pozvan sa neispravnim brojem argumenata.\nPrimer poziva: "
                        "./MatrixMultiplication m3x3.h5 m3x3.h5\n");
        exit(1);
    }
    
    float *matrix1 = NULL, *matrix2 = NULL, *result = NULL;
    unsigned long long rows1, cols1, rows2, cols2;
    char *input_file1 = argv[1], *input_file2 = argv[2];
    
#ifndef DISABLE_HDF5
    matrix1 = h5_load_matrix(input_file1, &rows1, &cols1);
    assert(matrix1 != NULL);
#else
    printf("HDF podrska onemogucena!\n");
#endif

#ifndef DISABLE_HDF5
    matrix2 = h5_load_matrix(input_file2, &rows2, &cols2);
    assert(matrix2 != NULL);
#else
    printf("HDF podrska onemogucena!\n");
#endif

    result = (float *) malloc(rows1 * cols2 * sizeof(float));
    assert(result != NULL);

    // matrix_multiply(matrix1, matrix2, rows1, cols1, cols2, result);
    matrix_multiply_openmp(matrix1, matrix2, rows1, cols1, cols2, result);

    char filename[301];
    sprintf(filename, "m%lldx%lld", rows1, cols2);
    h5_save_float_matrix(filename, result, rows1, cols2);
    
    if (matrix1 != NULL) free(matrix1);
    if (matrix2 != NULL) free(matrix2);
    if (result  != NULL) free(result);
    
    return 0;
}