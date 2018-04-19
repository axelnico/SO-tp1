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
arr6 = np.genfromtxt("multi-6")
arr7 = np.genfromtxt("multi-7")
arr8 = np.genfromtxt("multi-8")
arr9 = np.genfromtxt("multi-9")
arr10 = np.genfromtxt("multi-10")
con1Thread = [row[1] for row in arr1]
con2Thread = [row[1] for row in arr2]
con3Thread = [row[1] for row in arr3]
con4Thread = [row[1] for row in arr4]
con5Thread = [row[1] for row in arr5]
con6Thread = [row[1] for row in arr6]
con7Thread = [row[1] for row in arr7]
con8Thread = [row[1] for row in arr8]
con9Thread = [row[1] for row in arr9]
con10Thread = [row[1] for row in arr10]

conThreadsAvg = np.mean(con1Thread)
print conThreadsAvg

conThreadsAvg = np.mean(con2Thread)
print conThreadsAvg

conThreadsAvg = np.mean(con3Thread)
print conThreadsAvg

conThreadsAvg = np.mean(con4Thread)
print conThreadsAvg

conThreadsAvg = np.mean(con5Thread)
print conThreadsAvg
conThreadsAvg = np.mean(con6Thread)
print conThreadsAvg
conThreadsAvg = np.mean(con7Thread)
print conThreadsAvg
conThreadsAvg = np.mean(con8Thread)
print conThreadsAvg
conThreadsAvg = np.mean(con9Thread)
print conThreadsAvg
conThreadsAvg = np.mean(con10Thread)
print conThreadsAvg

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