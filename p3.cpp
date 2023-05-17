#include <iostream>
#include <cmath>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <fstream>
#include <mpi.h>

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


int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int ip, np;
    MPI_Comm_rank(MPI_COMM_WORLD, &ip);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    double a = 1.0;
    double b = 6.0;
    double exact_length = 35.0 / 8.0 + log(6.0);
    int n = atoi(argv[1]);
    ofstream outfile("p3_results.txt");

    double local_a = a + ip * (b - a) / np;
    double local_b = a + (ip + 1) * (b - a) / np;
    int local_n = n / np;

    double start_time = MPI_Wtime();
    double local_sum = riemann_sum(local_a, local_b, local_n);
    double end_time = MPI_Wtime();

    double local_time = end_time - start_time;
    double max_time;
    MPI_Reduce(&local_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    double global_sum;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    if (ip == 0) {
        if  (n == 1e8) {
        cout << "Number of processes: " << np << ", Execution time: " << max_time << ", Length of the curve: " << global_sum << endl;
        outfile << np << ", " << max_time << endl;
        } else {
        double error = abs(exact_length - global_sum);
        cout << n << "," <<  error << endl;
        outfile << n << ", " << error << endl;
        }
    }
    MPI_Finalize();
    outfile.close();
    return 0;
}

/*
int main() {
    double a = 1.0;
    double b = 6.0;
    int num_threads = 6;
    ofstream outfile("error_vs_n.txt");

    // Approximate exact length of the curve
    int n_exact = 1e8;
    double exact_length = riemann_sum(a, b, n_exact);

    for (int i = 1; i <= 6; i++) {
        int n = pow(10, i);

        double result = parrallel_riemann_sum(a, b, n, num_threads);
        double error = abs(exact_length - result);

        outfile << n << ", " << error << endl;
    }
    
    outfile.close();
    return 0;
}
*/