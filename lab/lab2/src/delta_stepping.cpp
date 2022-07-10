#pragma GCC optimize(3)
#include <iostream>
#include <omp.h>
#include <stdlib.h>
#include <vector>
#include <climits>
#include <fstream>
#include <time.h>
#include <algorithm>

#define N 10000
#define NUM_THREADS 4
using namespace std;


int delta = 3;
int max_bucket;
std::vector<int> tent(N, INT_MAX);
std::vector<int> B[N];
typedef std::vector<int> bucketType;
typedef std::vector<bucketType> bucketArray;
struct req{
    int w;
    int d;
};
std::vector<req> Req;

void delta_stepping(std::vector< std::vector<int> > G, int source);
bool b_is_empty();
void relax(int w, int d, int thread_id);


int main(int argc, char *argv[]) {
    omp_set_num_threads(NUM_THREADS);
    // delta = stoi(argv[1]);
    delta = 3;
    ifstream infile("./graph_10000.txt");
    std::vector< std::vector<int> > Graph(N, std::vector<int>(N, INT_MAX));
    int temp;
    if (!infile.is_open()) {
        cout << "Error opening file" << endl;
        exit(1);
    }
    for(int i=0; i<N; i++){
        for(int j=0; j<N; j++){
            infile >> temp;
            Graph[i][j] = temp;
        }
    }
    infile.close();
    auto start = omp_get_wtime();
    delta_stepping(Graph, 2);
    auto end = omp_get_wtime();
    cout << "serial Delta-Stepping, with delta=" << delta << ", time taken: " << end - start;
    return 0;
}

bool b_is_empty(){
    for(int i=0; i< max_bucket; i++){
        if(B[i].size() > 0){
            return false;
        }
    }
    return true;
}

void relax(int w, int d, int thread_id){
    if(d < tent[w]){
        if(tent[w] != INT_MAX){
            auto it = find(B[tent[w]/delta].begin(), B[tent[w]/delta].end(), w);
            if(it!=B[tent[w]/delta].end()){
                B[tent[w]/delta].erase(it);
            }
        }
        B[d/delta].push_back(w);
        if(d/delta > max_bucket){
            max_bucket = d / delta;
        }
        tent[w] = d;
    }
}


void delta_stepping(std::vector< std::vector<int> > G, int source) {
    std::vector<int> S;
    std::vector< std::vector<int> > light(N, std::vector<int>(0));
    std::vector< std::vector<int> > heavy(N, std::vector<int>(0));
    std::vector< std::vector<req> > local_req(NUM_THREADS, std::vector<req>(0));
    int i;
    #pragma omp parallel for
    for(i=0; i<N; i++){
        tent[i] = INT_MAX;
        for(int j=0; j<N; j++){
            if(G[i][j] > delta){
                heavy[i].push_back(j);
            } else if(G[i][j] <= delta ){
                light[i].push_back(j);
            }
        }
    }
    relax(source, 0, 0);
    max_bucket = 1;
    i = 0;
    while(!b_is_empty()){
        S.clear();
        while(!B[i].empty()){
            Req.clear();
            #pragma omp for
            for(int j=0; j<NUM_THREADS; j++){
                local_req[j].clear();
            }
            #pragma omp for
            for(int j=0; j<B[i].size(); j++){
                int thread_id = omp_get_thread_num();
                int v = B[i][j];
                for(int k=0; k<light[v].size(); k++){
                    int w = light[v][k];
                    req r;
                    r.w = w;
                    r.d = tent[v] + G[v][w];
                    local_req[thread_id].push_back(r);
                }
            }
            S.insert(S.end(), B[i].begin(), B[i].end());
            for(int j=0; j<NUM_THREADS; j++){
                Req.insert(Req.end(), local_req[j].begin(), local_req[j].end());   
            }
            B[i].clear();
            #pragma omp for
            for(int j=0; j<Req.size(); j++){
                int thread_id = omp_get_thread_num();
                relax(Req[j].w, Req[j].d, thread_id);
            }
        }
        Req.clear();
        #pragma omp for
        for(int j=0; j<NUM_THREADS; j++){
            local_req[j].clear();
        }
        #pragma omp for
        for(int j=0; j<S.size(); j++){
            int thread_id = omp_get_thread_num();
            int v=S[j];
            for(int k=0; k<heavy[v].size(); k++){
                int w = heavy[v][k];
                req r;
                r.w = w;
                r.d = tent[v] + G[v][w];
                local_req[thread_id].push_back(r);
            }
        }
        #pragma omp for
        for(int j=0; j<NUM_THREADS; j++){
            #pragma omp critical
            Req.insert(Req.end(), local_req[j].begin(), local_req[j].end());   
        }
        #pragma omp for
        for(int j=0; j<Req.size(); j++){
            int thread_id = omp_get_thread_num();
            relax(Req[j].w, Req[j].d, thread_id);
        }
        i++;
    }
}
