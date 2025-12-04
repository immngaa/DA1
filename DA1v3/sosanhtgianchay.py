import matplotlib.pyplot as plt

# Dữ liệu từ bảng
n = [90, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 2000]
#khi_gauss = [2.7490, 3.9824, 31.9846, 105.1703, 254.3677, 643.9742, 1038.3654, 1841.2069, 3430.59, 4630.9720, 6226.7034, 40683.3124]
#phan_ra_lu = [1.2339, 1.8798, 18.6494, 48.2193, 110.9927, 336.1064, 511.6687, 976.5159, 1181.5099, 1808.1262, 2205.0386, 35511.7132]
#phan_ra_lu_cai_tien = [5.3801, 5.5598, 48.4842, 83.8, 136.8891, 212.5506, 297.5459, 759.5601, 992.3609, 1307.7060, 1453.0963, 11212.8931]
khi_gauss=[0.003, 0.004, 0.031, 0.101, 0.257, 0.467, 0.811, 1.295, 2.124, 2.909, 3.902, 30.762]
phan_ra_lu=[0.002, 0.002, 0.013, 0.047, 0.150, 0.210, 0.360, 0.597, 0.794, 1.153, 1.664, 13.824]
phan_ra_lu_cai_tien=[0.005, 0.006, 0.044, 0.075, 0.115, 0.172, 0.246, 0.358, 0.495, 0.704, 1.014, 8.452]
# Vẽ biểu đồ
plt.figure(figsize=(10, 6))
plt.plot(n, khi_gauss, label='Khử Gauss', marker='o')
plt.plot(n, phan_ra_lu, label='Phân rã LU', marker='s')
plt.plot(n, phan_ra_lu_cai_tien, label='Phân rã LU cải tiến', marker='^')

# Cài đặt tiêu đề và nhãn
plt.title('Thời gian chạy của các thuật toán theo cấp ma trận')
plt.xlabel('Kích thước ma trận (n)')
plt.ylabel('Thời gian chạy (s)')
plt.legend()
plt.grid(True)

# Hiển thị biểu đồ
plt.show()