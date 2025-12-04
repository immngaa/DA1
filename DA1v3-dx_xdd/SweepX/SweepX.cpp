#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <stdexcept>
#include <numeric>

// Sử dụng namespace std
using namespace std;

// Định nghĩa kiểu dữ liệu Matrix
using Matrix = vector<vector<double>>;

// Ngưỡng nhỏ để kiểm tra giá trị gần bằng 0
const double epsilon = 1e-12;

// --- Hàm phụ trợ để in ma trận ra màn hình ---
void printMatrix(const Matrix& matrix, const string& title = "Matrix") {
    if (matrix.empty()) {
        cout << title << " is empty." << endl;
        return;
    }
    cout << title << ":" << endl;
    size_t rows = matrix.size();
    size_t cols = matrix[0].size();
    //cout << fixed << setprecision(4); // Định dạng output cho màn hình
    for (size_t i = 0; i < rows; ++i) {
        //cout << "[ ";
        for (size_t j = 0; j < cols; ++j) {
            cout << setw(10) << matrix[i][j] << " ";
        }
        cout << " " << endl;
    }
    cout << endl;
}


// --- Hàm chuyển vị ma trận ---
Matrix transposeMatrix(const Matrix& X) {
    if (X.empty()) {
        return Matrix();
    }
    size_t m = X.size();
    size_t n = X[0].size();
    Matrix Xt(n, vector<double>(m));
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            Xt[j][i] = X[i][j];
        }
    }
    return Xt;
}

