from math import factorial
import decimal

decimal.getcontext().prec = 110
iter_time = 20000
total = decimal.Decimal(0)
for k in range(iter_time):
    item = decimal.Decimal(1) / decimal.Decimal(str(factorial(k)))
    total += item
decimal.getcontext().prec = 99
print("Baseline wihtout parallelization")
print("e =", total.to_eng_string())