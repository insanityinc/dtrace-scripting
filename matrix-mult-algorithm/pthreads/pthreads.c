#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define NUM_EVENTS 1
double clearcache[30000000];

int thread_count;
pthread_mutex_t mutex;

int* size;

int nz;

int** matrix;
int* vector;
int* row_ind;
int* col_ind;
int* values;
int* solution;

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
    free(row_ind); free(col_ind); free(values); free(solution); free(size);
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
            //print_solution(size, aux);
            return -1;
        }
    }

    return 0;
}

void* compute_multiplication(void* rank)
{
    long my_rank = (long) rank;
    int i;
    int div = nz/thread_count;
    int rem = nz % thread_count;
    int my_first_i = my_rank * div;
    int my_last_i = my_first_i + div;
    int aux1, aux2, aux3, aux4;

    if (my_rank == thread_count-1)
        my_last_i += rem;

    for (i=my_first_i; i<my_last_i; i++) {
        aux1 = col_ind[i];
        aux2 = vector[aux1];
        aux3 = values[i];
        aux4 = row_ind[i];
        
        pthread_mutex_lock(&mutex);
        solution[aux4] += aux2 * aux3;
        pthread_mutex_unlock(&mutex);
        
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    long thread;
    pthread_t* thread_handles;
    int error;

    if (pthread_mutex_init(&mutex, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 

    thread_count = strtol(argv[1], NULL, 10);
    thread_handles = malloc(thread_count * sizeof(pthread_t));
    
    size = malloc(sizeof(int));

    matrix = read_matrix(size);;
    vector = read_vector(*size);;
    nz = count_non_zeros((*size), matrix);

    printf("nnz: %d\n", nz);

    row_ind = malloc(sizeof(int) * nz);
    col_ind = malloc(sizeof(int) * nz);
    values = malloc(sizeof(int) * nz);
    solution = malloc(sizeof(int) * (*size));

    for (int i=0; i<(*size); i++) {
        solution[i] = 0;
    }

    create_sparce_matrix((*size), matrix, row_ind, col_ind, values);

    clearCache();

    for (thread = 0; thread < thread_count; thread++) {
        error = pthread_create(&thread_handles[thread], NULL, compute_multiplication, (void*) thread);
        if (error != 0) {
          printf("\nThread can't be created :[%s]", strerror(error));
        }
    }

    for (thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);
   
    int check = check_solution((*size), matrix, vector, solution);
    if ( check == -1) {
        printf("Solution is WRONG!\n");
    }
    else if (check == 0) {
        printf("Solution is CORRECT!\n");
    }

    /* print_matrix((*size), matrix); */
    /* print_vector((*size), vector); */
    /* print_solution((*size), solution); */

    pthread_mutex_destroy(&mutex);
    
    free_everything(size, matrix, vector, row_ind, col_ind, values, solution);

    return 0;
}