// --- Hàm nhân hai ma trận ---
Matrix multiplyMatrices(const Matrix& A, const Matrix& B) {
    if (A.empty() || B.empty()) {
         throw runtime_error("Cannot multiply empty matrices.");
    }
    size_t rowsA = A.size();
    size_t colsA = A[0].size();
    size_t rowsB = B.size();
    size_t colsB = B[0].size();

    if (colsA != rowsB) {
        throw runtime_error("Matrix dimensions mismatch for multiplication.");
    }

    Matrix C(rowsA, vector<double>(colsB, 0.0));
    for (size_t i = 0; i < rowsA; ++i) {
        for (size_t j = 0; j < colsB; ++j) {
            for (size_t k = 0; k < colsA; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

     // --- Ghi ma trận C vào file input.txt ---
    const string filepath =
      R"(C:\Users\Admin\OneDrive - Hanoi University of Science and Technology\MI1 HUST\DA1\DA1v3-dx_xdd\input.txt)";
    ofstream out(filepath);
    if (!out) {
        cerr << "Không thể mở file để ghi: " << filepath << endl;
    } else {
        // Ghi kích thước ma trận
        out << rowsA << '\n';
        // Ghi từng hàng của C
        for (size_t i = 0; i < rowsA; ++i) {
            for (size_t j = 0; j < colsB; ++j) {
                out << C[i][j];
                if (j + 1 < colsB) out << ' ';
            }
            out << '\n';
        }
        out.close();
    }
    // --- Kết thúc ghi file ---
    return C;
}


// --- Hàm tính định thức THEO ĐÚNG THUẬT TOÁN SWEEP ---
// Lưu ý: Hàm này sẽ thay đổi ma trận đầu vào 'A' theo đúng các bước SWEEP
double calculateDeterminantStrictSweep(Matrix& A) {
    if (A.empty() || A.size() != A[0].size()) {
        throw runtime_error("Determinant calculation requires a square matrix.");
    }

    int n = A.size();
    double determinant = 1.0;

    // Vector để lưu trữ các giá trị a_ik gốc trước khi hàng k bị thay đổi
    vector<double> original_B_values(n);

    for (int k = 0; k < n; ++k) {
        // Bước 1: Đặt D = a_kk (Lấy pivot TRƯỚC khi biến đổi)
        double D = A[k][k];

        // Kiểm tra suy biến
        if (fabs(D) < epsilon) {
            // Nếu pivot bằng 0, ma trận suy biến, định thức bằng 0
            // Không cần tiếp tục, vì không thể chia cho D
            return 0.0;
        }
         // (Tùy chọn) Kiểm tra PSD: Nếu D < 0, có vấn đề (hoặc số học hoặc đầu vào)
         // if (D < 0) {
         //     cerr << "Warning: Negative pivot D=" << D << " at step k=" << k << ". Input might not be PSD or numerical issues." << endl;
             // Có thể chọn dừng hoặc tiếp tục tùy vào yêu cầu xử lý lỗi
         // }

        // Cập nhật định thức bằng giá trị pivot D *trước khi* biến đổi
        determinant *= D;

        // --- Thực hiện các bước biến đổi của SWEEP(k) ---

        // Lưu các giá trị B = a_ik gốc (cho i != k) trước khi hàng k thay đổi
        for(int i = 0; i < n; ++i) {
            if (i != k) {
                original_B_values[i] = A[i][k];
            }
        }

        // Bước 2: Chia hàng k cho D
        // Lưu ý: Bước này phải được thực hiện TRƯỚC bước 3.1
        for (int j = 0; j < n; ++j) {
            A[k][j] /= D;
        }
        // Sau bước này, A[k][k] tạm thời bằng 1

        // Bước 3: Với hàng i != k
        for (int i = 0; i < n; ++i) {
            if (i == k) continue; // Bỏ qua hàng pivot

            // Lấy giá trị B = a_ik gốc đã lưu
            double B = original_B_values[i];

            // Bước 3.1: A_i <- A_i - B * A_k (sử dụng hàng k đã được chia cho D)
            // Cập nhật toàn bộ hàng i
            for (int j = 0; j < n; ++j) {
                 A[i][j] = A[i][j] - B * A[k][j];
            }

            // Bước 3.2: Đặt a_ik = -B / D
            // Cập nhật phần tử trong cột pivot của hàng i
            A[i][k] = -B / D;
        }

        // Bước 4: Đặt a_kk = 1 / D
        // Cập nhật phần tử pivot gốc
        A[k][k] = 1.0 / D;

        // Optional: In ma trận sau mỗi bước SWEEP để kiểm tra
        // string title = "Matrix after SWEEP(" + to_string(k) + ")";
        // printMatrix(A, title);

    } // Kết thúc vòng lặp k

    // Định thức là tích các giá trị D đã được tích lũy
    return determinant;
}


int main() {
    // --- Đọc file Input ---
    ifstream inputFile("inputX.txt");
    if (!inputFile.is_open()) {
        cerr << "Error: Could not open input.txt" << endl;
        return 1;
    }

    int m, n;
    if (!(inputFile >> m >> n)) {
        cerr << "Error: Could not read matrix dimensions m and n." << endl;
        inputFile.close();
        return 1;
    }

    if (m <= 0 || n <= 0) {
         cerr << "Error: Invalid matrix dimensions m=" << m << ", n=" << n << endl;
         inputFile.close();
         return 1;
    }

    Matrix matrixX(m, vector<double>(n));

    cout << "Reading matrix X (" << m << "x" << n << ") from input.txt..." << endl;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (!(inputFile >> matrixX[i][j])) {
                 cerr << "Error: Failed to read matrix element X[" << i << "][" << j << "]" << endl;
                 inputFile.close();
                 return 1;
            }
        }
    }
    inputFile.close();
    cout << "Finished reading matrix X." << endl;

    // --- Tính A = X^T * X ---
    cout << "Calculating A = X^T * X..." << endl;
    Matrix matrixXt, matrixA;
    try {
        matrixXt = transposeMatrix(matrixX);
        matrixA = multiplyMatrices(matrixXt, matrixX);
    } catch (const runtime_error& e) {
        cerr << "Error during matrix calculation: " << e.what() << endl;
        return 1;
    }
    cout << "Finished calculating A. Matrix A is " << matrixA.size() << "x" << (matrixA.empty() ? 0 : matrixA[0].size()) << "." << endl;

    // --- In ma trận A ra màn hình ---
    printMatrix(matrixA, "Calculated Matrix A = X^T * X");

    // --- Mở file Output ---
    ofstream outputFile("output.txt");
     if (!outputFile.is_open()) {
        cerr << "Error: Could not open output.txt" << endl;
        return 1;
    }

    // --- Ghi cỡ ma trận A ---
    int sizeA = matrixA.size();
    outputFile << "Cỡ ma trận A (X^T * X): " << sizeA << "x" << sizeA << endl << endl;
    outputFile << fixed << setprecision(6);

    // --- Thực hiện tính toán định thức và đo thời gian 10 lần ---
    vector<double> timings_ms;
    double determinant_value = 0.0;

    cout << "Running timing tests for determinant calculation (using strict SWEEP)..." << endl;

    for (int test = 1; test <= 10; ++test) {
        Matrix currentA = matrixA; // Tạo bản sao cho mỗi lần test
        auto start_time = chrono::high_resolution_clock::now();
        double current_det = 0.0;
        try {
             // Gọi hàm tính định thức theo đúng SWEEP
             current_det = calculateDeterminantStrictSweep(currentA);
        } catch (const runtime_error& e) {
             cerr << "Error during determinant calculation: " << e.what() << endl;
             outputFile.close();
             return 1;
        }
        auto end_time = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration = end_time - start_time;
        timings_ms.push_back(duration.count());
        outputFile << "Test " << setw(2) << test << ": " << duration.count() << " ms" << endl;
        if (test == 1) {
            determinant_value = current_det;
        }
    }
     cout << "Timing tests finished." << endl;

    // --- Tính thời gian trung bình ---
    double average_time_ms = 0.0;
    if (!timings_ms.empty()) {
        average_time_ms = accumulate(timings_ms.begin(), timings_ms.end(), 0.0) / timings_ms.size();
    }

    // --- Ghi kết quả cuối cùng ---
    outputFile << endl;
    outputFile << "Giá trị định thức của A (det(X^T * X)): " << fixed << setprecision(10) << determinant_value << endl;
    outputFile << "Thời gian trung bình tính định thức: " << fixed << setprecision(6) << average_time_ms << " ms" << endl;

    outputFile.close();

    cout << "Results written to output.txt" << endl;

    return 0;
}