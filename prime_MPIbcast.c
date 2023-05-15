#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

int is_prime(int n)
{
    int i;
    if (n < 2)
    {
        return 0;
    }
    if (n == 2)
    {
        return 1;
    }
    if (n % 2 == 0)
    {
        return 0;
    }
    for (i = 3; i * i <= n; i += 2)
    {
        if (n % i == 0)
        {
            return 0;
        }
    }
    return 1;
}

int count_primes(int lower, int upper)
{
    int i, count = 0;
    for (i = lower; i < upper; i += 2)
    {
        if (is_prime(i))
        {
            count++;
        }
    }
    return count;
}

int main(int argc, char *argv[])
{
    int x, y, p, rank;
    int subrange_size, lower, upper, count, total_count;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        printf("Enter the lower bound of the range: ");
        fflush(stdout);
        if (scanf("%d", &x) != 1 || x < 2)
        {
            fprintf(stderr, "Error: Invalid input\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }

        printf("Enter the upper bound of the range: ");
        fflush(stdout);
        if (scanf("%d", &y) != 1 || y <= x)
        {
            fprintf(stderr, "Error: Invalid input\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }

        subrange_size = (y - x) / p;
    }

    MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&subrange_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    lower = x + rank * subrange_size;
    upper = lower + subrange_size;

    if (rank == p - 1)
    {
        upper = y;
    }

    count = count_primes(lower, upper);

    MPI_Reduce(&count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("Total count of prime numbers between %d and %d is %d\n", x, y, total_count);
    }

    MPI_Finalize();
    return 0;
}
