#pragma GCC optimize(3)
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <climits>
#include <fstream>
#include <time.h>
#define N 10000
using namespace std;

void serial_dijkstra(std::vector< std::vector<int> > G, int source);

int main() {
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
    serial_dijkstra(Graph, 2);
    clock_t end = clock();
    cout << endl << "Serial Dijkstra, " << "time taken: " << (double)(end - start) / CLOCKS_PER_SEC << endl;
    return 0;
}

void serial_dijkstra(std::vector< std::vector<int> > G, int source) {
    std::vector<int> visited(N, 0);
    std::vector<int> distance(N, 0);
    int min_dist = INT_MAX;
    int min_vert = -1;
    for (int i=0; i<N; i++) {
        distance[i] = G[source][i];
    }
    visited[source] = 1;
    for (int step = 1; step < N; step++){
        min_dist = INT_MAX;
        min_vert = -1;
        for (int j = 0; j < N; j++) {
            if (visited[j] == 0 && distance[j] < min_dist) {
                min_dist = distance[j];
                min_vert = j;
            }
        }
        if (min_vert > -1) {
            visited[min_vert] = 1;
            for (int k = 0; k < N; k++){
                if (visited[k] == 0 && G[min_vert][k] < INT_MAX && distance[k] > distance[min_vert] + G[min_vert][k]) {
                    distance[k] = G[min_vert][k] + distance[min_vert];
                }
            }
        }     
    }
}

