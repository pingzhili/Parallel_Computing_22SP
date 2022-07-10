#include <iostream>
#include <cmath>
#include <ctime>
#define N 1000000
using namespace std;

static double random_double_in_range(double max) {
    return static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/max));
}

static double d2r(double d) {
  return d / 180.0 * ((double) M_PI);
}

bool random_toss_neddle(double l, double a, double b){
    double x = random_double_in_range(10000);
    double y = random_double_in_range(10000);
    double angle = random_double_in_range(360.0);
    double x_ = x + l * cos(d2r(angle));
    double y_ = y + l * sin(d2r(angle));
    return ( (int)(x/a) != (int)(x_/a) ) or ( (int)(y/b) != (int)(y_/b) );
}

int main(){
    double l = 1, a = 1, b = 1;
    double pi;
    int log_step = 100000;
    int total_count, edge_count = 0;
    auto start_time = clock();
    for(int i=0; i<N; i++){
        total_count += 1;
        if (random_toss_neddle(l, a, b)) {
            edge_count += 1;
        }
        pi = 3.0 * (double)total_count / (double)edge_count;
        if (i % log_step == 0) {
            cout << "Step " << i << ", estimated Pi: " << pi << endl;
        }
    }
    auto end_time = clock();
    cout << "runtime: " << (double)(end_time - start_time) / CLOCKS_PER_SEC << endl;
    return 0;
}