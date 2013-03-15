from __future__ import division
import numpy, scipy
from numpy.linalg import inv, solve, det


a = numpy.matrix([[1, 2, 3], 
		  [3, 4, 5],
		  [5, 6, 7]])
b = numpy.matrix([[5, 6, 7],
		  [7, 8, 9], 
		  [9, 0, 1]])


print 'a = \n' , a
print 'b = \n' , b

inv_a = inv(a)
det_a = det(a)
print 'det_a'
print det_a
#print 'a- = \n' ,aa
#print 'aa- = \n' , inv(aa)



#print 'a + b = \n' , a + b
#print 'a * b = \n' , a * b
#print 'a / b = \n' , a / b
#print 'a \ b = \n' , a \ b

c = solve(a, b)

print c
print a * c
#print 'c * b = \n' , c * b
#print 'b * c = \n' , b * c
