import sys
def sort(data, n):
	for i in range(0, n):
		flag = 0
		for k in range(i,n):
			if data[k]%2==1:
				min = k
				flag = 1
				break
		if flag == 0:
			break
		k = min
		for j in range(k+1, n):
			if data[j] < data[min] and data[j]%2==1:
				min = j
		data[k],data[min] = data[min], data[k]
		
	for i in range(0, n):
		flag = 0
		for k in range(i,n):
			if data[k]%2==0:
				max = k
				flag = 1
				break
		if flag == 0:
			break
		
		k = max
		for j in range(k+1, n):
			if data[j] > data[max] and data[j]%2==0:
				max = j
		data[k],data[max] = data[max], data[k]
	return data


def main():
	sys.stdin.close()
	sys.stdin = open("C-large.in","r")
	sys.stdout = open("out.txt","w")

	case = sys.stdin.readline().strip('\n')
	for i in range(1, int(case)+1):
		num = sys.stdin.readline().strip('\n')
		num = int(num)
		data = []
		
		a = sys.stdin.readline().strip('\n')
		b = a.split()
		for j in range(0, num):
			data.append(int(b[j]))
			
		result = sort(data, num)
		line = ""
		for j in range(0, num):
			line += " " + str(result[j]) 
		print "Case #" + str(i) + ":" + line




if __name__== "__main__":
	main()
