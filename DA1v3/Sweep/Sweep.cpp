#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip> // Để dùng std::setprecision
#include <cmath>   // Để dùng std::fabs (giá trị tuyệt đối để kiểm tra gần bằng không)
#include <stdexcept> // Để dùng std::runtime_error

using namespace std;

// Hàm thực hiện toán tử SWEEP(k) trên ma trận
// Lưu ý: Chỉ số ma trận trong C++ bắt đầu từ 0 (0 đến n-1)
// Định nghĩa sử dụng chỉ số bắt đầu từ 1 (1 đến n), nên chúng ta cần điều chỉnh k.
void sweep_operator(vector<vector<double>>& matrix, int k_def, int n) {
    int k = k_def - 1; // Chuyển chỉ số 1-based từ định nghĩa sang 0-based của C++

    // Bước 1: Đặt D = a_kk (đã được thực hiện khi lấy pivot trước khi gọi hàm này)
    // Hàm này nhận ma trận *trước* bước này về mặt khái niệm,
    // nhưng pivot D được tính bên ngoài và sử dụng ở đây.
    // Tuy nhiên, các bước sửa đổi ma trận xảy ra ở đây.

    double D = matrix[k][k]; // Lấy lại giá trị pivot để sử dụng nội bộ trong sweep

    // Kiểm tra xem pivot có bằng không hoặc quá nhỏ không (quan trọng cho ổn định số)
    // Đối với ma trận SPD thực sự, D phải > 0.
    if (fabs(D) < 1e-12) { // Sử dụng một dung sai nhỏ thay vì == 0
        cerr << "Loi: Phan tu truc tai (" << k_def << "," << k_def << ") bang khong hoac gan bang khong. Ma tran co the suy bien hoac khong phai SPD." << endl;
        // Tùy thuộc vào yêu cầu, bạn có thể ném ngoại lệ hoặc thoát
        // Trong ví dụ này, chúng ta sẽ tiếp tục, nhưng kết quả có thể không đáng tin cậy.
        // Nếu D chính xác bằng 0, sẽ xảy ra lỗi chia cho không.
        if (D == 0.0) {
             throw runtime_error("Gap loi chia cho phan tu truc bang khong.");
        }
    }

    // --- Thực hiện các sửa đổi của SWEEP ---

    // Bước 2: Chia hàng k cho D.
    for (int j = 0; j < n; ++j) {
        matrix[k][j] /= D;
    }

    // Bước 3: Với mọi hàng i khác k
    for (int i = 0; i < n; ++i) {
        if (i == k) continue; // Bỏ qua chính hàng k

        // Đặt B = a_ik (giá trị trước khi sửa đổi hàng i trong bước này)
        double B = matrix[i][k];

        // Trừ B * hàng k khỏi hàng i.
        // Lưu ý: matrix[k][j] là giá trị *sau khi* đã chia cho D từ Bước 2
        for (int j = 0; j < n; ++j) {
            matrix[i][j] -= B * matrix[k][j];
        }

        // Đặt a_ik = -B/D
        // Điều này có vẻ thừa sau vòng lặp trên khi j=k,
        // nhưng định nghĩa nêu rõ ràng. Nó đảm bảo giá trị đúng được đặt,
        // có khả năng xử lý các sắc thái của số thực dấu phẩy động.
        matrix[i][k] = -B / D;
    }

    // Bước 4: Đặt a_kk = 1/D.
    // Điều này ghi đè giá trị matrix[k][k] đã trở thành 1 sau Bước 2.
    matrix[k][k] = 1.0 / D;
}

