import random

def sinh_va_in_ma_tran(n, min_val, max_val, filename=r"C:\Users\Admin\OneDrive - Hanoi University of Science and Technology\MI1 HUST\DA1\DA1v3\input.txt"):
    """
    Sinh ma trận vuông n×n với các phần tử nguyên ngẫu nhiên và ghi vào file
    
    Tham số:
        n (int): Kích thước ma trận
        min_val (int): Giá trị nhỏ nhất (mặc định: 0)
        max_val (int): Giá trị lớn nhất (mặc định: 9)
        filename (str): Tên file đầu ra (mặc định: "input.txt")
    """
    # Sinh ma trận
    ma_tran = [[random.randint(min_val, max_val) for _ in range(n)] for _ in range(n)]
    
    # Ghi vào file
    with open(filename, 'w') as file:
        # Ghi n vào dòng đầu tiên
        file.write(f"{n}\n")
        
        # Ghi ma trận
        for hang in ma_tran:
            # Chuyển mỗi phần tử thành chuỗi và nối với dấu cách
            file.write(' '.join(map(str, hang)) + '\n')

# Ví dụ sử dụng
if __name__ == "__main__":
    n = 2000  # Kích thước ma trận
    min_val = -10
    max_val = 10
    
    print(f"Đang tạo ma trận {n}x{n} ngẫu nhiên từ {min_val} đến {max_val} vào file input.txt...")
    sinh_va_in_ma_tran(n, min_val, max_val)
    print("Đã tạo file thành công!")