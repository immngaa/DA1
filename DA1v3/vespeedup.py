import matplotlib.pyplot as plt

# Dữ liệu
n = [90, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 2000]
S1 = [1.50, 2.00, 2.38, 2.15, 1.71, 2.22, 2.25, 2.17, 2.68, 2.52, 2.34, 2.22]
S2 = [0.40, 0.33, 0.30, 0.63, 1.30, 1.22, 1.46, 1.67, 1.60, 1.64, 1.64, 1.64]

# Thiết lập biểu đồ
plt.figure(figsize=(10, 6))

# Vẽ đường
plt.plot(n, S1, color='blue', linewidth=2, label='S1 = Gauss / LU')
plt.plot(n, S2, color='red', linewidth=2, label='S2 = LU / LU cải tiến')

# Giới hạn trục y và trục x
plt.ylim(0, 3)
plt.xlim(90, 2000)

# Chỉ đánh dấu xticks chính
xticks = [90, 200, 400, 600, 800, 1000, 2000]
plt.xticks(xticks)

# Tạo grid ngang
plt.grid(axis='y', color='gray', linestyle='--', linewidth=0.5)

# Bỏ viền trên & phải
for spine in ['top', 'right']:
    plt.gca().spines[spine].set_visible(False)

# Nhãn và tiêu đề
plt.xlabel('Kích thước ma trận (n)')
plt.ylabel('Hệ số tăng tốc')
plt.title('Hệ số tăng tốc S1 và S2 theo cấp ma trận')

# Chú giải
plt.legend(loc='upper right')

# Hiển thị
plt.show()
