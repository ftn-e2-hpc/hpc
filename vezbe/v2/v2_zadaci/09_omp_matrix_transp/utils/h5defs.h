//
// Created by gorana on 18.10.18..
//

#ifndef MATRIXUTILITIES_H5DEFS_H
#define MATRIXUTILITIES_H5DEFS_H

#include <stdlib.h>
#include <hdf5.h>

// Checks for error during hdf5 library function call (nonzero return value).
#define H5STATUS(e) \
    (if (e < 0) { printf("\nHDF5 error on line %d\n\n", __LINE__ ); exit 1; })

#endif //MATRIXUTILITIES_H5DEFS_H
