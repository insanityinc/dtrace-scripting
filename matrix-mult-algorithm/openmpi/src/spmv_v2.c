#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NUM_EVENTS 3
double clearcache [30000000];

void clearCache (void)
{
    for (unsigned i = 0; i < 30000000; ++i)
        clearcache[i] = i;
}

void print_sparceM(int nz, int* row_ind, int* col_ind, int* values)
{
    printf("\nInput Sparce Matrix in COO format\n");
    printf("ROW |COL |VALUES\n");
    for (int i=0; i<nz; i++)
        printf("%d   |%d   |%d\n", row_ind[i], col_ind[i], values[i]);
    printf("\n");
}

void print_vector(int size, int* vector)
{
    for (int i=0; i<size; i++)
        printf("%d\n", vector[i]);
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
            return -1;
        }
    }

    return 0;
}

void compute_multiplication(int nz, int* vector, int* row_ind, int* col_ind,
                            int* values, int* solution)
{
    int i=0;

    for (i=0; i<nz; i++) {
        solution[ row_ind[i] ] += vector[ col_ind[i] ] * values[i];
    }
}

int main(int argc, char *argv[])
{
    int* size;
    int** matrix;
    int* vector;
    int nz, MASTER=0, chunk;
    int *row_ind, *col_ind, *values;
    double tscat=0, tbcast=0;
    double sAux=0, eAux=0;

    MPI_Init(NULL, NULL);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    size = malloc(sizeof(int));
    matrix = read_matrix(size);
    vector = read_vector(*size);
    nz = count_non_zeros((*size), matrix);
    row_ind = malloc(sizeof(int) * nz);
    col_ind = malloc(sizeof(int) * nz);
    values = malloc(sizeof(int) * nz);
    create_sparce_matrix((*size), matrix, row_ind, col_ind, values);
    clearCache();
    chunk = nz / world_size; //round down (floor)

    sAux = MPI_Wtime();
    MPI_Bcast(vector, (*size), MPI_INT, MASTER, MPI_COMM_WORLD);
    eAux = MPI_Wtime();
    tbcast = eAux - sAux;

    int* row_ind_local = calloc(chunk, sizeof(int));
    int* col_ind_local = calloc(chunk, sizeof(int));
    int* values_local = calloc(chunk, sizeof(int));
    int* solution_local = calloc((*size), sizeof(int));
    double sAux_local=0, eAux_local=0, tcomp_local=0;

    if (world_rank == MASTER)
        sAux = MPI_Wtime();

    MPI_Scatter(row_ind, chunk, MPI_INT, row_ind_local, chunk, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Scatter(col_ind, chunk, MPI_INT, col_ind_local, chunk, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Scatter(values, chunk, MPI_INT, values_local, chunk, MPI_INT, MASTER, MPI_COMM_WORLD);

    if (world_rank == MASTER) {
        eAux = MPI_Wtime();
        tscat = eAux - sAux;
    }

    sAux_local = MPI_Wtime();
    for (int i=0; i<chunk; i++) {
        solution_local[ row_ind_local[i] ] += vector[ col_ind_local[i] ] * values_local[i];
    }
    eAux_local = MPI_Wtime();
    tcomp_local = eAux_local - sAux_local;

    int* solution_global = calloc((*size), sizeof(int));
    double tcomp_global=0;

    MPI_Reduce(solution_local, solution_global, (*size), MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);
    MPI_Reduce(&tcomp_local, &tcomp_global, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD);

    if (world_rank == MASTER) {

        // print_sparceM(nz, row_ind, col_ind, values);
        // printf("VETOR\n");
        // print_vector((*size), vector);
        // printf("\nSOLUCAO\n");
        // print_vector((*size), solution_global);

        //int check = check_solution((*size), matrix, vector, solution_global);

        // if ( check == -1)
        //     printf("Solution is WRONG!\n");
        // else if (check == 0)
        //     printf("Solution is CORRECT!\n");


        free_everything(size, matrix, vector, row_ind, col_ind, values, solution_global);

        FILE* fp = fopen("resultados.csv", "a");
        fprintf(fp, "%f,%f,%f,%f\n", tbcast*1000, tscat*1000, tcomp_global*1000, tbcast*1000 + tscat*1000 + tcomp_global*1000);
        fclose(fp);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
