from cProfile import label
from turtle import color
import matplotlib.pyplot as plt
import numpy as np

times = [53.709,45.551,38.252,32.934,28.093,24.509,22.032,20.371,19.838,19.293,18.920]
base = 89.593
x = [i for i in range(2, 13)]
radios = [base/i for i in times]

y_base = 1
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
ax.scatter(x, radios, s=10, c='r', marker='o', label="Paralized")
# ax.hlines(y=1, xmin=1.5, xmax=12.2, linewidth=1.5, color='b', label="Serialized")    
ax.legend()
plt.xlim(1.5, 12.5)
plt.ylim(1, 5)
plt.xlabel('Paralization cores')
plt.ylabel('Speedup ratio')
plt.title('Visualization of Paralization cores and Speedup ratio')
plt.show()

