#include <mpi.h>
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <cassert>
#include <ctime>
#include <assert.h>
using namespace std;

MPI_Datatype info_type;

typedef struct InfoT {
    int num_rows;
    int num_elem;
    int elem_displs;
} InfoT;

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
    int rank, n_proc, num_rows, num_elem, elem_displs, num_row_per_proc;
    int *proc_row_ptr, *proc_col_idx;
    double *proc_val, *local_res, *global_res, *rhs;
    InfoT proc_info;
    InfoT *info_procs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_proc);

    MPI_Datatype old_types[1];
    MPI_Aint displacements[1];
    int block_counts[1];
    displacements[0] = 0;
    old_types[0] = MPI_INT;
    block_counts[0] = 4;
    MPI_Type_create_struct(1, block_counts, displacements, old_types, &info_type);
    MPI_Type_commit(&info_type);

    int num_elems_procs[n_proc];
    int elem_displs_procs[n_proc];
    int num_rows_procs[n_proc];
    int row_displs_procs[n_proc];


    if(rank == 0) {
        if(argc != 4) {
            cout << "Usage: ./spmv_parallel <num_rows> <num_non_zero> <matrix_file>" << endl;
            exit(1);
        }
        MM.read_csr_matrix(atoi(argv[1]), atoi(argv[2]), argv[3]);
        num_rows = MM.n;
        rhs = std::vector<double>(num_rows, 1.0/num_rows).data();
        num_row_per_proc = round((double)num_rows/n_proc);
        for(int i=0; i<n_proc-1; i++){
            num_rows_procs[i] = num_row_per_proc;
            row_displs_procs[i] = i * num_row_per_proc;
        }
        num_rows_procs[n_proc-1] = num_rows - num_row_per_proc*(n_proc-1);
        row_displs_procs[n_proc-1] = (n_proc-1) * num_row_per_proc;

        info_procs = (InfoT*)malloc(n_proc * sizeof(InfoT));
        for(int i=0; i<n_proc; i++) {
            info_procs[i].num_rows = num_rows;
            info_procs[i].num_elem = MM.csr_row_ptr[i * num_row_per_proc + num_rows_procs[i]] - MM.csr_row_ptr[i * num_row_per_proc];
            num_elems_procs[i] = info_procs[i].num_elem;
            info_procs[i].elem_displs = MM.csr_row_ptr[i * num_row_per_proc];
            elem_displs_procs[i] = info_procs[i].elem_displs;
        }
    }

    MPI_Scatter(info_procs, 1, info_type, &proc_info, 1, info_type, 0, MPI_COMM_WORLD);
    num_rows = proc_info.num_rows;
    num_elem = proc_info.num_elem;
    elem_displs = proc_info.elem_displs;

    MPI_Bcast(num_rows_procs, n_proc, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(row_displs_procs, n_proc, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank!=0) {
        rhs = (double*)malloc(sizeof(double) * num_rows);
        num_row_per_proc = num_rows_procs[rank];
    }
    MPI_Bcast(rhs, num_rows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    proc_row_ptr = (int*)malloc(sizeof(int) * (num_row_per_proc+1));
    MPI_Scatterv(MM.csr_row_ptr.data(), num_rows_procs, row_displs_procs, MPI_INT, proc_row_ptr, num_row_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
    proc_row_ptr[num_row_per_proc] = num_elem + proc_row_ptr[0];

    proc_col_idx = (int*)malloc(sizeof(int) * num_elem);
    MPI_Scatterv(MM.csr_col_idx.data(), num_elems_procs, elem_displs_procs, MPI_INT, proc_col_idx, num_elem, MPI_INT, 0, MPI_COMM_WORLD);

    proc_val = (double*)malloc(sizeof(double) * num_elem);
    MPI_Scatterv(MM.csr_val.data(), num_elems_procs, elem_displs_procs, MPI_DOUBLE, proc_val, num_elem, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    local_res = (double*)malloc(sizeof(double) * num_rows);
    global_res = (double*)malloc(sizeof(double) * num_rows);
    double temp_res;

    clock_t start = clock();
    for(int i=0; i<num_rows; i++){
        temp_res = 0;
        for (int j=proc_row_ptr[i]; j<proc_row_ptr[i+1];j++){
            temp_res += proc_val[j-elem_displs] * rhs[proc_col_idx[j-elem_displs]];
        }
        local_res[i] = temp_res;
    }

    MPI_Allgatherv(local_res, num_row_per_proc, MPI_DOUBLE, global_res, num_rows_procs, row_displs_procs, MPI_DOUBLE, MPI_COMM_WORLD);
    clock_t end = clock();

    if(rank == 0){
        cout << "Time taken: " << (double)(end-start)/CLOCKS_PER_SEC << endl;
    }
    free(proc_row_ptr);
    free(proc_col_idx);
    free(proc_val);
    free(local_res);
    free(global_res);
    MPI_Finalize();
    return 0;
}