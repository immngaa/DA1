#include <iostream> // Sửa lại include cho cout, cerr
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include <cmath>     // Cho fabs, log
#include <chrono>    // Cho đo thời gian
// #include <bits/stdc++.h> // Nên tránh dùng, hãy include cụ thể

using namespace std;

// Toán tử SWEEP trên ma trận A (nửa xác định dương)
// Sửa A trực tiếp
void sweep_inplace(vector<vector<double>>& A, int k) {
    int n = A.size();
    double D_pivot = A[k][k]; // Đổi tên biến để tránh nhầm với D trong det_via_sweep
    if (fabs(D_pivot) < 1e-15) {
        // Nếu pivot quá nhỏ, có thể gây ra lỗi số học hoặc ma trận suy biến
        // Trong trường hợp này, thuật toán sweep chuẩn thường dừng hoặc xử lý đặc biệt.
        // Để đơn giản, ta có thể coi đây là trường hợp định thức = 0.
        // Tuy nhiên, det_via_sweep đã kiểm tra D trước khi nhân, nên ở đây có thể chỉ cần return.
        return;
    }

    // Bước 1: chia hàng k cho D_pivot
    for (int j = 0; j < n; ++j) {
        A[k][j] /= D_pivot;
    }

    // Bước 2: triệt tiêu cột k tại mọi hàng i ≠ k
    for (int i = 0; i < n; ++i) {
        if (i == k) continue;
        double B = A[i][k]; // Lưu trữ A[i][k] gốc trước khi nó có thể bị thay đổi nếu j=k
        for (int j = 0; j < n; ++j) {
            // if (j == k) continue; // Không cần dòng này, A[i][k] sẽ được cập nhật riêng
            A[i][j] -= B * A[k][j];
        }
        A[i][k] = -B / D_pivot; // Đặt lại phần tử ngoài đường chéo ở cột k
    }

    // Bước 3: đặt lại giá trị chéo
    A[k][k] = 1.0 / D_pivot;
}

// Tính định thức bằng sweep, ma trận A sẽ bị thay đổi
double det_via_sweep_inplace(vector<vector<double>>& A) { // Nhận A bằng tham chiếu không const
    int n = A.size();
    double det = 1.0;

    for (int k = 0; k < n; ++k) {
        double D_val = A[k][k]; // Giá trị pivot hiện tại
        if (fabs(D_val) < 1e-15) return 0.0; // Ma trận suy biến
        det *= D_val;
        sweep_inplace(A, k); // A sẽ bị thay đổi bởi sweep_inplace
    }
    return det;
}

int main() {
    ios_base::sync_with_stdio(false); // ios_base tốt hơn cho C++ streams
    cin.tie(nullptr);
    cout.tie(nullptr);

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

    // Tạo ma trận A_orig = X^T X
    vector<vector<double>> XtX_orig(n, vector<double>(n, 0.0));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            for (int l = 0; l < m; ++l) // đổi k thành l để tránh nhầm với k trong sweep
                XtX_orig[i][j] += X[l][i] * X[l][j];

    // Ghi file input.txt (nếu cần)
    const string filepath =
      R"(C:\Users\Admin\OneDrive - Hanoi University of Science and Technology\MI1 HUST\DA1\DA1v3-dx_xdd\input.txt)";
    ofstream out_debug(filepath); // Đổi tên biến để tránh nhầm lẫn
    if (!out_debug) {
        cerr << "Không thể mở file để ghi: " << filepath << endl;
    } else {
        out_debug << XtX_orig.size() << '\n';
        for (size_t i = 0; i <  XtX_orig.size(); ++i) {
            for (size_t j = 0; j <  XtX_orig.size(); ++j) {
                out_debug << XtX_orig[i][j];
                if (j + 1 <  XtX_orig.size()) out_debug << ' ';
            }
            out_debug << '\n';
        }
        out_debug.close();
    }

    // Mở file output
    ofstream fout("outputX.txt");
    if (!fout) {
        cerr << "Không thể mở file outputX.txt" << endl;
        return 1;
    }

    fout << "Cỡ ma trận: " << n << " x " << n << "\n";

    double total_ms = 0.0;
    vector<vector<double>> A_temp; // Ma trận tạm để chạy sweep

    for (int t = 1; t <= 100; ++t) {
        A_temp = XtX_orig; // SAO CHÉP MA TRẬN GỐC VÀO A_temp CHO MỖI LẦN CHẠY
        auto start = chrono::high_resolution_clock::now();
        double det_val = det_via_sweep_inplace(A_temp); // Truyền A_temp để nó bị thay đổi
        auto end = chrono::high_resolution_clock::now();
        double ms = chrono::duration<double, milli>(end - start).count();
        total_ms += ms;
        // fout << "Test " << t << ": " << fixed << setprecision(3) << ms << " ms, det = " << det_val << "\n"; // Có thể thêm det để debug
    }

    // Tính determinant cuối cùng (một lần nữa trên bản sao mới để lấy giá trị chính xác)
    A_temp = XtX_orig; // Reset A_temp một lần nữa
    double final_det = det_via_sweep_inplace(A_temp);
    fout << "Định thức: " << fixed << setprecision(10) << final_det << "\n";

    string sign_str; // Đổi tên biến
    if (final_det > 1e-15) sign_str = "+"; // So sánh với epsilon nhỏ
    else if (final_det < -1e-15) sign_str = "-";
    else sign_str = "0";
    fout << "Dấu của định thức: " << sign_str << "\n";

    double logval = (fabs(final_det) < 1e-15 ? -INFINITY : log(fabs(final_det)));
    fout << "Logarit GTTĐ của định thức: " << fixed << setprecision(10) << logval << "\n";

    fout << "Tổng thời gian 100 lần chạy: "
         << fixed << setprecision(6) << total_ms / 1000.0 << " s\n"; // Tăng độ chính xác cho giây

    fout.close();
    return 0;
}