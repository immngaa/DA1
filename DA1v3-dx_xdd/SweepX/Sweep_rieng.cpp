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
            cout << setw(10) << matrix[i][j] <<  " ";
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
    return C;
}

// --- Hàm thực hiện toán tử SWEEP(k) ---
// Biến đổi trực tiếp ma trận A
void sweep_operator(Matrix& A, int k) {
     if (A.empty() || A.size() != A[0].size()) {
        throw runtime_error("SWEEP operator requires a square matrix.");
    }
    int n = A.size();
    if (k < 0 || k >= n) {
        throw out_of_range("SWEEP index k is out of bounds.");
    }

    // Bước 1: Lấy pivot D = a_kk
    double D = A[k][k];

    // Kiểm tra pivot gần bằng 0
    if (fabs(D) < epsilon) {
        // Không thể thực hiện SWEEP nếu pivot bằng 0.
        // Trong ngữ cảnh tính định thức, điều này có nghĩa det=0.
        // Ở đây, vì hàm này chỉ thực hiện phép biến đổi, ta nên báo lỗi.
        throw runtime_error("Pivot element is zero or too small in SWEEP operator at k=" + to_string(k));
    }

    // Lưu các giá trị B = a_ik gốc (cho i != k) trước khi hàng k thay đổi
    vector<double> original_B_values(n);
    for(int i = 0; i < n; ++i) {
        if (i != k) {
            original_B_values[i] = A[i][k];
        }
    }

    // Bước 2: Chia hàng k cho D
    for (int j = 0; j < n; ++j) {
        A[k][j] /= D;
    }
    // A[k][k] tạm thời bằng 1

    // Bước 3: Với hàng i != k
    for (int i = 0; i < n; ++i) {
        if (i == k) continue;

        double B = original_B_values[i]; // Lấy giá trị a_ik gốc

        // Bước 3.1: A_i <- A_i - B * A_k (sử dụng hàng k đã chia)
        for (int j = 0; j < n; ++j) {
             A[i][j] = A[i][j] - B * A[k][j];
        }

        // Bước 3.2: Đặt a_ik = -B / D
        A[i][k] = -B / D;
    }

    // Bước 4: Đặt a_kk = 1 / D
    A[k][k] = 1.0 / D;
}


// --- Hàm tính định thức bằng cách lấy tích các pivot của SWEEP ---
// KHÔNG thay đổi ma trận gốc originalA
double determinant_from_sweep_pivots(const Matrix& originalA) {
    if (originalA.empty() || originalA.size() != originalA[0].size()) {
        throw runtime_error("Determinant calculation requires a square matrix.");
    }

    // Tạo bản sao để thực hiện các phép biến đổi mô phỏng
    Matrix A = originalA;
    int n = A.size();
    double determinant = 1.0;

    // Vector tạm để lưu giá trị B trong quá trình mô phỏng
    vector<double> temp_B_values(n);

    for (int k = 0; k < n; ++k) {
        // Lấy pivot D = a_kk từ ma trận *đang được biến đổi* A
        double D = A[k][k];

        // Kiểm tra suy biến
        if (fabs(D) < epsilon) {
            return 0.0; // Định thức bằng 0
        }

        // Nhân pivot vào định thức
        determinant *= D;

        // *** Mô phỏng các bước biến đổi của SWEEP(k) trên bản sao A ***
        // (Cần làm điều này để có được pivot chính xác ở bước k+1)

        // Lưu giá trị B = a_ik gốc (của bản sao A tại bước này)
        for(int i = 0; i < n; ++i) {
            if (i != k) {
                temp_B_values[i] = A[i][k];
            }
        }

        // Mô phỏng Bước 2: Chia hàng k cho D
        for (int j = 0; j < n; ++j) {
            A[k][j] /= D;
        }

        // Mô phỏng Bước 3: Với hàng i != k
        for (int i = 0; i < n; ++i) {
            if (i == k) continue;
            double B = temp_B_values[i];
            // Mô phỏng Bước 3.1
            for (int j = 0; j < n; ++j) {
                 A[i][j] = A[i][j] - B * A[k][j];
            }
            // Mô phỏng Bước 3.2
            A[i][k] = -B / D;
        }
        // Mô phỏng Bước 4
        A[k][k] = 1.0 / D;
    }

    return determinant;
}


int main() {
    const string fileinputX = R"(C:\Users\Admin\OneDrive - Hanoi University of Science and Technology\MI1 HUST\DA1\A1v3-dx_xdd\SweepX\inputX.txt)";

    // --- Đọc file Input ---
    ifstream inputFile(fileinputX);
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

    // Tính định thức một lần để lấy giá trị
    try {
         determinant_value = determinant_from_sweep_pivots(matrixA);
    } catch (const runtime_error& e) {
        cerr << "Error calculating determinant: " << e.what() << endl;
        outputFile.close();
        return 1;
    }


    cout << "Running timing tests for determinant calculation (using SWEEP pivots)..." << endl;
    for (int test = 1; test <= 10; ++test) {
        // Không cần tạo bản sao ở đây vì hàm determinant_from_sweep_pivots nhận const&
        // và tự tạo bản sao bên trong nó.
        auto start_time = chrono::high_resolution_clock::now();
        try {
             // Gọi hàm tính định thức (chỉ để đo thời gian)
             determinant_from_sweep_pivots(matrixA);
        } catch (const runtime_error& e) {
             // Lỗi này không nên xảy ra nếu lần tính đầu tiên thành công
             cerr << "Error during timed determinant calculation: " << e.what() << endl;
             outputFile.close();
             return 1;
        }
        auto end_time = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration = end_time - start_time;
        timings_ms.push_back(duration.count());
        outputFile << "Test " << setw(2) << test << ": " << duration.count() << " ms" << endl;
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

    // Ví dụ cách sử dụng hàm sweep_operator riêng lẻ (nếu cần)
    // Matrix tempA = matrixA; // Tạo bản sao nếu không muốn thay đổi matrixA gốc
    // try {
    //     cout << "\nApplying SWEEP(0) to a copy of A:" << endl;
    //     sweep_operator(tempA, 0);
    //     printMatrix(tempA, "Matrix after SWEEP(0)");
    //
    //     cout << "\nApplying SWEEP(1) to the result:" << endl;
    //     sweep_operator(tempA, 1);
    //     printMatrix(tempA, "Matrix after SWEEP(0) and SWEEP(1)");
    // } catch (const runtime_error& e) {
    //     cerr << "Error applying SWEEP operator: " << e.what() << endl;
    // }


    return 0;
}