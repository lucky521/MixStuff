import os, re, enchant
import numpy
from nltk import PorterStemmer 
d = enchant.Dict("en_US")

#build filename-list
file_name = file("file_name","w")
pwd = os.getcwd()
pwd = os.path.join(pwd, "dataset")

p_dir = os.listdir(pwd)
file_index = -1
for p in p_dir:
	cpwd = os.path.join(pwd, p)
	c_dir = os.listdir(cpwd)
	
	for c in c_dir:
		file_index = file_index + 1
		file_name.write(str(file_index) + "  " + os.path.join(cpwd, c) + '\n')

file_name.close()
#####################################################
# build word list
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
				s = match.group().lower()
				word = PorterStemmer().stem_word(s)
				if d.check(word) and len(word) > 1:
					wordset.add(word)
					
		tmp.close()


wordset = list(wordset)
wordset.sort()

###########################################################
# build word-to-index-dirt for later
word_index_dict = {}
word_index = -1
for word in wordset:
	word_index += 1
	word_index_dict[word] = word_index


############################################################
#build wordlist-file
word_list = file("wordlist", "w")
word_index = -1
for word in wordset:
	word_index += 1
	word_list.write(str(word_index) + "  " + word + "\n")
word_list.close()


############################################################
#build matrix
word_num = len(wordset)
file_num = 3044	#TODO:len(filelist)

init_row = [0 for i in range(0,file_num)]
init_list = [init_row for i in range(0, word_num)]

m = numpy.matrix(init_list)

############################################################

pwd = os.getcwd()
pwd = os.path.join(pwd, "dataset")

p_dir = os.listdir(pwd)

this_file_index = -1
for p in p_dir:
	cpwd = os.path.join(pwd, p)
	c_dir = os.listdir(cpwd)
	for c in c_dir:
		onefile = file(os.path.join(cpwd, c)) 
		this_file_index += 1	
		#Now this file is the [file_index]
		while True:
			line = onefile.readline()
			if len(line) == 0:
				break
			for match in re.finditer("(?<= )([a-z]|[A-Z])*([a-z]|[A-Z])(?= )", line) :
				s = match.group().lower()
				word = PorterStemmer().stem_word(s)
				if d.check(word) and len(word) > 1:
					#get this word's word_index
					this_word_index = word_index_dict[word]
					#compute
					m[(this_word_index, this_file_index)] += 1
				
		onefile.close()
		

#numpy.savetxt("the_matrix", m, fmt="%d", delimiter="  ")

############################################################
# build a sorted index for every line in the matrix
os.mkdir("result")
word_id = -1
for i in range(0, word_num):
	word_id += 1
	f = file("result/" + str(word_id), 'w')
	sort_wordlist = []
	mline = numpy.matrix.tolist(m[i])
	for j in range(0, file_num):
		temp = mline[0]
		if temp[j] != 0:
			tmp = [temp[j], j]
			sort_wordlist.append(tmp)
		
	sort_wordlist.sort(reverse = True)
	for z in sort_wordlist:
		f.write(str(z) + "\n")
	f.close()


