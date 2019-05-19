#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>

int cmp (const void * a, const void * b)
{
    /*sorting in acsending order*/
   return ( *(int*)a - *(int*)b );
}

int median(int size, int *arr)
{
    /*calculate the median of an sorted array*/
    int m = size/2;
    int median;
    // if(size%2 == 0)
    // {
    //     median = (arr[m]+arr[m-1])/2.0;
    // }
    // else
    // {
    //     median = arr[m];
    // }

    median = arr[m]; 
    /* because there are some large number in the input file, if we do plus then divide by 2 will
    get error, since the algorithm send out the numbers which smaller than the pivot, so we can 
    simplely assume the smaller one in the middle two numbers as median*/
    return median;
}


int main(int argc, char **argv)
{
    char *input = argv[1];
    char *output = argv[2];
    int type = atoi(argv[3]);
    MPI_Status status;
    double t_begin, t_end, t;
    double t_begin_s, t_end_s, t_s;

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


    if(n<p){
        s = 1;
    }
    
    else if(n%p != 0){
        if(rank == p-1) //last processor may contain less numbers than normal ones
        {
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
    
    qsort(chunk, s, sizeof(int),cmp);



    for(step = 1; step < p; step = 2*step) //loop for log2p times till converge
    {
        t_begin_s = MPI_Wtime();
        groupsize = p/step;

        c_size = s;
        med = median(c_size, chunk);
        // printf("#%d %d\n", rank, med);
        int *allmedian = 0;
        if(rank == 0)
        {
            allmedian = calloc(p, sizeof(int));
        }
        MPI_Gather(&med, 1, MPI_INT, allmedian, 1, MPI_INT, 0, MPI_COMM_WORLD);

       
        i = rank/groupsize; // the processor in which group
        int allpivot[p];
        if(rank == 0)
        {
            // for(int iii=0; iii<p; iii++)
            //     printf("%d ",allmedian[iii]);
            // printf("\n");
            // int allpivot[p];
            
            if(type == 1)
            {
                for(int ii=0; ii<p/groupsize; ii++){
                    for(int jj=0; jj<groupsize; jj++)
                        allpivot[ii*groupsize+jj] = allmedian[ii*groupsize];
                }
            }
            else
            {
                int groupmedians[p/groupsize][groupsize];
                // for(int j=0; j<groupsize; j++)
                // {
                //     groupmedians[j] = allmedian[i*groupsize+j];
                // }
                for(int ii=0; ii<p/groupsize; ii++){
                    for(int jj=0; jj<groupsize; jj++)
                        groupmedians[ii][jj] = allmedian[ii*groupsize+jj];
                }

                if(type == 2)
                {
                    // pivot = median(groupsize, groupmedians);
                for(int ii=0; ii<p/groupsize; ii++){
                    for(int jj=0; jj<groupsize; jj++)
                        allpivot[ii*groupsize+jj] = median(groupsize, groupmedians[ii]);
                }
                }
                if(type == 3)
                {
                    // int sum = 0;
                    // for(int jj=0; jj<groupsize; jj++)
                    // {
                    //     sum += groupmedians[jj];
                    // }
                    // pivot = sum/groupsize;
                for(int ii=0; ii<p/groupsize; ii++){
                    int sum = 0;
                    for(int jj=0; jj<groupsize; jj++){
                        sum += groupmedians[ii][jj]/groupsize;
                    }
                    for(int jj=0; jj<groupsize; jj++)
                        allpivot[ii*groupsize+jj] = sum;
                }
                }
            }
            free(allmedian);
        }
        // MPI_Bcast(&pivot, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(allpivot, 1, MPI_INT, &pivot, 1, MPI_INT, 0, MPI_COMM_WORLD);
        // printf("#%d\tpivot:%d\n",rank,pivot);
        t_end_s = MPI_Wtime();

        t_s = t_end_s - t_begin_s;
        printf("step%d t1:%f\n", step, t_s);

        if(rank>=i*groupsize && rank<(i+1)*groupsize)
        {
            t_begin_s = MPI_Wtime();
            int b = 0;
            for(b=0; b<c_size; b++)
            {
                if(chunk[b]>pivot)
                    break;
            }
            int* smaller = (int *)malloc(b*sizeof(int));
            int* larger = (int *)malloc((c_size-b)*sizeof(int));
            for(int ii=0; ii<b; ii++)
            {
                smaller[ii] = chunk[ii];
            }
            for(int ii=b; ii<c_size; ii++)
            {
                larger[ii-b] = chunk[ii];
            }
            free(chunk);

            if(rank<(i*groupsize+groupsize/2))
            {
                size = c_size-b;
                remain = b;
                // t_begin = MPI_Wtime();
                MPI_Send(&size,1,MPI_INT,rank+groupsize/2,111,MPI_COMM_WORLD);
                MPI_Send(larger,size,MPI_INT,rank+groupsize/2,rank,MPI_COMM_WORLD);
                MPI_Recv(&recvsize,1,MPI_INT,rank+groupsize/2,222,MPI_COMM_WORLD,&status);
                recv = (int *)malloc(recvsize*sizeof(int));

                MPI_Recv(recv,recvsize,MPI_INT,rank+groupsize/2,rank+groupsize/2,MPI_COMM_WORLD,&status);
                // t_end = MPI_Wtime();
                s = remain + recvsize;
                chunk = (int *)malloc(s*sizeof(int));
                for(int ii=0; ii<remain; ii++)
                {
                    chunk[ii] = smaller[ii];
                }
                for(int ii=0; ii<recvsize; ii++)
                {
                    chunk[remain+ii] = recv[ii];
                }
                // t_begin = MPI_Wtime();
                qsort(chunk, s, sizeof(int),cmp);
                // t_end = MPI_Wtime();
                // t = t_end - t_begin;/%f\n", rank, step,s, t);

                free(smaller);
                free(larger);
                free(recv);
                t_end_s = MPI_Wtime();
            }
            else
            {
                t_begin_s = MPI_Wtime();
                size = b;
                remain = c_size - b;
                // t_begin = MPI_Wtime();
                MPI_Recv(&recvsize,1,MPI_INT,rank-groupsize/2,111,MPI_COMM_WORLD,&status);
                recv = (int *)malloc(recvsize*sizeof(int));
                MPI_Recv(recv,recvsize,MPI_INT,rank-groupsize/2,rank-groupsize/2,MPI_COMM_WORLD,&status);
                MPI_Send(&size,1,MPI_INT,rank-groupsize/2,222,MPI_COMM_WORLD);
                MPI_Send(smaller,size,MPI_INT,rank-groupsize/2,rank,MPI_COMM_WORLD);
                // t_end = MPI_Wtime();
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
                // t_begin = MPI_Wtime();
                qsort(chunk, s, sizeof(int),cmp);  
                // t_end = MPI_Wtime();

                // t = t_end - t_begin;
                // printf("#%d\tstep%d\tsize:%d\tt:%f\n", rank, step,s, t);

                free(smaller);
                free(larger);
                free(recv);
                t_end_s = MPI_Wtime();
            }
        }
                t_s = t_end_s - t_begin_s;
        printf("step%d t2:%f\n", step, t_s);
    }

    // printf("#%d\tsize=%d\n", rank, c_size);
t_begin_s = MPI_Wtime();
    if(rank == 0)
    {
        data = (int *)malloc((n>p?n:p)*sizeof(int));
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
    }
    MPI_Gatherv(chunk,c_size,MPI_INT,data,chunksize,displs,MPI_INT,0,MPI_COMM_WORLD);
    free(chunk);
    t_end_s = MPI_Wtime();

    t_end = MPI_Wtime();

    if (rank == 0) 
    {
        t_s = t_end_s - t_begin_s;
        printf("t3:%f\n", t_s);
        t = t_end - t_begin;
        printf("%f\n", t);

        // FILE *output_file = fopen(output, "w+");
        // if (!output_file)
        // {
        //     printf("Error: failed to open output file\n");
        //     return -1;
        // }

        // int k;
        // for(int i=(n>p?0:p-n); i<(n>p?n:p); i++)
        // {
        //     fprintf(output_file, "%d ", data[i]);
        // }
        // fclose(output_file);

        free(data);
    }
    MPI_Finalize();
}
