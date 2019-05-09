#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int cmp (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}


int main(int argc, char **argv)
{
    char *input = argv[1];
    char *output = argv[2];
    int type = atoi(argv[3]);
    int n;
    int c, s;
    int *data, *chunk;

    MPI_Status status;
    int rank, p;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank==0)
    {
        FILE *input_file = fopen(input, "r");
        fseek(input_file, 0, SEEK_SET);
        fscanf(input_file, "%d", &n);
        c = (n%p!=0) ? n/p+1 : n/p;
        data = (int *)malloc(p*c*sizeof(int));
        for(int i=0; i<n; i++) 
            fscanf(input_file, "%d", &(data[i]));
        fclose(input_file);
        for(int i=n; i<c*p; i++)
            data[i] = 0;
    }

    MPI_Bcast(&c, 1, MPI_INT, 0, MPI_COMM_WORLD);
    chunk = (int *)malloc(c * sizeof(int));
    MPI_Scatter(data, c, MPI_INT, chunk, c, MPI_INT, 0, MPI_COMM_WORLD);
    s = (n >= c * (rank+1)) ? c : n - c * rank;
    qsort(chunk, s, sizeof(int),cmp);
}