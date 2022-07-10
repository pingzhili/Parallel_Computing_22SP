from mpi4py import MPI
from matplotlib import pyplot as plt
import numpy as np
from utils import get_mnist_data, init_layer, forward, loss_function, softmax, sigmoid

# MPI initialize
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

# Train params
log_step = 10
epochs = 5
lr = 1e-4
batch_size = 32

if rank == 0:
    # Init model
    l1 = init_layer(28*28,128)
    l2 = init_layer(128,10)
    for i in range(1, size):
        comm.send(l1, dest=i)
        comm.send(l2, dest=i)
else:
    l1 = comm.recv(source=0)
    l2 = comm.recv(source=0)
X, Y, X_test, Y_test = get_mnist_data()
print(f"Iteration times: {int(epochs*X.shape[0]//(batch_size*size))}")
np.random.seed(size*rank + rank)
for i in range(int(epochs*X.shape[0]//(batch_size*size))):
    sample_idx = np.random.randint(0, X.shape[0], size = batch_size)
    x=X[sample_idx].reshape((-1,28*28))
    y=Y[sample_idx]
    output, update_l1, update_l2 = forward(x, y, l1, l2)
    output_category = np.argmax(output, axis=1)
    loss = loss_function(output_category, y)
    if i % log_step == 0:
        print(f"Process {rank} epoch {int(i/(X.shape[0]//(batch_size*size)))} loss {loss.item()}")
    update_l1 = comm.allreduce(update_l1, op=MPI.SUM)
    update_l2 = comm.allreduce(update_l2, op=MPI.SUM)
    l1 -= lr * update_l1 / size
    l2 -= lr * update_l2 / size
