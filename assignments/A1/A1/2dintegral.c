/**********************************************************************
 * This program evaluates an integral using MPI/C
 *
 **********************************************************************/

#include <mpi.h>
#include <stdio.h>
#include <math.h>
#define MASTER 0        /* task ID of master task */

int main(int argc, char *argv[]) {
  int rank, size, rcode;
  const long int intervals;                  /* The sum is [globally]
                                             divided into this many
                                             intervals     */
  int chunk;             /* This many iterations will I do */
  int i, j, istart, istop;  /* Variables for the local loop   */
  double sum, dx, dy, globsum;
  double t_begin, t_end, t;

  scanf("%ld", &intervals);  /* Input interval number N */

  MPI_Init(&argc, &argv); /* Initialize MPI */

  MPI_Comm_size(MPI_COMM_WORLD, &size); /* Get the number of processors */
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* Get my number                */

  MPI_Bcast(&intervals, 1, MPI_LONG, 0, MPI_COMM_WORLD);

  t_begin = MPI_Wtime();

  chunk  = intervals/size;       /* Number of intervals per processor */
  istart = rank*chunk+1;         /* Calculate start and stop indices  */
  istop  = (rank + 1)*chunk;     /* for the local loop                */
  if (rank == size - 1) {
    istop = intervals;           /* Make sure the last processor      */
  }                              /* computes until the end            */

  dx  = 2.0/intervals;
  dy  = 2.0/intervals;
  sum = 0.0;
  for (i = istart; i <= istop; i++) { /* The local loop */
    double x = dx*(i - 0.5);
    for (j = 0; j <= intervals; j++) {
      double y = dy*(j - 0.5);
      sum += dx*dy*(x + sin(y) + 1);
    }
  }

  /* COMPUTE THE FINAL RESULT ADDING THE PARTIAL SUMS */
  MPI_Reduce(&sum, &globsum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

  t_end = MPI_Wtime();

  /*write file*/ 
  if (rank == MASTER) {
    t = t_end - t_begin;
    FILE *fp = fopen("A1output", "w+");
    if (!fp)
    {
      printf("Error: failed to open output file\n");
      return -1;
    }
    fprintf(fp, "%ld %.6f %.6f\n", intervals, globsum, t);
    fclose(fp);
  }

  MPI_Finalize(); /* Shut down and clean up MPI */

  return 0;
}
