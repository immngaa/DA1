#include <iostream> 
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include <cmath>     // Cho fabs, log
#include <chrono>    // Cho đo thời gian
using namespace std;

// Toán tử SWEEP trên ma trận A (nửa xác định dương)
void sweep(vector<vector<double>>& A, int k) {
    int n = A.size();
    double D = A[k][k];
    if (fabs(D) < 1e-15) return;  // tránh chia cho 0

    // Bước 1: chia hàng k cho D
    for (int j = 0; j < n; ++j) {
        A[k][j] /= D;
    }

    // Bước 2: triệt tiêu cột k tại mọi hàng i ≠ k
    for (int i = 0; i < n; ++i) {
        if (i == k) continue;
        double B = A[i][k];
        for (int j = 0; j < n; ++j) {
            A[i][j] -= B * A[k][j];
        }
        A[i][k] = -B / D;
    }

    // Bước 3: đặt lại giá trị chéo
    A[k][k] = 1.0 / D;
}

// Tính định thức bằng sweep
double det_via_sweep(const vector<vector<double>>& XtX) {
    int n = XtX.size();
    vector<vector<double>> A = XtX;  // sao chép để không thay gốc
    double det = 1.0;

    for (int k = 0; k < n; ++k) {
        double D = A[k][k];
        if (fabs(D) < 1e-15) return 0.0;
        det *= D;
        sweep(A, k);
    }
    return det;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Đọc từ file
    ifstream fin("inputX.txt");
    if (!fin) {
        cerr << "Không thể mở file inputX.txt" << endl;
        return 1;
    }

    int m, n;
    fin >> m >> n;
    vector<vector<double>> X(m, vector<double>(n));
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            fin >> X[i][j];
    fin.close();

    // Tạo ma trận A = X^T X
    vector<vector<double>> XtX(n, vector<double>(n, 0.0));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            for (int k = 0; k < m; ++k)
                XtX[i][j] += X[k][i] * X[k][j];
    const string filepath =
      R"(C:\Users\Admin\OneDrive - Hanoi University of Science and Technology\MI1 HUST\DA1\DA1v3-dx_xdd\input.txt)";
    ofstream out(filepath);
    if (!out) {
        cerr << "Không thể mở file để ghi: " << filepath << endl;
    } else {
        // Ghi kích thước ma trận
        out << XtX.size() << '\n';
        // Ghi từng hàng của C
        for (size_t i = 0; i <  XtX.size(); ++i) {
            for (size_t j = 0; j <  XtX.size(); ++j) {
                out << XtX[i][j];
                if (j + 1 <  XtX.size()) out << ' ';
            }
            out << '\n';
        }
        out.close();
    }
     //vector<vector<double>> A = XtX;
    // --- Kết thúc ghi file ---
    // Mở file output
    ofstream fout("outputX.txt");
    if (!fout) {
        cerr << "Không thể mở file outputX.txt" << endl;
        return 1;
    }

    fout << "Cỡ ma trận: " << n << " x " << n << "\n";

   double total_ms = 0.0;

for (int t = 1; t <= 100; ++t) {
    auto start = chrono::high_resolution_clock::now();
    double det = det_via_sweep(XtX);
    auto end = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(end - start).count();
    total_ms += ms;
    //fout << "Test " << t << ": " << fixed << setprecision(3) << ms << " ms\n";
}



    // Tính determinant cuối cùng (một lần nữa để lấy giá trị)
    double det = det_via_sweep(XtX);
    fout << "Định thức: " << setprecision(10) << det << "\n";
    
    // Dấu của định thức
    string sign;
    if (det > 0) sign = "+";
    else if (det < 0) sign = "-";
    else sign = "0";
    fout << "Dấu của định thức: " << sign << "\n";

    // Logarit giá trị tuyệt đối
    double logval = (det == 0.0 ? -INFINITY : log(fabs(det)));
    fout << "Logarit GTTĐ của định thức: " << logval << "\n";

    // Thời gian chạy trung bình
    fout << "Tổng thời gian 100 lần chạy: "
     << fixed << setprecision(3) << total_ms/1000.0 << " s\n";

    fout.close();
    return 0;
}
