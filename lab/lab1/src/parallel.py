from mpi4py import MPI
from math import factorial
import decimal

decimal.getcontext().prec = 110
iter_time = 20000
comm = MPI.COMM_WORLD
size = comm.Get_size()
rank = comm.Get_rank()
total = decimal.Decimal(0)
sum_proc = size - 1

if rank == size - 1:
    for i in range(sum_proc):
        s = comm.recv()
        total += s
    decimal.getcontext().prec = 99
    print("Parallelization with", sum_proc, "groups")
    print("e =", total.to_eng_string())
else:
    for i in range(round(iter_time/(size - 1))):
        k = (size - 1) * i + rank
        item = decimal.Decimal(1) / decimal.Decimal(factorial(k))
        total += item
    comm.send(total, dest=size-1)

