import numpy as np
import random
import sys

NON_ZERO_SCALE = 0.007
MATRIX_WIDTH = 1000000
orig_stdout = sys.stdout
sys.stdout = open(f'/Users/lipingzhi/Desktop/paral_course/lab/lab3/matrix_{MATRIX_WIDTH}.txt', 'w')
data = np.zeros((MATRIX_WIDTH, MATRIX_WIDTH))
non_zero_pos = []
non_zero_weight = []
i = 0
while True:
    rand_pos = random.randint(0, MATRIX_WIDTH*MATRIX_WIDTH - 1)
    if rand_pos not in non_zero_pos:
        i += 1
        rand_weight = random.random() * 100
        non_zero_weight.append(rand_weight)
        non_zero_pos.append(rand_pos)
    if i == int(NON_ZERO_SCALE*MATRIX_WIDTH*MATRIX_WIDTH):
        break
non_zero_pos.sort()
for i in non_zero_weight:
    print(i, end=' ')
print()
row_ptr = [0]
count = 0
line = 0
for i, e in enumerate(non_zero_pos):
    if int(e/MATRIX_WIDTH) != line:
        line = int(e/MATRIX_WIDTH)
        row_ptr.append(i)
row_ptr.append(len(non_zero_pos))
for i in row_ptr:
    print(i, end=' ')
print()
for i in non_zero_pos:
    print(i%MATRIX_WIDTH, end=' ')
sys.stdout = orig_stdout
assert len(row_ptr) == MATRIX_WIDTH+1
print("Non zero elements scale:", len(non_zero_pos)/MATRIX_WIDTH/MATRIX_WIDTH)