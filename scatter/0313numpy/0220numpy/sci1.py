import time
import numpy as np
t1 = time.time()
a = range(10000000)
b = range(10000000)
c = []
for i in range(len(a)):
	c.append(a[i] + b[i])

print type(a)
print type(b)
print type(c)

t2 = time.time()
print t2-t1

####################################
a = np.arange(10000000)
b = np.arange(10000000)
c = a + b
print type(a)
print type(b)
print type(c)
t3 = time.time()
print t3 - t2
