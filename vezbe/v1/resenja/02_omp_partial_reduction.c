#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

#define N 1011100
#define NUM_THREADS 4

void copyArray(int *src, int *dest, int size)
{
    for (int i = 0; i < size; i++)
    {
        dest[i] = src[i];
    }
}

int main()
{
    int arr[N], arr_temp[N];
    #ifdef PRINT
    printf("Array:");
    #endif
    for (int i = 0; i < N; i++)
    {
        arr[i] = rand() % 5;
        arr_temp[i] = arr[i];

        #ifdef PRINT
        printf(" %d", arr[i]);
        #endif
    }
    printf("\n\n");

    int slice_size = N / NUM_THREADS;

    double end, start = omp_get_wtime();
    for (int i = 0; i < N; i++)
    {
        if (i % slice_size != 0)
        {
            arr[(i / slice_size) * slice_size] += arr[i];
        }
    }
    end = omp_get_wtime();
    #ifdef PRINT
    printf("Array arr after sequential reduction:");
    for (int i = 0; i < N; i++)
    {
        printf(" %d", arr[i]);
    }
    printf("\n");
    #endif
    printf("Calculation time, sequential: %lf\n", end - start);

    copyArray(arr_temp, arr, N);

    printf("\n------------------------------\n\n");


    start = omp_get_wtime();
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int start_index = omp_get_thread_num() * slice_size;
        int partial_sum = 0;
        for (int i = 0; i < slice_size; i++)
        {
            partial_sum += arr[start_index + i];
        }

        arr[start_index] = partial_sum;
    }
    end = omp_get_wtime();
    #ifdef PRINT
    printf("Array after parallel reduction with less false sharing (no barrier):");
    for (int i = 0; i < N; i++)
    {
        printf(" %d", arr[i]);
    }
    printf("\n");
    #endif
    printf("Calculation time, parallel, less false sharing, no barrier: %lf\n", end - start);

    copyArray(arr_temp, arr, N);

    printf("\n------------------------------\n\n");


    start = omp_get_wtime();
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int start_index = omp_get_thread_num() * slice_size;
        int partial_sum = 0;
        for (int i = 0; i < slice_size; i++)
        {
            partial_sum += arr[start_index + i];
        }

        #pragma omp barrier

        arr[start_index] = partial_sum;
    }
    end = omp_get_wtime();
    #ifdef PRINT
    printf("Array after parallel reduction with less false sharing (with barrier):");
    for (int i = 0; i < N; i++)
    {
        printf(" %d", arr[i]);
    }
    printf("\n");
    #endif
    printf("Calculation time, parallel, less false sharing, with barrier: %lf\n", end - start);

    copyArray(arr_temp, arr, N);

    printf("\n------------------------------\n\n");


    start = omp_get_wtime();
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int start_index = omp_get_thread_num() * slice_size;
        for (int i = 1; i < slice_size; i++)
        {
            arr[start_index] += arr[start_index + i];
        }
    }
    end = omp_get_wtime();
    #ifdef PRINT
    printf("Array after parallel reduction with false sharing:");
    for (int i = 0; i < N; i++)
    {
        printf(" %d", arr[i]);
    }
    printf("\n");
    #endif
    printf("Calculation time, parallel, false sharing: %lf\n", end - start);
}