#pragma GCC optimize(3)
#include <iostream>
#include <omp.h>
#include <stdlib.h>
#include <vector>
#include <climits>
#include <fstream>
#include <algorithm>
#include <cmath>

#define N 10000
#define MAX_WEIGHT 110
using namespace std;

typedef std::vector<int> bucketType;
typedef std::vector<bucketType> bucketArray;
struct req{
    int w;
    int d;
};

int delta = 6;
std::vector<req> Req;
bucketArray buckets;
omp_lock_t* locks;
std::vector<int> dists(N, INT_MAX);
std::vector<int> B[N];
std::vector< std::vector<int> > Graph(N, std::vector<int>(N, INT_MAX));
int NUM_THREADS = 4;
int BUCKET_MIN_THRESHOLD = 1024;

void delta_stepping(std::vector< std::vector<int> > G, int source);
void init_buckets(int source);
void relax(int w, int d);
void relax_parallel(int source, bucketArray& local_buckets);
void bucket_fusion_parallel(bucketArray& local_buckets);
void copy_to_shared_buckets(bucketArray& shared_buckets, bucketArray& local_buckets);
void post_process();

int main(int argc, char *argv[]) {
    NUM_THREADS = stoi(argv[1]);
    delta = stoi(argv[2]);
    BUCKET_MIN_THRESHOLD = stoi(argv[3]);
    omp_set_num_threads(NUM_THREADS);
    ifstream infile("./graph_10000.txt");
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
    cout << NUM_THREADS << " threads parallelized Delta-Stepping, with delta=" << delta << ", threshold=" << BUCKET_MIN_THRESHOLD << ", time taken: " << end - start;
    return 0;
}


void delta_stepping(std::vector< std::vector<int> > G, int source) {
    init_buckets(source);
    /* shared variables */
    int min_bucket_index = INT_MAX;
    for(int i=0; i<buckets.size(); i++){
        if(!buckets[i].empty()){
            min_bucket_index = i;
            break;
        }
    }
    int next_min_bucket_index = INT_MAX;
    int prev_min_bucket_size = 0;
    int min_bucket_start = 0;
    bucketType *min_bucket = &buckets.at(min_bucket_index);

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        bucketArray local_buckets;
        while(min_bucket_index != INT_MAX){
            #pragma omp single nowait
            {
                prev_min_bucket_size = min_bucket->size();
            }
            #pragma omp for nowait schedule(dynamic, 64)
            for(int i=min_bucket_start; i<min_bucket->size(); i++){
                int source = min_bucket->at(i);
                relax_parallel(source, local_buckets);
            }

            bucket_fusion_parallel(local_buckets);
            #pragma omp barrier
            {
                copy_to_shared_buckets(buckets, local_buckets);
            }
            local_buckets.clear();
            #pragma omp barrier
            #pragma omp single nowait
            {
                auto update_bucket_size = min_bucket->size();
                if(prev_min_bucket_size == update_bucket_size){
                    min_bucket->clear();
                    min_bucket_index = INT_MAX;
                    for(int i=0; i<buckets.size(); i++){
                        if(!buckets.at(i).empty()){
                            min_bucket_index = i;
                            break;
                        }
                    }
                    if (min_bucket_index != INT_MAX){
                        min_bucket = &buckets.at(min_bucket_index);
                    }
                    min_bucket_start = 0;
                } else{
                    min_bucket_start = prev_min_bucket_size;
                }
            }
            #pragma omp barrier
        }
    }
    for(int i=0; i<sizeof(locks)/sizeof(locks[0]); i++){
        omp_destroy_lock(&locks[i]);
    }
}

void init_buckets(int source){
    int num_buckets = ceil(MAX_WEIGHT / delta) + 1;
    buckets.resize(num_buckets);
    dists.resize(N, INT_MAX);
    relax(source, 0);
    locks = new omp_lock_t[num_buckets];
    for (int i=0; i<num_buckets; i++) {
        omp_init_lock(&locks[i]);
    }
}

void relax(int w, int d){
    if(d < dists[w]){
        buckets[d/delta].push_back(w);
        dists[w] = d;
    }
}

void relax_parallel(int source, bucketArray& local_buckets){
    for(int j=0; j<N; j++){
        int weight = Graph[source][j];
        int &old_dist = dists[j];
        int new_dist = dists[source] + weight;
        if (new_dist < old_dist) {
            #pragma omp atomic write
            dists[j] = new_dist;
            auto bucket_index = new_dist / delta;
            if (bucket_index >= local_buckets.size()) {
                local_buckets.resize(bucket_index + 1);
            }
            local_buckets[bucket_index].push_back(j);
        }
    }
}

void bucket_fusion_parallel(bucketArray& local_buckets){
    if (!local_buckets.empty()){
        int i = 0;
        while(local_buckets[i].empty()){
            i++;
        }
        while(!local_buckets[i].empty() && local_buckets[i].size() < BUCKET_MIN_THRESHOLD){
            auto local_bucket_copy = local_buckets[i];
            local_buckets[i].resize(0);
            for (auto source: local_bucket_copy){
                relax_parallel(source, local_buckets);
            }
        }
    }
}

void copy_to_shared_buckets(bucketArray& shared_buckets, bucketArray& local_buckets){
    for (int i=0; i<local_buckets.size(); i++){
        if(!local_buckets[i].empty()){
            omp_set_lock(&locks[i]);
            bucketType &local_ref_bucket = local_buckets[i];
            bucketType &shared_ref_bucket = buckets[i];
            shared_ref_bucket.insert(shared_ref_bucket.end(), local_ref_bucket.begin(), local_ref_bucket.end());
            omp_unset_lock(&locks[i]);
        }
    }
}
