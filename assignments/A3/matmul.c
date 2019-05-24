#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

void scatter(float* matrix, float* mat_local, int n, int p_root, int tag) {
    int n_local = n/p_root;
    float *temp = (float *)malloc(n_local*n_local*sizeof(float));

    for(int i=0; i<p_root; i++) {
        for(int j=0; j<p_root; j++) {
            for(int p=0; p<n_local; p++) {
                for(int q=0; q<n_local; q++) {
                    temp[p*n_local+q] = matrix[i*n_local*n+j*n_local+q];
                }
            }

            if(i==0 && j==0) {
                memcpy(mat_local, temp, n_local*n_local*sizeof(float));
            } else {
                MPI_Send(temp, n_local*n_local, MPI_FLOAT, i*p_root+j, tag, MPI_COMM_WORLD);
            }
        }
    }
    free(temp);
}

int get_rank(int row, int col, int p_root) {
    int rank = ((row+p_root)%p_root)*p_root+(col+p_root)%p_root;
    return rank;
}

void local_matmul(int n, float* A, float* B, float* C) {
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            for(int k=0; k<n; k++){
                C[i*n+j] += A[i*n+k]*B[k*n+j];
            }
        }
    }
}

int main(int argc, char **argv) {
    char *input = argv[1];
    char *output = argv[2];

    MPI_Status status;
    MPI_Init(&argc, &argv);
    int rank, p, p_root;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int n, n_local, size;
    float *A, *B, *C;
    float *A_all, *B_all, *C_all;
    float *buf_A, *buf_B;
    int myrow, mycol;
    int t_begin, t_end, t;

    p_root = sqrt(p);
    if(p != p_root*p_root) {
        printf("Error: process number is not a square\n");
        return -1;
    }

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
        A_all = (float *)malloc(n*n*sizeof(float));
        B_all = (float *)malloc(n*n*sizeof(float));
        for(int i=0; i<n*n; i++) 
           fscanf(input_file, "%f", &(A_all[i]));
        for(int i=0; i<n*n; i++) 
           fscanf(input_file, "%f", &(B_all[i]));
        fclose(input_file);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(n%p_root != 0) {
        printf("Error: n is not divisible\n");
        return -1;
    }
    n_local = n/p_root;
    size = n_local*n_local;
    A = (float *)malloc(size*sizeof(float));
    B = (float *)malloc(size*sizeof(float));
    C = (float *)calloc(size, sizeof(float));
    buf_A = (float *)malloc(size*sizeof(float));
    buf_B = (float *)malloc(size*sizeof(float));

    if(rank == 0) {
        scatter(A_all, A, n, p_root, 111);
        scatter(B_all, B, n, p_root, 222);
        free(A_all);
        free(B_all);
    } else {
        MPI_Recv(A, size, MPI_FLOAT, 0, 111, MPI_COMM_WORLD, &status);
        MPI_Recv(B, size, MPI_FLOAT, 0, 222, MPI_COMM_WORLD, &status);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    t_begin = MPI_Wtime();

    myrow = rank/p_root;
    mycol = rank - myrow*p_root;

    for(int i=0; i<myrow; i++) {
        MPI_Sendrecv(A, size, MPI_FLOAT, get_rank(myrow, mycol-1, p_root), 333,
            buf_A, size, MPI_FLOAT, get_rank(myrow, mycol+1, p_root), 333,
            MPI_COMM_WORLD, &status);
        
        memcpy(A, buf_A, size*sizeof(float));
    }

    for(int j=0; j<mycol; j++) {
        MPI_Sendrecv(B, size, MPI_FLOAT, get_rank(myrow-1, mycol, p_root), 444,
            buf_B, size, MPI_FLOAT, get_rank(myrow+1, mycol, p_root), 444,
            MPI_COMM_WORLD, &status);
        
        memcpy(B, buf_B, size*sizeof(float));
    }

    /* Cannon's algorithm */
    for(int step=0; step<p_root; step++) {
        local_matmul(n_local, A, B, C);

        MPI_Sendrecv(A, size, MPI_FLOAT, get_rank(myrow, mycol-1, p_root), 333,
            buf_A, size, MPI_FLOAT, get_rank(myrow, mycol+1, p_root), 333,
            MPI_COMM_WORLD, &status);
        memcpy(A, buf_A, size*sizeof(float));

        MPI_Sendrecv(B, size, MPI_FLOAT, get_rank(myrow-1, mycol, p_root), 444,
            buf_B, size, MPI_FLOAT, get_rank(myrow+1, mycol, p_root), 444,
            MPI_COMM_WORLD, &status);
        memcpy(B, buf_B, size*sizeof(float));
    }
    free(A);
    free(B);
    free(buf_A);
    free(buf_B);
    t_end = MPI_Wtime();

    /* gather */
    if(rank != 0) {
        MPI_Send(C, size, MPI_FLOAT, 0, 555, MPI_COMM_WORLD);
        free(C);
    } else {
        C_all = (float *)malloc(n*n*sizeof(float));
        float* temp = (float *)calloc(size,sizeof(float));

        for(int i=0; i<p_root; i++) {
            for(int j=0; j<p_root; j++) {
                memset(temp, 0, sizeof(temp));
                if(i==0 && j==0) {
                    memcpy(temp, C, size);
                    free(C);
                } else {
                    MPI_Recv(temp, size, MPI_FLOAT, i*p_root+j, 555, MPI_COMM_WORLD, &status);
                }

                for(int ii=0; ii<n_local; ii++) {
                    for(int jj=0; jj<n_local; jj++) {
                        C_all[i*n_local*n+j*n_local+jj] = temp[ii*n_local+jj];
                    }
                }
            }
        }
        free(temp);

        t = t_begin - t_end;
        printf("%f\n", t);

        FILE *output_file = fopen(output, "w+");
        if (!output_file)
        {
            printf("Error: failed to open output file\n");
            return -1;
        }
        for(int t=0; t<n*n; t++) {
            fprintf(output_file, "%f.6 ", C_all[t]);
        }
        free(C_all);
    }
    MPI_Finalize();
    return 0;
}