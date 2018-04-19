import math
import numpy as np
import matplotlib.pyplot as plt
import pylab
import sys

arr1 = np.genfromtxt("multi-1")
arr2 = np.genfromtxt("multi-2")
arr3 = np.genfromtxt("multi-3")
arr4 = np.genfromtxt("multi-4")
arr5 = np.genfromtxt("multi-5")
max1Thread = [row[0] for row in arr1]
max2Thread = [row[0] for row in arr2]
max3Thread = [row[0] for row in arr3]
max4Thread = [row[0] for row in arr4]
max5Thread = [row[0] for row in arr5]

maxConcurrente1Thread = [row[1] for row in arr1]
maxConcurrente2Thread = [row[1] for row in arr2]
maxConcurrente3Thread = [row[1] for row in arr3]
maxConcurrente4Thread = [row[1] for row in arr4]
maxConcurrente5Thread = [row[1] for row in arr5]

print np.mean(max1Thread), np.mean(maxConcurrente1Thread)

print np.mean(max2Thread), np.mean(maxConcurrente2Thread)

print np.mean(max3Thread), np.mean(maxConcurrente3Thread)

print np.mean(max4Thread), np.mean(maxConcurrente4Thread)

print np.mean(max5Thread), np.mean(maxConcurrente5Thread)

# arrr = np.genfromtxt("multi")

# w = [row[0] for row in arrr]
# z = [row[1] for row in arrr]

# x = np.arange(2)

# fig, ax1 = plt.subplots()
# plt.bar(x, bars)
# ax1.set_xticks(x)
# ax1.set_xticklabels(['Sin threads', 'max 1 thread'])

# ax1.set_ylabel('Cantidad de ciclos de Clock')

# plt.show()