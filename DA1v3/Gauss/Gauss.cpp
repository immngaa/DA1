#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include <iomanip> 

using namespace std;
using namespace chrono;

void displayMatrix(const vector<vector<long double>>& matrix, ofstream& outFile) {
    for (const auto& row : matrix) {
        for (double value : row) {
            outFile << value << " ";
        }
        outFile << endl;
    }
}

long double calcDeterminant(vector<vector<long double>>& matrix, int n) {
    int swap_count = 0;
    if (n == 1) {
        return matrix[0][0];
    }

    for (int i = 0; i < n - 1; i++) {
        if (matrix[i][i] == 0) {
            bool swapped = false;
            for (int j = i + 1; j < n; j++) {
                if (matrix[j][i] != 0) {
                    swap(matrix[i], matrix[j]);
                    swap_count++;
                    swapped = true;
                    break;
                }
            }
            if (!swapped) return 0;  // Singular matrix
        }

        for (int j = i + 1; j < n; j++) {
            if (matrix[j][i] != 0) {
                long double factor = matrix[j][i] / matrix[i][i];
                for (int k = i; k < n; k++) {
                    matrix[j][k] -= factor * matrix[i][k];
                }
            }
        }
    }

    long double determinant = 1;
    for (int i = 0; i < n; i++) {
        determinant *= matrix[i][i];
    }

    if (swap_count % 2 != 0) {
        determinant = -determinant;
    }
    //displayMatrix(matrix, outFile); // Display the matrix after Gaussian elimination
    return determinant;
}

int main() {
    ifstream inFile("C:\\Users\\Admin\\OneDrive - Hanoi University of Science and Technology\\MI1 HUST\\DA1\\DA1v3\\input.txt");
    ofstream outFile("C:\\Users\\Admin\\OneDrive - Hanoi University of Science and Technology\\MI1 HUST\\DA1\\DA1v3\\output.txt");
    int n;
    inFile >> n;

    vector<vector<long double>> matrix(n, vector<long double>(n));

    // Reading matrix from input.txt
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inFile >> matrix[i][j];
        }
    }
    // Time measurement for multiple tests
    int num_tests = 10;
    double total_time = 0.0;
    long double final_determinant = 0.0;

    //outFile << "Parallel Run:" << endl;
    outFile << "Cỡ ma trận n = " << n << endl;

    for (int test = 1; test <= num_tests; test++) {
        auto start = chrono::high_resolution_clock::now();

        vector<vector<long double>> matrix_copy = matrix;  // Copy matrix for each test
        final_determinant = calcDeterminant(matrix_copy, n);

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double, std::milli> test_duration = end - start;
        total_time += test_duration.count(); // count() trả về số thực, đơn vị là ms


        outFile << "Test " << test << ": " << test_duration.count() << "ms" << endl;
    }

    double avg_time = total_time / num_tests;
    outFile << "Định thức: " << final_determinant << endl;
    outFile << "Logarit GTTĐ định thức: " << log(fabs(final_determinant))<< endl;
    outFile << "Thời gian trung bình: " << fixed << setprecision(4) << avg_time << " ms" << endl;
    //outFile << "Ma tran sau khi khu Gauss:" << endl;
    //displayMatrix(matrix, outFile);
    
    inFile.close();
    outFile.close();

    return 0;
}
