#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <cassert>
#include <ctime>
#include <assert.h>
using namespace std;

class CSRMatrix{
public:
    int n;
    int nnz;
    std::vector<int> csr_row_ptr;
    std::vector<int> csr_col_idx;
    std::vector<double> csr_val;
    CSRMatrix(){
        csr_row_ptr.clear();
        csr_col_idx.clear();
        csr_val.clear();
    }
    ~CSRMatrix(){
        csr_row_ptr.clear();
        csr_col_idx.clear();
        csr_val.clear();
    }
    void read_csr_matrix(int n_in, int nnz_in, string filepath);
};

void CSRMatrix::read_csr_matrix(int n_in, int nnz_in, string filepath){
    n = n_in;
    nnz = nnz_in;
    ifstream fin(filepath);
    int temp_int;
    double temp_double;
    if (!fin.is_open()){
        cout << "FILE ERROR OPEN!" << endl;
        exit(1);
    } else {
        for(int i=0; i<nnz; i++){
            fin >> temp_double;
            csr_val.push_back(temp_double);
        }
        for(int i=0; i<n+1; i++){
            fin >> temp_int;
            csr_row_ptr.push_back(temp_int);
            
        }
        for(int i=0; i<nnz; i++){
            fin >> temp_int;
            csr_col_idx.push_back(temp_int);
        }
    }
    assert(csr_row_ptr.size() == n+1);
    assert(csr_col_idx.size() == nnz);
    assert(csr_val.size() == nnz);
    cout << "CSR matrix read successfully, with " << "n=" << n << " nnz=" << nnz << endl;
    fin.close();
}

int main(int argc, char** argv) {
    CSRMatrix MM;
    int num_rows;
    if(argc != 4) {
        cout << "Usage: ./spmv_parallel <num_rows> <num_non_zero> <matrix_file>" << endl;
        exit(1);
    }
    MM.read_csr_matrix(atoi(argv[1]), atoi(argv[2]), argv[3]);
    num_rows = MM.n;
    std::vector<double> rhs(num_rows, 1.0/(double)num_rows);
    std::vector<double> res(num_rows, 0.0);
    auto start = clock();
    for(int i=0; i<num_rows; i++){
        for(int j=MM.csr_row_ptr[i]; j<MM.csr_row_ptr[i+1]; j++){
            res[i] += MM.csr_val[j] * rhs[MM.csr_col_idx[j]];
        }
    }
    auto end = clock();
    cout << "Time taken: " << (double)(end-start)/CLOCKS_PER_SEC << endl;
    return 0;
}