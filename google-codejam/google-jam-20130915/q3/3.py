import sys

def sort(param):
	times = 0
	plen = len(param)
	for i in range(1,plen):
		flag = 0
		key = param[i]
		for j in range(1, i+1)[::-1]:
			if j>0 and key<param[j-1]:
				flag = 1
				param[j] = param[j-1]
		param[j-1] = key
		if flag == 1:
			times = times + 1
	return times





def main():
	sys.stdin.close()
	sys.stdin = open("C-small-2-attempt0.in","r")
	sys.stdout = open("out.txt","w")

	case = sys.stdin.readline().strip('\n')
	for i in range(1, int(case)+1):
		num = sys.stdin.readline().strip('\n')
		param = []
		for j in range(0, int(num)):
			param.append(sys.stdin.readline().strip('\n'))
		print "Case #" + str(i) + ": " + str(sort(param))





if __name__== "__main__":
	main()
