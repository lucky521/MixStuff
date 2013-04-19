import os, re

word_list = file("wordlist", "w")

pwd = os.getcwd()
pwd = os.path.join(pwd ,"dataset")


p_dir = os.listdir(pwd)

global wordset
wordset = set()

for p in p_dir:
	c_pwd = os.path.join(pwd, p)
	c_dir = os.listdir(c_pwd)
	
	for c in c_dir:
		tmp = file(os.path.join(c_pwd, c))

		while True:
			line = tmp.readline()
			if len(line) == 0 :
				break
			for match in re.finditer("(?<= )([a-z]|[A-Z])*([a-z]|[A-Z])(?= )", line) :
				wordset.add(match.group().lower())


		tmp.close()




print len(wordset)
word_index = -1
for word in wordset:
	word_index += 1
	word_list.write(str(word_index) + "  " + word + "\n")
word_list.close()