int main() {
    const string input_filename = "input.txt";
    const string output_filename = "output.txt";

    ifstream fin(input_filename);
    ofstream fout(output_filename);

    // Kiểm tra xem tệp đầu vào có mở được không
    if (!fin.is_open()) {
        cerr << "Loi: Khong the mo tep dau vao: " << input_filename << endl;
        return 1;
    }

    // Kiểm tra xem tệp đầu ra có mở được không
    if (!fout.is_open()) {
        cerr << "Loi: Khong the mo tep dau ra: " << output_filename << endl;
        fin.close(); // Đóng tệp đầu vào nếu mở tệp đầu ra thất bại
        return 1;
    }

    int n;
    fin >> n; // Đọc kích thước ma trận

    // Kiểm tra kích thước ma trận hợp lệ
    if (n <= 0) {
         cerr << "Loi: Kich thuoc ma tran khong hop le n = " << n << endl;
         fin.close();
         fout.close();
         return 1;
    }

    // Tạo và đọc ma trận
    vector<vector<double>> matrix(n, vector<double>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            // Kiểm tra đọc thành công phần tử ma trận
            if (!(fin >> matrix[i][j])) {
                 cerr << "Loi: Khong the doc phan tu ma tran tai (" << i+1 << "," << j+1 << ")" << endl;
                 fin.close();
                 fout.close();
                 return 1;
            }
        }
    }

    fin.close(); // Đóng tệp đầu vào sau khi đọc xong

    // --- Tính định thức bằng SWEEP ---
    double determinant_product = 1.0; // Khởi tạo tích các phần tử trục
    vector<vector<double>> working_matrix = matrix; // Làm việc trên một bản sao

    try {
        // Vòng lặp k từ 1 đến n theo định nghĩa
        for (int k = 1; k <= n; ++k) {
            int k_idx = k - 1; // Chỉ số 0-based để truy cập vector C++

            // Lấy phần tử trục *trước* khi thực hiện SWEEP(k)
            double pivot_D = working_matrix[k_idx][k_idx];

            // Kiểm tra tính hợp lệ của pivot (cần thiết cho SPD)
            if (pivot_D <= 1e-12) { // Kiểm tra xem có không dương hoặc quá nhỏ không
                 cerr << "Canh bao: Phan tu truc D = " << pivot_D
                           << " tai buoc k=" << k << " khong duong hoac gan bang khong."
                           << " Ma tran co the khong phai SPD hoac co dieu kien xau." << endl;
                 // Nếu bằng không hoàn toàn, định thức bằng 0.
                 if (pivot_D == 0.0) {
                     determinant_product = 0.0;
                     break; // Dừng xử lý tiếp
                 }
                 // Nếu hơi âm do sai số chính xác, nó vi phạm SPD.
                 // Nếu dương rất nhỏ, có thể dẫn đến vấn đề số học.
            }

            // Cập nhật tích định thức
            determinant_product *= pivot_D;

            // Thực hiện toán tử SWEEP(k) trên ma trận làm việc
            // Truyền k (1-based) theo yêu cầu của các bước định nghĩa khái niệm
            // Hàm sweep_operator sẽ tự xử lý chỉ số 0-based bên trong
            sweep_operator(working_matrix, k, n);
        }

        // Ghi định thức cuối cùng vào tệp đầu ra
        // fixed và setprecision để định dạng số thực
        fout << fixed << setprecision(10) << determinant_product << endl;
        cout << "Da tinh toan dinh thuc thanh cong va ghi vao tep " << output_filename << endl;

    } catch (const runtime_error& e) { // Bắt lỗi runtime cụ thể (ví dụ: chia cho 0)
        cerr << "Loi Runtime trong qua trinh SWEEP: " << e.what() << endl;
        fout << "Loi: Tinh toan that bai do phan tu truc bang khong." << endl; // Ghi lỗi vào tệp output
        fout.close();
        return 1;
    } catch (...) { // Bắt các loại lỗi không mong muốn khác
        cerr << "Da xay ra loi khong mong doi trong qua trinh tinh toan." << endl;
        fout << "Loi: Da xay ra loi khong mong doi." << endl; // Ghi lỗi vào tệp output
        fout.close();
        return 1;
    }

    fout.close(); // Đóng tệp đầu ra

    return 0; // Kết thúc chương trình thành công
}