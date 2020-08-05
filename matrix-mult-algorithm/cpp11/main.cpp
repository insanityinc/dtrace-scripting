#include "coo.hpp"
#include <iostream>

using namespace std;

mutex m;

void compute_mult(Coo *c, int vector[], int solution[], int t_id, int t_count);
int check_sol(Coo* c, int *vector, int *solution, int size);

int main(int argc, char *argv[]) {

  if (argc != 4) return 1;
  srand(time(0));

  istringstream iss1( argv[1] );
  istringstream iss2( argv[2] );
  istringstream iss3( argv[3] );

  int size; float per; int threads_number;
  iss1 >> size; iss2 >> per; iss3 >> threads_number;

  int* vector; int* solution;
  vector= new int[size]; solution= new int[size];

  for (int i=0; i<size; ++i)
    solution[i]=0;

  for (int i= 0; i<size; ++i)
    vector[i]= (rand() % MAX_NUMBER);

  thread threads[threads_number];

  Coo* c = new Coo(size, per);

  for (int i = 0; i < threads_number; ++i)
    threads[i] = thread(compute_mult, c, vector, solution, i, threads_number);

  for (int i= 0; i< threads_number; ++i)
    threads[i].join();

  // for (int i= 0; i< c->get_nnz(); ++i)
  //   solution[ c->get_row_ind_i(i) ] += vector[ c->get_col_ind_i(i) ] * c->get_value_i(i);

  int check = check_sol(c, vector, solution, size);
  if ( check == 1)
    cout << "Invalid!" << endl;
  else if (check == 0)
    cout << "Valid!" << endl;

  delete [] vector;
  delete [] solution;
  return 0;
}

void compute_mult(Coo* c, int vector[], int solution[], int t_id, int t_count) {

  int i;
  int div = c->get_nnz() / t_count;
  int rem = c->get_nnz() % t_count;
  int my_first_i = t_id * div;
  int my_last_i = my_first_i + div;

  if (t_id == t_count-1)
    my_last_i += rem;
  
  int aux1, aux2, aux3, aux4;

  for (i=my_first_i; i<my_last_i; i++) {

    aux1 = c->get_col_ind_i(i);
    aux2 = vector[aux1];
    aux3 = c->get_value_i(i);
    aux4 = c->get_row_ind_i(i);

    m.lock();
    solution[aux4] += aux2 * aux3;
    m.unlock();

  }
}

int check_sol(Coo* c, int *vector, int *solution, int size) {

  /* Allocate auxiliar structs */
  int* v1 = new int[size];
  int** m1 = new int*[size];
  for (int i= 0; i< size; ++i)
    m1[i]= new int[size];

  for (int i=0; i<size; ++i)
    v1[i]=0;

  for (int i=0; i<size; ++i)
    for (int j=0; j<size; ++j)
      m1[i][j]=0;

  /* Fill auxiliar structs */
  for (int i= 0; i<c->get_nnz(); ++i) {
    m1[c->get_row_ind_i(i)][c->get_col_ind_i(i)] = c->get_value_i(i);
  }

  // for (int i=0; i<size; ++i)
  //   cout << v1[i] << " ";
  // cout << endl;
  // cout << endl;

  /* Basic matrix vector multiplication algorithm */
  for (int i=0; i<size; i++)
    for (int j=0; j<size; j++)
      v1[i] += m1[i][j] * vector[j];

  // for (int i=0; i<size; ++i)
  //   cout << v1[i] << " ";
  // cout << endl;
  // cout << endl;
  // for (int i=0; i<size; ++i)
  //   cout << solution[i] << " ";
  // cout << endl;
  // cout << endl;

  /* Check if booth solutions are equal */
  for (int i=0; i<size; i++) {
    if (v1[i] != solution[i]) {
      // cout << "i: " << i << " " << v1[i] << " " << solution[i] << endl;
      return 1;
    }
  }

  return 0;
}
