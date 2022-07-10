from cProfile import label
from turtle import color
import matplotlib.pyplot as plt
import numpy as np

# times = [2.49017, 1.94814, 1.62848, 1.4967, 1.33864, 1.23522, 1.1777, 1.15495, 1.13027, 1.10091, 1.06557, 1.03333, 1.02899, 1.02580, 1.02569]
# x = [i for i in range(2, 17)]
# radios = [4.656/i for i in times]

# y_base = 1
# fig = plt.figure()
# ax = fig.add_subplot(1, 1, 1)
# ax.scatter(x, radios, s=10, c='r', marker='o', label="Paralized")
# ax.hlines(y=1, xmin=1.5, xmax=16.2, linewidth=1.5, color='b', label="Serialized")    
# ax.legend()
# plt.xlim(1.5, 16.5)
# plt.ylim(0, 5)
# plt.xlabel('Paralization cores')
# plt.ylabel('Speedup ratio')
# plt.title('Visualization of Paralization cores and Speedup ratio')
# plt.show()

# fig = plt.figure()
# ax = fig.add_subplot(1, 1, 1)
# ax.scatter(x, yo, s=5, c='r', marker='o', label="Optimized")
# ax.scatter(x, yno, s=5, c='b', marker='o', label="Non-Optimized")
# ax.legend()
# plt.xlim(2.5, 9.5)
# plt.ylim(0.25, 1.75)
# plt.xlabel('Paralization cores')
# plt.ylabel('Speedup ratio')
# plt.title('Visualization of Paralization cores and Speedup ratio')
# plt.show()
data = [1.3067, 1.2258, 1.1797, 1.2775, 1.2014, 1.1824, 1.069, 1.0155, 1.0924, 1.0784, 1.1287, 1.116, 1.236, 1.1636, 1.199, 1.2659, 1.17, 1.2012, 1.1545, 1.422, 1.2796, 1.1291, 1.4082, 1.1581, 1.3351, 1.2521, 1.129, 1.1394, 1.2032, 1.233, 1.2821, 1.2954, 1.2898, 1.1919, 1.2831, 1.3801, 1.4007, 1.419, 1.6022, 1.4927, 1.375, 1.382, 1.3416, 1.3402, 1.2927, 1.3296, 1.3107, 1.5175, 1.4325, 1.3623, 1.268, 1.3384, 1.3832, 1.3995, 1.3574, 1.382, 1.3357, 1.4429, 1.31, 1.4712, 1.4061, 1.3435, 1.4559]
data=np.array(data).reshape(9,7)
delta = [3, 6, 10, 30, 60, 100, 300, 600, 1000]
x = [i+1 for i in range(len(delta))]
threshold = [32, 64, 128, 256, 1024, 2048, 3072]
data = data.T
fig = plt.figure()
# ax = fig.add_subplot(1, 1, 1)
colors = ['b', 'g', 'r', 'c', 'm', 'y', 'k']
for i, c in enumerate(colors):
    # ax.scatter(x, data[i], s=10, c=c, marker='x', linestyle='-', label=f"threshold = {threshold[i]}")
    plt.plot(x, data[i], color=c, marker='x', linestyle='--', label=f"threshold = {threshold[i]}")
plt.legend()
# ax.legend()
# plt.xlim(3, 9.5)
plt.ylim(1.0, 1.65)
plt.xlabel('Delta')
plt.ylabel('Runtime')
plt.title('Visualization of parameters and runtime')
plt.xticks(x, delta)
plt.show()