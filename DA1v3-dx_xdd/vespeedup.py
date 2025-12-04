import matplotlib.pyplot as plt

# Dữ liệu
n = [20, 30, 40, 50, 60, 70, 80, 90, 100]
S3 = [1.20, 1.25, 1.50, 1.27, 1.42, 1.26, 1.34, 1.24, 1.19]
S4 = [1.50, 1.82, 1.85, 1.88, 1.94, 2.17, 2.02, 1.94, 1.75]


# Thiết lập biểu đồ
plt.figure(figsize=(10, 6))

# Vẽ đường
plt.plot(n, S3, color='blue', linewidth=2, label='S3 = SWEEP / Gauss')
plt.plot(n, S4, color='red', linewidth=2, label='S4 = SWEEP / LU ')

# Giới hạn trục y và trục x
plt.ylim(0, 4)
plt.xlim(20, 100)

# Chỉ đánh dấu xticks chính
xticks = [20, 40, 60, 80, 100]
plt.xticks(xticks)

# Tạo grid ngang
plt.grid(axis='y', color='gray', linestyle='--', linewidth=0.5)

# Bỏ viền trên & phải
for spine in ['top', 'right']:
    plt.gca().spines[spine].set_visible(False)

# Nhãn và tiêu đề
plt.xlabel('Kích thước ma trận (n)')
plt.ylabel('Hệ số tăng tốc')
plt.title('Hệ số tăng tốc S3 và S4 theo cấp ma trận')

# Chú giải
plt.legend(loc='upper right')

# Hiển thị
plt.show()
