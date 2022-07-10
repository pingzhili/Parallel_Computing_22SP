#pragma GCC optimize(3)
#include <iostream>
#include <omp.h>
#include <stdlib.h>
#include <vector>
#include <climits>
#include <fstream>
#include <time.h>
#define N 10000
using namespace std;

void parallel_dijkstra(std::vector< std::vector<int> > G, int source);

int main() {
    int num_threads = 4;
    omp_set_num_threads(num_threads);
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
    clock_t start = clock();
    parallel_dijkstra(Graph, 2);
    clock_t end = clock();
    cout << endl << num_threads << " threads parallelized Dijkstra, " << "time taken: " << (double)(end - start) / CLOCKS_PER_SEC << endl;
    return 0;
}

void parallel_dijkstra(std::vector< std::vector<int> > G, int source) {
    /* threads shared variables */
    std::vector<int> visited(N, 0);
    std::vector<int> distance(N, 0);
    int min_dist;
    int min_vert;

    /* threads local varaibles*/
    int thread_id;
    int thread_first;
    int thread_last;
    int thread_min_dist;
    int thread_min_vert;
    int thread_step;
    int num_threads;

    int i;
    for (i=0; i<N; i++) {
        distance[i] = G[source][i];
        visited[i] = 0;
    }
    visited[source] = 1;
    # pragma omp parallel private (thread_first, thread_id, thread_last, thread_min_dist, thread_min_vert, thread_step) shared (visited, min_dist, min_vert, distance, G) 
    {
        num_threads = omp_get_num_threads();
        thread_id = omp_get_thread_num();
        thread_first = thread_id * N /num_threads;
        thread_last = (thread_id + 1) * N / num_threads - 1;
        for (thread_step = 1; thread_step < N; thread_step++){
            # pragma omp single 
            {
                min_dist = INT_MAX;
                min_vert = -1;
            }
            int j;
            thread_min_dist = INT_MAX;
            thread_min_vert = -1;
            for (j = thread_first; j <= thread_last; j++) {
                if (visited[j] == 0 && distance[j] < thread_min_dist) {
                    thread_min_dist = distance[j];
                    thread_min_vert = j;
                }
            }
            # pragma omp critical 
            {
                if (thread_min_dist < min_dist) {
                    min_dist = thread_min_dist;
                    min_vert = thread_min_vert;
                }
            }
            #pragma omp barrier // sync threads
            #pragma omp single 
            {
                if (min_vert != -1) {
                    visited[min_vert] = 1;
                }
            }
            if (min_vert != -1){
                for (int k = thread_first; k<= thread_last; k++){
                    if (visited[k] == 0 && G[min_vert][k] < INT_MAX && distance[k] > distance[min_vert] + G[min_vert][k]) {
                        distance[k] = G[min_vert][k] + distance[min_vert];
                    }
                }
            }
            #pragma omp barrier // sync threads
        }
    }
    // cout << "The distance vector is" << endl;
    // for (i=0; i<N; i++) {
    //     cout << distance[i] << " ";
    // }
}
