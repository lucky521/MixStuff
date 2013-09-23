import sys

table = ["zero","one","two","three","four","five","six","seven","eight","nine"]
time = ["0","1","double","triple","quadruple","quintuple","sextuple","septuple","octuple","nonuple","decuple"]

def convert(s):
	word = ""
	p1 = 0
	p2 = 1
	while p2<len(s):
		if s[p2] != s[p2-1]:
			if p2 == p1+1:
				word += " " + table[int(s[p1])]
				p1 += 1
				p2 += 1
			else:
				if p2-p1 > 10:
					word += " " + time[10] + " " + table[int(s[p1])]
					word += " " + time[p2-p1-10] + " " + table[int(s[p1])]
				else:	
					word += " " + time[p2-p1] + " " + table[int(s[p1])]
				p1 = p2
				p2 += 1
		else:
			p2 += 1
			
	if p1 == len(s)-1:
		word += " " + table[int(s[p1])]
	else:
		if p2-p1 > 10:
			word += " " + time[10] + " " + table[int(s[p1])]
			word += " " + time[p2-p1-10] + " " + table[int(s[p1])]
		else:	
			word += " " + time[p2-p1] + " " + table[int(s[p1])]
		
	return word



def spell(number, separate):
	partnum = len(separate)
	idx = 0
	words = "" 
	for i in range(0, partnum):
		s = number[idx:idx+int(separate[i])]
		words += convert(s)
		idx += int(separate[i])
	return words


def main():
	sys.stdin.close()
	sys.stdin = open("A-large.in","r")
	sys.stdout = open("out.txt","w")

	case = sys.stdin.readline().strip('\n')
	for i in range(1, int(case)+1):
		line = sys.stdin.readline().strip('\n')
		line = line.split()
		phonenumber = line[0]
		separate = line[1]
		separate = separate.split('-')
		
		result = spell(phonenumber, separate)

		print "Case #" + str(i) + ":" + result


if __name__== "__main__":
	main()
