#include <omp.h>
#include <stdio.h>
#include <unistd.h>

#ifndef N
#define N 10
#endif

int fib(int n) {
    if (n == 0 || n == 1)
        return n;

    int a, b;

    a = fib(n - 1);
    b = fib(n - 2);
    return a + b;
}

int main()
{
    int res;

    double end, start = omp_get_wtime();
    res = fib(N);
    end = omp_get_wtime();
    printf("fib(%d): %d. Vreme izvršenja: %lf.\n", N, res, end - start);
}