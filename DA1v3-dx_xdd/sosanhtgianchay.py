import matplotlib.pyplot as plt

# Dữ liệu
n = [20, 30, 40, 50, 60, 70, 80, 90, 100]
gauss = [0.005, 0.016, 0.032, 0.077, 0.123, 0.215, 0.310, 0.422, 0.628]
lu =    [0.004, 0.011, 0.026, 0.052, 0.090, 0.125, 0.205, 0.270, 0.429]
sweep = [0.006, 0.020, 0.048, 0.098, 0.175, 0.271, 0.414, 0.523, 0.750]
plt.figure(figsize=(10, 6))
# Vẽ biểu đồ
plt.plot(n, gauss, marker='o', label='Khử Gauss')
plt.plot(n, lu, marker='s', label='Phân rã LU')
plt.plot(n, sweep, marker='^', label='Toán tử SWEEP')

plt.xlabel('Kích thước ma trận (n)')
plt.ylabel('Thời gian chạy cho 100 lần (s)')
plt.title('Thời gian chạy 100 lần của các thuật toán theo cấp ma trận')
plt.legend()
plt.grid(True)
plt.show()
