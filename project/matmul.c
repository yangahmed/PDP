#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

void scatter(float* matrix, float* mat_local, int n, int p_root, int tag) {
    /* scatter given matrix to the processors */
    int n_local = n/p_root;
    float *temp = (float *)malloc(n_local*n_local*sizeof(float));

    for(int i=0; i<p_root; i++) {
        for(int j=0; j<p_root; j++) {
            int ii = i*n_local*n;
            int jj = j*n_local;
            for(int k=0; k<n_local; k++) {
                for(int l=0; l<n_local; l++) {
                    temp[k*n_local+l] = matrix[ii+jj+l];
                }
                ii += n;
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
    /* get the globle rank to help shift the blocks */
    int rank = ((row+p_root)%p_root)*p_root+(col+p_root)%p_root;
    return rank;
}

void local_matmul(int n, float* A, float* B, float* C) {
    /* matrix multiplication */
    // float* temp = (float *)calloc(n*n, sizeof(float));
    for(int i=0; i<n; i++){
        for(int k=0; k<n; k++){
            for(int j=0; j<n; j++){
                // temp[i*n+j] += A[i*n+k]*B[k*n+j];
                C[i*n+j] += A[i*n+k]*B[k*n+j];
            }
            // C[i*n+j] += temp[i*n+j];
        }
    }
    // free(temp);
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
    double t_begin, t_end, t;
    int m_col;

    p_root = sqrt(p);
    if(p != p_root*p_root) {
        printf("Error: process number is not a square\n");
        return -1;
    }

    /* read from file */
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
    buf_A = (float *)calloc(size, sizeof(float));
    buf_B = (float *)calloc(size, sizeof(float));

    t_begin = MPI_Wtime();

    /* scatter the two matrices */
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

    myrow = rank/p_root;
    mycol = rank - myrow*p_root;

    MPI_Comm row_comm;
    MPI_Comm_split(MPI_COMM_WORLD, myrow, mycol, &row_comm);

    /*Fox's algorithm*/
    m_col = myrow;
    if(mycol == m_col) {
        memcpy(buf_A, A, size*sizeof(float));
    }
    MPI_Bcast(buf_A, size, MPI_FLOAT, m_col, row_comm);
    // t_begin = MPI_Wtime();
    local_matmul(n_local, buf_A, B, C);
    // t_end = MPI_Wtime();

    for(int step=1; step<p_root; step++) {
        printf("step");
        MPI_Sendrecv(B, size, MPI_FLOAT, get_rank(myrow-1, mycol, p_root), 444,
            buf_B, size, MPI_FLOAT, get_rank(myrow+1, mycol, p_root), 444,
            MPI_COMM_WORLD, &status);
        memcpy(B, buf_B, size*sizeof(float));

        m_col = (m_col+1)%p_root;
        if(mycol == m_col) {
            memcpy(buf_A, A, size*sizeof(float));
        }
        MPI_Bcast(buf_A, size, MPI_FLOAT, m_col, row_comm);
        local_matmul(n_local, buf_A, B, C);
    }
    free(A);
    free(B);
    free(buf_A);
    free(buf_B);
    MPI_Comm_free(&row_comm);


    /* gather */
    if(rank != 0) {
        MPI_Send(C, size, MPI_FLOAT, 0, 555, MPI_COMM_WORLD);
        free(C);
    } else {
        C_all = (float *)malloc(n*n*sizeof(float));
        float* temp = (float *)calloc(size, sizeof(float));

        for(int i=0; i<p_root; i++) {
            for(int j=0; j<p_root; j++) {
                memset(temp, 0, sizeof(temp));
                if(i==0 && j==0) {
                    memcpy(temp, C, size*sizeof(float));
                    free(C);
                } else {
                    MPI_Recv(temp, size, MPI_FLOAT, i*p_root+j, 555, MPI_COMM_WORLD, &status);
                }

                int ii = i*n_local*n;
                int jj = j*n_local;
                for(int k=0; k<n_local; k++) {
                    for(int l=0; l<n_local; l++) {
                        C_all[ii+jj+l] = temp[k*n_local+l];
                    }
                    ii += n;
                }
            }
        }
        free(temp);
    }

    t_end = MPI_Wtime();

    /* output */
    if(rank == 0) {
        t = t_end - t_begin;
        printf("%f\n", t);
/*
        FILE *output_file = fopen(output, "w+");
        if (!output_file)
        {
            printf("Error: failed to open output file\n");
            return -1;
        }
        for(int i=0; i<n; i++) {
            for(int j=0; j<n; j++) {
                fprintf(output_file, "%.6f ", C_all[i*n+j]);
            }
            fprintf(output_file, "\n");
        }
        */
        free(C_all);
    }
    MPI_Finalize();
    return 0;
}