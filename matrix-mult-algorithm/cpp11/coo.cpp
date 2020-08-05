#include "coo.hpp"

using namespace std;

Coo::Coo(int size_arg, float per_arg) {
  size= size_arg; per= per_arg;
  nnz= size * per;

  row_ind = new int[nnz];
  col_ind = new int[nnz];
  val = new int[nnz];

  for (int i= 0; i< nnz; ++i) {
    row_ind[i]= (rand() % size);
    col_ind[i]= (rand() % size);
    val[i]= (rand() % MAX_NUMBER);
  }
  sort(row_ind, row_ind+nnz);
}

int Coo::get_nnz() { return nnz; }
int Coo::get_row_ind_i(int i_arg) { return row_ind[i_arg]; }
int Coo::get_col_ind_i(int i_arg) { return col_ind[i_arg]; }
int Coo::get_value_i(int i_arg) { return val[i_arg]; }

void Coo::print_coo() {
  for (int i= 0; i< nnz; ++i)
    cout << row_ind[i] << "  " << col_ind[i] << "  " << val[i] << endl;
}

Coo::~Coo() {
  delete [] row_ind;
  delete [] col_ind;
  delete [] val;
}
