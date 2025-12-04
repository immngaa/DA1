import math
import time

def sweep(A, k):
    n = len(A)
    D = A[k][k]
    if abs(D) < 1e-15:
        return
    for j in range(n):
        A[k][j] /= D
    for i in range(n):
        if i == k:
            continue
        B = A[i][k]
        for j in range(n):
            A[i][j] -= B * A[k][j]
        A[i][k] = -B / D
    A[k][k] = 1.0 / D

def det_via_sweep(XtX):
    n = len(XtX)
    A = [row[:] for row in XtX]
    det = 1.0
    for k in range(n):
        D = A[k][k]
        if abs(D) < 1e-15:
            return 0.0
        det *= D
        sweep(A, k)
    return det

def main():
    # Đọc file inputX.txt
    try:
        with open(r"C:\Users\Admin\OneDrive - Hanoi University of Science and Technology\MI1 HUST\DA1\DA1v3-dx_xdd\Sweepdx\inputX.txt", "r") as fin:
            m, n = map(int, fin.readline().split())
            X = [list(map(float, fin.readline().split())) for _ in range(m)]
    except FileNotFoundError:
        print("Không thể mở file inputX.txt")
        return 1

    # Tạo ma trận XtX = X^T X
    XtX = [[0.0]*n for _ in range(n)]
    for i in range(n):
        for j in range(n):
            s = 0.0
            for k in range(m):
                s += X[k][i] * X[k][j]
            XtX[i][j] = s

    # Ghi ma trận XtX ra file với filepath giữ nguyên từ C++
    filepath = r"C:\Users\Admin\OneDrive - Hanoi University of Science and Technology\MI1 HUST\DA1\DA1v3-dx_xdd\input.txt"
    try:
        with open(filepath, "w") as out:
            out.write(f"{len(XtX)}\n")
            for row in XtX:
                out.write(" ".join(map(str, row)) + "\n")
    except IOError:
        print(f"Không thể mở file để ghi: {filepath}")

    # Mở file outputX.txt để ghi kết quả
    try:
        fout = open(r"C:\Users\Admin\OneDrive - Hanoi University of Science and Technology\MI1 HUST\DA1\DA1v3-dx_xdd\Sweepdx\outputX.txt", "w", encoding="utf-8")
    except IOError:
        print("Không thể mở file outputX.txt")
        return 1

    fout.write(f"Cỡ ma trận: {n} x {n}\n")

    total_ms = 0.0
    for t in range(1, 101):
        start = time.perf_counter()
        det = det_via_sweep(XtX)
        end = time.perf_counter()
        ms = (end - start) * 1000
        total_ms += ms
        # Nếu muốn in từng lần test có thể mở dòng dưới:
        # fout.write(f"Test {t}: {ms:.3f} ms\n")

    # Tính lại det một lần nữa để lấy kết quả chính xác
    det = det_via_sweep(XtX)
    fout.write(f"Định thức: {det:.10f}\n")

    if det > 0:
        sign = "+"
    elif det < 0:
        sign = "-"
    else:
        sign = "0"
    fout.write(f"Dấu của định thức: {sign}\n")

    if det == 0.0:
        logval = float("-inf")
    else:
        logval = math.log(abs(det))
    fout.write(f"Logarit GTTĐ của định thức: {logval}\n")

    fout.write(f"Tổng thời gian 100 lần chạy: {total_ms/1000:.3f} s\n")

    fout.close()
    return 0

if __name__ == "__main__":
    main()
