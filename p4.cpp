#include <chrono>
#include <mpi.h>
#include <fstream>
#include <iostream>

using namespace std;

template <typename T>
void my_broadcast(T* data, int count, int root, MPI_Comm comm) {
    int rank, size;
    int tag = 0;
    MPI_Status status;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (rank == root) {
        for (int i = 0; i < size; i++) {
            if (i != root) {
                MPI_Send(data, count, MPI_BYTE, i, tag, comm);
            }
        }
    } else {
        MPI_Recv(data, count, MPI_BYTE, root, tag, comm, &status);
    }
} 

int main(int argc, char** argv) {
    int rank, size_;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size_);

    const int root = 0;
    MPI_Comm comm = MPI_COMM_WORLD;
    

    for (long long size = 8; size <= 128LL * 1024 * 1024; size *= 2) {
        char* data = new char[size];

        // Measure time for my_broadcast
        auto start = chrono::high_resolution_clock::now();
        my_broadcast(data, size, root, comm);
        auto end = chrono::high_resolution_clock::now();
        auto my_bcast_time = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        long double my_broadcast_bw = static_cast<long double> (size) / my_bcast_time;

        // Measure time for MPI_Bcast
        start = chrono::high_resolution_clock::now();
        MPI_Bcast(data, size, MPI_BYTE, root, comm);
        end = chrono::high_resolution_clock::now();
        auto mpi_bcast_time = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        long double MPI_Bcast_bw = static_cast<long double> (size) / mpi_bcast_time;
        MPI_Barrier(MPI_COMM_WORLD);
        delete[] data;
        // Output the results
        /*
        cout << "Message size: " << size << " bytes\n";
        cout << "my_broadcast Bandwidth: " << my_broadcast_bw *1e6 << " bytes per sec\n";
        cout << "MPI_Bcast Bandwidth: " << MPI_Bcast_bw * 1e6  << " bytes per sec\n\n";
        */

        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) {
        ofstream outfile1("p4_my_broadcast_results.txt", ios_base::app);
        outfile1 << "Message size: " << size << ", my_broadcast Bandwidth: " << my_broadcast_bw *1.e9 << endl;
        outfile1.close();
        ofstream outfile2("p4_MPI_Bcast_results.txt", ios_base::app);
        outfile2 << "Message size: " << size << ", MPI_Bcast Bandwidth: " << MPI_Bcast_bw * 1.e9 << endl;
        outfile2.close();
        }

    }

    MPI_Finalize();
    return 0;
}