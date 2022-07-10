from cProfile import label
import matplotlib.pyplot as plt
import numpy as np

# x = [i for i in range(3, 17)]
y = [0.3949, 0.5952, 0.7923, 0.99, 1.1877, 1.3869, 1.5813, 1.7724, 1.9737, 2.1425, 2.3105, 2.492, 2.7407, 2.9331]
x = [i for i in range(3, 10)]
y_no = [1991.987, 1267.008, 945.445, 756.898, 630.055, 539.89, 473.281]
yno = [746.98/item for item in y_no]
yo = y[:len(yno)]
for i in range(len(yno)):
    print(yno[i]-yo[i])

# y_base = 1
# fig = plt.figure()
# ax = fig.add_subplot(1, 1, 1)
# ax.scatter(x, y, s=10, c='r', marker='o', label="Paralized")
# ax.hlines(y=1, xmin=2.5, xmax=16.2, linewidth=1.5, color='b', label="Serialized")    
# ax.legend()
# plt.xlim(2.5, 16.5)
# plt.ylim(0, 3)
# plt.xlabel('Paralization cores')
# plt.ylabel('Speedup ratio')
# plt.title('Visualization of Paralization cores and Speedup ratio')
# plt.show()

fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
ax.scatter(x, yo, s=5, c='r', marker='o', label="Optimized")
ax.scatter(x, yno, s=5, c='b', marker='o', label="Non-Optimized")
ax.legend()
plt.xlim(2.5, 9.5)
plt.ylim(0.25, 1.75)
plt.xlabel('Paralization cores')
plt.ylabel('Speedup ratio')
plt.title('Visualization of Paralization cores and Speedup ratio')
plt.show()