//
// Created by gorana on 18.10.18..
//

#ifndef MATRIXUTILITIES_H5_MATRIX_UTILS_H
#define MATRIXUTILITIES_H5_MATRIX_UTILS_H

#include <hdf5.h>
#include "h5defs.h"

/* Move this to a separate file */
float *generate_float_matrix(unsigned long long rows, unsigned long long cols);
double *generate_double_matrix(unsigned int rows, unsigned int cols);               // TODO implement
int *generate_int_matrix(unsigned int rows, unsigned int cols);                     // TODO implement
void print_float_matrix(float *matrix, unsigned long long rows, unsigned long long cols);
void print_float_vector(float *vector, unsigned long long len);
/* **************************** */

/**
 * Generates matrix data and saves it to the hdf5 file.
 *
 * @param filename Name of the output file.
 * @param rows Number of rows in the generated matrix.
 * @param cols Number of columns in the generated matrix.
 */
void h5_generate_matrix(const char *filename, unsigned int rows, unsigned int cols);

/**
 * Creates hdf5 file containing the data of rows x cols dimension.
 * @param filename If file should be saved to the working directory then just filename
 * otherwise absolute or relative path to the file.
 * @param data Pointer to the matrix data to be saved. If memory less then
 * rows x columns is allocated, function might exit ungracefully.
 * @param rows Input matrix row number.
 * @param cols Input matrix column number.
 */
void h5_save_float_matrix(const char *filename, float *data, unsigned int rows, unsigned int cols);

/**
 * ???
 * Loads matrix data from file specified and returns it to the caller as an
 * twodimensional array.
 *
 * @return A pointer to the loaded matrix data.
 */
void *h5_load_matrix(const char *filename, unsigned long long *rows, unsigned long long *cols);

#endif //MATRIXUTILITIES_H5_MATRIX_UTILS_H
