import numpy


word_num = 5
file_num = 5

init_row = [i for i in range(0,word_num)]
#print init_row
#print len(init_row)

init_list = [init_row for i in range(0, file_num)]

m = numpy.matrix(init_list)
m[(2,3)] += 1
print m
print

for i in range(0, word_num):
	l = numpy.matrix.tolist(m[i])
	print l
	print

	t = l[0]
	print t[2]
	

