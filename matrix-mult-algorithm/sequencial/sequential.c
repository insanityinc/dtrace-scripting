#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double clearcache [30000000];

void clearCache (void) {
    for (unsigned i = 0; i < 30000000; ++i)
        clearcache[i] = i;
}

void print_matrix(int size, int** matrix)
{
    for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void print_vector(int size, int* vector)
{
    for (int i=0; i<size; i++)
        printf("%d\n", vector[i]);
}

void print_sparceM(int nz, int* row_ind, int* col_ind, int* values)
{
    printf("\nInput Sparce Matrix in COO format\n");
    printf("ROW |COL |VALUES\n");
    for (int i=0; i<nz; i++)
        printf("%d   |%d   |%d\n", row_ind[i], col_ind[i], values[i]);
    printf("\n");
}

void print_solution(int size, int* solution)
{
    printf("Solution: \n");
    for (int i=0; i<size; i++)
        printf("\t%d\n", solution[i]);
}

int** read_matrix(int* size)
{
    FILE* fp = fopen("data/matrix","r");
    fscanf(fp, "%d", size);

    int** matrix = malloc((*size) * sizeof(int*));
    for (int i=0; i<(*size); i++)
        matrix[i] = malloc((*size) * sizeof(int));

    for (int i=0; i<(*size); i++)
        for (int j=0; j<(*size); j++)
            fscanf(fp, "%d", &matrix[i][j]);

    fclose(fp);
    return matrix;
}

int* read_vector(int size)
{
    FILE* fp = fopen("data/vector","r");
    int* vector = malloc(size * sizeof(int));
    for (int i=0; i<size; i++)
        fscanf(fp, "%d", &vector[i]);
    fclose(fp);
    return vector;
}

void free_everything(int* size, int** matrix, int* vector, int* row_ind,
                     int* col_ind, int* values, int* solution)
{
    for (int i=0; i<(*size); i++)
        free(matrix[i]);

    free(matrix);  free(vector);
    free(row_ind); free(col_ind); free(values); free(solution);
    free(size);
}

int count_non_zeros(int size, int** matrix)
{
    int c=0;
    for (int i=0; i<size; i++)
        for (int j=0; j<size; j++)
            if (matrix[i][j]!=0)
                c++;
    return c;
}

void create_sparce_matrix(int size, int** matrix,
                          int* row_ind, int* col_ind, int* values)
{
    int k=0;
    for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            if (matrix[i][j]!=0) {
                row_ind[k]=i;
                col_ind[k]=j;
                values[k]=matrix[i][j];
                k++;
            }
        }
    }
}

int n_threads(int size, int* row_ind)
{
    int n=0;
    for (int i=1; i<size; i++) {
        if (row_ind[i]!=row_ind[i-1]) {
            n++;
        }
    }
    return n;
}

int check_solution(int size, int** matrix, int* vector, int* solution)
{
    int* aux = malloc(sizeof(int) * (size));
    for (int i=0; i<size; i++)
        aux[i] = 0;

    for (int i=0; i<size; i++)
        for (int j=0; j<size; j++)
            aux[i] += matrix[i][j] * vector[j];

    for (int i=0; i<size; i++) {
        if (aux[i] != solution[i]) {
            //print_solution(size, aux);
            return -1;
        }
    }

    return 0;
}

void compute_multiplication(int nz, int* vector, int* row_ind, int* col_ind,
                            int* values, int* solution)
{
    double start, end;
    int i=0;

    start = omp_get_wtime();

    for (i=0; i<nz; i++)
        solution[ row_ind[i] ] += vector[ col_ind[i] ] * values[i];

    end = omp_get_wtime();

    double timeMs = (end-start)*1000;
    printf("Time compute_multiplication: %f milliseconds.\n", timeMs);
}

int main(int argc, char *argv[])
{
    int* size = malloc(sizeof(int));
    int** matrix = read_matrix(size);;
    int* vector = read_vector(*size);;
    int nz = count_non_zeros((*size), matrix);

    int* row_ind = malloc(sizeof(int) * nz);
    int* col_ind = malloc(sizeof(int) * nz);
    int* values = malloc(sizeof(int) * nz);
    int* solution = malloc(sizeof(int) * (*size));

    for (int i=0; i<(*size); i++)
        solution[i] = 0;

    create_sparce_matrix((*size), matrix, row_ind, col_ind, values);
    clearCache();

    compute_multiplication(nz, vector, row_ind, col_ind, values, solution);

    // print_sparceM(nz, row_ind, col_ind, values);
    // print_solution((*size), solution);

    int check = check_solution((*size), matrix, vector, solution);

    if ( check == -1)
        printf("Solution is WRONG!\n");
    else if (check == 0)
        printf("Solution is CORRECT!\n");

    free_everything(size, matrix, vector, row_ind, col_ind, values, solution);

    return 0;
}
