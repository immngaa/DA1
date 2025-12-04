#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include <chrono>
#include <cmath>

using namespace std;
using namespace std::chrono;

// Hàm hoán đổi hàng và đếm số lần hoán đổi
void swapRows(vector<vector<long double>>& matrix, int row1, int row2, int n) {
    for (int i = 0; i < n; i++) {
        swap(matrix[row1][i], matrix[row2][i]);
    }
}

// Hàm phân rã LU với hoán đổi hàng và tính số lần hoán đổi
void luDecompositionWithPivoting(vector<vector<long double>>& A, vector<vector<long double>>& L, vector<vector<long double>>& U, int n, int& swapCount) {
    // Khởi tạo đường chéo chính của L bằng 1
    for (int i = 0; i < n; i++) {
        L[i][i] = 1;
    }

    swapCount = 0;  // Khởi tạo số lần hoán đổi hàng

    for (int j = 0; j < n; j++) {
        // Kiểm tra xem phần tử trên đường chéo chính có phải là 0 không
        if (A[j][j] == 0) {
            // Nếu là 0, hoán đổi với một hàng khác có phần tử không bằng 0 ở cột j
            bool swapped = false;
            for (int k = j + 1; k < n; k++) {
                if (A[k][j] != 0) {
                    swapRows(A, j, k, n);  // Hoán đổi hàng trong ma trận A
                    //swapRows(L, j, k, n);   // Hoán đổi trong ma trận L
                    swapped = true;
                    swapCount++;  // Tăng số lần hoán đổi
                    break;
                }
            }
            if (!swapped) {
                throw runtime_error("Không thể phân rã LU, ma trận không khả nghịch.");
            }
        }

        // Tính các phần tử của U
        for (int i = 0; i <= j; i++) {
            long double sum = 0;
            for (int k = 0; k < i; k++) {
                sum += U[k][j] * L[i][k];
            }
            U[i][j] = A[i][j] - sum;
        }

        // Tính các phần tử của L
        for (int i = j + 1; i < n; i++) {
            long double sum = 0;
            for (int k = 0; k < j; k++) {
                sum += U[k][j] * L[i][k];
            }
            L[i][j] = (A[i][j] - sum) / U[j][j];
        }
    }
}

// Hàm tính định thức từ ma trận U và số lần hoán đổi
double determinant(const vector<vector<long double>>& U, int n, int swapCount) {
    long double log_det = 0.0;
    int sign = (swapCount % 2 == 0) ? 1 : -1;

    for (int i = 0; i < n; i++) {
        if (U[i][i] == 0.0) {
            return 0.0; // Ma trận suy biến → định thức bằng 0
        }
        if (U[i][i] < 0.0) {
            sign *= -1; // Cập nhật dấu nếu phần tử chéo âm
        }
        log_det += logl(fabsl(U[i][i]));
    }

    //return sign * expl(log_det);
    return log_det;
    /*
    long double det = 1;
    for (int i = 0; i < n; i++) {
        det *= U[i][i]; // Nhân các phần tử trên đường chéo chính của U
    }

    // Nếu số lần hoán đổi lẻ, nhân định thức với -1
    if (swapCount % 2 != 0) {
        det = -det;
    }

    return det;*/
    
}

// Hàm đọc ma trận từ file
vector<vector<long double>> readMatrixFromFile(const string& filename, int& n) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Không thể mở file.");
    }
    file >> n;
    vector<vector<long double>> matrix(n, vector<long double>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            file >> matrix[i][j];
        }
    }
    return matrix;
}

// Hàm ghi kết quả vào file
/*void writeResultsToFile(const string& filename, const vector<vector<long double>>& L, const vector<vector<long double>>& U, double det, int n) {*/
void writeResultsToFile(const string& filename, long double det, double n) {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Không thể mở file để ghi.");
    }

    file << fixed << setprecision(4);
    /*
    // In ma trận L
    file << "Ma trận L:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            file << L[i][j] << " ";
        }
        file << "\n";
    }

    // In ma trận U
    file << "\nMa trận U:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            file << U[i][j] << " ";
        }
        file << "\n";
    }
    */
    // In định thức
    file << "\nĐịnh thức: " << det << "\n";
    file <<"Thời gian TB:" << n <<endl;
    // Kiểm tra nếu file đã ghi thành công
    if (file.good()) {
        cout << "Ket qua duoc ghi vao file: " << filename << endl;
    } else {
        cerr << "Có lỗi khi ghi kết quả vào file." << endl;
    }
}

int main() {
    string inputFile = "C:\\Users\\Admin\\OneDrive - Hanoi University of Science and Technology\\MI1 HUST\\DA1\\DA1v3\\input.txt";
    string outputFile = "C:\\Users\\Admin\\OneDrive - Hanoi University of Science and Technology\\MI1 HUST\\DA1\\DA1v3\\output.txt"; // Đảm bảo rằng tên file là đúng
    int n;
    ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        cerr << "Không thể mở file để ghi." << endl;
        return 1;
    }
    try {
        // Đo thời gian bắt đầu
        auto start = high_resolution_clock::now();

        // Đọc ma trận từ file
        vector<vector<long double>> A = readMatrixFromFile(inputFile, n);
        vector<vector<long double>> A_copy = A;
        // Khởi tạo ma trận L và U với kích thước n x n
        vector<vector<long double>> L(n, vector<long double>(n, 0));
        vector<vector<long double>> U(n, vector<long double>(n, 0));

        int swapCount = 0;  // Biến đếm số lần hoán đổi hàng

        // Thực hiện nhiều thử nghiệm (10 lần) để tính thời gian trung bình
        int num_tests = 10;
        double total_time = 0.0;
        long double final_determinant = 0.0;

        // Thực hiện kiểm tra nhiều lần
        for (int test = 1; test <= num_tests; test++) {
            auto bdau = high_resolution_clock::now();
            // Phân rã LU với hoán đổi hàng
            luDecompositionWithPivoting(A_copy, L, U, n, swapCount);

            // Tính định thức
            long double det = determinant(U, n, swapCount);
            final_determinant = det;
            auto ketthuc = high_resolution_clock::now();

            chrono::duration<double, std::milli>duration_LU =(ketthuc - bdau);
            total_time += duration_LU.count();
            cout << "Test " << test << ": " << duration_LU.count() << " ms." << endl;
        }
        cout << "logarit_dinh thuc: "<<final_determinant;
        // Tính thời gian trung bình
        double avg_time = total_time / num_tests;
        cout << "\n Thoi gian trung binh: " << fixed << setprecision(4) << avg_time << " ms." << endl;

        // Ghi kết quả vào file
        //writeResultsToFile(outputFile, final_determinant, avg_time);

        // Đo thời gian kết thúc
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start); // Thời gian thực hiện

        //cout << "Ket qua duoc ghi vao file. " << endl;
        //cout << "Thoi gian thuc hien: " << duration.count() << " milliseconds." << endl;
    } catch (const exception& e) {
        cerr << "Lỗi: " << e.what() << endl;
        return 1;
    }

    return 0;
}
