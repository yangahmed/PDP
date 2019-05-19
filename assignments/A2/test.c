#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    char *input = argv[1];
    char *output = argv[2];
    int type = atoi(argv[3]);
    MPI_Status status;
    double t_begin, t_end, t;

    MPI_Init(&argc, &argv);
    int n;
    int c, s;
    int *data, *chunk, *recv;
    int step, groupsize;
    int pivot, size, recvsize, remain;
    int rank, p, i;
    int c_size, med, *chunksize;
    int *displs;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank==0)
    {
        FILE *input_file = fopen(input, "r");
	if (!input_file)
        {
            printf("Error: failed to open input file\n");
            return -1;
        }
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

    t_begin = MPI_Wtime();
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&c, 1, MPI_INT, 0, MPI_COMM_WORLD);
    chunk = (int *)malloc(c * sizeof(int));
    MPI_Scatter(data, c, MPI_INT, chunk, c, MPI_INT, 0, MPI_COMM_WORLD);

    printf("%d\n", chunk[0]);

    MPI_Finalize();
}