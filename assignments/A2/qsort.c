#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

int cmp (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

int main(int argc, char **argv){
    printf("start\n");
    char *input = argv[1];
    int n, *data, i;
    double t_begin, t_end, t;

    printf("start reading\n");
    FILE *input_file = fopen(input, "r");
    if (!input_file)
    {
        printf("Error: failed to open input file\n");
        return -1;
    }
    fseek(input_file, 0, SEEK_SET);
    fscanf(input_file, "%d", &n);
    data = (int *)malloc(n*sizeof(int));
    for(i=0; i<n; i++) 
        fscanf(input_file, "%d", &(data[i]));
    fclose(input_file);
    printf("reading end\n");

    t_begin = time(NULL);
    qsort(data, n, sizeof(int),cmp);
    t_end = time(NULL);

    t = t_end - t_begin;
    printf("%f\n", t);
}
