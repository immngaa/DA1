#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <cmath>
using namespace std;

// Hàm phân rã LU theo thuật toán trong ảnh
bool LUDECOMPOSITION(vector<vector<double>>& A, vector<vector<double>>& L, vector<vector<double>>& U, int n, int& swapCount) {
    for (int j = 0; j < n; j++) {
        if (fabs(A[j][j]) < 1e-10) {
            bool found = false;
            for (int k = j + 1; k < n; k++) {
                if (fabs(A[k][j]) > 1e-10) {
                    swap(A[j], A[k]);
                    swap(L[j], L[k]);
                    swapCount++;
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << "Khong the phan ra LU" << endl;
                return false;
            }
        }

        for (int i = 0; i <= j; i++) {
            double sum = 0;
            for (int k = 0; k <= i - 1; k++) {
                sum += U[k][j] * L[i][k];
            }
            U[i][j] = A[i][j] - sum;
            if (isinf(U[i][j]) || isnan(U[i][j])) {
                cout << "Tran so trung gian khi tinh U[" << i << "][" << j << "]" << endl;
                return false;
            }
        }

        for (int i = j + 1; i < n; i++) {
            double sum = 0;
            for (int k = 0; k <= j - 1; k++) {
                sum += U[k][j] * L[i][k];
            }
            if (fabs(U[j][j]) < 1e-10) {
                cout << "Khong the phan ra LU (U[" << j << "][" << j << "] qua nho)" << endl;
                return false;
            }
            L[i][j] = (A[i][j] - sum) / U[j][j];
            if (isinf(L[i][j]) || isnan(L[i][j])) {
                cout << "Tran so trung gian khi tinh L[" << i << "][" << j << "]" << endl;
                return false;
            }
        }
    }
    return true;
}

// Hàm tính logarit của giá trị tuyệt đối định thức và dấu
void determinant(vector<vector<double>>& U, int n, int swapCount, double& logDetAbs, int& sign, double scaleLog) {
    logDetAbs = 0.0;
    sign = (swapCount % 2 == 0) ? 1 : -1;

    for (int i = 0; i < n; i++) {
        if (fabs(U[i][i]) < 1e-10) {
            logDetAbs = -numeric_limits<double>::infinity(); // Định thức bằng 0
            sign = 0;
            return;
        }
        logDetAbs += log(fabs(U[i][i]));
        sign *= (U[i][i] < 0 ? -1 : 1);
    }
    logDetAbs += scaleLog; // Điều chỉnh hệ số chuẩn hóa
}

// Hàm chuẩn hóa ma trận
double normalizeMatrix(vector<vector<double>>& A, int n) {
    double maxVal = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            maxVal = max(maxVal, fabs(A[i][j]));
        }
    }
    if (maxVal == 0) return 0.0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] /= maxVal;
        }
    }
    return n * log(maxVal); // Trả về logarit của hệ số chuẩn hóa
}

int main() {
    ifstream inp("C:\\Users\\Admin\\OneDrive - Hanoi University of Science and Technology\\MI1 HUST\\DA1\\DA1v3-dx_xdd\\input.txt");
    if (!inp) {
        cout << "Khong mo duoc file input.txt" << endl;
        return 1;
    }

    int n;
    inp >> n;

    // Khởi tạo ma trận
    vector<vector<double>> A(n, vector<double>(n));
    vector<vector<double>> L(n, vector<double>(n, 0));
    vector<vector<double>> U(n, vector<double>(n, 0));

    // Đọc ma trận từ file
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inp >> A[i][j];
        }
        L[i][i] = 1;
    }
    inp.close();

    // Chuẩn hóa ma trận
    double scaleLog = normalizeMatrix(A, n);

    // Lưu ma trận gốc để tính lại nhiều lần
    vector<vector<double>> A_original = A;

    // Đo thời gian 10 lần
    vector<double> times(100);
    double logDetAbs = 0;
    int sign = 1;
    bool success = true;
    for (int t = 0; t < 100; t++) {
        int swapCount = 0;
        A = A_original;
        L.assign(n, vector<double>(n, 0));
        U.assign(n, vector<double>(n, 0));
        for (int i = 0; i < n; i++) L[i][i] = 1;

        auto start = chrono::high_resolution_clock::now();
        success = LUDECOMPOSITION(A, L, U, n, swapCount);
       //

        if (t == 0 && success) {
            determinant(U, n, swapCount, logDetAbs, sign, scaleLog);
        }

        if (!success) break;
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration = end - start;
        times[t] = duration.count();

    }

    // In kết quả
    if (success) {
        cout << fixed << setprecision(6);
        cout << "Thoi gian tinh dinh thuc 100 lan (ms):\n";
        //for (int t = 0; t < 100; t++) {
           // cout << "Test " << t + 1 << ": " << times[t] << " ms\n";
        //}
        double avg_time = 0;
        for (double t : times) avg_time += t;
        //avg_time /= 10;

        // In định thức dưới dạng logarit và dấu
        if (sign == 0 || logDetAbs == -numeric_limits<double>::infinity()) {
            cout << "Dinh thuc: 0\n";
        } else {
            cout << "Logarit dinh thuc: " << logDetAbs << endl;
            cout << "Dau cua dinh thuc: " << (sign > 0 ? "+" : "-") << endl;
           // if (logDetAbs < 300 && logDetAbs > -300) { // Chỉ in giá trị thật nếu không tràn số
                double det = sign * exp(logDetAbs);
                cout << "Dinh thuc: " << det << endl;
            //} else {
                //cout << "Dinh thuc qua lon de bieu dien truc tiep\n";
            //}
        }
        cout << "Tong thoi gian 100 lan chay: " << fixed << setprecision(3) << avg_time/1000.0 << " s\n";
    }

    return 0;
}