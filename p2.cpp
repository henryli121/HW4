#include <iostream>
#include <cmath>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <fstream>

using namespace std;

double f(double x) {
    return log(x) - pow(x, 2)/ 8;
}

double df (double x) {
    return 1/x - x/4;
}

double integrand(double x) {
    double dfx = df(x);
    return sqrt(1 + dfx * dfx);
}


double riemann_sum(double a, double b, int n) {
    double h = (b - a) / n;
    double sum = 0.0;
    
    for (int i = 0; i < n; i++) {
        sum += integrand(a + i*h); 
    }

    return sum * h;
}

void compute_partial_sum(double &sum, mutex &sum_mutex, double a, double h, int n, int num_threads, int i) {
    double partial_sum = 0.0;
    int start = (n / num_threads) * i;
    int end = (i == num_threads - 1) ? n : (n / num_threads) * (i + 1);
    for (int j = start; j < end; j++) {
        double x = a + j * h;
        partial_sum += integrand(x);
    }
    partial_sum *= h;

    sum_mutex.lock();
    sum += partial_sum;
    sum_mutex.unlock();
}

double parrallel_riemann_sum(double a, double b, int n, int num_threads) {
    double h = (b - a) / n;
    double sum = 0.0;
    vector<thread> threads(num_threads);
    mutex sum_mutex;

    for (int i = 0; i < num_threads; i++) {
        threads[i] = thread(compute_partial_sum, ref(sum), ref(sum_mutex), a, h, n, num_threads, i);
    };

    for (int i = 0; i < num_threads; i++) {
        threads[i].join();
    }

    return sum;
}

int main(int argc, char **argv) {
    double a = 1;
    double b = 6;
    double exact_length = 35.0 / 8.0 + log(6.0);
    int n = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    auto start_time = chrono::high_resolution_clock::now();

    double result = parrallel_riemann_sum(a, b, n, num_threads);
        
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
    
    double error = abs(exact_length - result);

    if (n == 1e8) {
        cout << "Number of threads: " << num_threads << ", Length of the curve: " << result 
    << ", Time taken: " << duration << " milliseconds" << endl;
    } else {
    cout << n << "," << setprecision(19) << error << endl;
    }
return 0;
}