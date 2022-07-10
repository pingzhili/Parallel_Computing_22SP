import matplotlib.pyplot as plt
import numpy as np

"""
>>> ps_x = [14.841,9.475,7.648,6.954,6.583,6.075,6.126,6.119,6.184]
>>> ra_x = [9.914,8.221,7.263,6.678,6.275,5.787,5.859,5.659,5.529]
"""

times = [9.914,8.221,7.263,6.678,6.275,5.787,5.859,5.659,5.529]
base = 8.641
x = [i for i in range(2, 11)]
radios = [base/i for i in times]

fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
ax.scatter(x, radios, s=10, c='r', marker='o', label="Ring All-reduce")  
times = [14.841,9.475,7.648,6.954,6.583,6.075,6.126,6.119,6.184]
base = 8.641
x = [i for i in range(2, 11)]
radios = [base/i for i in times]
ax.scatter(x, radios, s=10, c='b', marker='o', label="Parameter Server") 
ax.legend()
plt.xlim(1.5, 10.5)
plt.ylim(0.5, 1.7)
plt.xlabel('Paralization cores')
plt.ylabel('Speedup ratio')
plt.title('Visualization of Paralization cores and Speedup ratio')
plt.show()

