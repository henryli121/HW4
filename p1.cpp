#include <iostream>
#include <vector>
#include <random>
#include <fstream>

using namespace std;
random_device rd;
mt19937 gen(rd());
uniform_real_distribution<double> dist(0.0, 1.0);

// Create the matrix and fill it with random values
vector<double> rand_matrix(int n) {
    vector<double> matrix(n * n);
    for (int i = 0; i < n * n; i++) {
        matrix[i] = dist(gen);
    }
    return matrix;
}

void WriteMatrix(const vector<double>& matrix, int n, const string& fileName) {
    ofstream outFile(fileName, ios::out | ios::binary);
    if (outFile.is_open()) {
        outFile.write(reinterpret_cast<const char*>(matrix.data()), sizeof(double) * n * n);
        outFile.close();
    } else {
        cerr << "Failed to open file: " << fileName << endl;
        exit(1);
    }
}

vector<double> ReadMatrix(int n, const string& fileName) {
    vector<double> matrix(n * n);
    ifstream inFile(fileName, ios::binary);
    if (inFile) {
        inFile.read(reinterpret_cast<char*>(matrix.data()), sizeof(double) * matrix.size());
        if (!inFile) {
            cerr << "Error: could not read file" << endl;
            exit(1);
        }
    } else {
        cerr << "Error: could not open file for reading" << endl;
        exit(1);
    }
    return matrix;
}

int main() {
    ofstream writeOutputFile("write_speed_vs_dimension.txt");
    ofstream readOutputFile("read_speed_vs_dimension.txt");

    for (int n = 32; n <= 16384; n *= 2) {
        vector<double> matrix = rand_matrix(n);
        string fileName = "matrix_" + to_string(n) + ".bin";

        auto startWrite = chrono::high_resolution_clock::now();
        WriteMatrix(matrix, n, fileName);
        auto endWrite = chrono::high_resolution_clock::now();
        chrono::duration<double> writeDuration = endWrite - startWrite;

        auto startRead = chrono::high_resolution_clock::now();
        vector<double> readMatrix = ReadMatrix(n, fileName);
        auto endRead = chrono::high_resolution_clock::now();
        chrono::duration<double> readDuration = endRead - startRead;

        double writeBandwidth = static_cast<double>(sizeof(double) * n * n) / writeDuration.count();
        double readBandwidth = static_cast<double>(sizeof(double) * n * n) / readDuration.count();

        writeOutputFile << n << ", " << writeBandwidth << endl;
        readOutputFile << n << ", " << readBandwidth << endl;
    }

    writeOutputFile.close();
    readOutputFile.close();

    cout << "Write speed results written to 'write_speed_vs_dimension.txt'" << endl;
    cout << "Read speed results written to 'read_speed_vs_dimension.txt'" << endl;

    return 0;
}
