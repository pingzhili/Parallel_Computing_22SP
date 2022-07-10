import numpy as np
import sys

N = 100000
orig_stdout = sys.stdout
f = open(f'/Users/lipingzhi/Desktop/paral_course/lab/lab2/graph_{N}.txt', 'w')
sys.stdout = f
data = np.random.randint(90, size = (N, N)) + 1
for i in range(N):
    data[i][i] = 0
for i in range(N):
    for j in range(N):
        if j == N-1:
            print(data[i][j])
        else:
            print(data[i][j], end=' ')
sys.stdout = orig_stdout
f.close()