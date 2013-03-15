#from __future__ import division
import numpy
from numpy.linalg import inv, solve, det

b = numpy.loadtxt("b.txt", delimiter=",")

print type(b)
a = numpy.matrix(b)
print type(a)


numpy.savetxt("b2.txt", a, fmt="%d", delimiter="\t")

