import math
import numpy as np
import matplotlib.pyplot as plt
import pylab
import sys

arr = np.genfromtxt("single")
sinThread = [row[0] for row in arr]
conThread = [row[1] for row in arr]

sinThreadsAvg = np.mean(sinThread)
print sinThreadsAvg
conThreadsAvg = np.mean(conThread)
print conThreadsAvg
bars = [sinThreadsAvg, conThreadsAvg]

# arrr = np.genfromtxt("multi")

# w = [row[0] for row in arrr]
# z = [row[1] for row in arrr]

# x = np.arange(2)

# fig, ax1 = plt.subplots()
# plt.bar(x, bars)
# ax1.set_xticks(x)
# ax1.set_xticklabels(['Sin threads', 'Con 1 thread'])

# ax1.set_ylabel('Cantidad de ciclos de Clock')

# plt.show()