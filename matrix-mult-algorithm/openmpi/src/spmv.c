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
    /* MPI */
    int id_proc, total_procs, MASTER=0;

    int* size = malloc(sizeof(int));
    int** matrix = read_matrix(size);
    int* vector = read_vector(*size);
    int nz = count_non_zeros((*size), matrix);

    int* row_ind = malloc(sizeof(int) * nz);
    int* col_ind = malloc(sizeof(int) * nz);
    int* values = malloc(sizeof(int) * nz);
    int* solution_local = calloc((*size), sizeof(int));
    int* solution_global = calloc((*size), sizeof(int));

    create_sparce_matrix((*size), matrix, row_ind, col_ind, values);
    clearCache();

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &id_proc);
    MPI_Comm_size(MPI_COMM_WORLD, &total_procs);

    if(id_proc == MASTER)
    {
        int chunck_init_m = 0, dest_proc = 1;
        for (int i=0; i<nz-1; i++) {
            if (row_ind[i] != row_ind[i+1]) {
                MPI_Send(&chunck_init_m, 1, MPI_INT, dest_proc, 0, MPI_COMM_WORLD);
                if (dest_proc < total_procs) {
                    dest_proc++;
                }
                else {
                    MPI_Barrier(MPI_COMM_WORLD);
                    dest_proc = 1;
                }
                chunck_init_m = i+1;
            }
        }
    }
    if (id_proc != MASTER)
    {
        int chunck_init_s = 0, res=0;
        MPI_Recv(&chunck_init_s, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int j=chunck_init_s; (row_ind[j] != row_ind[j+1]) && (j<(nz-1)); j++) {
            res += values[j] * vector[col_ind[j]];
            printf("SLAVE %d: %d\n", chunck_init_s, res );
        }
        solution_local[chunck_init_s] = res;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Reduce(solution_local, solution_global, nz, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);

    if (id_proc == MASTER) {

        print_sparceM(nz, row_ind, col_ind, values);
        printf("VETOR\n");
        print_vector((*size), vector);
        printf("\nSOLUCAO\n");
        print_vector((*size), solution_global);

        int check = check_solution((*size), matrix, vector, solution_global);

        if ( check == -1)
            printf("Solution is WRONG!\n");
        else if (check == 0)
            printf("Solution is CORRECT!\n");

        free_everything(size, matrix, vector, row_ind, col_ind, values, solution_global);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
