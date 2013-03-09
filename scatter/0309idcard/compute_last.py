#the length of ID is 18.
weight = [7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2]
endone = [1, 0, 'X', 9, 8, 7, 6, 5, 4, 3, 2]
while True:
	try:
		myid = input() #just [int]
		break
	except:
		print 'Only number input! Try again.'
myid = str(myid)
s = 0
for i in range(0, 17):
	n = int(myid[i])
	s = s + n*weight[i]

print endone[s%11] 
	


