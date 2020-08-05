#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MVALUES 10 /* range do random 0 - 5 */

void free_everything(int size, int** matrix, int* vector)
{
    for (int i=0; i<size; i++) {
        free(matrix[i]);
    }
    free(matrix);
    free(vector);
}

void print_vector(int size, int* vector)
{
    FILE* fp = fopen("data/vector", "w+");

    for (int i=0; i<size; i++) {
        fprintf(fp, "%d ", vector[i]);
    }
    fprintf(fp, "\n");
    fclose(fp);
}

void print_matrix(int size, int** matrix)
{
    FILE* fp = fopen("data/matrix", "w+");

    fprintf(fp, "%d\n", size);

    for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            fprintf(fp, "%d ", matrix[i][j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

int main(int argc, char *argv[])
{
    int size = atoi(argv[1]);
    float NNZper = atof(argv[2]);

    srand(time(NULL));
    int NNZ = round((size * size) * NNZper);

    int** matrix = malloc(sizeof(int*) * size);
    for (int i=0; i<size; i++) {
        matrix[i] = malloc(sizeof(int) * size);
        for (int j=0; j<size; j++) {
            matrix[i][j] = 0;
        }
    }

    int* vector = malloc(sizeof(int) * size);
    for (int i=0; i<size; i++) {
        vector[i] = (int)rand()/(int)(RAND_MAX/MVALUES);
    }

    for (int i=0; i<NNZ;) {
        int index_i = (int) (size * ((double) rand() / (RAND_MAX + 1.0)));
        int index_j = (int) (size * ((double) rand() / (RAND_MAX + 1.0)));
        if (matrix[index_i][index_j]) {
            continue;
        }
        matrix[index_i][index_j] = (int)rand()/(int)(RAND_MAX/MVALUES);
        ++i;
    }

    print_matrix(size, matrix);
    print_vector(size, vector);

    free_everything(size, matrix, vector);

    return 0;
}
