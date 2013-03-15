#from __future__ import division
import numpy 

from numpy.linalg import inv, solve, det

data = [[1,2,3,4,5,6,7,8,9],
	[2,3,4,5,6,7,8,9,1],
	[3,4,5,6,7,8,9,1,2],
	[4,5,6,7,8,9,1,2,3],
	[5,6,7,8,9,1,2,3,4],
	[6,7,8,9,1,2,3,4,5],
	[7,8,9,1,2,3,4,5,6],
	[8,9,1,2,3,4,5,6,7],
	[9,1,2,3,4,5,6,7,8]
	]

print type(data)
print len(data)
a = numpy.matrix(data)

b = numpy.matrix([[1,2,3,4,5,6,7,8,9],
		  [9,1,2,3,4,5,6,7,8],
	    	  [2,3,4,5,6,7,8,9,1],
	    	  [5,6,7,8,9,1,2,3,4],
	    	  [6,7,8,9,1,2,3,4,5],
	    	  [7,8,9,1,2,3,4,5,6],
	    	  [8,9,1,2,3,4,5,6,7],
	    	  [3,4,5,6,7,8,9,1,2],
	    	  [4,5,6,7,8,9,1,2,3]
	    	 ])

print 'a ='
print a 
print 'b ='
print b 


c = solve(a, b)
print c
print a * c

d = a * b
for i in range(0,5):
	d =  d * b
print d



numpy.savetxt("ma", b)
