#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <omp.h>

using namespace std;
using namespace chrono;

void luDecomposition(vector<vector<long double>>& matrix, int n, int& sign, vector<int>& permVector, long double& determinant) {
    sign = 1;
    
    for (int j = 0; j < n; j++) {
        // Tìm pivot (tuần tự để tránh contention)
        int pivot = j;
        long double maxVal = fabsl(matrix[j][j]);
        for (int i = j + 1; i < n; i++) {
            long double ai = fabsl(matrix[i][j]);
            if (ai > maxVal) {
                maxVal = ai;
                pivot = i;
            }
        }

        if (maxVal == 0) {
            throw runtime_error("Ma trận không khả nghịch, pivot bằng 0.");
        }

        // Hoán đổi hàng
        if (pivot != j) {
            swap(matrix[pivot], matrix[j]);
            swap(permVector[pivot], permVector[j]);
            sign *= -1;
        }

        // Song song hóa tính toán matrix[i][j]
        #pragma omp parallel for if(n > 100) // Chỉ song song nếu n lớn
        for (int i = j + 1; i < n; i++) {
            matrix[i][j] /= matrix[j][j];
        }

        // Song song hóa vòng lặp cập nhật ma trận
        #pragma omp parallel for if(n > 100) collapse(2) schedule(static)
        for (int i = j + 1; i < n; i++) {
            for (int k = j + 1; k < n; k++) {
                matrix[i][k] -= matrix[i][j] * matrix[j][k];
            }
        }
    }

    // Tính định thức sử dụng logarit
    long double log_det = 0.0;
    int local_sign = sign;
    for (int i = 0; i < n; i++) {
        if (matrix[i][i] == 0) {
            determinant = 0.0;
            return;
        }
        if (matrix[i][i] < 0) {
            local_sign *= -1;
        }
        log_det += logl(fabsl(matrix[i][i]));
    }
    determinant = local_sign * expl(log_det);
}

// Hàm in ma trận ra file
void printMatrixToFile(const vector<vector<long double>>& matrix, int n, const string& name, ofstream& outFile) {
    outFile << name << " Matrix:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            outFile << fixed << setprecision(4) << matrix[i][j] << " ";
        }
        outFile << "\n";
    }
}

// Hàm nhập ma trận từ file
void readMatrixFromFile(const string& filename, vector<vector<long double>>& matrix, int& n) {
    ifstream inFile(filename);
    if (!inFile) {
        throw runtime_error("Không thể mở tệp " + filename);
    }
    inFile >> n;
    matrix.resize(n, vector<long double>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inFile >> matrix[i][j];
        }
    }
    inFile.close();
}

// Hàm lưu kết quả ra file
void writeResultToFile(const string& filename, const vector<vector<long double>>& L, const vector<vector<long double>>& U, long double determinant, double avg_time) {
    ofstream outFile(filename);
    if (!outFile) {
        throw runtime_error("Không thể mở tệp " + filename);
    }
    printMatrixToFile(L, L.size(), "L", outFile);
    printMatrixToFile(U, U.size(), "U", outFile);
    outFile << "Dinh thuc: " << fixed << setprecision(4) << determinant << "\n";
    outFile << "Thời gian trung bình: " << fixed << setprecision(4) << avg_time << " ms\n";
    outFile.close();
}

int main() {
    omp_set_num_threads(omp_get_max_threads()); // Sử dụng tất cả luồng có sẵn

    int n;
    vector<vector<long double>> matrix;
    readMatrixFromFile("C:\\Users\\Admin\\OneDrive - Hanoi University of Science and Technology\\MI1 HUST\\DA1\\DA1v3-dx_xdd\\input.txt", matrix, n); // Thay đường dẫn nếu cần

    vector<vector<long double>> L(n, vector<long double>(n, 0));
    vector<vector<long double>> U(n, vector<long double>(n, 0));
    vector<int> permVector(n);
    for (int i = 0; i < n; i++) {
        permVector[i] = i;
    }

    double total_time = 0.0;
    int num_tests = 10;
    long double determinant = 0;

    try {
        // Thực hiện nhiều thử nghiệm
        for (int test = 1; test <= num_tests; test++) {
            // Sao chép ma trận gốc
            vector<vector<long double>> matrix_copy = matrix;
            int sign = 1;

            auto start = high_resolution_clock::now();
            luDecomposition(matrix_copy, n, sign, permVector, determinant);
            auto end = high_resolution_clock::now();

            chrono::duration<double, std::milli> duration =(end - start);
            total_time += duration.count(); // count() trả về số thực, đơn vị là ms
            cout << "Test " << test << " : " << fixed << setprecision(4) << duration.count() << " ms " << endl;
        }
        cout << "Dinh thuc: " <<determinant <<endl;
        // Tính thời gian trung bình
        double avg_time = total_time / num_tests;
        cout << "Thoi gian trung binh: " << fixed << setprecision(4) << avg_time << " ms\n";

        // Tách L và U từ ma trận cuối cùng
        vector<vector<long double>> matrix_copy = matrix;
        int sign = 1;
        luDecomposition(matrix_copy, n, sign, permVector, determinant);

        #pragma omp parallel for if(n > 100) collapse(2)
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i > j) {
                    L[i][j] = matrix_copy[i][j];
                    U[i][j] = 0;
                } else if (i == j) {
                    L[i][j] = 1;
                    U[i][j] = matrix_copy[i][j];
                } else {
                    L[i][j] = 0;
                    U[i][j] = matrix_copy[i][j];
                }
            }
        }

        // Lưu kết quả
        //writeResultToFile("C:\\Users\\Admin\\OneDrive - Hanoi University of Science and Technology\\MI1 HUST\\DA1\\DA1v3\\output.txt", L, U, determinant, avg_time);
        //cout << "Kết quả đã lưu vào output.txt\n";
    } catch (const exception& e) {
        cerr << "Lỗi: " << e.what() << endl;
        return 1;
    }

    return 0;
}
//g++ -fopenmp OpenMP.cpp -o OpenMP
// ./OpenMP
// file cuối