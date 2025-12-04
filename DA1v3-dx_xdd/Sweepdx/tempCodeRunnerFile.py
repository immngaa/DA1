# sinhmatran.py

import numpy as np

def sinh_ma_tran_nguyen(m, n, positive_definite=True, seed=None):
    """
    Sinh ma trận X (m x n) gồm các số nguyên trong khoảng [-1, 1]
    sao cho A = X^T X là ma trận đối xứng, và nửa xác định dương hoặc xác định dương.

    Args:
        m (int): số hàng
        n (int): số cột
        positive_definite (bool): nếu True, đảm bảo A xác định dương (nếu m >= n)
        seed (int or None): giá trị seed

    Returns:
        X (np.ndarray): ma trận X số nguyên
        A (np.ndarray): ma trận A = X^T X
    """
    if seed is not None:
        np.random.seed(seed)

    if positive_definite and m < n:
        raise ValueError("Để A xác định dương, cần m >= n")

    # Sinh ma trận số nguyên trong khoảng [-1, 1]
    X = np.random.randint(-1, 2, size=(m, n))  # giá trị từ -1 đến 1

    if positive_definite:
        while np.linalg.matrix_rank(X) < n:
            X = np.random.randint(-1, 2, size=(m, n))

    A = X.T @ X
    return X, A

def ghi_vao_file(X, filename):
    m, n = X.shape
    with open(filename, "w") as f:
        f.write(f"{m} {n}\n")
        for row in X:
            f.write(" ".join(str(val) for val in row) + "\n")

if __name__ == "__main__":
    m, n = 41, 20  # bạn có thể thay đổi
    X, A = sinh_ma_tran_nguyen(m, n, positive_definite=True, seed=42)
    ghi_vao_file(X, r"C:\Users\Admin\OneDrive - Hanoi University of Science and Technology\MI1 HUST\DA1\DA1v3-dx_xdd\Sweepdx\inputX.txt")
    print("Đã ghi ma trận X (số nguyên) vào file inputX.txt")
