#include <iostream>
#include <vector>
#include <fstream>   // Để đọc/ghi file
#include <chrono>    // Để đo thời gian
#include <cmath>     // Cho std::fabs
#include <iomanip>   // Cho std::fixed, std::setprecision
#include <stdexcept> // Cho std::runtime_error
#include <numeric>   // Cho std::accumulate

// Sử dụng namespace std để code ngắn gọn hơn theo yêu cầu
using namespace std;

// Định nghĩa kiểu dữ liệu Matrix cho dễ đọc
using Matrix = vector<vector<double>>;

// Ngưỡng nhỏ để kiểm tra giá trị gần bằng 0
const double epsilon = 1e-12;

// --- Hàm tính định thức (Dựa trên SWEEP pivots) ---
// Lưu ý: Hàm này sẽ thay đổi ma trận đầu vào 'A'
double calculateDeterminantSweepPivots(Matrix& A) {
    if (A.empty() || A.size() != A[0].size()) {
        throw runtime_error("Matrix must be square and non-empty.");
    }

    int n = A.size();
    double determinant = 1.0;

    for (int k = 0; k < n; ++k) {
        // 1. Lấy phần tử trụ D = a_kk
        double D = A[k][k];

        // Kiểm tra điều kiện suy biến
        if (fabs(D) < epsilon) {
            return 0.0; // Ma trận suy biến, định thức bằng 0
        }
        // (Tùy chọn) Kiểm tra PSD: Nếu D < 0, ma trận gốc không phải PSD
        // if (D < 0) {
        //     cerr << "Warning: Negative pivot encountered at step " << k << ". Input might not be PSD." << endl;
        // }

        // 2. Cập nhật định thức
        determinant *= D;

        // 3. Thực hiện biến đổi hàng (tương tự Gauss để cập nhật các pivot sau)
        // Lưu giá trị B = a_ik trước khi hàng k bị thay đổi
        vector<double> B_values(n);
        for(int i = 0; i < n; ++i) {
            if (i != k) {
                B_values[i] = A[i][k];
            }
        }

        // Chia hàng k cho D (chỉ cần từ cột k trở đi)
        for (int j = k; j < n; ++j) {
            A[k][j] /= D;
        }
        // A[k][k] bây giờ = 1

        // Cập nhật các hàng i != k: A_i <- A_i - B * A_k
        for (int i = 0; i < n; ++i) {
            if (i == k) continue;

            double B = B_values[i]; // Giá trị a_ik ban đầu

            // Cập nhật các phần tử của hàng i (chỉ cần từ cột k)
            // A[i][j] = A[i][j] - B * A[k][j] (A[k][j] là giá trị sau khi chia cho D)
            for (int j = k; j < n; ++j) {
                 A[i][j] = A[i][j] - B * A[k][j];
            }
        }
    } // Kết thúc vòng lặp k

    return determinant;
}


int main() {
    // --- Đọc file Input ---
    ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        cerr << "Error: Could not open input.txt" << endl;
        return 1;
    }

    int n;
    inputFile >> n; // Đọc cỡ ma trận

    if (n <= 0) {
         cerr << "Error: Invalid matrix size n = " << n << endl;
         inputFile.close();
         return 1;
    }

    Matrix matrix(n, vector<double>(n)); // Khởi tạo ma trận n x n

    // Đọc các phần tử ma trận
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (!(inputFile >> matrix[i][j])) {
                 cerr << "Error: Failed to read matrix element at [" << i << "][" << j << "]" << endl;
                 inputFile.close();
                 return 1;
            }
        }
    }
    inputFile.close();

    // --- Mở file Output ---
    ofstream outputFile("output.txt");
     if (!outputFile.is_open()) {
        cerr << "Error: Could not open output.txt" << endl;
        return 1;
    }

    // --- Ghi cỡ ma trận ---
    outputFile << "Cỡ ma trận: " << n << endl << endl;
    outputFile << fixed << setprecision(6); // Định dạng output số thực

    // --- Thực hiện tính toán và đo thời gian 10 lần ---
    vector<double> timings_ms; // Lưu thời gian (milliseconds)
    double determinant_value = 0.0;

    cout << "Running timing tests..." << endl; // Thông báo ra console

    for (int test = 1; test <= 10; ++test) {
        // Quan trọng: Tạo bản sao của ma trận gốc cho mỗi lần test
        // vì hàm tính định thức sẽ thay đổi ma trận được truyền vào.
        Matrix currentMatrix = matrix;

        auto start_time = chrono::high_resolution_clock::now();

        // Tính định thức
        double current_det = calculateDeterminantSweepPivots(currentMatrix);

        auto end_time = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration = end_time - start_time;

        timings_ms.push_back(duration.count());

        // Ghi thời gian của lần test này
        outputFile << "Test " << setw(2) << test << ": " << duration.count() << " ms" << endl;

        // Lưu giá trị định thức (chỉ cần lấy từ lần đầu tiên là đủ)
        if (test == 1) {
            determinant_value = current_det;
        }
         // Optional: Check consistency (should be the same if code is correct)
         // else if (fabs(determinant_value - current_det) > epsilon * 100) { // Allow slightly larger tolerance due to potential floating point variations
         //     cerr << "Warning: Determinant inconsistency detected between runs!" << endl;
         // }
    }
     cout << "Timing tests finished." << endl;

    // --- Tính thời gian trung bình ---
    double average_time_ms = 0.0;
    if (!timings_ms.empty()) {
        average_time_ms = accumulate(timings_ms.begin(), timings_ms.end(), 0.0) / timings_ms.size();
    }

    // --- Ghi kết quả cuối cùng ---
    outputFile << endl;
    outputFile << "Giá trị định thức: " << fixed << setprecision(10) << determinant_value << endl; // Tăng độ chính xác cho định thức
    outputFile << "Thời gian trung bình tính định thức: " << fixed << setprecision(6) << average_time_ms << " ms" << endl;

    outputFile.close();

    cout << "Results written to output.txt" << endl;

    return 0;
}
