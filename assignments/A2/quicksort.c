#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>

int cmp (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

int median(int size, int *arr)
{
    int m = size/2;
    int median;
    if(size%2 == 0)
    {
        median = (arr[m]+arr[m-1])/2;
    }
    else
    {
        median = arr[m];
    }
    return median;
}


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
    int rank, p;
    int c_size, med, *chunksize;
    int *displs;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank==0)
    {
	//printf("start reading\n");
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
           { fscanf(input_file, "%d", &(data[i]));}
        fclose(input_file);
	//printf("reading end\n");
        for(int i=n; i<c*p; i++)
            data[i] = 0;
    }

    t_begin = MPI_Wtime();
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&c, 1, MPI_INT, 0, MPI_COMM_WORLD);
    chunk = (int *)malloc(c * sizeof(int));
    MPI_Scatter(data, c, MPI_INT, chunk, c, MPI_INT, 0, MPI_COMM_WORLD);
    if(n%p !=0){
        if(rank == p-1)
        {
            // printf("s=%d,n=%d,c=%d,rank=%d\n", s, n, c, rank);
            s = n - c*rank;

            int* temp[s];
            for(int ii=0; ii<s; ii++)
                temp[ii] = chunk[ii];
            free(chunk);
            chunk = (int *)malloc(s * sizeof(int));
            for(int ii=0; ii<s; ii++)
                chunk[ii] = temp[ii];
        }
        else
        {
            s = c;
        }
    }
    else
    {
        s = c;
    }
    // printf("rank:%d, s=%d\n", rank, s);
    
    qsort(chunk, s, sizeof(int),cmp);

    // for(int i = 0; i < p; i++){
    //     if(rank == i){
    //         for(int j = 0; j < s; j++)
    //             printf("%d ",chunk[j]);
    //     }
    // }
    // printf("\n");



    for(step = 1; step < p; step = 2*step)
    {
        //printf("rank:%d  step:%d\n",rank, step);
        groupsize = p/step;

        c_size = s;
        med = median(c_size, chunk);
        int *allmedian = 0;
        if(rank == 0)
        {
            allmedian = calloc(p, sizeof(int));
        }
        MPI_Gather(&med, 1, MPI_INT, allmedian, 1, MPI_INT, 0, MPI_COMM_WORLD);

        for(int i=0; i<step; i++)
        {
            // printf("rank:%d  i:%d\n",rank, i);
            if(rank == 0)
            {
                if(type == 1)
                {
                    pivot = allmedian[i*groupsize];
                }
                else
                {
                    int groupmedians[groupsize];
                    for(int j=0; j<groupsize; j++)
                    {
                        groupmedians[j] = allmedian[i*groupsize+j];
                    }

                    if(type == 2)
                    {
                        pivot = median(groupsize, groupmedians);
                    }
                    if(type == 3)
                    {
                        int sum = 0;
                        for(int jj=0; jj<groupsize; jj++)
                        {
                            sum += groupmedians[jj];
                        }
                        pivot = sum/groupsize;
                    }
                }
            }
            MPI_Bcast(&pivot, 1, MPI_INT, 0, MPI_COMM_WORLD);
            // printf("step:%d pivot=%d\n",step, pivot);


            if(rank>=i*groupsize && rank<(i+1)*groupsize)
            {
                int b = 0;
                for(b=0; b<c_size; b++)
                {
                    if(chunk[b]>pivot)
                        break;
                }
                int* smaller = (int *)malloc(b*sizeof(int));
                int* larger = (int *)malloc((c_size-b)*sizeof(int));
                // printf("b=%d\n",b);
                for(int ii=0; ii<b; ii++)
                {
                    smaller[ii] = chunk[ii];
                }
                for(int ii=b; ii<c_size; ii++)
                {
                    larger[ii-b] = chunk[ii];
                }
                free(chunk);
                printf("rank: %d ready", rank);
                // for(int jj=0; jj<b; jj++)
                //     printf("%d ", smaller[jj]);

                // printf("\n");

                // for(int jjj=0; jjj<c_size-b; jjj++)
                //     printf(" %d ", larger[jjj]);
                // printf("\n");

                if(rank<(i*groupsize+groupsize/2))
                {
                    size = c_size-b;
                    remain = b;
                    MPI_Send(&size,1,MPI_INT,rank+groupsize/2,111,MPI_COMM_WORLD);
                    MPI_Send(larger,size,MPI_INT,rank+groupsize/2,rank,MPI_COMM_WORLD);
                    MPI_Recv(&recvsize,1,MPI_INT,rank+groupsize/2,111,MPI_COMM_WORLD,&status);
                    recv = (int *)malloc(recvsize*sizeof(int));
                    printf("#%d size=%d recvsize=%d\n");

                    // printf("1 rsize=%d\n", recvsize);
                    MPI_Recv(recv,recvsize,MPI_INT,rank+groupsize/2,rank+groupsize/2,MPI_COMM_WORLD,&status);
                    // printf("recv: %d\n", recv[1]);
                    s = remain + recvsize;
                    // printf("recvs: %d\n", recvsize);
                    // printf("new s:%d\n", s);
                    chunk = (int *)malloc(s*sizeof(int));
                    for(int ii=0; ii<remain; ii++)
                    {
                        chunk[ii] = smaller[ii];
                    }
                    for(int ii=0; ii<recvsize; ii++)
                    {
                        chunk[remain+ii] = recv[ii];
                    }
                    qsort(chunk, s, sizeof(int),cmp);
                    free(smaller);
                    free(larger);
                    free(recv);
                }
                else
                {
                    size = b;
                    remain = c_size - b;
                    MPI_Send(&size,1,MPI_INT,rank-groupsize/2,111,MPI_COMM_WORLD);
                    MPI_Send(smaller,size,MPI_INT,rank-groupsize/2,rank,MPI_COMM_WORLD);
                    MPI_Recv(&recvsize,1,MPI_INT,rank-groupsize/2,111,MPI_COMM_WORLD,&status);
                    recv = (int *)malloc(recvsize*sizeof(int));
                    printf("#%d size=%d recvsize=%d\n");
                    // printf("2 rsize=%d\n", recvsize);
                    MPI_Recv(recv,recvsize,MPI_INT,rank-groupsize/2,rank-groupsize/2,MPI_COMM_WORLD,&status);
                    // printf("recv: %d\n", recv[1]);
                    s = remain + recvsize;
                    chunk = (int *)malloc(s*sizeof(int));
                    for(int ii=0; ii<remain; ii++)
                    {
                        chunk[ii] = larger[ii];
                    }
                    for(int ii=0; ii<recvsize; ii++)
                    {
                        chunk[remain+ii] = recv[ii];
                    }
                    qsort(chunk, s, sizeof(int),cmp);
                    free(smaller);
                    free(larger);
                    free(recv);
                }
            }
        }
	printf("rank %d finish step %d\n", rank, step);
    }

    if(rank == 0)
    {
        data = (int *)malloc(n*sizeof(int));
        chunksize = (int *)malloc(p*sizeof(int));
    }
    c_size = s;
    MPI_Gather(&c_size, 1, MPI_INT, chunksize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(rank == 0)
    {
        displs = (int *)calloc(p, sizeof(int));
        for(int i=1; i<p; i++)
        {
            displs[i] = displs[i-1] + chunksize[i-1];
        }
	printf("g\n");
    }
    MPI_Gatherv(chunk,c_size,MPI_INT,data,chunksize,displs,MPI_INT,0,MPI_COMM_WORLD);

    t_end = MPI_Wtime();

    if (rank == 0) 
    {
        t = t_end - t_begin;
        printf("%f\n", t);
/*
        FILE *output_file = fopen(output, "w+");
        if (!output_file)
        {
            printf("Error: failed to open output file\n");
            return -1;
        }
        for(int i=0; i<n; i++)
        {
            fprintf(output_file, "%d ", data[i]);
        }
        fclose(output_file);
*/
    }
    MPI_Finalize();
}
