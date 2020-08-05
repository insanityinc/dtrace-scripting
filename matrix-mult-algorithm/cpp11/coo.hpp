#include <iostream>
#include <algorithm>
#include <sstream>
#include <time.h>
#include <mutex>
#include <thread>

#define MAX_NUMBER 100

class Coo {
public:
  Coo(int,float);
  ~Coo();
  int get_nnz();
  void print_coo();
  int get_row_ind_i(int);
  int get_col_ind_i(int);
  int get_value_i(int);

private:
  int size, nnz;
  float per;

  int* row_ind;
  int* col_ind;
  int* val;
};
