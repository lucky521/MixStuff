import sys
def judge(m_dict):
	set1 = set()
	set2 = set()
	k = m_dict.keys()
	set1.add(k[0])
	for w in m_dict[k[0]]:
		set2.add(w)
	while(1):
		flag = 0
		for key in m_dict:
			if key in set1:
				for w in m_dict[key]:
					if w in set1:
						return 0
					else:
						set2.add(w)
			elif key in set2:
				for w in m_dict[key]:
					if w in set2:
						return 0
					else:
						set1.add(w)
			else:
				flag == 1
		if flag==0:
			return 1

def main():
	sys.stdin.close()
	sys.stdin = open("A-small-1-attempt1.in","r")
	sys.stdout = open("out.txt","w")

	case = sys.stdin.readline().strip('\n')
	for i in range(1, int(case)+1):
		num = sys.stdin.readline().strip('\n')

		m_dict = {}
		for j in range(0, int(num)):
			a = sys.stdin.readline().strip('\n')
			b = a.split()
			if m_dict.has_key(b[0]):
				m_dict[b[0]].append(b[1])
			else:
				m_dict[b[0]] = []
				m_dict[b[0]].append(b[1])

			if m_dict.has_key(b[1]):
				m_dict[b[1]].append(b[0])
			else:
				m_dict[b[1]] = []
				m_dict[b[1]].append(b[0])


		result = judge(m_dict)
		if result == 1:
			print "Case #" + str(i) + ": Yes"
		else:
			print "Case #" + str(i) + ": No"



if __name__== "__main__":
	main()
